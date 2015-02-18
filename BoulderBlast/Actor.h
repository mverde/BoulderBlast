#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;

//BASE CLASS//
class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, int health, StudentWorld* world, Direction dir = none)
		:GraphObject(imageID, startX, startY, dir), m_health(health), m_world(world), m_alive(true)
	{ setVisible(true); }

	virtual void doSomething()
	{ return; }

	virtual void onHit()
	{ return; }

	virtual bool canMove(int x, int y) const = 0;

	virtual StudentWorld* getWorld() const;
	virtual bool isAlive() const;
	virtual void setDead();

	virtual ~Actor()
	{ getGraphObjects().erase(this); }

private:
	int m_health;
	bool m_alive;
	StudentWorld* m_world;
};

inline
StudentWorld* Actor::getWorld() const
{ return m_world; }

inline
bool Actor::isAlive() const
{ return m_alive; }

inline
void Actor::setDead()
{ m_alive = false; }

//DERIVED ACTORS//
class Wall : public Actor
{
public:
	Wall(int startX, int startY, StudentWorld* world)
		:Actor(IID_WALL, startX, startY, 0, world)
	{}

	virtual bool canMove(int x, int y) const
	{ return false; }

	virtual ~Wall()
	{ getGraphObjects().erase(this); }
};

class Player : public Actor
{
public:
	Player(int startX, int startY, StudentWorld* world)
		:Actor(IID_PLAYER, startX, startY, 20, world, right), m_ammo(20)
	{}

	virtual void doSomething();
	virtual void onHit();
	virtual bool canMove(int x, int y) const;

	virtual ~Player()
	{ getGraphObjects().erase(this); }

private:
	int m_ammo;
};

#endif // ACTOR_H_
