#pragma once
#include "olcPixelGameEngine.h"
#include <vector>
#include <unordered_set>

class Animation
{
public:
	Animation();
	Animation(olc::Sprite* sheet, float animationTotalTime, int frameCount, int frameWidth, int frameHeight);
	~Animation();

	int getCurrentFrame();

	void update(float fElapsedTime);
	void resetCurrentTime();

	olc::Sprite* sheet;
	float animationTotalTime;
	float currentTime;

	int frameCount;
	int frameWidth;
	int frameHeight;

private:
	float animationFrameTime;
};
