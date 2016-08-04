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

#include "tests/mindi/single_group_by.h"

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

  shared_ptr<OperatorNode> SingleGroupby::Run() {
    Mindi* mindi = new Mindi();
    vector<Column*> edges;
    edges.push_back(new Column("edges", 0, INTEGER_TYPE));
    edges.push_back(new Column("edges", 1, INTEGER_TYPE_SEC));
    
    Relation* edges_rel = new Relation("edges", edges);
    vector<Relation*> edges_rels;
    edges_rels.push_back(edges_rel);

    OperatorInterface* edge_op = new InputOperator(FLAGS_hdfs_input_dir, edges_rels, edges_rel);
    shared_ptr<OperatorNode> temp = shared_ptr<OperatorNode>(new OperatorNode(edge_op));

    vector<Column*> sum_group_by_cols;
    sum_group_by_cols.push_back(edges[0]->clone());
    shared_ptr<OperatorNode> sum_group_by =
      mindi->GroupBy(temp, sum_group_by_cols, PLUS_GROUP,
                     edges[1], "sum_group_by");

    return sum_group_by;
  }

} // namespace mindi
} // namespace tests
} // namespace musketeer
