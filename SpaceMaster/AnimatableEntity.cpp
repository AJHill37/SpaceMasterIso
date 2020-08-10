#include "olcPixelGameEngine.h"
#include "AnimatableEntity.h"
#include <math.h>       /* fmod */

AnimatableEntity::AnimatableEntity()
{
	this->currentAnimation = NULL;
}


AnimatableEntity::AnimatableEntity(std::unordered_map<std::string, Animation*> animations)
{
	this->animations = animations; 
	this->currentAnimation = animations.find("IDLE") != animations.end() ? animations["IDLE"] : NULL;
}

AnimatableEntity::~AnimatableEntity()
{
}

bool AnimatableEntity::getAnimation(std::string animationTitle, Animation& foundAnimation)
{
	auto t = animations.find(animationTitle);
	if (t == animations.end()) return false;
	foundAnimation = *t->second;
	return true;
}

void AnimatableEntity::addAnimation(std::string animationTitle, Animation* placedAnimation)
{
	animations[animationTitle] = placedAnimation;
}

int AnimatableEntity::getNextFrame()
{
	return currentAnimation->getCurrentFrame();
}

Animation* AnimatableEntity::getCurrentAnimation()
{
	return currentAnimation;
}

void AnimatableEntity::setCurrentAnimation(std::string animationTitle)
{
	this->currentAnimation = animations[animationTitle];
}


void AnimatableEntity::update(float fElapsedTime)
{
	currentAnimation->update(fElapsedTime);
}






