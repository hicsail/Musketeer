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

#ifndef MUSKETEER_AGGREGATION_H
#define MUSKETEER_AGGREGATION_H

#include "ir/operator_interface.h"

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

class Aggregation : public OperatorInterface {
 public:
  Aggregation(const string& input_dir, const pANTLR3_BASE_TREE condition_tree,
              const vector<Column*>& group_bys_, const GroupByType group_by_type_,
              const vector<Relation*>& relations,
              const vector<Column*>& columns_, Relation* output_rel):
    OperatorInterface(input_dir, relations, output_rel,
                      new ConditionTree(condition_tree)),
      group_bys(group_bys_), columns(columns_), group_by_type(group_by_type_) {
  }

  Aggregation(const string& input_dir,
              const vector<pANTLR3_BASE_TREE>& condition_tree,
              const vector<Column*>& group_bys_, const GroupByType group_by_type_,
              const vector<Relation*>& relations,
              const vector<Column*> columns_, Relation* output_rel):
    OperatorInterface(input_dir, relations, output_rel,
                      new ConditionTree(condition_tree)),
      group_bys(group_bys_), columns(columns_), group_by_type(group_by_type_) {
  }

  Aggregation(const string& input_dir, ConditionTree* condition_tree,
              const vector<Column*>& group_bys_, GroupByType group_by_type_,
              const vector<Relation*>& relations,
              const vector<Column*> columns_, Relation* output_rel):
    OperatorInterface(input_dir, relations, output_rel, condition_tree),
      group_bys(group_bys_), columns(columns_), group_by_type(group_by_type_) {
  }

  Aggregation(const string& input_dir, const pANTLR3_BASE_TREE condition_tree,
              const vector<Column*>& group_bys_, const string& operator_,
              const vector<Relation*>& relations,
              const vector<Column*>& columns_, Relation* output_rel):
    OperatorInterface(input_dir, relations, output_rel,
                      new ConditionTree(condition_tree)),
      group_bys(group_bys_), columns(columns_), group_by_type(type_lookup(operator_)) {
  }

  Aggregation(const string& input_dir,
              const vector<pANTLR3_BASE_TREE>& condition_tree,
              const vector<Column*>& group_bys_, const string& operator_,
              const vector<Relation*>& relations,
              const vector<Column*> columns_, Relation* output_rel):
    OperatorInterface(input_dir, relations, output_rel,
                      new ConditionTree(condition_tree)),
      group_bys(group_bys_), columns(columns_), group_by_type(type_lookup(operator_)) {
  }

  Aggregation(const string& input_dir, ConditionTree* condition_tree,
              const vector<Column*>& group_bys_, string operator_,
              const vector<Relation*>& relations,
              const vector<Column*> columns_, Relation* output_rel):
    OperatorInterface(input_dir, relations, output_rel, condition_tree),
      group_bys(group_bys_), columns(columns_), group_by_type(type_lookup(operator_)) {
  }

  virtual ~Aggregation() = 0;

  GroupByType type_lookup(string operator_);
  void update_columns();
  vector<Column*> get_group_bys();
  GroupByType get_group_by_type();
  vector<Column*> get_columns();
  bool hasAction();
  bool hasGroupby();
  bool mapOnly();
  pair<uint64_t, uint64_t> get_output_size(
      map<string, pair<uint64_t, uint64_t> >* rel_size);
  
 protected:
  vector<Column*> group_bys;
  vector<Column*> columns;
  GroupByType group_by_type;
};

} // namespace ir
} // namespace musketeer
#endif
