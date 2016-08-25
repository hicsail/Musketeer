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

#include "ir/aggregation.h"

#include <limits>
#include <map>
#include <utility>
#include <vector>

namespace musketeer {
namespace ir {

  GroupByType Aggregation::type_lookup(string operator_) {
    if (operator_ == "+") {
      return PLUS_GROUP;
    }
    else if (operator_ == "-") {
      return MINUS_GROUP;
    }
    else if (operator_ == "*") {
      return DIVIDE_GROUP;
    }
    else if (operator_ == "/") {
      return MULTIPLY_GROUP;
    }
    else {
      LOG(FATAL) << "Unknown group by operator: " << operator_;
      return PLUS_GROUP; // default 
    }
  }

  Aggregation::~Aggregation() {}

  void Aggregation::set_group_bys(vector<Column*> group_bys_) {
    group_bys = group_bys_;
  }
  
  void Aggregation::set_columns(vector<Column*> columns_) {
    columns = columns_;
  }
  
  vector<Column*> Aggregation::get_group_bys() {
    return group_bys;
  }

  GroupByType Aggregation::get_group_by_type() {
    return group_by_type;
  }

  vector<Column*> Aggregation::get_columns() {
    return columns;
  }

  bool Aggregation::hasGroupby() {
    if ((group_bys.size() == 1 && group_bys[0]->get_relation().empty()) ||
        group_bys.size() == 0) {
      return false;
    } else {
      return true;
    }
  }

  pair<uint64_t, uint64_t> Aggregation::get_output_size(
      map<string, pair<uint64_t, uint64_t> >* rel_size) {
    vector<Relation*> rels = get_relations();
    string input_rel = rels[0]->get_name();
    pair<uint64_t, uint64_t> agg_rel_size;
    if (rel_size->find(input_rel) != rel_size->end()) {
      agg_rel_size = make_pair(1, (*rel_size)[input_rel].second);
    } else {
      // This should not happen.
      LOG(INFO) << "Called out of order";
      agg_rel_size = make_pair(1, numeric_limits<uint64_t>::max());
    }
     return UpdateIfSmaller(get_output_relation()->get_name(), agg_rel_size,
                           rel_size);
  }

  bool Aggregation::mapOnly() {
    return false;
  }

  bool Aggregation::hasAction() {
    return !hasGroupby();
  }

  void Aggregation::update_columns() {
    string relation = get_relations()[0]->get_name();
    vector<Column*> cols = get_columns();
    for (vector<Column*>::iterator it = cols.begin(); it != cols.end();
         ++it) {
      (*it)->set_relation(relation);
    }
    vector<Column*> group_bys = get_group_bys();
    for (vector<Column*>::iterator it = group_bys.begin();
         it != group_bys.end(); ++it) {
      (*it)->set_relation(relation);
    }
  }

} // namespace ir
} // namespace musketeer
