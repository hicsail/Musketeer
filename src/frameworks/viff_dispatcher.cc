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

#include "frameworks/viff_dispatcher.h"

#include <cstdlib>
#include <iostream>
#include <string>

#include "base/common.h"

namespace musketeer {
namespace framework {

  ViffDispatcher::ViffDispatcher() {
  }

  void ViffDispatcher::Execute(string job_path, string job_options) {
    // XXX(malte): hack hack hack
    string cmd = "python " + job_path + " " + job_options;
    LOG(INFO) << "viff run started for: " << job_path;
    system(cmd.c_str());
    LOG(INFO) << "viff run ended for: " << job_path;
  }

} // namespace framework
} // namespace musketeer
