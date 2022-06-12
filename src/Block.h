#pragma once
#include <opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <string>

using namespace cv;
using namespace std;

enum ERROR
{
	 NoSuchFile = 0
};

class Block
{
public:
	Mat block_img;
	string file_path;

	Block(string img_path);
	//~Block();
};
