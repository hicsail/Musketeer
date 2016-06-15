// Copyright (c) 2015 Ionel Gog <ionel.gog@cl.cam.ac.uk>

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

#include "translation/translator_viff.h"

#include <boost/lexical_cast.hpp>
#include <ctemplate/template.h>
#include <sys/time.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>

#include "base/common.h"
#include "ir/column.h"
#include "ir/condition_tree.h"

namespace musketeer {
namespace translator {

  using ctemplate::mutable_default_template_cache;

  TranslatorViff::TranslatorViff(const op_nodes& dag,
                                 const string& class_name):
    TranslatorInterface(dag, class_name) {
  }

  string TranslatorViff::GetBinaryPath(OperatorInterface* op) {
    return op->get_code_dir() + class_name + "_code/" + class_name + ".py";
  }

  string TranslatorViff::GetSourcePath(OperatorInterface* op) {
    return op->get_code_dir() + class_name + "_code/" + class_name + ".py";
  }

  pair<string, string> TranslatorViff::GetInputPathsAndRelationsCode(
      const op_nodes& dag) {
    queue<shared_ptr<OperatorNode> > to_visit;
    set<shared_ptr<OperatorNode> > visited;
    set<string> known_rels;
    set<string> input_paths;
    set<string> input_rels;
    for (op_nodes::const_iterator it = dag.begin(); it != dag.end(); ++it) {
      to_visit.push(*it);
      visited.insert(*it);
    }
    while (!to_visit.empty()) {
      shared_ptr<OperatorNode> cur_node = to_visit.front();
      to_visit.pop();
      OperatorInterface* op = cur_node->get_operator();
      vector<Relation*> relations = op->get_relations();
      string output_relation = op->get_output_relation()->get_name();
      for (vector<Relation*>::iterator rel_it = relations.begin();
           rel_it != relations.end(); ++rel_it) {
        string rel_name = (*rel_it)->get_name();
        if (known_rels.insert(rel_name).second) {
          if (!output_relation.compare(rel_name)) {
            input_rels.insert(rel_name + "_input");
          } else {
            input_rels.insert(rel_name);
          }
          input_paths.insert(op->CreateInputPath(*rel_it));
        }
      }
      known_rels.insert(output_relation);
      if (!cur_node->IsLeaf()) {
        op_nodes children = cur_node->get_loop_children();
        op_nodes non_loop_children = cur_node->get_children();
        children.insert(children.end(), non_loop_children.begin(),
                        non_loop_children.end());
        for (op_nodes::iterator it = children.begin(); it != children.end();
             ++it) {
          if (visited.insert(*it).second) {
            to_visit.push(*it);
          }
        }
      }
    }
    string input_paths_code = "";
    for (set<string>::iterator it = input_paths.begin();
         it != input_paths.end(); ++it) {
      LOG(INFO) << "Job input: " << *it;
      input_paths_code += "FileInputFormat.addInputPath(job, new Path(\"" +
        *it + "\"));\n";
    }
    string input_rels_code = "";
    for (set<string>::iterator it = input_rels.begin(); it != input_rels.end();
         ++it) {
      input_rels_code += "String[] " + *it +
        " = value.toString().trim().split(\" \");\n";
    }
    return make_pair(input_paths_code, input_rels_code);
  }

  string TranslatorViff::GenerateCode() {
    LOG(INFO) << "Viff generate code";
    // PrintRelationOpNodes("", dag);
    // TemplateDictionary dict("job");
    // set<string> inputs = set<string>();
    // vector<Relation*> input_relations = *DetermineInputs(dag, &inputs);
    // cout << "size: " << input_relations.size() << endl;
    // for (std::vector<Relation*>::iterator i = input_relations.begin(); i != input_relations.end(); ++i) {
    //   cout << "input relation: " << (*i)->get_name() << endl;
    // }
    shared_ptr<OperatorNode> op_node = dag[0];
    OperatorInterface* op = op_node->get_operator();
    std::vector<Relation*> v = op->get_relations();
    // cout << v[0]->get_name() << endl;
    pair<string, string> code = GetInputPathsAndRelationsCode(dag);
    cout << code.first << " " << code.second << endl;

    return WriteToFiles(op, "dummy foobar");
  }

  ViffJobCode* TranslatorViff::Translate(AggOperatorSEC* op) {
    string input_path = op->get_input_paths()[0];
    return NULL;
  }

  string TranslatorViff::WriteToFiles(OperatorInterface* op, 
                                      const string& op_code) {
    ofstream job_file;
    string source_file = GetSourcePath(op);
    string path = op->get_code_dir() + op->get_output_relation()->get_name() +
      "_code/";
    string create_dir = "mkdir -p " + path;
    std::system(create_dir.c_str());
    job_file.open(source_file.c_str());
    job_file << op_code;
    job_file.close();
    return source_file;
  }

} // namespace translator
} // namespace musketeer
