// Copyright (c) 2016 Nikolaj Volgushev <nikolaj@bu.edu>

/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR
 * A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

#include "mpc/obligation.h"
#include "ir/value.h"
#include "ir/condition_tree.h"

namespace musketeer {
namespace mpc {

    Obligation::Obligation(shared_ptr<OperatorNode> op_node, int index) {
        OperatorInterface* _op = op_node->get_operator();
        // TODO(nikolaj): check operator type
        op = dynamic_cast<Aggregation*>(_op->toMPC());
        type = op->get_group_by_type();
        string rel_name = _op->get_output_relation()->get_name();
        Relation* new_rel = _op->get_output_relation()->copy(
            rel_name + "_obl_" + boost::lexical_cast<string>(index));
        op->set_output_relation(new_rel);
        update_op_columns(_op);
    }   

    OperatorInterface* Obligation::get_operator() {
        return op;
    }

    GroupByType Obligation::get_group_by_type() {
        return type;
    }

    string Obligation::get_name() {
        return op->get_output_relation()->get_name();
    }

    int Obligation::get_agg_index() {
        return op->get_columns()[0]->get_index();
    }

    vector<Column*> Obligation::get_group_bys() {
        return op->get_group_bys();
    }

    // This is a bit of a hack for now
    void Obligation::update_op_columns(OperatorInterface* parent) {
        vector<Column*> parent_out_cols = parent->get_output_relation()->get_columns();
        string rel_out_name = parent->get_output_relation()->get_name();
        Column* key_col = parent_out_cols.back();
        vector<Column*> columns;
        columns.push_back(
            new Column(rel_out_name, key_col->get_index(), key_col->get_type()));
        vector<Column*> group_bys;
        for (vector<Column*>::iterator i = parent_out_cols.begin(); 
             i != parent_out_cols.end() - 1; ++i) {
            group_bys.push_back(new Column(rel_out_name, (*i)->get_index(), (*i)->get_type()));
        }
        op->set_columns(columns);
        op->set_group_bys(group_bys);
    }

    void Obligation::PassThrough(shared_ptr<OperatorNode> op_node) {
        // TODO(nikolaj): Implement.
    }

    bool Obligation::CanPassOperator(OperatorInterface* other_op, Obligation* other_obl) {
        switch (other_op->get_type()) {
        case SELECT_OP:
            return CanPass(dynamic_cast<SelectOperator*>(other_op));
        case MUL_OP:
            return CanPass(dynamic_cast<MulOperator*>(other_op));
        case DIV_OP:
            return CanPass(dynamic_cast<DivOperator*>(other_op));
        default:
            return false;
        }
    }

    bool Obligation::CanPass(SelectOperator* other) {
        // SelectOp could either be a projection on the columns of the input
        // or a conditional filter (or both?)
        if (op->get_output_relation()->get_columns().size() 
            != other->get_output_relation()->get_columns().size()) {
            // the columns have changed so we can't push
            // TODO(nikolaj): do a finer grained analysis here.
            // there might be cases when it's still okay to push the obligation.
            return false;
        }
        // TODO(nikolaj): check if columns were reordered

        ConditionTree* tree = other->get_condition_tree();
        CondOperator* cond_op = tree->get_cond_operator();

        if (cond_op) {
            // gist: not safe to filter on *value* columns 
            // but safe to filter on *key* columns
            ConditionTree* left = tree->get_left();
            ConditionTree* right = tree->get_right();

            if (!(left->isColumn() || right->isColumn() || left->isValue() || right->isValue())) {
                // Won't deal with multi-level condition trees for now
                return false;
            }

            if (left->isColumn() && right->isColumn()) {
                // both operands are columns, so we can't push
                return false;
            }
            if (left->isValue() && right->isValue()) {
                // both operands should not be values
                LOG(ERROR) << "Both operands of condition tree are values";
                return false;
            }
            // at this point either left or right must be a column
            Column* col = NULL;
            int index_agg = op->get_columns()[0]->get_index();
            if (left->isColumn()) {
                col = left->get_column();
            }
            else if (right->isColumn()) {
                col = right->get_column();
            }
            return index_agg == col->get_index();
        }
        else {
            // no conditional operator means we have a singleton value or column
            Value* val = tree->get_value();
            if (val && val->get_type() == BOOLEAN_TYPE && val->get_value() == "true") {
                // safe to push since all rows are retained so the key space does
                // not change
                return true;
            }
            // conditional is column or false so we can't push 
            // (the key space might have changed)
            return false;
        }

        return true;
    }

    bool Obligation::CanPass(MulOperator* other) {
        // sanity check: output sizes must be same
        if (op->get_output_relation()->get_columns().size() 
            != other->get_output_relation()->get_columns().size()) {
            LOG(ERROR) << "Output sizes of obligation and mul operator different.";
            return false;
        }

        // assuming no nested ops, only single product
        vector<Value*> values = other->get_values();
        Value* left = values[0];
        Value* right = values[1];
        
        Column* left_col = dynamic_cast<Column*>(left);
        Column* right_col = dynamic_cast<Column*>(right);
        int index_agg = op->get_columns()[0]->get_index();
        int index_proj;

        if (left_col) {
            index_proj = left_col->get_index();
        }
        else if (right_col) {
            index_proj = right_col->get_index();
        }
        else {
            LOG(ERROR) << "Both operands in the arithmetic expression are values.";
            return false;
        }
        // TODO(nikolaj): Should check somewhere if math operations commute

        if (index_agg == index_proj) {
            // it's safe to push if the projection only affects
            // the agg column
            return true;
        }
        // Reaching this point means that we're potentially affecting the key space
        // of the aggregation
        if (left_col && right_col) {
            // We're computing the product of two columns. No way of knowing if either
            // one is 0 so can't safely push.
            return false;
        }

        if (left->get_value() == "0" || right->get_value() == "0") {
            // multiplication by 0 changes key space so we can't push
            return false;
        }

        return true;
    }

    bool Obligation::CanPass(DivOperator* other) {
        // TODO(nikolaj): this is basically identical to the CanPass method for
        // MulOperator (and other projections). This needs refactoring. 

        if (op->get_output_relation()->get_columns().size() 
            != other->get_output_relation()->get_columns().size()) {
            LOG(ERROR) << "Output sizes of obligation and mul operator different.";
            return false;
        }

        // assuming no nested ops, only single product
        vector<Value*> values = other->get_values();
        Value* left = values[0];
        Value* right = values[1];
        
        Column* left_col = dynamic_cast<Column*>(left);
        Column* right_col = dynamic_cast<Column*>(right);
        int index_agg = op->get_columns()[0]->get_index();
        int index_proj;

        if (left_col) {
            index_proj = left_col->get_index();
        }
        else if (right_col) {
            index_proj = right_col->get_index();
        }
        else {
            LOG(ERROR) << "Both operands in the arithmetic expression are values.";
            return false;
        }
        // TODO(nikolaj): Should check somewhere if math operations commute

        if (index_agg == index_proj) {
            // it's safe to push if the projection only affects
            // the agg column
            return true;
        }
        // Reaching this point means that we're potentially affecting the key space
        // of the aggregation
        if (left_col && right_col) {
            // We're computing the product of two columns. No way of knowing if either
            // one is 0 so can't safely push.
            return false;
        }

        if (left->get_value() == "0" || right->get_value() == "0") {
            // multiplication by 0 changes key space so we can't push
            return false;
        }

        return true;
    }

    bool Obligation::CanMerge(Obligation& other_obl) {
        OperatorInterface* other = other_obl.get_operator();
        if (op->get_output_relation()->get_columns().size() 
            != other->get_output_relation()->get_columns().size()) {
            return false;
        }

        int agg_index = get_agg_index();
        int other_agg_index = other_obl.get_agg_index();

        if (agg_index != other_agg_index) {
            return false;
        }

        if (type != other_obl.get_group_by_type()) {
            return false;
        }

        // currently there is no need to check if all the indeces
        // of the group by cols align since that is an invariant of
        // how they are created

        return true;
    }

} // namespace mpc
} // namespace musketeer
