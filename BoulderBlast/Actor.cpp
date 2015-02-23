#include <vector>
#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;

//BULLET IMPLEMENTATIONS//
bool Bullet::doBullet()	//returns false if dead, otherwise checks for actors at current location and acts appropriately
{
	vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		Wall* wall = dynamic_cast<Wall*>(actors[i]);	//TODO: add factory case
		Organism* organism = dynamic_cast<Organism*>(actors[i]);

		if (actors[i]->getX() == getX() && actors[i]->getY() == getY())
		{
			if (organism != nullptr)
			{
				organism->getHit();
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

//HOLE IMPLEMENTATIONS//
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
			if (boulder != nullptr)
			{
				bool moveB;
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