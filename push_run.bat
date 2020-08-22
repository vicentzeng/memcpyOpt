adb root
adb remount
ping -n 3 127.1 >nul

rm -rf ./DCIM/

call build.bat

::adb shell rm -rf ./sdcard/DCIM/Camera/rr.raw
adb shell mkdir /data/data/test/
adb push libs\armeabi-v7a\mem_i_test /data/data/test/

::adb push libs\arm64-v8a\sharpcheck /data/data/test/sharpcheck_64
::adb push libs\arm64-v8a\libsharpdetectcore.so /vendor/lib64/


adb shell chmod 777 /data/data/test/mem_i_test

adb shell /data/data/test/mem_i_test
::adb shell chmod 777 /data/data/test/sharpcheck_64
::adb shell ./data/data/test/mem_i_test
::adb shell /data/data/test/sharpcheck_64
::adb pull ./sdcard/DCIM/ ./
::adb shell rm -rf ./sdcard/DCIM/*
pause
