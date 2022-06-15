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

double k = 0.85; //ɨ�跶Χϵ��
double limit = 4.6e6; //ģ��ƥ������ֵ
float jump_k = 1.375f; //��Ծ����Ͱ�ѹʱ��ת��ϵ��
int top = 520; //������Ϸ��Ϣ�ĸ߶�
int run_round = 0; //��������
int YAxis_offset = -25; //��ҽŲ�����Y��ƫ����

int main()
{
	system("pause");
	//ʹOpenCV���������־
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
		//ƥ�����
		matchTemplate(screenshot, player, player_result, TM_SQDIFF);
		minMaxLoc(player_result, NULL, NULL, &p_minloc, NULL); //Ѱ�����ƥ��
		Point p_end(p_minloc.x + player.cols, p_minloc.y + player.rows);
		Point foot(p_minloc.x + (int)player.cols * 0.5, p_minloc.y + player.rows + YAxis_offset);
		//ƥ����������ָʾ��
		Point center;
		//�ü�ͼ��ȥ��������Ϸ��Ϣ����ҽ��µ����ò���
		Mat cropped_sc = screenshot(Range(top, foot.y), Range(0, screenshot.cols));
		//�ü���������귢���仯
		Point temp_p_end(p_end.x, foot.y - top);
		Point temp_p_start(p_minloc.x, p_minloc.y - top);
		//��Ե�����ȡ��һ������ı�Ե����˹ģ�����ڽ���
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
			//������λ�ã���ֹ����
			double wipe_k = 0.02;
			for (int i = temp_p_start.y * (1 - wipe_k); i < temp_p_end.y; i++) //�ʵ�����ʹ������ɾ�
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
			Point block_right; end = false; //�����ɨ��
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
