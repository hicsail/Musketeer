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

#ifndef MUSKETEER_OPERATOR_INTERFACE_H
#define MUSKETEER_OPERATOR_INTERFACE_H

#include <stdint.h>

#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "base/common.h"
#include "base/flags.h"
#include "ir/condition_tree.h"
#include "ir/relation.h"

namespace musketeer {
namespace ir {

class OperatorInterface {
 public:
  OperatorInterface(const string& input_dir_,
                    const vector<Relation*>& relations_,
                    Relation* output_relation_):
    has_groupby(false), input_dir(input_dir_), relations(relations_),
    output_relation(output_relation_), rename(false), condition_tree(NULL) {
  }

  OperatorInterface(const string& input_dir_,
                    const vector<Relation*>& relations_,
                    Relation* output_relation_,
                    ConditionTree* condition_tree_):
    has_groupby(false), input_dir(input_dir_), relations(relations_),
    output_relation(output_relation_), rename(false),
    condition_tree(condition_tree_) {
  }


  virtual ~OperatorInterface() {
    delete output_relation;
    for (vector<Relation*>::iterator it = relations.begin();
         it != relations.end(); ++it) {
      delete *it;
    }
    relations.clear();
    delete condition_tree;
  }

  vector<Relation*> get_relations() {
    return relations;
  }

  void set_relations(vector<Relation*> vec) {
    relations = vec;
  }

  void set_output_relation(Relation* rel) {
    output_relation = rel;
  }

  Relation* get_output_relation() {
    return output_relation;
  }
  string get_output_path() {
    return get_input_dir() + output_relation->get_name() + "/";
  }

  string get_code_dir() {
    if (*FLAGS_generated_code_dir.rbegin() == '/') {
      return FLAGS_generated_code_dir;
    } else {
      return FLAGS_generated_code_dir + "/";
    }
  }

  // The returned path always ends with a /.
  string get_input_dir() {
    if (*input_dir.rbegin() == '/') {
      return input_dir;
    } else {
      return input_dir + "/";
    }
  }

  vector<string> get_input_paths() {
    vector<string> input_paths;
    for (vector<Relation*>::iterator it = relations.begin();
         it != relations.end(); ++it) {
      input_paths.push_back(get_input_dir() + (*it)->get_name() + "/");
    }
    return input_paths;
  }

  string CreateInputPath(Relation* relation) {
    return get_input_dir() + relation->get_name() + "/";
  }

  void set_rename(bool rename_) {
    if (rename_) {
      if (!rename) {
        rename = true;
        output_relation->set_name(output_relation->get_name() + "_tmp");
      }
    } else {
      if (rename) {
        rename = false;
        string cur_name = output_relation->get_name();
        output_relation->set_name(cur_name.substr(0, cur_name.length() - 4));
      }
    }
  }

  bool get_rename() {
    return rename;
  }

  ConditionTree* get_condition_tree() {
    return condition_tree;
  }

  void set_condition_tree(ConditionTree* new_condition_tree) {
    condition_tree = new_condition_tree;
  }

  // TODO(IONEL): UGLY! Copied code from utils.
  // Add the two numbers. If overflow then returns numeric limit.
  uint64_t SumNoOverflow(uint64_t a, uint64_t b) {
    if (b > numeric_limits<uint64_t>::max() - a) {
      return numeric_limits<uint64_t>::max();
    }
    return a + b;
  }

  // Mul the two numbers. If overflow then returns numeric limit.
  uint64_t MulNoOverflow(uint64_t a, uint64_t b) {
    if (a == 0) {
      return 0;
    }
    if (b > numeric_limits<uint64_t>::max() / a) {
      return numeric_limits<uint64_t>::max();
    }
    return a * b;
  }

  virtual OperatorType get_type() = 0;

  const char* get_type_string() {
    switch(get_type()) {
    case AGG_OP:
      return "AGG_OP";
    case AGG_OP_MPC:
      return "AGG_OP_MPC";
    case BLACK_BOX_OP:
      return "BLACK_BOX_OP";
    case COUNT_OP:
      return "COUNT_OP";
    case CROSS_JOIN_OP:
      return "CROSS_JOIN_OP";
    case DIFFERENCE_OP:
      return "DIFFERENCE_OP";
    case DISTINCT_OP:
      return "DISTINCT_OP";
    case DIV_OP:
      return "DIV_OP";
    case INPUT_OP:
      return "INPUT_OP";
    case INTERSECTION_OP:
      return "INTERSECTION_OP";
    case JOIN_OP:
      return "JOIN_OP";
    case MAX_OP:
      return "MAX_OP";
    case MIN_OP:
      return "MIN_OP";
    case MUL_OP:
      return "MUL_OP";
    case PROJECT_OP:
      return "PROJECT_OP";
    case SELECT_OP:
      return "SELECT_OP";
    case SORT_OP:
      return "SORT_OP";
    case SUB_OP:
      return "SUB_OP";
    case SUM_OP:
      return "SUM_OP";
    case UDF_OP:
      return "UDF_OP";
    case UNION_OP:
      return "UNION_OP";
    case WHILE_OP:
      return "WHILE_OP";
    case SELECT_OP_MPC:
      return "SELECT_OP_MPC";
    case MUL_OP_MPC:
      return "MUL_OP_MPC";
    case JOIN_OP_MPC:
      return "JOIN_OP_MPC";
    case DIV_OP_MPC:
      return "DIV_OP_MPC";
    default: {
      LOG(ERROR) << "Unknown operator type: " << get_type();
      return NULL;
    }
  }
  }

  virtual bool mapOnly() = 0;

  virtual bool hasAction() {
    return false;
  }

  virtual bool hasGroupby() {
    return has_groupby;
  }

  virtual bool isMPC() {
    return false;
  }

  virtual OperatorInterface* toMPC() {
    LOG(FATAL) << "No MPC equivalent!";
    return NULL;
  }

  virtual pair<uint64_t, uint64_t> get_output_size(
      map<string, pair<uint64_t, uint64_t> >* rel_size) = 0;

  virtual OperatorInterface* clone() = 0;

  // This is used to avoid overwritting the size estimate when the operator
  // is updated in a while loop.
  pair<uint64_t, uint64_t> UpdateIfSmaller(
      const string& output_rel, pair<uint64_t, uint64_t> new_rel_size,
      map<string, pair<uint64_t, uint64_t> >* rel_size) {
    if (rel_size->find(output_rel) != rel_size->end()) {
      pair<uint64_t, uint64_t> cur_estimated_size = (*rel_size)[output_rel];
      cur_estimated_size.first = max(cur_estimated_size.first,
                                     new_rel_size.first);
      cur_estimated_size.second = min(cur_estimated_size.second,
                                        new_rel_size.second);
      (*rel_size)[output_rel] = cur_estimated_size;
    } else {
      (*rel_size)[output_rel] = new_rel_size;
    }
    return (*rel_size)[output_rel];
  }

 private:
  bool has_groupby;
  string input_dir;
  vector<Relation*> relations;
  Relation* output_relation;
  bool rename;
  ConditionTree* condition_tree;
};

} // namespace ir
} // namespace musketeer
#endif
