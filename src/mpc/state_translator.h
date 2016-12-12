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

#ifndef MUSKETEER_MPC_STATE_TRANSLATOR_H
#define MUSKETEER_MPC_STATE_TRANSLATOR_H

#include "mpc/environment.h"
#include "base/utils.h"
#include <map>
#include <list>
#include <tuple>

namespace musketeer {
namespace mpc {

    class StateTranslator {
    public:
        StateTranslator() {
        };
        // Translates a state to a string in the dagre format and 
        // stores result in states
        void StoreAsDagre(shared_ptr<OperatorNode> current, op_nodes dag,
                          Environment& obls, map<string, bool>& mode);

        // Writes accumulated states out to file
        void WriteStatesToFile(const string& out_path);

        // Writes code source out to file
        void WriteCodeToFile(const string& code_path, const string& out_path);

    private:
        vector<string> states;

        string ToDagre(string name, string op_name, bool isMpc, bool highlight);
        string ToDagre(shared_ptr<OperatorNode> node, bool isMpc, bool highlight);

        // Translates a state and returns string representation
        string ToDagre(shared_ptr<OperatorNode> current, op_nodes dag,
                       Environment& obls, map<string, bool>& mode);
    }; 


} // namespace mpc
} // namespace musketeer

#endif
