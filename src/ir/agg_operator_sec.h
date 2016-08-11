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

#ifndef MUSKETEER_AGG_OPERATOR_SEC_H
#define MUSKETEER_AGG_OPERATOR_SEC_H

// #include "ir/operator_interface.h"
#include "ir/agg_operator.h"

#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base/common.h"
#include "ir/column.h"
#include "ir/condition_tree.h"
#include "ir/relation.h"
#include "RLPlusLexer.h"
#include "RLPlusParser.h"

namespace musketeer {
namespace ir {

class AggOperatorSEC : public AggOperator {
 public:

  AggOperatorSEC(const string& input_dir, ConditionTree* condition_tree,
                 const vector<Column*>& group_bys_, string operator_,
                 const vector<Relation*>& relations,
                 const vector<Column*> columns_, Relation* output_rel):
    AggOperator(input_dir, condition_tree, group_bys_, operator_, relations,
                columns_, output_rel) {
  }

  OperatorType get_type();
  bool isMPC();
  OperatorInterface* clone();

 private:
  vector<Column*> group_bys;
  string math_operator;
  vector<Column*> columns;
};

} // namespace ir
} // namespace musketeer
#endif
