#pragma once

#include "olcPixelGameEngine.h"
#include "Animation.h"
#include <unordered_set>
#include <queue>          

class AnimatableEntity 
{
	public:
		AnimatableEntity();
		AnimatableEntity(std::unordered_map<std::string, Animation*> animations);
		~AnimatableEntity();

		bool getAnimation(std::string animationTitle, Animation& foundAnimation);
		void addAnimation(std::string animationTitle, Animation* placedAnimation);
		int getNextFrame();
		Animation* getCurrentAnimation();
		void setCurrentAnimation(std::string animationTitle);

		void update(float fElapsedTime);


	private:
		std::unordered_map<std::string, Animation*> animations;
		Animation* currentAnimation;
};
