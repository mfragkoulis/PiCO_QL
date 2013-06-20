#!/bin/sh

echo "\n*Testing examples in separate execution thread.*\n"

cd BankApp
echo "In BankApp..."
echo "-> Generating files."
make prep > /dev/null
if ! ruby pico_ql_generator.rb pico_ql_dsl.sql
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Building."
make clean > /dev/null
if ! make PICO_QL_JOIN_THREADS=1 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Executing tests."
if ! ./bank_app > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
cat pico_ql_test_output.txt
if ! egrep -q -i 'Test successful' pico_ql_test_output.txt
then
  diff pico_ql_test_current.txt pico_ql_test_success.txt
fi

cd ../Chess
echo "\nIn Chess..."
echo "-> Generating files."
make prep > /dev/null
if ! ruby pico_ql_generator.rb pico_ql_dsl.sql
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Building."
make clean > /dev/null
if ! make PICO_QL_JOIN_THREADS=1 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Executing tests."
if ! ./chess > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
cat pico_ql_test_output.txt
if ! egrep -q -i 'Test successful' pico_ql_test_output.txt
then
  diff pico_ql_test_current.txt pico_ql_test_success.txt
fi

cd ../VRP
echo "\nIn VRP..."
echo "-> Generating files."
make prep > /dev/null
if ! ruby pico_ql_generator.rb pico_ql_dsl.sql
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Building."
make clean > /dev/null
if ! make PICO_QL_JOIN_THREADS=1 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Executing tests."
if ! ./schedule solomon.txt 2 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
cat pico_ql_test_output.txt
if ! egrep -q -i 'Test successful' pico_ql_test_output.txt
then
  diff pico_ql_test_current.txt pico_ql_test_success.txt
fi

cd ../CApp
echo "\n CApp..."
echo "-> Generating files."
make prep > /dev/null
make clean > /dev/null
rm pico_ql_internal.c pico_ql_search.c > /dev/null
if ! ruby pico_ql_generator.rb pico_ql_dsl.sql
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Building."
if ! make G_CXX=1 PICO_QL_JOIN_THREADS=1 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Executing tests."
if ! ./capp > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
cat pico_ql_test_output.txt
if ! egrep -q -i 'Test successful' pico_ql_test_output.txt
then
  diff pico_ql_test_current.txt pico_ql_test_success.txt
fi

cd ..
echo "\n*Testing examples with single threaded version, no memory assistance for temporary variables, and polymorphism support.*\n"

cd BankApp
echo "In BankApp..."
echo "-> Generating files."
make prep > /dev/null
if ! ruby pico_ql_generator.rb pico_ql_dsl.sql no_mem_mgt
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Building."
make clean > /dev/null
if ! make PICO_QL_SINGLE_THREADED=1 PICO_QL_HANDLE_TEXT_ARRAY=1 PICO_QL_HANDLE_POLYMORPHISM=1 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Executing tests."
if ! ./bank_app > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
cat pico_ql_test_output.txt
if ! egrep -q -i 'Test successful' pico_ql_test_output.txt
then
  diff pico_ql_test_current.txt pico_ql_test_success.txt
fi

cd ../Chess
echo "\nIn Chess..."
echo "-> Generating files."
make prep > /dev/null
if ! ruby pico_ql_generator.rb pico_ql_dsl.sql no_mem_mgt
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Building."
make clean > /dev/null
if ! make PICO_QL_SINGLE_THREADED=1 PICO_QL_HANDLE_TEXT_ARRAY=1 PICO_QL_HANDLE_POLYMORPHISM=1 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Executing tests."
if ! ./chess > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
cat pico_ql_test_output.txt
if ! egrep -q -i 'Test successful' pico_ql_test_output.txt
then
  diff pico_ql_test_current.txt pico_ql_test_success.txt
fi

cd ../VRP
echo "\nIn VRP..."
echo "-> Generating files."
make prep > /dev/null
if ! ruby pico_ql_generator.rb pico_ql_dsl.sql no_mem_mgt 
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Building."
make clean > /dev/null
if ! make PICO_QL_SINGLE_THREADED=1 PICO_QL_HANDLE_TEXT_ARRAY=1 PICO_QL_HANDLE_POLYMORPHISM=1 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Executing tests."
if ! ./schedule solomon.txt 2 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
cat pico_ql_test_output.txt
if ! egrep -q -i 'Test successful' pico_ql_test_output.txt
then
  diff pico_ql_test_current.txt pico_ql_test_success.txt
fi

cd ../CApp
echo "\n CApp..."
echo "-> Generating files."
make prep > /dev/null
make clean > /dev/null
if ! ruby pico_ql_generator.rb pico_ql_dsl.sql C
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Building."
if ! make PICO_QL_JOIN_THREADS=1 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Executing tests."
if ! ./capp > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
cat pico_ql_test_output.txt
if ! egrep -q -i 'Test successful' pico_ql_test_output.txt
then
  diff pico_ql_test_current.txt pico_ql_test_success.txt
fi

cd ../Polymorphism
echo "\nIn Polymorphism..."
echo "-> Generating files."
make prep > /dev/null
if ! ruby pico_ql_generator.rb pico_ql_dsl.sql no_mem_mgt 
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Building."
make clean > /dev/null
if ! make PICO_QL_SINGLE_THREADED=1 PICO_QL_HANDLE_TEXT_ARRAY=1 PICO_QL_HANDLE_POLYMORPHISM=1 > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
echo "-> Executing tests."
if ! ./poly > /dev/null
then
  echo "Failure in this step.Troubleshoot."
  exit
fi
cat pico_ql_test_output.txt
if ! egrep -q -i 'Test successful' pico_ql_test_output.txt
then
  diff pico_ql_test_current.txt pico_ql_test_success.txt
fi

echo "\n"
echo "****************************************"
echo "* End of tests. All tests operational. *"
echo "****************************************"
