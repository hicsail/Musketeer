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

#include "frameworks/viff_framework.h"

#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// #define VIFF_START_TIME 15.0
// #define NUM_VIFF_MACHINES 100

namespace musketeer {
namespace framework {

  using musketeer::translator::TranslatorViff;
  // using musketeer::monitor::ViffMonitor;

  ViffFramework::ViffFramework(): FrameworkInterface() {
    dispatcher_ = new ViffDispatcher();
    monitor_ = NULL;
  }

  string ViffFramework::Translate(const op_nodes& dag,
                                  const string& relation) {
    translator::TranslatorViff translator_viff =
      translator::TranslatorViff(dag, relation);
    return translator_viff.GenerateCode();
  }

  void ViffFramework::Dispatch(const string& binary_file,
                               const string& relation) {
    if (!FLAGS_dry_run) {
      dispatcher_->Execute(binary_file, relation);
    }
  }

  FmwType ViffFramework::GetType() {
    return FMW_VIFF;
  }

  double ViffFramework::ScoreClusterState() {
    return 0.0;
  }


  uint32_t ViffFramework::ScoreDAG(const node_list& nodes,
                                   const relation_size& rel_size) {
    node_set to_schedule;
    int32_t num_ops_to_schedule = 0;
    op_nodes input_nodes;
    input_nodes.push_back(nodes.front());
    for (node_list::const_iterator it = nodes.begin(); it != nodes.end();
         ++it) {
      to_schedule.insert(*it);
      num_ops_to_schedule++;
    }
    if (CanMerge(input_nodes, to_schedule, num_ops_to_schedule)) {
      // LOG(INFO) << "Can merge in " << FrameworkToString(GetType());
      VLOG(2) << "Can merge in " << FrameworkToString(GetType());
      set<string> input_names;
      uint64_t input_data_size = GetDataSize(
          *DetermineInputs(input_nodes, &input_names), rel_size);
      
      return min(static_cast<double>(FLAGS_max_scheduler_cost),
                 ScoreRuntime(input_data_size, nodes, rel_size));
      
    } else {
      VLOG(2) << "Cannot merge in " << FrameworkToString(GetType());
      return numeric_limits<uint32_t>::max();
    }
  }

  double ViffFramework::ScoreOperator(shared_ptr<OperatorNode> op_node, const relation_size& rel_size) {
    OperatorInterface* op = op_node->get_operator();
    if (op->isMPC()) {
      cout << "Secure operator detected." << endl;
      return 1.0; 
    }
    else { 
      cout << "Non-secure operator detected." << endl;
      return FLAGS_max_scheduler_cost;
    }
  }

  double ViffFramework::ScoreCompile() {
    return 0.0;
  }

  double ViffFramework::ScorePull(uint64_t data_size_kb) {
    return 0.0;
  }

  double ViffFramework::ScoreLoad(uint64_t data_size_kb) {
    return 0.0;
  }

  double ViffFramework::ScoreRuntime(uint64_t data_size_kb,
                                     const node_list& nodes,
                                     const relation_size& rel_size) {
    double cur_cost = 0;
    for (node_list::const_iterator it = nodes.begin(); it != nodes.end();
         ++it) {
      double op_cost = ScoreOperator((*it), rel_size);
      cur_cost += op_cost;
    }
    return cur_cost;
  }

  double ViffFramework::ScorePush(uint64_t data_size_kb) {
    return 0.0;
  }

  bool ViffFramework::CanMerge(const op_nodes& dag,
                               const node_set& to_schedule,
                               int32_t num_ops_to_schedule) {
    // make sure all operators are secure operators (no mixing operators for now)
    for (node_set::const_iterator it = to_schedule.begin();
         it != to_schedule.end(); ++it) {
      if (!(*it)->get_operator()->isMPC()) {
        return false;
      }
    }
    return true;
  }

} // namespace framework
} // namespace musketeer
