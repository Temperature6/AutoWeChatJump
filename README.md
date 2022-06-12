# 基于OpenCV的全自动微信跳一跳

### 原理

![](.\running.jpg)

运行时

1、用ADB获取手机截图，传输到电脑上。

2、使用OpenCV处理图像

​		1、使用模板匹配寻找玩家位置

​		2、二值化图片，提取边缘

​		3、用边缘来判断下一个方块的位置

3、计算从玩家脚下到下一个方块中心的距离

4、使用ADB的模拟按压命令按压对应的时间

5、循环

### 注意

1、由于使用的是模板匹配寻找玩家，所以player.png的分辨率会影响匹配的精准度，您可以在自己的手机上截图，把玩家抠出来，最好只剩下玩家，背景透明且没有多余像素

2、

```c++
int time = dist * 1.375;
```

1.375是根据距离计算按压时间（ms）的系数，请自行更改以提高精准度

### 参考

[mzlogin/awesome-adb: ADB Usage Complete / ADB 用法大全 (github.com)](https://github.com/mzlogin/awesome-adb)

https://developer.android.com/studio/command-line/adb.html

[opencv/opencv: Open Source Computer Vision Library (github.com)](https://github.com/opencv/opencv)

https://opencv.org/