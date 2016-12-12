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

#include "ir/relation.h"

#include <algorithm>
#include <string>

namespace musketeer {

  string Relation::get_name() {
    return name;
  }

  void Relation::set_name(string name_) {
    name = name_;
  }

  vector<Column*> Relation::get_columns() {
    return columns;
  }

  void Relation::set_immutable(bool imm) {
    immutable = imm;
  }

  bool Relation::isImmutable() {
    return immutable;
  }

  Relation* Relation::copy(string new_name) {
    return new Relation(new_name, columns, owners);
  }

  string Relation::get_owner_string() {
    string owner_str = "";
    for (set<Owner*>::iterator i = owners.begin(); 
         i != owners.end(); ++i) {
        owner_str += (*i)->get_name() + " ";
    }
    if (owner_str == "") {
      return "EMPTY";
    }
    return owner_str;
  }

  bool Relation::has_owner(string owner_id) {
    // TODO: shouldn't iterate over set. Instead override owner equality function
    // and use contains
    for (std::set<Owner*>::iterator i = owners.begin(); i != owners.end(); ++i) {
      if ((*i)->get_name() == owner_id) {
        return true;
      }
    }
    return false;
  }

  set<Owner*> Relation::get_owners() {
    return owners;
  }

  void Relation::add_owner(Owner* owner) {
    owners.insert(owner);
  }

  void Relation::add_owners(set<Owner*> owners_) {
    for (set<Owner*>::iterator i = owners_.begin(); i != owners_.end(); ++i) {
      owners.insert(*i);
    }
  }

  bool Relation::isShared() {
    return (owners.size() > 1);    
  }

} // namespace musketeer
