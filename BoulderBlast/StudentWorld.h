#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <vector>
#include <string>
#include <cstdlib>
#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
const int TO_CHAR = 48;

class Actor;
class Player;
class Level;

class StudentWorld : public GameWorld
{
public:
	StudentWorld::StudentWorld(std::string assetDir)
		: GameWorld(assetDir), m_bonus(1000), m_nJewels(0), m_nextLev(false)
	{}

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	std::vector<Actor*>* getActors();
	Player* getPlayer() const;
	int getJewels() const;
	
	void setDisplayText();
	std::string formatDisplayText(const int& score, const int& level, const int& lives, const int& health, const int& ammo, const int& bonus);
	std::string formatLevel(const int& lev);

	void deleteDead();
	void gotJewel();
	void setAdvanceLevel();
	void createBullet(const int& x, const int& y, const GraphObject::Direction& dir);
	void dropGoodie(const int& x, const int& y, const char& goodie);
	void createKlepto(const int&x, const int& y, const bool& angry);

	virtual ~StudentWorld();

private:
	std::vector<Actor*> m_actors;
	Player* m_player;
	int m_bonus;
	int m_nJewels;
	bool m_nextLev;
};

inline std::vector<Actor*>* StudentWorld::getActors()
{ return &m_actors; }

inline Player* StudentWorld::getPlayer() const
{ return m_player; }

inline int StudentWorld::getJewels() const
{ return m_nJewels; }

inline void StudentWorld::gotJewel()
{ m_nJewels--; }

inline void StudentWorld::setAdvanceLevel()
{ m_nextLev = !m_nextLev; }

#endif // STUDENTWORLD_H_
