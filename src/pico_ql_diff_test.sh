#!/bin/sh
#
#   Compare the current test results with the successful 
#   prototype and print the outcome.
#
#
#   Copyright 2012 Marios Fragkoulis
#
#   Licensed under the Apache License, Version 2.0
#   (the "License");you may not use this file except in
#   compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in
#   writing, software distributed under the License is
#   distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
#   express or implied.
#   See the License for the specific language governing
#   permissions and limitations under the License.

if diff pico_ql_test_current.txt pico_ql_test_success.txt >/dev/null 2>&1
then 
    echo "Test successful." > pico_ql_test_output.txt
else
    echo "Test produced unexpected results. See test_current.txt ." > pico_ql_test_output.txt
fi
