# Person/Car Detection Demo Compile and Run

## Synopsis:
This folder contains the works of person/car detection demo. The target of this demo is to show how to compile and run the code on Linux system.

## File structure: 
Pull to your local drive and decompress the linux_cpu.tar.gz and linux_gpu.tar.gz, you will get following file structure:
* NOTE: the source code in /src/ and demo code in /test_lib/ are the same for linux_cpu/ and linux_gpu/. Duplicate them just for clarity reason.
```
person_sdk_upload/  
+-- linux_cpu/
?  +-- run_sh.sh      : the script to complie and run the demo
?  +-- lib/           : the compiled *.so and *.h will be placed in this folder
?  +-- src/           : the source code from darknet and self-defined api functions
?  +-- test_lib/      : 
?  ?  +-- model/      : by default, the configure file, weight file, and input video file are in this folder
?  ?  +-- main.cpp    : the demo function shows how to use the api function
+-- linux_gpu/
?  +-- Makefile       : for complie
?  +-- run_sh.sh      : the script to run the demo
?  +-- lib/           : the compiled *.o will be placed in this folder
?  +-- src/           : the source code from darknet and self-defined api functions
?  +-- test_lib/      : 
?  ?  +-- model/      : by default, the configure file, weight file, and input video file are in this folder
?  ?  +-- main.cpp    : the demo function shows how to use the api function
```

## How to compile and run CPU demo: 
1. Open terminal in ./person_sdk_upload/linux_cpu/ folder.
2. Run the script by "bash run_sh.sh"
3. Done. The script will compile the code and run the demo automatically.
* NOTE: on the bottom of the script, "./main ./model/navlab.cfg ./model/navlab_300000.weights ./model/IR13.asf", you should change the path before running.


## How to compile and run GPU demo: 
1. Open terminal in ./person_sdk_upload/linux_gpu/ folder.
2. Compile the code by "make"
3. Run the script by "bash run_sh.sh"
4. Done.
* NOTE: on the bottom of the run_sh.sh", "./main ./model/navlab.cfg ./model/navlab_300000.weights ./model/IR13.asf", you should change the path before running.
