 #!/bin/bash
cd ./Release/tests
ctest --output-on-failure
if [ $? -ne 0 ]; then
  exit $?
fi
cd ../../Debug/tests
ctest --output-on-failure
if [ $? -ne 0 ]; then
  exit $?
fi
