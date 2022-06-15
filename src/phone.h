#pragma once
#pragma warning(disable:4996)
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>

using namespace cv;
using namespace std;

class Phone
{
public:
	string screenshot_path = "";
	float k = 1.375f;
	Mat ScreenCap(string save_path = ".\\temp_screenshot.png");
	void Jump(double dist);

	Phone(float k_);
};

