/*
	Copyright 2022 (c) AQ Lab. All rights reserved.
	Auto WeChat Jump (Based on OpenCV and MatchTemplate)
*/
#include <opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <cmath>
#include <vector>
#include "Block.h"
#include "phone.h"

using namespace std;
using namespace cv;

void QuickShow(Mat img, int key = 0, string win_name = "Window", float k = 0.3f);

double k = 0.85; //扫描范围系数
double limit = 4.6e6; //模板匹配限制值
float jump_k = 1.375f; //跳跃距离和按压时间转换系数
int top = 520; //顶部游戏信息的高度
int run_round = 0; //进行轮数
int YAxis_offset = -25; //玩家脚部坐标Y轴偏移量

int main()
{
	system("pause");
	//使OpenCV不再输出日志
	cv::utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);

	Mat player = imread(".\\game_img\\player.png");
	Mat center_img = imread(".\\game_img\\center_ths.png", CV_8U);
	Phone my_phone(jump_k);

	while (true)
	{
		Mat screenshot = my_phone.ScreenCap();
		//Mat screenshot = imread(".\\screenshot\\end.jpg");
		Mat player_result;
		Point p_minloc;
		//匹配玩家
		matchTemplate(screenshot, player, player_result, TM_SQDIFF);
		minMaxLoc(player_result, NULL, NULL, &p_minloc, NULL); //寻找最佳匹配
		Point p_end(p_minloc.x + player.cols, p_minloc.y + player.rows);
		Point foot(p_minloc.x + (int)player.cols * 0.5, p_minloc.y + player.rows + YAxis_offset);
		//匹配连跳中心指示点
		Point center;
		//裁剪图像，去掉上面游戏信息和玩家脚下的无用部分
		Mat cropped_sc = screenshot(Range(top, foot.y), Range(0, screenshot.cols));
		//裁剪后相对坐标发生变化
		Point temp_p_end(p_end.x, foot.y - top);
		Point temp_p_start(p_minloc.x, p_minloc.y - top);
		//边缘检测提取下一个方块的边缘，高斯模糊用于降噪
		GaussianBlur(cropped_sc.clone(), cropped_sc, Size(5, 5), 0);
		Mat outline_sc;
		Canny(cropped_sc, outline_sc, 1, 10);
		threshold(outline_sc, outline_sc, 100, 255, THRESH_BINARY);

		Mat center_result; double center_minVal; Point center_minLoc;
		matchTemplate(outline_sc, center_img, center_result, TM_SQDIFF);
		minMaxLoc(center_result, &center_minVal, NULL, &center_minLoc);
		if (center_minVal < limit)
		{
			center = Point(center_minLoc.x + center_img.cols * 0.5, center_minLoc.y + center_img.rows * 0.5 + top);
		}
		else
		{
			//填充玩家位置，防止干扰
			double wipe_k = 0.02;
			for (int i = temp_p_start.y * (1 - wipe_k); i < temp_p_end.y; i++) //适当扩大使其擦除干净
			{
				for (int j = temp_p_start.x * (1 - wipe_k); j < temp_p_end.x * (1 + wipe_k); j++)
				{
					outline_sc.at<unsigned char>(i, j) = 0;
				}
			}
			Point block_top; bool end = false;
			for (int i = 0; i < outline_sc.rows; i++)
			{
				for (int j = outline_sc.cols - 1; j > 0; j--)
				{
					if (outline_sc.at<uchar>(i, j) > 200)
					{
						block_top = Point(j, i);
						end = true;
						break;
					}
				}
				if (end)
				{
					break;
				}
			}
			Point block_right; end = false; //向左侧扫描
			for (int i = outline_sc.cols - 1; i > 0; i--)
			{
				for (int j = 0; j < outline_sc.rows * k; j++)
				{
					if (outline_sc.at<uchar>(j, i) > 200)
					{
						block_right = Point(i, j);
						end = true;
						break;
					}
				}
				if (end)
				{
					break;
				}
			}
			center = Point(block_top.x, block_right.y + top);
		}

		line(outline_sc, Point(0, outline_sc.rows * k), Point(outline_sc.cols, outline_sc.rows * k), 255);
		QuickShow(outline_sc, 1, "Threshold", 0.5);
		line(screenshot, foot, center, Scalar(0, 0, 255), 4);
		rectangle(screenshot, p_minloc, p_end, Scalar(0, 255, 0), 4);

		double distance = sqrt(pow(foot.x - center.x, 2) + pow(foot.y - center.y, 2));
		printf("\n[Round=%d]Distance is %.2f pixels, Template match value= %.2f ,Used:%s",
			run_round + 1 ,
			distance,
			center_minVal,
			center_minVal > limit ? "No" : "Yes");

		QuickShow(screenshot, 1, "Recognition Result");
		my_phone.Jump(distance);
		run_round++;
	}
}

void QuickShow(Mat img, int key, string win_name, float k)
{
	Mat output;
	resize(img, output, Size((int)img.cols * k, (int)img.rows * k));
	imshow(win_name, output);
	waitKey(key);
}
