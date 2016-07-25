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

#ifndef MUSKETEER_SELECT_OPERATOR_SEC_H
#define MUSKETEER_SELECT_OPERATOR_SEC_H

#include "ir/operator_interface.h"

#include <stdint.h>

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

class SelectOperatorSEC : public OperatorInterface {
 public:
  
  SelectOperatorSEC(const string& input_dir, ConditionTree* condition_tree,
                 const vector<Column*>& columns_,
                 const vector<Relation*>& relations,
                 Relation* output_relation):
    OperatorInterface(input_dir, relations, output_relation, condition_tree),
      columns(columns_) {
  }

  ~SelectOperatorSEC() {
    for (vector<Column*>::iterator it = columns.begin();
         it != columns.end(); ++it) {
      delete *it;
    }
    columns.clear();
  }

  OperatorType get_type();
  bool mapOnly();
  vector<Column*> get_columns();
  pair<uint64_t, uint64_t> get_output_size(
      map<string, pair<uint64_t, uint64_t> >* rel_size);
  OperatorInterface* clone();

 private:
  vector<Column*> columns;
};

} // namespace ir
} // namespace musketeer
#endif
