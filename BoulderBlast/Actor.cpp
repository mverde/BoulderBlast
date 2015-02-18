#include <vector>
#include "Actor.h"
#include "StudentWorld.h"
using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//PLAYER IMPLEMENTATIONS//
void Player::doSomething()	//TODO: implement shooting, dying, picking up items, pushing boulders
{
	if (!isAlive())
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
			getWorld()->playSound(SOUND_PLAYER_FIRE);
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

void Player::onHit()
{
	return;
}

bool Player::canMove(int x, int y) const
{
	vector<Actor*> actors = getWorld()->getActors();
	
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->getX() == x && actors[i]->getY() == y)
			return false;
	}
	return true;
}