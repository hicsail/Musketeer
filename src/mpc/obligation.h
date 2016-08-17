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
#include <map>

namespace musketeer {
namespace mpc {

    // this should really be a class hiearchy
    class Obligation {
    public:
        Obligation(OperatorType op_type_, string group_by_type_):
            op_type(op_type_), group_by_type(group_by_type_) {
            };
        OperatorType get_op_type() const;
        string get_group_by_type() const;

        friend std::ostream& operator<<(std::ostream& _stream, Obligation const& obl) { 
            _stream << "Obligation(" << obl.get_op_type() << ", " << obl.get_group_by_type() << ")";
            return _stream;
        };
        
    private:
        OperatorType op_type;
        string group_by_type;
    }; 

} // namespace mpc
} // namespace musketeer

#endif
