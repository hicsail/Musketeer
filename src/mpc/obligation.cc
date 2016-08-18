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

#include "mpc/obligation.h"

namespace musketeer {
namespace mpc {

    OperatorInterface* Obligation::get_operator() {
        return op;
    }

    void Obligation::PassThrough(shared_ptr<OperatorNode> op_node) {
        // OperatorInterface* _op = op_node->get_operator();
        // vector<Relation*> input_rels;
        // Relation* par_out_rel = _op->get_output_relation();
        // input_rels.push_back(par_out_rel);
        // op->set_relations(input_rels);
        // // TODO(nikolaj): Also need to handle operator-specific columns and fields (e.g., group_by_cols)

        // op_nodes children = op_node->get_children();
        // for (op_nodes::iterator i = children.begin(); i != children.end(); ++i) {
        //     vector<Relation*> child_rels;
        //     for (vector<Relation*>::iterator r = .begin(); i != .end(); ++i)
        //     {
                
        //     }
        // }

    };      

} // namespace mpc
} // namespace musketeer
