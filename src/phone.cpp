#include "phone.h"

Mat Phone::ScreenCap(string save_path)
{
	screenshot_path = save_path;
	remove(screenshot_path.c_str());
	char cmd_[100];
	sprintf_s(cmd_, "adb exec-out screencap -p > %s", save_path.c_str());
	//printf(cmd_);
	system(cmd_);
	while (!imread(save_path).empty())
	{
		return imread(save_path);
	}
}

void Phone::Jump(double dist)
{
	int time = dist * k;
	char cmd[150];
	int randX = 800 + rand() % 20;
	int randY = 1000 + rand() % 50;
	printf("\nPress At (%d,%d) for %d ms", randX, randY, time);
	sprintf_s(cmd, "adb shell input swipe %d %d %d %d %d", randX, randY, randX, randY, time);
	//printf(cmd);
	system(cmd);
	_sleep(2000 + rand()%500);
}

Phone::Phone(float k_)
{
	k = k_;
	srand(time(0));
}
