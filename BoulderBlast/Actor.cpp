#include <vector>
#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;

//////////////////////////////////////////////
//////////BASE CLASS IMPLEMENTATIONS//////////
//////////////////////////////////////////////
//ROBOT IMPLEMENTATIONS//
Robot::Robot(int startID, int startX, int startY, StudentWorld* world, int health, Direction dir)
	:Entity(startID, startX, startY, world, health, dir), m_ticks(0)
{
	m_tickCap = (28 - world->getLevel()) / 4;

	if (m_tickCap < 3)
		m_tickCap = 3;
}

bool Robot::nothingInTheWay(const int& pX, const int& pY, const vector<Actor*>& actors) const
{
	Wall* wall;
	Boulder* boulder;
	Entity* entity;

	for (int i = 0; i < actors.size(); i++)
	{
		wall = dynamic_cast<Wall*>(actors[i]);
		boulder = dynamic_cast<Boulder*>(actors[i]);
		entity = dynamic_cast<Entity*>(actors[i]);

		if (wall != nullptr || boulder != nullptr || entity != nullptr)
		{
			switch (getDirection())
			{
			case up:
				if (actors[i]->getX() == pX && actors[i]->getY() > getY() && actors[i]->getY() < pY)
					return false;
				break;
			case down:
				if (actors[i]->getX() == pX && actors[i]->getY() < getY() && actors[i]->getY() > pY)
					return false;
				break;
			case left:
				if (actors[i]->getY() == pY && actors[i]->getX() < getX() && actors[i]->getX() > pX)
					return false;
				break;
			case right:
				if (actors[i]->getY() == pY && actors[i]->getX() > getX() && actors[i]->getX() < pX)
					return false;
				break;
			}
		}
	}

	return true;
}

bool Robot::decideToFire()
{
	Player* player = getWorld()->getPlayer();
	bool fire = false;

	switch (getDirection())
	{
	case up:
		if (player->getX() == getX() && player->getY() > getY())
			fire = nothingInTheWay(player->getX(), player->getY(), getWorld()->getActors());
		break;
	case down:
		if (player->getX() == getX() && player->getY() < getY())
			fire = nothingInTheWay(player->getX(), player->getY(), getWorld()->getActors());
		break;
	case left:
		if (player->getX() < getX() && player->getY() == getY())
			fire = nothingInTheWay(player->getX(), player->getY(), getWorld()->getActors());
		break;
	case right:
		if (player->getX() > getX() && player->getY() == getY())
			fire = nothingInTheWay(player->getX(), player->getY(), getWorld()->getActors());
		break;
	}

	return fire;
}

bool Robot::canMove(const int& x, const int& y) const
{
	if (x > 15 || x < 0 || y > 15 || y < 0)
		return false;

	Player* player = getWorld()->getPlayer();
	vector<Actor*> actors = getWorld()->getActors();

	if (player->getX() == x && player->getY() == y)
		return false;
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->getX() == x && actors[i]->getY() == y)
		{
			if (!actors[i]->canBeSteppedOn())
				return false;
		}
	}

	return true;
}

/////////////////////////////////////////////////
//////////DERIVED CLASS IMPLEMENTATIONS//////////
/////////////////////////////////////////////////
//BULLET IMPLEMENTATIONS//
bool Bullet::doBullet()	//returns false if dead, otherwise checks for actors at current location and acts appropriately
{
	vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		Wall* wall = dynamic_cast<Wall*>(actors[i]);	//TODO: add factory case
		Entity* organism = dynamic_cast<Entity*>(actors[i]);

		if (actors[i]->getX() == getX() && actors[i]->getY() == getY())
		{
			if (organism != nullptr)
			{
				organism->onHit();
				setDead();
				return false;
			}
			else if (wall != nullptr)
			{
				setDead();
				return false;
			}
		}
	}

	Player* player = getWorld()->getPlayer();
	if (player->getX() == getX() && player->getY() == getY())
	{
		player->getHit();
		player->onHit();
		setDead();
		return false;
	}
	return true;
}

void Bullet::doSomething()	//if alive, moves and hits actors if applicable
{
	if (!isAlive())
		return;
	
	bool doAgain = doBullet();
	switch (getDirection())
	{
	case up:
		moveTo(getX(), getY() + 1);
		break;
	case down:
		moveTo(getX(), getY() - 1);
		break;
	case left:
		moveTo(getX() - 1, getY());
		break;
	case right:
		moveTo(getX() + 1, getY());
		break;
	}

	if (doAgain)
		doBullet();
}

//HOLE IMPLEMENTATION//
void Hole::doSomething()	//if alive, checks for a boulder at current location to swallow
{
	if (!isAlive())
		return;

	vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->getX() == getX() && actors[i]->getY() == getY())
		{
			Boulder* boulder = dynamic_cast<Boulder*>(actors[i]);
			if (boulder != nullptr)
			{
				boulder->setDead();
				setDead();
			}
		}
	}
}

//EXIT IMPLEMENTATION//
void Exit::doSomething()
{
	if (getWorld()->getJewels() > 0)
		return;
	else if (!isVisible())
	{
		setVisible(true);
		getWorld()->playSound(SOUND_REVEAL_EXIT);
	}

	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
		getWorld()->setAdvanceLevel();
}

//PLAYER IMPLEMENTATIONS//
void Player::shoot(const int& x, const int& y, Direction dir)	//shoots a bullet and decrements ammo, if possible
{
	if (m_ammo < 1)
		return;

	getWorld()->playSound(SOUND_PLAYER_FIRE);
	m_ammo--;
	getWorld()->createBullet(x, y, getWorld(), dir);
}

void Player::doSomething()	//checks for key input then does nothing, moves, shoots, or restarts
{
	if (!isAlive())	//TODO: implement dying, picking up items
		return;

	int key;
	if (getWorld()->getKey(key))
	{
		switch (key)
		{
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_SPACE:
			switch (getDirection())
			{
			case up:
				shoot(getX(), getY() + 1, up);
				break;
			case down:
				shoot(getX(), getY() - 1, down);
				break;
			case left:
				shoot(getX() - 1, getY(), left);
				break;
			case right:
				shoot(getX() + 1, getY(), right);
				break;
			}
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (canMove(getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (canMove(getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			break;
		case KEY_PRESS_LEFT:
			setDirection(left);
			if (canMove(getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if (canMove(getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			break;
		default:
			break;
		}
	}
}

void Player::onHit()	//gets hit or dies based on remaining health
{
	getHit();

	if (getHealth() > 0)
		getWorld()->playSound(SOUND_PLAYER_IMPACT);
	else
	{
		getWorld()->playSound(SOUND_PLAYER_DIE);
		setDead();
	}
}

bool Player::canMove(const int& x, const int& y) const	//moves to x, y if not stopped by a robot, wall, or unpushable boulder
{	
	if (x > 15 || x < 0 || y > 15 || y < 0)	//TODO: check for robots as well
		return false;

	vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->getX() == x && actors[i]->getY() == y)
		{
			Boulder* boulder = dynamic_cast<Boulder*>(actors[i]);
			bool moveB = false;

			if (boulder != nullptr)
			{
				switch (getDirection())
				{
				case up:
					moveB = boulder->canMove(x, y + 1);
					break;
				case down:
					moveB = boulder->canMove(x, y - 1);
					break;
				case left:
					moveB = boulder->canMove(x - 1, y);
					break;
				case right:
					moveB = boulder->canMove(x + 1, y);
					break;
				}

				if (moveB)
				{
					boulder->push(x, y, getDirection());
					return true;
				}
				else
					return false;
			}
			else if (!actors[i]->canBeSteppedOn())
				return false;
		}
	}

	return true;
}

//BOULDER IMPLEMENTATIONS//
void Boulder::onHit()
{
	getHit();

	if (getHealth() <= 0)
		setDead();
}

bool Boulder::canMove(const int& x, const int& y) const	//returns true if x, y is empty or contains a hole
{
	if (x > 15 || x < 0 || y > 15 || y < 0)
		return false;

	vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		Hole* hole = dynamic_cast<Hole*>(actors[i]);
		if (actors[i]->getX() == x && actors[i]->getY() == y)
		{
			if (hole != nullptr)
				return true;
			return false;
		}
	}

	return true;
}

void Boulder::push(const int& x, const int& y, const Direction& dir)	//moves in specified direction (called only by player)
{
	switch (dir)
	{
	case up:
		moveTo(x, y + 1);
		break;
	case down:
		moveTo(x, y - 1);
		break;
	case left:
		moveTo(x - 1, y);
		break;
	case right:
		moveTo(x + 1, y);
		break;
	}
}

//SNARLBOT IMPLEMENTATIONS//
void Snarlbot::reverseDirection()
{
	switch (getDirection())
	{
	case up:
		setDirection(down);
		break;
	case down:
		setDirection(up);
		break;
	case left:
		setDirection(right);
		break;
	case right:
		setDirection(left);
		break;
	}
}

void Snarlbot::doSomething()
{
	if (!isAlive())
		return;
	else if (getTicks() < getTickCap())
	{
		setTicks(getTicks() + 1);
		return;
	}
	setTicks(1);

	bool fired = decideToFire();

	switch (getDirection())
	{
	case up:
		if (fired)
		{
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			getWorld()->createBullet(getX(), getY() + 1, getWorld(), up);
		}
		else
		{
			if (canMove(getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			else
				reverseDirection();
		}
		break;
	case down:
		if (fired)
		{
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			getWorld()->createBullet(getX(), getY() - 1, getWorld(), down);
		}
		else
		{
			if (canMove(getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			else
				reverseDirection();
		}
		break;
	case left:
		if (fired)
		{
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			getWorld()->createBullet(getX() - 1, getY(), getWorld(), left);
		}
		else
		{
			if (canMove(getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			else
				reverseDirection();
		}
		break;
	case right:
		if (fired)
		{
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			getWorld()->createBullet(getX() + 1, getY(), getWorld(), right);
		}
		else
		{
			if (canMove(getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			else
				reverseDirection();
		}
		break;
	}
}

void Snarlbot::onHit()
{
	getHit();

	if (getHealth() > 0)
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
	else
	{
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(100);
		setDead();
	}
}

//KLEPTOBOT IMPLEMENTATIONS//
void Kleptobot::turn()
{
	setTurnDist();
	setDist(1);
	bool checked[] = { false, false, false, false };
	bool allChecked = false;
	int checking = rand() % 4;

	while (true)
	{
		switch (checking)	//up, down, left, right
		{
		case 0:
			if (allChecked)
			{
				setDirection(up);
				return;
			}
			else if (!checked[0] && canMove(getX(), getY() + 1))
			{
				setDirection(up);
				moveTo(getX(), getY() + 1);
				return;
			}
			else
			{
				checking = rand() % 4;
				checked[0] = true;
			}
			break;
		case 1:
			if (allChecked)
			{
				setDirection(down);
				return;
			}
			else if (!checked[1] && canMove(getX(), getY() - 1))
			{
				setDirection(down);
				moveTo(getX(), getY() - 1);
				return;
			}
			else
			{
				checking = rand() % 4;
				checked[1] = true;
			}
			break;
		case 2:
			if (allChecked)
			{
				setDirection(left);
				return;
			}
			else if (!checked[2] && canMove(getX() - 1, getY()))
			{
				setDirection(left);
				moveTo(getX() - 1, getY());
				return;
			}
			else
			{
				checking = rand() % 4;
				checked[2] = true;
			}
			break;
		case 3:
			if (allChecked)
			{
				setDirection(right);
				return;
			}
			else if (!checked[3] && canMove(getX() + 1, getY()))
			{
				setDirection(right);
				moveTo(getX() + 1, getY());
				return;
			}
			else
			{
				checking = rand() % 4;
				checked[3] = true;
			}
			break;
		}

		allChecked = true;
		for (int i = 0; i < 4; i++)
		{
			if (!checked[i])
			{
				allChecked = false;
				break;
			}
		}
	}
}

bool Kleptobot::attemptSteal()
{
	if (m_goodie != 'n')
		return false;

	vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->getX() == getX() && actors[i]->getY() == getY())
		{
			int decider = rand() % 10;
			Life* l = dynamic_cast<Life*>(actors[i]);
			Health* h = dynamic_cast<Health*>(actors[i]);
			Ammo* a = dynamic_cast<Ammo*>(actors[i]);

			if (decider == 0 && (l != nullptr || h !=nullptr || a !=nullptr))
			{
				actors[i]->setDead();
				getWorld()->playSound(SOUND_ROBOT_MUNCH);

				if (l != nullptr)
					m_goodie = 'l';
				else if (h != nullptr)
					m_goodie = 'h';
				else
					m_goodie = 'a';
				return true;
			}
		}
	}

	return false;
}

void Kleptobot::doSomething()
{
	if (!isAlive())
		return;
	else if (getTicks() < getTickCap())
	{
		setTicks(getTicks() + 1);
		return;
	}
	else if (attemptSteal())
		return;

	setTicks(1);

	switch (getDirection())
	{
	case up:
		if (getDist() < getTurnDist())
		{
			if (canMove(getX(), getY() + 1))
			{
				moveTo(getX(), getY() + 1);
				setDist(getDist() + 1);
			}
			else
				turn();
		}
		else
			turn();
		break;
	case down:
		if (getDist() < getTurnDist())
		{
			if (canMove(getX(), getY() - 1))
			{
				moveTo(getX(), getY() - 1);
				setDist(getDist() + 1);
			}
			else
				turn();
		}
		else
			turn();
		break;
	case left:
		if (getDist() < getTurnDist())
		{
			if (canMove(getX() - 1, getY()))
			{
				moveTo(getX() - 1, getY());
				setDist(getDist() + 1);
			}
			else
				turn();
		}
		else
			turn();
		break;
	case right:
		if (getDist() < getTurnDist())
		{
			if (canMove(getX() + 1, getY()))
			{
				moveTo(getX() + 1, getY());
				setDist(getDist() + 1);
			}
			else
				turn();
		}
		else
			turn();
		break;
	}
}

void Kleptobot::onHit()
{
	getHit();

	if (getHealth() > 0)
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
	else
	{
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(10);
		getWorld()->dropGoodie(getX(), getY(), m_goodie);
		setDead();
	}
}

//JEWEL IMPLEMENTATION//
void Jewel::doSomething()
{
	if (!isAlive())
		return;

	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
	{
		getWorld()->increaseScore(50);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->gotJewel();
	}
}

//EXTRA LIFE IMPLEMENTATION//
void Life::doSomething()
{
	if (!isAlive())
		return;

	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
	{
		getWorld()->increaseScore(1000);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->incLives();
	}
}

//RESTORE HEALTH IMPLEMENTATION//
void Health::doSomething()
{
	if (!isAlive())
		return;

	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
	{
		getWorld()->increaseScore(500);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->restoreHealth();
	}
}

//AMMO IMPLEMENTATION//
void Ammo::doSomething()
{
	if (!isAlive())
		return;

	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
	{
		getWorld()->increaseScore(100);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->gotAmmo();
	}
}