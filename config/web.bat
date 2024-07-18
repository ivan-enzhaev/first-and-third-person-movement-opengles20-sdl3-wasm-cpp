@echo OFF
cd ..
mkdir build
mkdir build\web
cd build\web
emcmake cmake -DSDL3_DIR=E:\libs\SDL3\prefix\lib\cmake\SDL3 ..\..
