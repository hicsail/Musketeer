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

 private:
  pair<string, string> GetInputPathsAndRelationsCode(const op_nodes& dag);
  ViffJobCode* Translate(AggOperatorSEC* op);
  string GetBinaryPath(OperatorInterface* op);
  string GetSourcePath(OperatorInterface* op);
  string WriteToFiles(OperatorInterface* op, const string& op_code);
};

} // namespace translator
} // namespace musketeer
#endif
