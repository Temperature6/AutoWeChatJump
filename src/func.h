#pragma once
#include <opencv.hpp>
#include <opencv2/core/utils/logger.hpp>

struct BlockLoc
{
	Point begin, end;
	double minVal;
};
