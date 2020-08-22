adb root
adb remount
ping -n 3 127.1 >nul
rm -rf ./DCIM/
adb shell rm -rf ./sdcard/DCIM/*
adb push ./raw_rr_1696x1248_3184_0121-171416_5.raw ./sdcard/DCIM/raw_rr_1696x1248_3184_0121-171416_5.raw
pause