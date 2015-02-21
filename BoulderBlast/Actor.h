#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;

//////////BASE CLASSES//////////
//ACTOR//
class Actor : public GraphObject	//Actor is any item in the game that takes up a square and can interact with other Actors
{
public:
	Actor(int imageID, int startX, int startY, StudentWorld* world, Direction dir = none)
		:GraphObject(imageID, startX, startY, dir), m_world(world), m_alive(true)
	{ setVisible(true); }

	virtual void doSomething();
	virtual bool canBeSteppedOn() const = 0;

	StudentWorld* getWorld() const;
	bool isAlive() const;
	void setDead();

	virtual ~Actor()
	{ getGraphObjects().erase(this); }

private:
	bool m_alive;
	StudentWorld* m_world;
};

inline void Actor::doSomething()
{ return; }

inline StudentWorld* Actor::getWorld() const
{ return m_world; }

inline bool Actor::isAlive() const
{ return m_alive; }

inline void Actor::setDead()
{ m_alive = false; }

//ORGANISM//
class Organism : public Actor	//includes players, robots, and boulders
{
public:
	Organism(int imageID, int startX, int startY, StudentWorld* world, int health, Direction dir = none)
		:Actor(imageID, startX, startY, world, dir), m_health(health)
	{ setVisible(true); }

	virtual void onHit() = 0;
	virtual bool canMove(const int& x, const int& y) const = 0;	//if specified point is available for movement
	virtual bool canBeSteppedOn() const;

	void getHit();
	int getHealth() const;

	virtual ~Organism()
	{ getGraphObjects().erase(this); }

private:
	int m_health;
};

inline bool Organism::canBeSteppedOn() const
{ return false; }

inline void Organism::getHit()
{ m_health -= 2; }

inline int Organism::getHealth() const
{ return m_health; }

//////////DERIVED CLASSES//////////
//WALL//
class Wall : public Actor
{
public:
	Wall(int startX, int startY, StudentWorld* world)
		:Actor(IID_WALL, startX, startY, world)
	{}

	virtual bool canBeSteppedOn() const;

	virtual ~Wall()
	{ getGraphObjects().erase(this); }
};

inline bool Wall::canBeSteppedOn() const
{ return false; }

//BULLET//
class Bullet : public Actor
{
public:
	Bullet(int startX, int startY, StudentWorld* world, Direction dir)
		:Actor(IID_BULLET, startX, startY, world, dir)
	{}
	
	bool doBullet();
	virtual void doSomething();
	virtual bool canBeSteppedOn() const;

	virtual ~Bullet()
	{ getGraphObjects().erase(this); }
};

inline bool Bullet::canBeSteppedOn() const
{ return true; }

//HOLE//
class Hole : public Actor
{
public:
	Hole(int startX, int startY, StudentWorld* world)
		:Actor(IID_HOLE, startX, startY, world)
	{}

	virtual void doSomething();
	virtual bool canBeSteppedOn() const;

	virtual ~Hole()
	{ getGraphObjects().erase(this); }
};

inline bool Hole::canBeSteppedOn() const
{ return false; }

//PLAYER//
class Player : public Organism
{
public:
	Player(int startX, int startY, StudentWorld* world)
		:Organism(IID_PLAYER, startX, startY, world, 20, right), m_ammo(20)
	{}

	virtual void doSomething();
	virtual void onHit();
	virtual bool canMove(const int& x, const int& y) const;
	int getAmmo() const;

	virtual ~Player()
	{ getGraphObjects().erase(this); }

private:
	int m_ammo;
};

inline int Player::getAmmo() const
{ return m_ammo; }

class Boulder : public Organism
{
public:
	Boulder(int startX, int startY, StudentWorld* world)
		:Organism(IID_BOULDER, startX, startY, world, 10)
	{}

	virtual void doSomething();
	virtual void onHit();
	virtual bool canMove(const int& x, const int& y) const;
	virtual void push(const int& x, const int& y, const Direction& dir);

	virtual ~Boulder()
	{ getGraphObjects().erase(this); }
};

inline void Boulder::doSomething()
{ return; }

#endif // ACTOR_H_
