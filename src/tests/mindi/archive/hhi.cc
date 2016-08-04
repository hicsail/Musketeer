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

#include "tests/mindi/netflix.h"

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

  shared_ptr<OperatorNode> Netflix::Run() {
    Mindi* mindi = new Mindi();
    
    vector<Column*> input_cols;
    input_cols.push_back(new Column("input", 0, INTEGER_TYPE));
    input_cols.push_back(new Column("input", 1, INTEGER_TYPE_SEC));

    Relation* input_rel = new Relation("input", input_cols);
    vector<Relation*> input_rels;
    input_rels.push_back(input_rel);

    OperatorInterface* input_op = new InputOperator(FLAGS_hdfs_input_dir, input_rels, input_rel);
    
    // For now a dummy selection but the specific use case might require an actual selection here
    ConditionTree* selected_input_cond_tree =
      new ConditionTree(new CondOperator("=="),
                        new ConditionTree(input_cols[0]->clone()),
                        new ConditionTree(input_cols[0]->clone()));
    
    shared_ptr<OperatorNode> selected_input =
      mindi->Where(shared_ptr<OperatorNode>(new OperatorNode(input_op)),
                   selected_input_cond_tree,
                   "selected_input");

    vector<Column*> selected_input_cols = 
      selected_input->get_operator()->get_output_relation()->get_columns();
    
    ConditionTree* first_val_blank_cond_tree =
      new ConditionTree(new CondOperator("-"),
                        new ConditionTree(selected_input_cols[0]->clone()),
                        new ConditionTree(selected_input_cols[0]->clone()));
    
    vector<Column*> first_val_blank_cols;
    first_val_blank_cols.push_back(selected_input_cols[0]->clone());
    first_val_blank_cols.push_back(selected_input_cols[1]->clone());

    shared_ptr<OperatorNode> first_val_blank =
      mindi->Select(selected_input,
                    first_val_blank_cols,
                    first_val_blank_cond_tree,
                    "first_val_blank");

    vector<Column*> local_rev_group_by_cols;
    local_rev_group_by_cols.push_back(first_val_blank_cols[0]->clone());
    shared_ptr<OperatorNode> local_rev =
      mindi->GroupBy(first_val_blank, local_rev_group_by_cols, PLUS_GROUP,
                     first_val_blank_cols[1]->clone(), "local_rev");

    vector<Column*> total_rev_group_by_cols;
    total_rev_group_by_cols.push_back(local_rev_group_by_cols[0]->clone());
    shared_ptr<OperatorNode> total_rev =
      mindi->GroupBySEC(local_rev, total_rev_group_by_cols, PLUS_GROUP,
                        local_rev_group_by_cols[1]->clone(), "total_rev");

    vector<Column*> left_cols;
    left_cols.push_back(total_rev->get_operator()->get_output_relation()->get_columns()[0]->clone());
    vector<Column*> right_cols;
    right_cols.push_back(local_rev->get_operator()->get_output_relation()->get_columns()[0]->clone());

    shared_ptr<OperatorNode> total_local_rev = 
      mindi->Join(total_rev, "total_local_rev", local_rev, left_cols, right_cols);

    vector<Column*> total_local_rev_cols = 
      total_local_rev->get_operator()->get_output_relation()->get_columns();
    
    ConditionTree* market_share_cond_tree =
      new ConditionTree(new CondOperator("/"),
                        new ConditionTree(total_local_rev_cols[1]->clone()),
                        new ConditionTree(total_local_rev_cols[2]->clone()));
    
    vector<Column*> market_share_cols;
    market_share_cols.push_back(total_local_rev_cols[0]->clone());
    market_share_cols.push_back(total_local_rev_cols[1]->clone());

    shared_ptr<OperatorNode> market_share =
      mindi->Select(total_local_rev, market_share_cols, market_share_cond_tree,
                    "market_share");
    
    ConditionTree* market_share_squared_tree =
      new ConditionTree(new CondOperator("*"),
                        new ConditionTree(market_share_cols[1]->clone()),
                        new ConditionTree(market_share_cols[1]->clone()));
    
    vector<Column*> market_share_squared_cols;
    market_share_squared_cols.push_back(market_share_cols[0]->clone());
    market_share_squared_cols.push_back(market_share_cols[1]->clone());

    shared_ptr<OperatorNode> market_share_squared =
      mindi->Select(market_share,
                    market_share_squared_cols,
                    market_share_squared_tree,
                    "market_share_squared");

    vector<Column*> hhi_group_by_cols;
    hhi_group_by_cols.push_back(market_share_squared_cols[0]->clone());
    shared_ptr<OperatorNode> hhi =
      mindi->GroupBySEC(market_share_squared, hhi_group_by_cols, PLUS_GROUP,
                        market_share_squared_cols[1]->clone(), "hhi");

    return selected_input;
  }

} // namespace mindi
} // namespace tests
} // namespace musketeer
