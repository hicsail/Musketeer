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

#include "base/utils.h"
#include "base/common.h"
#include "mpc/obligation.h"
#include <map>

namespace musketeer {
namespace mpc {

    class Environment {
    public:
        Environment() {
        };
        
        // TODO(nikolaj): Implement destructor

        // Push back single obligation to rel_name
        void push_obligation(string rel_name, Obligation* obl);
        // Push back obligations to rel_name
        void push_obligations(string rel_name, vector<Obligation*> obls);
        // Determine if there are any obligations on rel_name
        bool has_obligation(string rel_name);
        // Returns last obligation on rel_name
        // Returns NULL if no obligation to pop
        Obligation* pop_obligation(string rel_name);

        friend std::ostream& operator<<(std::ostream& _stream, Environment const& env) { 
            _stream << "Env()" << "\n";
            for (map<string, vector<Obligation*>>::const_iterator it = env.env.begin(); it != env.env.end(); ++it) {
                _stream << it->first << " {\n";
                for (vector<Obligation*>::const_iterator o = (it->second).begin(); o != (it->second).end(); ++o) {
                    _stream << "\t" << (*o)->get_group_by_type() << "\n";
                }
                _stream << "}\n";
            }
            return _stream;
        };
    
    private:
        map<string, vector<Obligation*>> env; // queue instead of vector would be more natural

        void extend(vector<Obligation*>& left, vector<Obligation*>& right);
    }; 


} // namespace mpc
} // namespace musketeer

#endif
