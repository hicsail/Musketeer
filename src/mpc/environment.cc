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
    
    void Environment::init_for(string rel_name) {
        if (env.find(rel_name) == env.end()) {
            env[rel_name];
        }
    }

    void Environment::push_obligation(string rel_name, Obligation* obl) {
        if (env.find(rel_name) == env.end()) {
            env[rel_name];
        }
        env[rel_name].push_back(obl);
        cout << env[rel_name].size() << endl;
    }

    bool Environment::has_obligation(string rel_name) {
        return !env[rel_name].empty();
    }
    
    Obligation* Environment::pop_obligation(string rel_name) {
        if (!has_obligation(rel_name)) {
            LOG(ERROR) << "Tried to pop non-existent obligation.";
            return NULL;
        }
        Obligation* obl = env[rel_name].front();
        env[rel_name].pop_front();
        return obl;
    }

    string Environment::toJSON() {
        string json = "{\n";

        for (map<string, list<Obligation*>>::const_iterator it = env.begin(); it != env.end(); ++it) {
            for (list<Obligation*>::const_iterator o = (it->second).begin(); o != (it->second).end(); ++o) {
                json += "'" + it->first + "' : '" + (*o)->get_name() + "',\n";
            }
        }
        json += "}\n";

        return json;
    }

    std::ostream& operator<<(std::ostream& _stream, Environment const& env) { 
        _stream << "Env()" << "\n";
        for (map<string, list<Obligation*>>::const_iterator it = env.env.begin(); it != env.env.end(); ++it) {
            _stream << it->first << " {\n";
            for (list<Obligation*>::const_iterator o = (it->second).begin(); o != (it->second).end(); ++o) {
                if ((*o)->get_blocked_by()) {
                    string blocker_name = (*o)->get_blocked_by()->get_operator()->get_output_relation()->get_name();
                    _stream << "\t" << (*o)->get_name() << blocker_name << "\n";                        
                }
                else {
                    _stream << "\t" << (*o)->get_name() << " " << "no blocker" << "\n";    
                }
            }
            _stream << "}\n";
        }
        return _stream;
    };

} // namespace mpc
} // namespace musketeer
