// Copyright (c) 2016 Nikolaj Volgushev <nikolaj@bu.edu>

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

#ifndef MUSKETEER_MPC_COMPILER_H
#define MUSKETEER_MPC_COMPILER_H

#include "base/utils.h"
#include "base/common.h"
#include "mpc/obligation.h"
#include "mpc/environment.h"
#include "frontends/operator_node.h"
#include "ir/operator_interface.h"

#include <map>

namespace musketeer {
namespace mpc {

	// TODO(nikolaj): add check for while loops. loops are currently NOT supported
    class DAGRewriterMPC {
    public:
        DAGRewriterMPC();
        void RewriteDAG(op_nodes& dag, op_nodes* result_dag);
    
    private:

        bool ProcessObligation(Obligation* obl, shared_ptr<OperatorNode> cur, 
                               string par_rel_name, Environment& obls);
        bool ProcessObligation(Obligation* left_obl, Obligation* right_obl,
                                shared_ptr<OperatorNode> cur, string left_name,
                                string right_name, Environment& obls);
        bool EmitObligation(shared_ptr<OperatorNode> node, Environment& obls);
        void DeriveObligations(op_nodes& order, Environment& obls, 
                               map<string, bool>& mpc_mode);
        void InitEnvAndMode(Environment& obls, map<string, bool>& mpc_mode,
                            set<string>* inputs);
        void RewriteDAG(op_nodes& dag, Environment& obls, map<string, bool>& mpc_mode,
                        op_nodes* result_dag);
        void InsertNode(shared_ptr<OperatorNode> at_node, 
                        shared_ptr<OperatorNode> new_node);        
    }; 
    
} // namespace mpc
} // namespace musketeer

#endif
