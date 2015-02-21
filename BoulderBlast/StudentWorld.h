#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include <vector>
#include <map>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
const int TO_CHAR = 48;

class Actor;
class Player;
class Level;

class StudentWorld : public GameWorld
{
public:
	StudentWorld::StudentWorld(std::string assetDir)
		: GameWorld(assetDir), m_bonus(1000)
	{}

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	std::vector<Actor*> getActors() const;
	Player* getPlayer() const;
	
	void setDisplayText();
	std::string formatDisplayText(const int& score, const int& level, const int& lives, const int& health, const int& ammo, const int& bonus);
	std::string formatNum(const int& num, const int& digits, const bool& spaces);
	std::string formatLevel(const int& lev);

	void deleteDead();
	void createBullet(const int& x, const int& y, StudentWorld* world, const GraphObject::Direction& dir);

	virtual ~StudentWorld();

private:
	std::vector<Actor*> m_actors;
	Player* m_player;
	int m_bonus;
};

inline std::vector<Actor*> StudentWorld::getActors() const
{ return m_actors; }

inline Player* StudentWorld::getPlayer() const
{ return m_player; }

#endif // STUDENTWORLD_H_
