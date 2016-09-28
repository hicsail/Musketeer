// Copyright (c) 2015 Ionel Gog <ionel.gog@cl.cam.ac.uk>

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

#include "tests/mindi/test.h"

#include <vector>

#include "base/common.h"
#include "base/flags.h"
#include "ir/column.h"
#include "ir/value.h"
#include "ir/condition_tree.h"
#include "ir/input_operator.h"

namespace musketeer {
namespace tests {
namespace mindi {

  using ir::CondOperator;
  using ir::InputOperator;
  using ir::ConditionTree;
  using musketeer::Value;

  vector<Column*> Test::col(shared_ptr<OperatorNode> op_node) {
    return op_node->get_operator()->get_output_relation()->get_columns();
  }

  vector<shared_ptr<OperatorNode>> Test::proj_union_test() {
    Mindi* mindi = new Mindi();

    vector<Column*> input_cols;
    input_cols.push_back(new Column("cab_data", 0, INTEGER_TYPE));
    input_cols.push_back(new Column("cab_data", 1, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 2, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 3, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 4, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 5, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 6, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 7, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 8, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 9, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 10, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 11, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 12, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 13, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 14, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 15, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 16, STRING_TYPE));
    input_cols.push_back(new Column("cab_data", 17, INTEGER_TYPE_PRIV));

    set<Owner*> owners;
    owners.insert(new Owner("cab company"));
    owners.insert(new Owner("other cab company"));
    
    Relation* input_rel = new Relation("cab_data", input_cols, owners);
    vector<Relation*> input_rels;
    input_rels.push_back(input_rel);

    OperatorInterface* input_op = new InputOperator(FLAGS_hdfs_input_dir, input_rels, input_rel);
    shared_ptr<OperatorNode> input_op_node = shared_ptr<OperatorNode>(new OperatorNode(input_op));
    
    vector<Column*> selected_cols;
    selected_cols.push_back(col(input_op_node)[0]->clone());
    selected_cols.push_back(col(input_op_node)[17]->clone());
    
    shared_ptr<OperatorNode> selected_input =
      mindi->Select(input_op_node,
                    selected_cols,
                    "selected_input");

    vector<Column*> other_agg_cols;
    other_agg_cols.push_back(col(selected_input)[1]->clone());
    shared_ptr<OperatorNode> other_agg =
      mindi->GroupBy(selected_input, other_agg_cols, PLUS_GROUP,
                     col(selected_input)[0]->clone(), "other_agg");

    ConditionTree* first_val_blank_cond_tree =
      new ConditionTree(new CondOperator("*"),
                        new ConditionTree(col(selected_input)[0]->clone()),
                        new ConditionTree(new Value("0", INTEGER_TYPE)));
    
    vector<Column*> first_val_blank_cols;
    first_val_blank_cols.push_back(col(selected_input)[0]->clone());
    first_val_blank_cols.push_back(col(selected_input)[1]->clone());

    shared_ptr<OperatorNode> first_val_blank =
      mindi->Select(selected_input,
                    first_val_blank_cols,
                    first_val_blank_cond_tree,
                    "first_val_blank");

    vector<Column*> local_rev_group_by_cols;
    local_rev_group_by_cols.push_back(col(first_val_blank)[1]->clone());
    shared_ptr<OperatorNode> local_rev =
      mindi->GroupBy(first_val_blank, local_rev_group_by_cols, PLUS_GROUP,
                     col(first_val_blank)[0]->clone(), "local_rev");

    shared_ptr<OperatorNode> combined = 
      mindi->Concat(local_rev, "combined", other_agg);

    vector<shared_ptr<OperatorNode>> dag;
    dag.push_back(selected_input);
    return dag;
  }

  vector<shared_ptr<OperatorNode>> Test::join_test() {
    Mindi* mindi = new Mindi();

    vector<shared_ptr<OperatorNode>> parents;

    vector<Column*> tax_input_cols;
    tax_input_cols.push_back(new Column("tax_data", 0, INTEGER_TYPE));
    tax_input_cols.push_back(new Column("tax_data", 1, INTEGER_TYPE_PRIV));

    set<Owner*> tax_owners;
    tax_owners.insert(new Owner("tax company"));
    
    Relation* tax_input_rel = new Relation("tax_data", tax_input_cols, tax_owners);
    vector<Relation*> tax_input_rels;
    tax_input_rels.push_back(tax_input_rel);

    OperatorInterface* tax_input_op = new InputOperator(FLAGS_hdfs_input_dir, tax_input_rels, tax_input_rel);
    shared_ptr<OperatorNode> tax_input_op_node = 
      shared_ptr<OperatorNode>(new OperatorNode(tax_input_op, parents));
    
    vector<Column*> ed_input_cols;
    ed_input_cols.push_back(new Column("ed_data", 0, INTEGER_TYPE));
    ed_input_cols.push_back(new Column("ed_data", 1, INTEGER_TYPE_PRIV));

    set<Owner*> ed_owners;
    ed_owners.insert(new Owner("ed company"));
    ed_owners.insert(new Owner("another ed company"));
    
    Relation* ed_input_rel = new Relation("ed_data", ed_input_cols, ed_owners);
    vector<Relation*> ed_input_rels;
    ed_input_rels.push_back(ed_input_rel);

    OperatorInterface* ed_input_op = new InputOperator(
      FLAGS_hdfs_input_dir, ed_input_rels, ed_input_rel);
    shared_ptr<OperatorNode> ed_input_op_node = 
      shared_ptr<OperatorNode>(new OperatorNode(ed_input_op, parents));
    
    vector<Column*> tax_tot_group_by_cols;
    tax_tot_group_by_cols.push_back(col(tax_input_op_node)[0]->clone());
    shared_ptr<OperatorNode> tax_tot =
      mindi->GroupBy(tax_input_op_node, tax_tot_group_by_cols, PLUS_GROUP,
                     col(tax_input_op_node)[1]->clone(), "tax_tot");

    vector<Column*> ed_tot_group_by_cols;
    ed_tot_group_by_cols.push_back(col(ed_input_op_node)[0]->clone());
    shared_ptr<OperatorNode> ed_tot =
      mindi->GroupBy(ed_input_op_node, ed_tot_group_by_cols, PLUS_GROUP,
                     col(ed_input_op_node)[1]->clone(), "ed_tot");

    vector<Column*> left;
    left.push_back(col(tax_tot)[0]);
    vector<Column*> right;
    right.push_back(col(ed_tot)[0]);

    shared_ptr<OperatorNode> tax_ed = 
      mindi->Join(tax_tot, "tax_ed", ed_tot, left, right);

    vector<shared_ptr<OperatorNode>> dag;
    dag.push_back(tax_tot);
    dag.push_back(ed_tot);
    return dag;
  }

  vector<shared_ptr<OperatorNode>> Test::Run() {
    return join_test();
  }

} // namespace mindi
} // namespace tests
} // namespace musketeer
