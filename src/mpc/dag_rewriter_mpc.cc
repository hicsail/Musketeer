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

#include "mpc/dag_rewriter_mpc.h"
#include "ir/agg_operator.h"

namespace musketeer {
namespace mpc {
    
    DAGRewriterMPC::DAGRewriterMPC() {}

    void DAGRewriterMPC::RewriteDAG(op_nodes& dag, op_nodes* result_dag) {
        op_nodes order = op_nodes();
        TopologicalOrder(dag, &order);
        Environment obls;
        map<string, bool> mode;
        set<string> inputs;
        DetermineInputs(dag, &inputs);
        InitEnvAndMode(obls, mode, &inputs);
        DeriveObligations(order, obls, mode);
        cout << "BAH" << endl;
        cout << obls;      
    }

    bool DAGRewriterMPC::CanPass(Obligation* obl, OperatorInterface* cur) {
        // TODO(nikolaj): Implement.
        return false;
    }

    bool DAGRewriterMPC::ProcessObligation(Obligation* obl, OperatorInterface* cur,  
                                           string par_rel_name, Environment& obls) {
        if (CanPass(obl, cur)) {
            // TODO(nikolaj): Implement.
            return false;
        }
        else {
            // Block obligation by pushing it back to parent.
            // This also means that we need to enter MPC mode.
            obls.push_obligation(par_rel_name, obl);
            return true;
        }
    }

    bool DAGRewriterMPC::EmitObligation(shared_ptr<OperatorNode> node, Environment& obls) {
        OperatorType op_type = node->get_operator()->get_type();
        string rel_name = node->get_operator()->get_output_relation()->get_name();
        int num_children = node->get_children().size();
        if (op_type == AGG_OP) {
            string group_by_type = dynamic_cast<ir::AggOperator*>(node->get_operator())->get_operator();
            for (int i = 0; i < num_children; ++i) {
                obls.push_obligation(rel_name, new Obligation(op_type, group_by_type));
            }
            // Leaf special case (sort of ugly)
            if (num_children == 0) {
                obls.push_obligation(rel_name, new Obligation(op_type, group_by_type));    
            }
            return false; // just emitted an obligation so we can stay in local mode
        }
        else if (op_type == JOIN_OP) {
            obls.push_obligations(rel_name, vector<Obligation*>()); // intialize obligations
            return true;
        }
        else {
            obls.push_obligations(rel_name, vector<Obligation*>()); // intialize obligations
            return true;
        }
    }

    void DAGRewriterMPC::DeriveObligations(op_nodes& order, Environment& obls, 
                                           map<string, bool>& mpc_mode) {
        for (vector<shared_ptr<OperatorNode>>::iterator cur = order.begin(); cur != order.end(); ++cur) {
            Relation* rel = (*cur)->get_operator()->get_output_relation();
            vector<shared_ptr<OperatorNode>> parents = (*cur)->get_parents();
            if (parents.size() == 0) {
                mpc_mode[rel->get_name()] = false;
                obls.push_obligations(rel->get_name(), vector<Obligation*>());
            }
            else if (parents.size() == 1) {
                string par_name = parents[0]->get_operator()->get_output_relation()->get_name();
                
                if (mpc_mode[par_name]) {
                    // We're already in MPC mode. No need to push obligations further.
                    mpc_mode[rel->get_name()] = true;
                    continue; // make sure this applies to the outer loop
                }

                if (obls.has_obligation(par_name)) {
                    // Push or block parent obligation and update mode.
                    Obligation* par_obl = obls.pop_obligation(par_name);
                    mpc_mode[rel->get_name()] = 
                        ProcessObligation(par_obl, (*cur)->get_operator(),
                                          rel->get_name(), obls);
                }
                else {
                    // We didn't push or block obligations so check if we need to
                    // emit new obligations and update mode
                    EmitObligation(*cur, obls);
                    // mpc_mode[rel->get_name()] = EmitObligation(*cur, obls); 
                }
            }
            else if (parents.size() == 2) {
                // TODO(nikolaj): Implement.
            }
            else {
                LOG(ERROR) << "Unexpected number of parent nodes";
            }
        }
    }

    void DAGRewriterMPC::InitEnvAndMode(Environment& obls, map<string, bool>& mpc_mode,
                                        set<string>* inputs) {
        for (set<string>::iterator i = inputs->begin(); i != inputs->end(); ++i) {
            mpc_mode[(*i)] = false;
            obls.push_obligations((*i), vector<Obligation*>());
        }
    }

} // namespace mpc
} // namespace musketeer
