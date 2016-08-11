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

  shared_ptr<OperatorNode> Test::Run() {
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
    input_cols.push_back(new Column("cab_data", 17, INTEGER_TYPE_SEC));

    Relation* input_rel = new Relation("cab_data", input_cols);
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
    local_rev_group_by_cols.push_back(col(first_val_blank)[0]->clone());
    shared_ptr<OperatorNode> local_rev =
      mindi->GroupBy(first_val_blank, local_rev_group_by_cols, PLUS_GROUP,
                     col(first_val_blank)[1]->clone(), "local_rev");

    ConditionTree* local_rev_per_cond_tree =
      new ConditionTree(new CondOperator("/"),
                        new ConditionTree(col(local_rev)[1]->clone()),
                        new ConditionTree(new Value("10", INTEGER_TYPE)));
    
    vector<Column*> local_rev_per_cols;
    local_rev_per_cols.push_back(col(local_rev)[0]->clone());
    local_rev_per_cols.push_back(col(local_rev)[1]->clone());

    shared_ptr<OperatorNode> local_rev_per =
      mindi->Select(local_rev,
                    local_rev_per_cols,
                    local_rev_per_cond_tree,
                    "local_rev_per");

    ConditionTree* local_rev_scaled_cond_tree =
      new ConditionTree(new CondOperator("/"),
                        new ConditionTree(col(local_rev)[1]->clone()),
                        new ConditionTree(new Value("1000", INTEGER_TYPE)));
    
    vector<Column*> local_rev_scaled_cols;
    local_rev_scaled_cols.push_back(col(local_rev)[0]->clone());
    local_rev_scaled_cols.push_back(col(local_rev)[1]->clone());

    shared_ptr<OperatorNode> local_rev_scaled =
      mindi->Select(local_rev,
                    local_rev_scaled_cols,
                    local_rev_scaled_cond_tree,
                    "local_rev_scaled");

    vector<Column*> total_rev_group_by_cols;
    total_rev_group_by_cols.push_back(col(local_rev_scaled)[0]->clone());
    shared_ptr<OperatorNode> total_rev =
      mindi->GroupBySEC(local_rev_scaled, total_rev_group_by_cols, PLUS_GROUP,
                        col(local_rev_scaled)[1]->clone(), "total_rev"); // double check on the column

    // vector<Column*> left;
    // left.push_back(col(local_rev_per)[0]);
    // vector<Column*> right;
    // right.push_back(col(total_rev)[0]);

    // shared_ptr<OperatorNode> local_total_rev = 
    //   mindi->JoinSEC(local_rev_per, "local_total_rev", total_rev, left, right);

    // ConditionTree* market_share_cond_tree =
    //   new ConditionTree(new CondOperator("/"),
    //                     new ConditionTree(col(local_total_rev)[1]->clone()),
    //                     new ConditionTree(col(local_total_rev)[2]->clone()));
    
    // vector<Column*> market_share_cols;
    // market_share_cols.push_back(col(local_total_rev)[0]->clone());
    // market_share_cols.push_back(col(local_total_rev)[1]->clone());
    // market_share_cols.push_back(col(local_total_rev)[2]->clone());

    // shared_ptr<OperatorNode> market_share =
    //   mindi->SelectSEC(local_total_rev, market_share_cols, market_share_cond_tree,
    //                    "market_share");
    
    // ConditionTree* market_share_squared_tree =
    //   new ConditionTree(new CondOperator("*"),
    //                     new ConditionTree(col(market_share)[1]->clone()),
    //                     new ConditionTree(col(market_share)[1]->clone()));
    
    // vector<Column*> market_share_squared_cols;
    // market_share_squared_cols.push_back(col(market_share)[0]->clone());
    // market_share_squared_cols.push_back(col(market_share)[1]->clone());
    // market_share_squared_cols.push_back(col(market_share)[2]->clone());

    // shared_ptr<OperatorNode> market_share_squared =
    //   mindi->SelectSEC(market_share,
    //                    market_share_squared_cols,
    //                    market_share_squared_tree,
    //                    "market_share_squared");

    // vector<Column*> hhi_group_by_cols;
    // hhi_group_by_cols.push_back(col(market_share_squared)[0]->clone());
    // shared_ptr<OperatorNode> hhi =
    //   mindi->GroupBySEC(market_share_squared, hhi_group_by_cols, PLUS_GROUP,
    //                     col(market_share_squared)[1]->clone(), "hhi");

    return selected_input;
  }

} // namespace mindi
} // namespace tests
} // namespace musketeer
