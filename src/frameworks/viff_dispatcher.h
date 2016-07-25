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

#ifndef MUSKETEER_VIFF_DISPATCHER_H
#define MUSKETEER_VIFF_DISPATCHER_H

#include "frameworks/dispatcher_interface.h"

#include <string>

#include "base/common.h"

namespace musketeer {
namespace framework {

class ViffDispatcher : public DispatcherInterface {
 public:
  ViffDispatcher();
  void Execute(string job_path, string job_options);
};

} // namespace framework
} // namespace musketeer
#endif
