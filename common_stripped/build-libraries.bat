@echo off

echo Building OpenCV...
start /D OpenCV /WAIT build-opencv.bat

echo Building Qwt...
start /D qwt-6.0.0-rc5 /WAIT build-qwt.bat

echo Building Boost...
start /D Boost /WAIT build-boost.bat

echo Building IPC...
start /D IPC /WAIT build-ipc.bat

pause
