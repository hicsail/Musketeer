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

#ifndef MUSKETEER_TRANSLATOR_VIFF_H
#define MUSKETEER_TRANSLATOR_VIFF_H

#include "translation/translator_interface.h"

#include <ctemplate/template.h>

#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "base/common.h"
#include "ir/column.h"
#include "translation/viff_job_code.h"

namespace musketeer {
namespace translator {

using ctemplate::TemplateDictionary;
using namespace musketeer::ir; // NOLINT

class TranslatorViff : public TranslatorInterface {
 public:
  TranslatorViff(const op_nodes& dag, const string& class_name);
  string GenerateCode();
  static map<string, Relation*> relations;

  
 private:
  string TranslateHeader();
  string TranslateGatherLeaves(set<shared_ptr<OperatorNode>> leaves);
  string TranslateMakeShares(set<pair<Relation*, string>> input_rels_paths);
  string TranslateProtocolInput(set<pair<Relation*, string>> input_rels_paths);
  string TranslateDataTransfer();
  string TranslateStoreLeaves(set<shared_ptr<OperatorNode>> leaves);
  set<pair<Relation*, string>> GetInputRelsAndPaths(const op_nodes& dag);
  vector<Relation*>* DetermineInputsSpark(const op_nodes& dag,
                                                           set<string>* inputs,
                                                           set<string>* visited);
  bool CanSchedule(OperatorInterface* op, set<string>* processed);
  void TranslateDAG(string* code, const op_nodes& next_set,
                    set<shared_ptr<OperatorNode> >* leaves,
                    set<string>* processed);
  string GetBinaryPath(OperatorInterface* op);
  string GetSourcePath(OperatorInterface* op);
  string WriteToFiles(OperatorInterface* op, const string& op_code);
  
  ViffJobCode* Translate(AggOperatorSEC* op);
  ViffJobCode* Translate(SelectOperatorSEC* op);
  ViffJobCode* Translate(MulOperatorSEC* op);
  ViffJobCode* Translate(DivOperatorSEC* op);
  ViffJobCode* Translate(JoinOperatorSEC* op);

  ViffJobCode* TranslateMathOp(OperatorInterface* op, vector<Value*> values,
                               ConditionTree* condition_tree, string math_op);
  
  string GenerateLambda(const string& op,
                        Relation* rel, Value* left_val,
                        Value* right_val, Relation* output_rel);
  string GenerateColumnTypes(Relation* rel);
  string GenerateAggSECOp(const string& op);
};

} // namespace translator
} // namespace musketeer
#endif
