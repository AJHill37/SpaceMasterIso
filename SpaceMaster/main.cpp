/*
	Coding Quickie: Isometric Tiles
	"Owww... My insides hurt :(" - javidx9

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018-2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Relevant Video: https://youtu.be/ukkbNKTgf5U

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, �OneLoneCoder 2019
*/

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "AnimatableEntity.h"
#include <queue>          
#include <unordered_map>
#include <vector>

struct WorldTile {
	AnimatableEntity* animatableEntity;
	int baseTile;
	bool isSelected;
	WorldTile()
	{
		animatableEntity = NULL;
		isSelected = false;
		baseTile = 0;
	}
};

struct fromToCords {
	olc::vi2d* from;
	olc::vi2d* to;
	fromToCords(olc::vi2d* f, olc::vi2d* t)
	{
		from = f;
		to = t;
	}
};

// Override base class with your custom functionality
class IsometricDemo : public olc::PixelGameEngine
{
public:
	IsometricDemo()
	{
		sAppName = "Coding Quickie: Isometric Tiles";
	}

private:
	// Number of tiles in world
	olc::vi2d vWorldSize = { 14, 10 };

	// Size of single tile graphic
	olc::vi2d vTileSize = { 40, 20 };

	// Where to place tile (0,0) on screen (in tile size steps)
	olc::vi2d vOrigin = { 5, 1 };

	// Sprite that holds all imagery
	olc::Sprite* sprIsom = nullptr;

	// Pointer to create 2D world array
	WorldTile* pWorld = nullptr;

	//The currently selected world tile.
	WorldTile* selected = nullptr;

	//This holds a queue of all of the locations we plan on moving a animatable entity through.
	std::queue<fromToCords*> moveQueue;
	fromToCords* currentMove = nullptr;
	float moveTimer = 0.0f;
	float moveSpeed = 0.2f;

	//

public:
	bool OnUserCreate() override
	{
		// Load sprites used in demonstration
		sprIsom = new olc::Sprite("isometric_demo.png");

		// Create empty world
		pWorld = new WorldTile[vWorldSize.x * vWorldSize.y]{};
		//pWorld = new int[vWorldSize.x * vWorldSize.y]{ 1 };
		// (0,0) is at top, defined by vOrigin, so draw from top to bottom
		// to ensure tiles closest to camera are drawn last
		for (int y = 0; y < vWorldSize.y; y++)
		{
			for (int x = 0; x < vWorldSize.x; x++)
			{
				if (x == 5 && y == 5) {
					pWorld[y * vWorldSize.x + x].animatableEntity = generateAnimatableEntity();
				}
			}
		}
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::WHITE);

		if (currentMove) {
			moveTimer += fElapsedTime;
		}

		if (moveTimer >= moveSpeed) {
			//If we have more moves we preserve the "fraction" of a move to make it so that the movement does
			//not appear overly "jittery"
			moveTimer = !moveQueue.empty() ? moveTimer - moveSpeed : 0.0f;
			delete currentMove;
			currentMove = nullptr;
		}

		if (currentMove == nullptr && !moveQueue.empty()) {
			currentMove = moveQueue.front();
			moveQueue.pop();
			olc::vi2d* from = currentMove->from;
			olc::vi2d* to = currentMove->to;
			pWorld[to->y * vWorldSize.x + to->x].animatableEntity = pWorld[from->y * vWorldSize.x + from->x].animatableEntity;
			pWorld[from->y * vWorldSize.x + from->x].animatableEntity = nullptr;
			//moveAnimatableEntity()
		}


		// Get Mouse in world
		olc::vi2d vMouse = { GetMouseX(), GetMouseY() };

		// Work out active cell
		olc::vi2d vCell = { vMouse.x / vTileSize.x, vMouse.y / vTileSize.y };

		// Work out mouse offset into cell
		olc::vi2d vOffset = { vMouse.x % vTileSize.x, vMouse.y % vTileSize.y };

		// Sample into cell offset colour
		olc::Pixel col = sprIsom->GetPixel(3 * vTileSize.x + vOffset.x, vOffset.y);

		// Work out selected cell by transforming screen cell
		olc::vi2d vSelected =
		{
			(vCell.y - vOrigin.y) + (vCell.x - vOrigin.x),
			(vCell.y - vOrigin.y) - (vCell.x - vOrigin.x)
		};

		// "Bodge" selected cell by sampling corners
		if (col == olc::RED) vSelected += {-1, +0};
		if (col == olc::BLUE) vSelected += {+0, -1};
		if (col == olc::GREEN) vSelected += {+0, +1};
		if (col == olc::YELLOW) vSelected += {+1, +0};

		// Handle mouse click to toggle if a tile is visible or not
		if (GetMouse(0).bPressed)
		{
			// Guard array boundary
			if (vSelected.x >= 0 && vSelected.x < vWorldSize.x && vSelected.y >= 0 && vSelected.y < vWorldSize.y) {
				selected = &pWorld[vSelected.y * vWorldSize.x + vSelected.x];
				++selected->baseTile %= 6;
				//++pWorld[vSelected.y * vWorldSize.x + vSelected.x] %= 6;
			}
		}

		if(GetKey(olc::Key::UP).bPressed){
			// Guard array boundary
			if (vSelected.x >= 0 && vSelected.x < vWorldSize.x && vSelected.y >= 1 && vSelected.y < vWorldSize.y) {
				selected = &pWorld[vSelected.y * vWorldSize.x + vSelected.x];
				if (selected->animatableEntity) {
					olc::vi2d* from = new olc::vi2d(vSelected.x, vSelected.y);
					olc::vi2d* to = new olc::vi2d(--vSelected.x, --vSelected.y);
					moveQueue.push(new fromToCords(from, to));
				}
			}
		}
		
		// Draw World - has binary transparancy so enable masking
		SetPixelMode(olc::Pixel::MASK);

		// (0,0) is at top, defined by vOrigin, so draw from top to bottom
		// to ensure tiles closest to camera are drawn last
		for (int y = 0; y < vWorldSize.y; y++)
		{
			for (int x = 0; x < vWorldSize.x; x++)
			{
				// Convert cell coordinate to world space
				olc::vi2d vWorld = ToScreen(x, y);

				drawBaseTile(vWorld, x, y);
				drawAnimatableEntities(vWorld, x, y, fElapsedTime);
				
			}
		}

		// Draw Selected Cell - Has varying alpha components
		SetPixelMode(olc::Pixel::ALPHA);

		// Convert selected cell coordinate to world space
		olc::vi2d vSelectedWorld = ToScreen(vSelected.x, vSelected.y);

		// Draw "highlight" tile
		DrawPartialSprite(vSelectedWorld.x, vSelectedWorld.y, sprIsom, 0 * vTileSize.x, 0, vTileSize.x, vTileSize.y);

		// Go back to normal drawing with no expected transparency
		SetPixelMode(olc::Pixel::NORMAL);

		// Draw Debug Info
		DrawString(4, 4, "Mouse   : " + std::to_string(vMouse.x) + ", " + std::to_string(vMouse.y), olc::BLACK);
		DrawString(4, 14, "Cell    : " + std::to_string(vCell.x) + ", " + std::to_string(vCell.y), olc::BLACK);
		DrawString(4, 24, "Selected: " + std::to_string(vSelected.x) + ", " + std::to_string(vSelected.y), olc::BLACK);
		return true;
	}

	
	olc::vi2d ToScreen(int x, int y) {
		return olc::vi2d
		{
			(vOrigin.x * vTileSize.x) + (x - y) * (vTileSize.x / 2),
			(vOrigin.y * vTileSize.y) + (x + y) * (vTileSize.y / 2)
		};
	}

	void drawAnimatableEntities(olc::vi2d vWorld, int x, int y, float fElapsedTime) {
		if (pWorld[y * vWorldSize.x + x].animatableEntity != NULL) {
			if (currentMove && currentMove->to && currentMove->to->x == x && currentMove->to->y == y) {
				olc::vi2d from = ToScreen(currentMove->from->x, currentMove->from->y);
				olc::vi2d to = ToScreen(currentMove->to->x, currentMove->to->y);

				float moveFration = moveTimer / moveSpeed;
				vWorld.x = from.x + (int)((float)(to.x - from.x) * moveFration);
				vWorld.y = from.y + (int)((float)(to.y - from.y) * moveFration);
			}

			AnimatableEntity* nextEntity = pWorld[y * vWorldSize.x + x].animatableEntity;
			nextEntity->update(fElapsedTime);
			Animation* nAnim = nextEntity->getCurrentAnimation();
			DrawPartialSprite(vWorld.x, vWorld.y - vTileSize.y, nAnim->sheet, nextEntity->getNextFrame() * vTileSize.x, 0, nAnim->frameWidth, nAnim->frameHeight);
		}
	}

	void drawBaseTile(olc::vi2d vWorld, int x, int y) {
		switch (pWorld[y * vWorldSize.x + x].baseTile)
		{
		case 0:
			// Invisble Tile
			DrawPartialSprite(vWorld.x, vWorld.y, sprIsom, 1 * vTileSize.x, 0, vTileSize.x, vTileSize.y);
			break;
		case 1:
			// Visible Tile
			DrawPartialSprite(vWorld.x, vWorld.y, sprIsom, 2 * vTileSize.x, 0, vTileSize.x, vTileSize.y);
			break;
		case 2:
			// Tree
			DrawPartialSprite(vWorld.x, vWorld.y - vTileSize.y, sprIsom, 0 * vTileSize.x, 1 * vTileSize.y, vTileSize.x, vTileSize.y * 2);
			break;
		case 3:
			// Spooky Tree
			DrawPartialSprite(vWorld.x, vWorld.y - vTileSize.y, sprIsom, 1 * vTileSize.x, 1 * vTileSize.y, vTileSize.x, vTileSize.y * 2);
			break;
		case 4:
			// Beach
			DrawPartialSprite(vWorld.x, vWorld.y - vTileSize.y, sprIsom, 2 * vTileSize.x, 1 * vTileSize.y, vTileSize.x, vTileSize.y * 2);
			break;
		default:
			// Water
			DrawPartialSprite(vWorld.x, vWorld.y - vTileSize.y, sprIsom, 3 * vTileSize.x, 1 * vTileSize.y, vTileSize.x, vTileSize.y * 2);
			break;
		}
	}

	AnimatableEntity* generateAnimatableEntity() {
		std::unordered_map<std::string, Animation*> animations;
		animations["IDLE"] = new Animation(new olc::Sprite("NorthArrow.png"), 1.0f, 6, 40, 40);
		animations["NORTH_ARROW"] = new Animation(new olc::Sprite("NorthArrow.png"), 1.0f, 6, 40, 40);
		animations["NORTH_EAST_ARROW"] = new Animation(new olc::Sprite("NorthEastArrow.png"), 1.0f, 6, 40, 40);
		animations["NORTH_WEST_ARROW"] = new Animation(new olc::Sprite("NorthWestArrow.png"), 1.0f, 6, 40, 40);
		animations["EAST_ARROW"] = new Animation(new olc::Sprite("EastArrow.png"), 1.0f, 6, 40, 40);
		animations["WEST_ARROW"] = new Animation(new olc::Sprite("WestArrow.png"), 1.0f, 6, 40, 40);
		animations["SOUTH_EAST_ARROW"] = new Animation(new olc::Sprite("SouthEastArrow.png"), 1.0f, 6, 40, 40);
		animations["SOUTH_WEST_ARROW"] = new Animation(new olc::Sprite("SouthWestArrow.png"), 1.0f, 6, 40, 40);
		animations["SOUTH_ARROW"] = new Animation(new olc::Sprite("SouthArrow.png"), 1.0f, 6, 40, 40);
		AnimatableEntity* retVal = new AnimatableEntity(animations);
		retVal->setCurrentAnimation(getRandomAnimationString());
		return retVal;
	}

	std::string getRandomAnimationString() {
		switch (rand() % 8)
		{
			case 0:
				return "NORTH_ARROW";
			case 1:
				return "NORTH_EAST_ARROW";
			case 2:
				return "NORTH_WEST_ARROW";
			case 3:
				return "EAST_ARROW";
			case 4:
				return "WEST_ARROW";
			case 5:
				return "SOUTH_EAST_ARROW";
			case 6:
				return "SOUTH_WEST_ARROW";
			default:
				return "SOUTH_ARROW";
		}
	}

	void moveAnimatableEntity(int oldI, int newI) {
		if (pWorld[oldI].animatableEntity != NULL) {
			pWorld[newI].animatableEntity = pWorld[oldI].animatableEntity;
			pWorld[oldI].animatableEntity = NULL;
		}
	}


	std::vector<fromToCords> generateShortestPath(int start, int end) {
	}

	olc::vi2d* getXYFromIndex(int index) {
		return new olc::vi2d(index % vWorldSize.x, index / vWorldSize.x);
	}

	int getIndexFromXY(int x, int y) {
		return y * vWorldSize.x + x;
	}
};


int main()
{
	IsometricDemo demo;
	if (demo.Construct(512, 480, 2, 2))
		demo.Start();
	return 0;
}