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

#ifndef MUSKETEER_DUMMY_OPERATOR_H
#define MUSKETEER_DUMMY_OPERATOR_H

#include "ir/operator_interface.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace musketeer {
namespace ir {

class DummyOperator : public OperatorInterface {
 public:
  DummyOperator(const string& input_dir, const vector<Relation*>& relations,
                Relation* output_rel):
    OperatorInterface(input_dir, relations, output_rel) {
  }

  OperatorType get_type();
  bool mapOnly();
  bool isMPC();
  pair<uint64_t, uint64_t> get_output_size(
      map<string, pair<uint64_t, uint64_t> >* rel_size);
  OperatorInterface* clone();
};

} // namespace ir
} // namespace musketeer
#endif
