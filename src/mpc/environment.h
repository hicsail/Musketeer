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

#ifndef MUSKETEER_MPC_ENVIRONMENT_H
#define MUSKETEER_MPC_ENVIRONMENT_H

#include "mpc/obligation.h"
#include <map>
#include <list>

namespace musketeer {
namespace mpc {

    class Environment {
    public:
        Environment() {
        };
        
        // TODO(nikolaj): Destructor

        // Initialize empty entry under rel_name
        void init_for(const string& rel_name);
        // Push back single obligation to rel_name
        void push_obligation(const string& rel_name, Obligation* obl);
        // Determine if there are any obligations on rel_name
        bool has_obligation(const string& rel_name);
        // Returns last obligation on rel_name
        // Returns NULL if no obligation to pop
        Obligation* pop_obligation(const string& rel_name);
        // Returns all obligations for rel_name
        // Empty list if none
        list<Obligation*> obligations_for(const string& rel_name);

        friend std::ostream& operator<<(std::ostream& _stream, Environment const& env);
        
    private:
        map<string, list<Obligation*>> env;
    }; 


} // namespace mpc
} // namespace musketeer

#endif
