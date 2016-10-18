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

#include "mpc/state_translator.h"
#include <queue>

namespace musketeer {
namespace mpc {
    
    void StateTranslator::StoreAsDagre(shared_ptr<OperatorNode> current, op_nodes dag,
                                       Environment& obls, map<string, bool>& mode) {
        states.push_back(ToDagre(current, dag, obls, mode));
    }

    void StateTranslator::WriteStatesToFile(const string& out_path) {
        // TODO(nikolaj): the code below doesn't belong here
        string entire_json = "[";
        for (vector<string>::iterator i = states.begin(); i != states.end(); ++i) {
            entire_json += *i;
            if (i != states.end() - 1) {
                entire_json += ", "; 
            }
        }
        entire_json += "]";

        ofstream result_file(out_path);
        result_file << entire_json;
        result_file.close();
    }

    string StateTranslator::ToDagre(string name, string op_name, bool isMpc, bool highlight) {
        string mpc_str = isMpc ? "true" : "false";
        string highlight_str = highlight ? "true" : "false";
        return "{ \"id\" : \"" + name + 
               "\", \"label\" : \"" + op_name + 
               "\", \"mpc\" : " + mpc_str +
               ", \"highlight\" : " + highlight_str +
               " }";
    }

    string StateTranslator::ToDagre(shared_ptr<OperatorNode> node, bool isMpc, bool highlight) {
        string name = node->get_operator()->get_output_relation()->get_name();
        string op_name = node->get_operator()->get_type_string();
        return ToDagre(name, op_name, isMpc, highlight); 
    }

    string StateTranslator::ToDagre(shared_ptr<OperatorNode> current, op_nodes dag,
                                    Environment& obls, map<string, bool>& mode) {
        set<shared_ptr<OperatorNode> > visited;
        queue<shared_ptr<OperatorNode> > to_visit;
        vector<string> nodes;
        vector<string> edges;
        
        for (op_nodes::iterator it = dag.begin(); it != dag.end(); ++it) {
            to_visit.push(*it);
            visited.insert(*it);
        }

        while (!to_visit.empty()) {
            shared_ptr<OperatorNode> cur_node = to_visit.front();
            string cur_name = cur_node->get_operator()->get_output_relation()->get_name();
            string cur_op_name = cur_node->get_operator()->get_type_string();
            to_visit.pop();
            
            nodes.push_back(ToDagre(cur_node, mode[cur_name], cur_node == current));

            queue<mpc::Obligation*> non_blocked;
            map<string, mpc::Obligation*> obl_lookup;
            list<Obligation*> current_obls = obls.obligations_for(cur_name);

            for (list<mpc::Obligation*>::iterator o = current_obls.begin(); o != current_obls.end(); ++o) {
                mpc::Obligation* obl = *o;
                shared_ptr<OperatorNode> blocker = obl->get_blocked_by();
            
                if (blocker) {
                  string blocker_name = blocker->get_operator()->get_output_relation()->get_name();
                  obl_lookup[blocker_name] = obl;
                }
                else {
                  non_blocked.push(obl);
                }
            }

            if (!cur_node->IsLeaf()) {

                op_nodes children = cur_node->get_loop_children();
                op_nodes non_loop_children = cur_node->get_children();
                children.insert(children.end(), non_loop_children.begin(),
                                non_loop_children.end());
                for (op_nodes::iterator it = children.begin(); it != children.end(); ++it) {
                    string child_name = (*it)->get_operator()->get_output_relation()->get_name();
                    string child_op_name = (*it)->get_operator()->get_type_string();
                    mpc::Obligation* obl = obl_lookup[child_name];
                    if (!obl) {
                        if (!non_blocked.empty()) {
                            obl = non_blocked.front();
                            non_blocked.pop();
                        }
                    }
                  
                    if (!obl) {
                        edges.push_back("{ \"from\": \"" + cur_name + "\", \"to\": \"" + child_name + "\"}");
                    }
                    else {
                        string obl_op_name = obl->get_operator()->get_type_string();
                        nodes.push_back(ToDagre(obl->get_name(), 
                            obl_op_name.substr(0, obl_op_name.length() - 4), true, false));
                        edges.push_back("{ \"from\": \"" + cur_name + "\", \"to\": \"" + obl->get_name() + "\" }");
                        edges.push_back("{ \"from\": \"" + obl->get_name() + "\", \"to\": \"" + child_name + "\" }");
                    }

                    if (visited.insert(*it).second) {
                        to_visit.push(*it);
                    }
                }
            }
        }
        
        string node_string = " \"nodes\" : [";
        for (vector<string>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
            node_string += *i;
            if (i != nodes.end() - 1) {
                node_string += ", "; 
            }
        }
        node_string += " ] ";

        string edge_string = " \"edges\" : [";
        for (vector<string>::iterator i = edges.begin(); i != edges.end(); ++i) {
            edge_string += *i;
            if (i != edges.end() - 1) {
                edge_string += ", "; 
            }    
        }
        edge_string += " ] ";
        return "{ " + node_string + ", " + edge_string + " }";
    }    

} // namespace mpc
} // namespace musketeer
