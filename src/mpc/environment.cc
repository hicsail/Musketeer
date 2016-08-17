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

#include "mpc/environment.h"

namespace musketeer {
namespace mpc {
    
    void Environment::push_obligation(string rel_name, Obligation* obl) {
        if (env.find(rel_name) == env.end()) {
            env[rel_name] = vector<Obligation*>();
        }
        env[rel_name].push_back(obl);
    }

    void Environment::push_obligations(string rel_name, vector<Obligation*> obls) {
        if (env.find(rel_name) == env.end()) {
            env[rel_name] = vector<Obligation*>();
        }
        extend(env[rel_name], obls);
    }

    bool Environment::has_obligation(string rel_name) {
        return !env[rel_name].empty();
    }
    
    Obligation* Environment::pop_obligation(string rel_name) {
        if (!has_obligation(rel_name)) {
            LOG(ERROR) << "Tried to pop non-existent obligation.";
            return NULL;
        }
        Obligation* obl = env[rel_name].back();
        env[rel_name].pop_back();
        return obl;
    }

    // TODO(nikolaj): make this a macro and move into utils
    // extend left with right
    void Environment::extend(vector<Obligation*>& left, vector<Obligation*>& right) {
        left.reserve(left.size() + distance(right.begin(),
                     right.end()));
        left.insert(left.end(), right.begin(), right.end());
    }


} // namespace mpc
} // namespace musketeer
