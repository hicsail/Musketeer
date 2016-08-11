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

#ifndef MUL_OPERATOR_SEC_H
#define MUL_OPERATOR_SEC_H

#include "ir/mul_operator.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base/common.h"
#include "ir/column.h"
#include "ir/condition_tree.h"
#include "ir/relation.h"
#include "ir/value.h"
#include "RLPlusLexer.h"
#include "RLPlusParser.h"

namespace musketeer {
namespace ir {

class MulOperatorSEC : public MulOperator {
 public:
  
  MulOperatorSEC(const string& input_dir, ConditionTree* condition_tree,
                 const vector<Relation*>& relations, const vector<Value*>& values_,
                 Relation* output_rel):
    MulOperator(input_dir, condition_tree, relations, values_, output_rel) {
  }

  bool isMPC();
  OperatorType get_type();
  OperatorInterface* clone();
  
};

} // namespace ir
} // namespace musketeer
#endif
