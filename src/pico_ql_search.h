/*
 *   Declare the user interface to pico_ql_search.cpp.
 *
 *   Copyright 2012 Marios Fragkoulis
 *
 *   Licensed under the Apache License, Version 2.0
 *   (the "License");you may not use this file except in
 *   compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in
 *   writing, software distributed under the License is
 *   distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *   express or implied.
 *   See the License for the specific language governing
 *   permissions and limitations under the License.
 */

#ifndef PICO_QL_SEARCH_H
#define PICO_QL_SEARCH_H

#ifdef __cplusplus
namespace picoQL {
  extern "C" {
#endif

  int pico_ql_serve(int port_number);
  void pico_ql_register(const void *collection, const char * col_name);

#ifdef __cplusplus
  }
}
#endif

#endif
