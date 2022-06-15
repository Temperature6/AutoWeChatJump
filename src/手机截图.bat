@echo off
adb shell screencap -p /sdcard/temp_screenshot.png
adb pull /sdcard/temp_screenshot.png .\temp_screenshot.png
adb shell rm -f /sdcard/temp_screenshot.png
pause
