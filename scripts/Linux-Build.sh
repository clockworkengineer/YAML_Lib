 #!/bin/bash
 cmake -S . -B Release -DCMAKE_BUILD_TYPE=Release
 cmake --build Release 

 cmake -S . -B Debug -DCMAKE_BUILD_TYPE=Debug
 cmake --build Debug 