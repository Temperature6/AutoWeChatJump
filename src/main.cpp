/*
	Copyright 2022 (c) AQ Lab. All rights reserved.
	Auto WeChat Jump (Based on OpenCV and MatchTemplate)
*/
#include <opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <cmath>
#include <io.h>
#include <vector>
#include "Block.h"
#include "phone.h"

using namespace std;
using namespace cv;

void QuickShow(Mat img, int key = 0, string win_name = "Window", float k = 0.3f);

int main()
{
	//ʹOpenCV���������־
	cv::utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);

	Mat player = imread(".\\game_img\\player.png");
	Phone my_phone;
	int round = 0;
	while (true)
	{
		Mat screenshot = my_phone.ScreenCap();
		//Mat screenshot = imread(".\\screenshot\\sc7.jpg");
		Mat player_result;
		Point p_minloc;
		//ƥ�����
		matchTemplate(screenshot, player, player_result, TM_SQDIFF);
		minMaxLoc(player_result, NULL, NULL, &p_minloc, NULL); //Ѱ�����ƥ��
		Point p_end(p_minloc.x + player.cols, p_minloc.y + player.rows);
		
		Point foot(p_minloc.x + (int)player.cols * 0.5, p_minloc.y + player.rows);
		//�ü�ͼ��ȥ��������Ϸ��Ϣ����ҽ��µ����ò���
		int top = 520; //������Ϸ��Ϣ�ĸ߶�
		Mat cropped_sc = screenshot(Range(top, foot.y), Range(0, screenshot.cols));
		//�ü���������귢���仯
		Point temp_p_end(p_end.x, p_end.y - top);
		Point temp_p_start(p_minloc.x, p_minloc.y - top);
		//��Ե�����ȡ��һ������ı�Ե����˹ģ�����ڽ���
		GaussianBlur(cropped_sc.clone(), cropped_sc, Size(5, 5), 0);
		Mat outline_sc;
		Canny(cropped_sc, outline_sc, 1, 10);
		threshold(outline_sc, outline_sc, 100, 255, THRESH_BINARY);
		QuickShow(outline_sc, 1, "Threshold");
		//������λ�ã���ֹ����
		for (int i = temp_p_start.y; i < temp_p_end.y; i++) //�ʵ�����ʹ������ɾ�
		{
			for (int j = temp_p_start.x; j < temp_p_end.x; j++)
			{
				//printf("X=%d,Y=%d\n", j, i);
				outline_sc.at<unsigned char>(i, j) = 0;
			}
		}
		Point block_top; bool end = false;
		for (int i = 0; i < outline_sc.rows; i++)
		{
			for (int j = outline_sc.cols - 1; j > 0; j--)
			{
				if (outline_sc.at<uchar>(i, j) > 100)
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
		Point block_right; end = false;
		for (int i = outline_sc.cols - 1; i > 0; i--)
		{
			for (int j = 0; j < outline_sc.rows * 0.8; j++)
			{
				if (outline_sc.at<uchar>(j, i) > 100)
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
		Point center(block_top.x, block_right.y + top);
		line(screenshot, foot, center, Scalar(0, 0, 255), 4);
		rectangle(screenshot, p_minloc, p_end, Scalar(0, 255, 0), 4);
		double distance = sqrt(pow(foot.x - center.x, 2) + pow(foot.y - center.y, 2));
		printf("\n[Round=%d]Distance is %.2f pixels",round + 1 , distance);
		QuickShow(screenshot, 1);
		my_phone.Jump(distance);
		round++;
	}
}

void QuickShow(Mat img, int key, string win_name, float k)
{
	Mat output;
	resize(img, output, Size((int)img.cols * k, (int)img.rows * k));
	imshow(win_name, output);
	startWindowThread();
	waitKey(key);
}
