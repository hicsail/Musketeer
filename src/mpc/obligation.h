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
#include "ir/aggregation.h"
#include "ir/relation.h"
#include "ir/select_operator.h"
#include "ir/mul_operator.h"
#include "ir/union_operator.h"
#include "frontends/operator_node.h"

#include <boost/lexical_cast.hpp>
#include <map>

using namespace musketeer::ir;

namespace musketeer {
namespace mpc {

    // TODO(nikolaj): Make an obligation class hiearchy
    class Obligation {
    public:
        Obligation(shared_ptr<OperatorNode> op_node, int index);
        ~Obligation() {
            // TODO(nikolaj): Implement.
            delete op;
        }

        OperatorInterface* get_operator();
        GroupByType get_group_by_type();
        string get_name();
        
        // Must call this method when pushing the obligation through a node.
        // Relations, columns, etc. on op will get updated.
        void PassThrough(shared_ptr<OperatorNode> op_node);
        // other_obl is null for unary operators
        bool CanPassOperator(OperatorInterface* other_op, Obligation* other_obl);
        
        bool CanPass(SelectOperator* other);
        bool CanPass(MulOperator* other);
        bool CanPass(UnionOperator* other_op, Obligation* other_obl);

    private:
        Aggregation* op;
        GroupByType type;

        void update_op_columns(OperatorInterface* parent);
        bool CanMerge(Obligation& other);
    }; 

} // namespace mpc
} // namespace musketeer

#endif
