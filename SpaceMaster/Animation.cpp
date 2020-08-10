#include "Animation.h"

Animation::Animation()
{
	this->sheet = NULL;
	this->animationTotalTime = 0.0f;
	this->frameCount = 0;
	this->frameWidth = 0;
	this->frameHeight = 0;
	this->animationFrameTime = 0.0f;
	this->currentTime = 0.0f;
}

Animation::Animation(olc::Sprite* sheet, float animationTotalTime, int frameCount, int frameWidth, int frameHeight)
{
	this->sheet = sheet;
	this->animationTotalTime = animationTotalTime;
	this->frameCount = frameCount;
	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
	this->animationFrameTime = animationTotalTime / (float) frameCount;
}

Animation::~Animation()
{
}

int Animation::getCurrentFrame()
{
	return (int)(currentTime / animationFrameTime) % frameCount;
}

void Animation::update(float fElapsedTime)
{
	currentTime = fmod(currentTime + fElapsedTime, animationTotalTime);
}

void Animation::resetCurrentTime()
{
	this->currentTime = 0.0f;
}
