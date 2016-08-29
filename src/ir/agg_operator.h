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

#ifndef MUSKETEER_AGG_OPERATOR_H
#define MUSKETEER_AGG_OPERATOR_H

#include "ir/aggregation.h"

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

class AggOperator : public Aggregation {
 public:
  AggOperator(const string& input_dir, const pANTLR3_BASE_TREE condition_tree,
              const vector<Column*>& group_bys_, const string& operator_,
              const vector<Relation*>& relations,
              const vector<Column*>& columns_, Relation* output_rel):
    Aggregation(input_dir, condition_tree, group_bys_, operator_, relations,
                columns_, output_rel),
      math_operator(operator_) {
  }

  AggOperator(const string& input_dir,
              const vector<pANTLR3_BASE_TREE>& condition_tree,
              const vector<Column*>& group_bys_, const string& operator_,
              const vector<Relation*>& relations,
              const vector<Column*> columns_, Relation* output_rel):
    Aggregation(input_dir, condition_tree, group_bys_, operator_, relations,
                columns_, output_rel),
      math_operator(operator_) {
  }

  AggOperator(const string& input_dir, ConditionTree* condition_tree,
              const vector<Column*>& group_bys_, string operator_,
              const vector<Relation*>& relations,
              const vector<Column*> columns_, Relation* output_rel):
    Aggregation(input_dir, condition_tree, group_bys_, operator_, relations,
                columns_, output_rel),
      math_operator(operator_) {
  }

  ~AggOperator() {
    for (vector<Column*>::iterator it = group_bys.begin();
         it != group_bys.end(); ++it) {
      delete *it;
    }
    group_bys.clear();
    for (vector<Column*>::iterator it = columns.begin();
         it != columns.end(); ++it) {
      delete *it;
    }
    columns.clear();
  }

  void update_columns();
  vector<Column*> get_group_bys();
  string get_operator();
  vector<Column*> get_columns();
  virtual OperatorType get_type();
  bool hasAction();
  bool hasGroupby();
  bool mapOnly();
  pair<uint64_t, uint64_t> get_output_size(
      map<string, pair<uint64_t, uint64_t> >* rel_size);
  OperatorInterface* clone();
  OperatorInterface* toMPC();

 protected:
  string math_operator;
};

} // namespace ir
} // namespace musketeer
#endif
