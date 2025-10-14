@echo off
cd build
cmake ..
cmake --build . --config Release
echo ====Running Program=====
.\Release\myio_test.exe