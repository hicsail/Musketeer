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

#ifndef MUSKETEER_OBLIGATION_H
#define MUSKETEER_OBLIGATION_H

#include "base/utils.h"
#include "base/common.h"
#include "ir/operator_interface.h"
#include "frontends/operator_node.h"
#include "ir/relation.h"

#include <boost/lexical_cast.hpp>
#include <map>

namespace musketeer {
namespace mpc {

    // this should really be a class hiearchy
    class Obligation {
    public:
        Obligation(shared_ptr<OperatorNode> op_node, int index) {
            OperatorInterface* _op = op_node->get_operator();
            op = _op->toMPC();
            string rel_name = _op->get_output_relation()->get_name();
            Relation* new_rel = _op->get_output_relation()->copy(rel_name + "_obl_" + boost::lexical_cast<string>(index));
            op->set_output_relation(new_rel);
            // PassThrough(op_node);
        };
        
        OperatorInterface* get_operator();
        void PassThrough(shared_ptr<OperatorNode> op_node);

        friend std::ostream& operator<<(std::ostream& _stream, Obligation const& obl) { 
            // TODO(nikolaj): Implement.
            _stream << "Obligation(" << ")";
            return _stream;
        };
        
    private:
        OperatorInterface* op;
    }; 

} // namespace mpc
} // namespace musketeer

#endif
