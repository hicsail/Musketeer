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

    bool Obligation::CanPassOperator(OperatorInterface* other) {
        switch (other->get_type()) {
        case SELECT_OP:
            return CanPass(dynamic_cast<SelectOperator*>(other));
        case MUL_OP:
            return CanPass(dynamic_cast<MulOperator*>(other));
        default:
            return false;
        }
    }

    bool Obligation::CanPass(SelectOperator* other) {
        return false;
    }

    bool Obligation::CanPass(MulOperator* other) {
        // sanity check: output sizes must be same
        if (op->get_output_relation()->get_columns().size() 
            != other->get_output_relation()->get_columns().size()) {
            LOG(ERROR) << "Output sizes of obligation and operator different.";
            return false;
        }

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

} // namespace mpc
} // namespace musketeer
