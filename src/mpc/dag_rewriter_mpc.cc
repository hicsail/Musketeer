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
#include <queue>
#include <algorithm>

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
        RewriteDAG(dag, obls, mode, result_dag);
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
            LOG(INFO) << rel_name << " is an AGG_OP. Emitting obligation.";
            for (int i = 0; i < num_children; ++i) {
                obls.push_obligation(rel_name, new Obligation(node, i));
            }
            // Leaf special case (sort of ugly)
            if (num_children == 0) {
                obls.push_obligation(rel_name, new Obligation(node, 0));    
            }
            return false; // just emitted an obligation so we can stay in local mode
        }
        else if (op_type == JOIN_OP) {
            obls.init_for(rel_name);
            return true;
        }
        else {
            obls.init_for(rel_name);
            return false;
        }
    }

    void DAGRewriterMPC::DeriveObligations(op_nodes& order, Environment& obls, 
                                           map<string, bool>& mpc_mode) {
        for (vector<shared_ptr<OperatorNode>>::iterator cur = order.begin(); cur != order.end(); ++cur) {
            Relation* rel = (*cur)->get_operator()->get_output_relation();
            LOG(INFO) << "Deriving obligations for " << rel->get_name();
            vector<shared_ptr<OperatorNode>> parents = (*cur)->get_parents();
            if (parents.size() == 0) {
                mpc_mode[rel->get_name()] = false;
                obls.init_for(rel->get_name());
            }
            else if (parents.size() == 1) {
                string par_name = parents[0]->get_operator()->get_output_relation()->get_name();
                LOG(INFO) << "Found parent: " << par_name;

                if (mpc_mode[par_name]) {
                    // We're already in MPC mode. No need to push obligations further.
                    mpc_mode[rel->get_name()] = true;
                    continue;
                }

                if (obls.has_obligation(par_name)) {
                    // Push or block parent obligation and update mode.
                    Obligation* par_obl = obls.pop_obligation(par_name);
                    mpc_mode[rel->get_name()] = 
                        ProcessObligation(par_obl, (*cur)->get_operator(),
                                          par_name, obls);
                }
                else {
                    // We didn't push or block obligations so check if we need to
                    // emit new obligations and update mode
                    mpc_mode[rel->get_name()] = EmitObligation(*cur, obls); 
                }
            }
            else if (parents.size() == 2) {
                // TODO(nikolaj): Implement.
                LOG(ERROR) << "Unsupported number of parent nodes";
            }
            else {
                LOG(FATAL) << "Unexpected number of parent nodes";
            }
        }
    }

    void DAGRewriterMPC::InitEnvAndMode(Environment& obls, map<string, bool>& mpc_mode,
                                        set<string>* inputs) {
        for (set<string>::iterator i = inputs->begin(); i != inputs->end(); ++i) {
            mpc_mode[(*i)] = false;
            obls.init_for((*i));
        }
    }

    void DAGRewriterMPC::InsertNode(shared_ptr<OperatorNode> at_node, 
                                    shared_ptr<OperatorNode> new_node) {
        op_nodes at_children = at_node->get_children();
        OperatorInterface* at_op = at_node->get_operator();
        Relation* at_rel = at_op->get_output_relation();

        OperatorInterface* new_op = new_node->get_operator();
        Relation* new_rel = new_op->get_output_relation();

        LOG(INFO) << "Inserting obligation node: " << new_rel->get_name();

        new_node->set_children(at_children);
        
        op_nodes wrapper;
        wrapper.push_back(new_node);
        at_node->set_children(wrapper);

        for (vector<shared_ptr<OperatorNode>>::iterator c = at_children.begin();
             c != at_children.end(); ++c) {
            // Disconnect each child from at_node, connect to new_node
            op_nodes child_parents = (*c)->get_parents();
            child_parents.erase(remove(child_parents.begin(), child_parents.end(), at_node), 
                                child_parents.end());
            child_parents.push_back(new_node);
            
            // Update the relations attached to the operator on the child node
            OperatorInterface* child_op = (*c)->get_operator();
            vector<Relation*> child_rels = child_op->get_relations();
            vector<Relation*> updated_rels;

            for (vector<Relation*>::iterator r = child_rels.begin(); 
                 r != child_rels.end(); ++r) {
                if (*r != at_rel) {
                    updated_rels.push_back(*r);
                }
                else {
                    updated_rels.push_back(new_rel);    
                }
            }
            child_op->set_relations(updated_rels);
            child_op->update_columns();
        }

    }

    void DAGRewriterMPC::RewriteDAG(op_nodes& dag, Environment& obls, 
                                    map<string, bool>& mpc_mode,
                                    op_nodes* result_dag) {
        set<shared_ptr<OperatorNode> > visited;
        queue<shared_ptr<OperatorNode> > to_visit;
        for (op_nodes::iterator it = dag.begin(); it != dag.end(); ++it) {
            to_visit.push(*it);
            visited.insert(*it);
        }

        while (!to_visit.empty()) {
            shared_ptr<OperatorNode> cur_node = to_visit.front();
            string rel_name = cur_node->get_operator()->get_output_relation()->get_name();
            to_visit.pop();

            if (!cur_node->IsLeaf()) {
                op_nodes children = cur_node->get_children();
                
                for (op_nodes::iterator it = children.begin(); it != children.end();
                     ++it) {
                    if (visited.insert(*it).second) {
                        to_visit.push(*it);
                    }
                }
            }

            // Note that the conditions below are mutually exclusive (by design)
            if (mpc_mode[rel_name]) {
                OperatorInterface* mpc_op = cur_node->get_operator()->toMPC();
                cur_node->replace_operator(mpc_op);   
            }
            else if (obls.has_obligation(rel_name)) {
                OperatorInterface* obl_op = obls.pop_obligation(rel_name)->get_operator();
                InsertNode(cur_node, make_shared<OperatorNode>(obl_op));
            }
        }        
    }

} // namespace mpc
} // namespace musketeer