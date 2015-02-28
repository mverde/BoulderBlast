#ifndef ACTOR_H_
#define ACTOR_H_

#include <cstdlib>
#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;

////////////////////////////////
//////////BASE CLASSES//////////
////////////////////////////////
//ACTOR//
class Actor : public GraphObject	//Actor is any item in the game that takes up a square and can interact with other Actors
{
public:
	Actor(int imageID, int startX, int startY, StudentWorld* world, Direction dir = none)
		:GraphObject(imageID, startX, startY, dir), m_world(world), m_alive(true)
	{
		setVisible(true);
		setDs();
	}

	virtual void doSomething() = 0;
	virtual bool canBeSteppedOn() const = 0;

	StudentWorld* getWorld() const;
	bool isAlive() const;
	void Die();

	int getDx() const;
	int getDy() const;
	void setDs();

	virtual ~Actor()
	{ getGraphObjects().erase(this); }

private:
	bool m_alive;
	StudentWorld* m_world;
	int m_dx;
	int m_dy;
};

inline StudentWorld* Actor::getWorld() const
{ return m_world; }

inline bool Actor::isAlive() const
{ return m_alive; }

inline void Actor::Die()
{ m_alive = false; }

inline int Actor::getDx() const
{ return m_dx; }

inline int Actor::getDy() const
{ return m_dy; }

//ENTITY//
class Entity : public Actor	//includes players, robots, and boulders
{
public:
	Entity(int imageID, int startX, int startY, StudentWorld* world, int health, Direction dir = none)
		:Actor(imageID, startX, startY, world, dir), m_health(health)
	{}

	virtual void doSomething() = 0;
	virtual void onHit() = 0;
	virtual bool canMove(const int& x, const int& y) const = 0;	//if specified point is available for movement
	virtual bool canBeSteppedOn() const;

	void getHit();
	void setHealth(const int& health);
	int getHealth() const;

	virtual ~Entity()
	{ getGraphObjects().erase(this); }

private:
	int m_health;
};

inline bool Entity::canBeSteppedOn() const
{ return false; }

inline void Entity::getHit()
{ m_health -= 2; }

inline void Entity::setHealth(const int& health)
{ m_health = health; }

inline int Entity::getHealth() const
{ return m_health; }

//ROBOT//
class Robot : public Entity	//includes Snarlbots, Kleptobots, and Angry Kleptobots
{
public:
	Robot(int imageID, int startX, int startY, StudentWorld* world, int health, int score, Direction dir);

	virtual void doSomething() = 0;
	virtual void onHit() = 0;
	virtual bool canMove(const int& x, const int& y) const;

	bool nothingInTheWay(const int& pX, const int& pY, const std::vector<Actor*>& actors) const;
	bool decideToFire();

	int getTickCap() const;
	int getTicks() const;
	void setTicks(const int& ticks);
	int getScore() const;

	virtual ~Robot()
	{ getGraphObjects().erase(this); }

private:
	int m_tickCap;
	int m_ticks;
	int m_scoreAward;
};

inline void Robot::setTicks(const int& ticks)
{ m_ticks = ticks; }

inline int Robot::getTickCap() const
{ return m_tickCap; }

inline int Robot::getTicks() const
{ return m_ticks; }

inline int Robot::getScore() const
{ return m_scoreAward; }

//GOODIE//
class Goodie : public Actor	//includes Jewels, Lives, Health, and Ammo
{
public:
	Goodie(int imageID, int startX, int startY, StudentWorld* world)
		:Actor(imageID, startX, startY, world)
	{}

	virtual void doSomething() = 0;
	virtual bool canBeSteppedOn() const;

	virtual ~Goodie()
	{ getGraphObjects().erase(this); }
};

inline bool Goodie::canBeSteppedOn() const
{ return true; }


///////////////////////////////////
//////////DERIVED CLASSES//////////
///////////////////////////////////
//WALL//
class Wall : public Actor
{
public:
	Wall(int startX, int startY, StudentWorld* world)
		:Actor(IID_WALL, startX, startY, world)
	{}

	virtual void doSomething();
	virtual bool canBeSteppedOn() const;

	virtual ~Wall()
	{ getGraphObjects().erase(this); }
};

inline void Wall::doSomething()
{ return; }

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

//EXIT//
class Exit : public Actor
{
public:
	Exit(int startX, int startY, StudentWorld* world)
		:Actor(IID_EXIT, startX, startY, world)
	{ setVisible(false); }

	virtual void doSomething();
	virtual bool canBeSteppedOn() const;

	virtual ~Exit()
	{ getGraphObjects().erase(this); }
};

inline bool Exit::canBeSteppedOn() const
{ return true; }

//FACTORY//
class Factory : public Actor
{
public:
	Factory(int starX, int startY, StudentWorld* world, bool angry)
		:Actor(IID_ROBOT_FACTORY, starX, startY, world), m_angry(angry)
	{}

	virtual void doSomething();
	virtual bool canBeSteppedOn() const;

	void damageBotOnTop() const;

	virtual ~Factory()
	{ getGraphObjects().erase(this); }

private:
	bool m_angry;
};

inline bool Factory::canBeSteppedOn() const
{ return false; }

//PLAYER//
class Player : public Entity
{
public:
	Player(int startX, int startY, StudentWorld* world)
		:Entity(IID_PLAYER, startX, startY, world, 20, right), m_ammo(20)
	{}

	virtual void doSomething();
	virtual void onHit();

	void shoot();
	void restoreHealth();
	void gotAmmo();

	virtual bool canMove(const int& x, const int& y) const;
	int getAmmo() const;

	virtual ~Player()
	{ getGraphObjects().erase(this); }

private:
	int m_ammo;
};

inline void Player::restoreHealth()
{ setHealth(20); }

inline void Player::gotAmmo()
{ m_ammo += 20; }

inline int Player::getAmmo() const
{ return m_ammo; }

//BOULDER//
class Boulder : public Entity
{
public:
	Boulder(int startX, int startY, StudentWorld* world)
		:Entity(IID_BOULDER, startX, startY, world, 10)
	{}

	virtual void doSomething();
	virtual void onHit();
	virtual bool canMove(const int& x, const int& y) const;

	void push(const int& x, const int& y);

	virtual ~Boulder()
	{ getGraphObjects().erase(this); }
};

inline void Boulder::doSomething()
{ return; }

//SNARLBOT//
class Snarlbot : public Robot
{
public:
	Snarlbot(int startX, int startY, StudentWorld* world, Direction dir)
		:Robot(IID_SNARLBOT, startX, startY, world, 10, 100, dir)
	{}

	void reverseDirection();

	virtual void doSomething();
	virtual void onHit();

	virtual ~Snarlbot()
	{ getGraphObjects().erase(this); }
};

//KLEPTOBOT//
class Kleptobot : public Robot	//includes Kleptobots and Angry Kleptobots
{
public:
	Kleptobot(int startX, int startY, StudentWorld* world, int health = 5, int imageID = IID_KLEPTOBOT, int score = 10)
		:Robot(imageID, startX, startY, world, health, score, right), m_dist(0), m_goodie('n'), m_turnDist((rand() % 6) + 1)
	{}

	void turn();
	bool attemptSteal();

	virtual void doSomething();
	virtual void onHit();

	int getTurnDist() const;
	void setTurnDist();
	int getDist() const;
	void setDist(const int& dist);

	virtual ~Kleptobot()
	{ getGraphObjects().erase(this); }

private:
	char m_goodie;
	int m_turnDist;
	int m_dist;
};

inline int Kleptobot::getTurnDist() const
{ return m_turnDist; }

inline void Kleptobot::setTurnDist()
{ m_turnDist = (rand() % 6) + 1; }

inline int Kleptobot::getDist() const
{ return m_dist; }

inline void Kleptobot::setDist(const int& dist)
{ m_dist = dist; }

//ANGRY KLEPTOBOT//
class AngryKlepto : public Kleptobot
{
public:
	AngryKlepto(int startX, int startY, StudentWorld* world)
		:Kleptobot(startX, startY, world, 8, IID_ANGRY_KLEPTOBOT, 20)
	{}

	virtual void doSomething();

	virtual ~AngryKlepto()
	{ getGraphObjects().erase(this); }
};


//JEWEL//
class Jewel : public Goodie
{
public:
	Jewel(int startX, int startY, StudentWorld* world)
		:Goodie(IID_JEWEL, startX, startY, world)
	{}

	virtual void doSomething();

	virtual ~Jewel()
	{ getGraphObjects().erase(this); }
};

//EXTRA LIFE//
class Life : public Goodie
{
public:
	Life(int startX, int startY, StudentWorld* world)
		:Goodie(IID_EXTRA_LIFE, startX, startY, world)
	{}

	virtual void doSomething();

	virtual ~Life()
	{ getGraphObjects().erase(this); }
};

//HEALTH//
class Health : public Goodie
{
public:
	Health(int startX, int startY, StudentWorld* world)
		:Goodie(IID_RESTORE_HEALTH, startX, startY, world)
	{}

	virtual void doSomething();

	virtual ~Health()
	{ getGraphObjects().erase(this); }
};

//AMMO//
class Ammo : public Goodie
{
public:
	Ammo(int startX, int startY, StudentWorld* world)
		:Goodie(IID_AMMO, startX, startY, world)
	{}

	virtual void doSomething();

	virtual ~Ammo()
	{ getGraphObjects().erase(this); }
};

#endif // ACTOR_H_
