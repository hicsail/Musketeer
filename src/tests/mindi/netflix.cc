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
#include "ir/condition_tree.h"
#include "ir/input_operator.h"

namespace musketeer {
namespace tests {
namespace mindi {

  using ir::CondOperator;
  using ir::InputOperator;

  shared_ptr<OperatorNode> Netflix::Run() {
    Mindi* mindi = new Mindi();
    vector<Column*> edges;
    edges.push_back(new Column("edges", 0, INTEGER_TYPE));
    edges.push_back(new Column("edges", 1, INTEGER_TYPE_SEC));
    
    Relation* edges_rel = new Relation("edges", edges);
    vector<Relation*> edges_rels;
    edges_rels.push_back(edges_rel);

    OperatorInterface* edge_op = new InputOperator(FLAGS_hdfs_input_dir, edges_rels, edges_rel);
    
    ConditionTree* edges_sel_cond_tree =
      new ConditionTree(new CondOperator("*"),
                        new ConditionTree(edges[1]->clone()),
                        new ConditionTree(edges[1]->clone()));
    vector<Column*> temp_sel_cols;
    temp_sel_cols.push_back(edges[0]->clone());
    temp_sel_cols.push_back(edges[1]->clone());

    shared_ptr<OperatorNode> edges_sel =
      mindi->SelectSEC(shared_ptr<OperatorNode>(new OperatorNode(edge_op)),
                       temp_sel_cols,
                       edges_sel_cond_tree,
                       "edges_sel");

    vector<Column*> edges_sel_cols =
      edges_sel->get_operator()->get_output_relation()->get_columns();
    vector<Column*> sum_group_by_cols;
    sum_group_by_cols.push_back(edges_sel_cols[0]->clone());
    shared_ptr<OperatorNode> sum_group_by =
      mindi->GroupBySEC(edges_sel, sum_group_by_cols, PLUS_GROUP,
                        edges_sel_cols[1], "sum_group_by");

    return edges_sel->get_parents()[0];

    // Mindi* mindi = new Mindi();
    // vector<Column*> edges;
    // edges.push_back(new Column("edges", 0, INTEGER_TYPE));
    // edges.push_back(new Column("edges", 1, INTEGER_TYPE_SEC));
    
    // Relation* edges_rel = new Relation("edges", edges);
    // vector<Relation*> edges_rels;
    // edges_rels.push_back(edges_rel);

    // OperatorInterface* edge_op = new InputOperator(FLAGS_hdfs_input_dir, edges_rels, edges_rel);
    
    // ConditionTree* edges_sel_cond_tree =
    //   new ConditionTree(new CondOperator("*"),
    //                     new ConditionTree(edges[0]->clone()),
    //                     new ConditionTree(edges[0]->clone()));
    // vector<Column*> temp_sel_cols;
    // temp_sel_cols.push_back(edges[0]->clone());
    // temp_sel_cols.push_back(edges[1]->clone());

    // shared_ptr<OperatorNode> edges_sel =
    //   mindi->Select(shared_ptr<OperatorNode>(new OperatorNode(edge_op)),
    //                 temp_sel_cols,
    //                 edges_sel_cond_tree,
    //                 "edges_sel");
    
    // vector<Column*> edges_sel_cols =
    //   edges_sel->get_operator()->get_output_relation()->get_columns();
    // vector<Column*> sum_group_by_cols;
    // sum_group_by_cols.push_back(edges_sel_cols[0]->clone());
    // shared_ptr<OperatorNode> sum_group_by =
    //   mindi->GroupBy(edges_sel, sum_group_by_cols, PLUS_GROUP,
    //                  edges_sel_cols[1], "sum_group_by");

    // vector<Column*> prev_output_cols =
    //   sum_group_by->get_operator()->get_output_relation()->get_columns();
    
    // vector<Column*> sec_sum_group_by_cols;
    // sec_sum_group_by_cols.push_back(prev_output_cols[0]->clone());
    // shared_ptr<OperatorNode> sec_sum_group_by =
    //   mindi->GroupBySEC(sum_group_by, sec_sum_group_by_cols, PLUS_GROUP,
    //                  prev_output_cols[1], "sec_sum_group_by");

    // return edges_sel;
  }

} // namespace mindi
} // namespace tests
} // namespace musketeer
