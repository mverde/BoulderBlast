#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <vector>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Level;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir), m_player(nullptr), m_bonus(1000)
	{}

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	std::vector<Actor*> getActors() const;

	virtual ~StudentWorld();

private:
	std::vector<Actor*> m_actors;
	Actor* m_player;
	int m_bonus;
};

inline
std::vector<Actor*> StudentWorld::getActors() const
{ return m_actors; }

#endif // STUDENTWORLD_H_
