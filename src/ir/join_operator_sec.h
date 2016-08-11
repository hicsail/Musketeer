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

#ifndef MUSKETEER_JOIN_OPERATOR_SEC_H
#define MUSKETEER_JOIN_OPERATOR_SEC_H

#include "ir/join_operator.h"

#include <stdint.h>

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base/common.h"
#include "ir/column.h"
#include "ir/relation.h"

namespace musketeer {
namespace ir {

class JoinOperatorSEC: public JoinOperator {
 public:
  JoinOperatorSEC(const string& input_dir, const vector<Relation*>& relations,
               vector<Column*> left_cols, vector<Column*> right_cols,
               Relation* output_relation):
    JoinOperator(input_dir, relations, left_cols, right_cols, output_relation) {
  }

  ~JoinOperatorSEC() {
    // TODO(ionel): Implement.
  }

  OperatorType get_type();
  bool isMPC();
  OperatorInterface* clone();
};

} // namespace ir
} // namespace musketeer
#endif
