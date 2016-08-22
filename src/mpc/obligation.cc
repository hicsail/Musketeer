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

    }

    bool Obligation::CanPassOperator(OperatorInterface* other) {
        switch (other->get_type()) {
        case SELECT_OP:
            return CanPass(dynamic_cast<SelectOperator*>(other));
        default:
            return false;
        }
    }

    bool Obligation::CanPass(SelectOperator* other) {
        return false;
    }

} // namespace mpc
} // namespace musketeer