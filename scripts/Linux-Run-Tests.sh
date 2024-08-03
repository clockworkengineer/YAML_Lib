 #!/bin/bash
 cd ./Release/tests/
 ./YAML_Lib_Unit_Tests
if [ $? -eq 0 ]; then
  cd ../../
else
  exit $?
fi
 cd ./Debug/tests/
 ./YAML_Lib_Unit_Tests
if [ $? -eq 0 ]; then
  cd ../../
else
  exit $?
fi
