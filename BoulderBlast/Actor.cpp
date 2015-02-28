#include <vector>
#include "Actor.h"
#include "StudentWorld.h"
using namespace std;

//////////////////////////////////////////////
//////////BASE CLASS IMPLEMENTATIONS//////////
//////////////////////////////////////////////
//ACTOR IMPLEMENTATION//
void Actor::setDs()	//sets the changes in x and y required to move in the current direction
{
	switch (getDirection())
	{
	case none:
		m_dx = 0, m_dy = 0;
		break;
	case up:
		m_dx = 0, m_dy = 1;
		break;
	case down:
		m_dx = 0, m_dy = -1;
		break;
	case left:
		m_dx = -1, m_dy = 0;
		break;
	case right:
		m_dx = 1, m_dy = 0;
		break;
	}
}

//ROBOT IMPLEMENTATIONS//
Robot::Robot(int imageID, int startX, int startY, StudentWorld* world, int health, int score, Direction dir)	//sets how many ticks a Robot "rests"
	:Entity(imageID, startX, startY, world, health, dir), m_ticks(0), m_scoreAward(score)						//before doingSomething
{
	m_tickCap = (28 - world->getLevel()) / 4;

	if (m_tickCap < 3)
		m_tickCap = 3;
}

bool Robot::nothingInTheWay(const int& pX, const int& pY, const vector<Actor*>& actors) const	//returns true if there is a clear line of sight from
{																								//this to the Player at (x,y)
	Wall* wall;
	Boulder* boulder;
	Entity* entity;
	Factory* factory;

	for (int i = 0; i < actors.size(); i++)
	{
		wall = dynamic_cast<Wall*>(actors[i]);
		boulder = dynamic_cast<Boulder*>(actors[i]);
		entity = dynamic_cast<Entity*>(actors[i]);	
		factory = dynamic_cast<Factory*>(actors[i]);

		if (wall != nullptr || boulder != nullptr || entity != nullptr || factory != nullptr)
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

bool Robot::decideToFire()		//returns true and fires at the Player if it is within line of sight of the Robot
{
	Player* player = getWorld()->getPlayer();
	bool fire = false;

	switch (getDirection())
	{
	case up:
		if (player->getX() == getX() && player->getY() > getY())
			fire = nothingInTheWay(player->getX(), player->getY(), *getWorld()->getActors());
		break;
	case down:
		if (player->getX() == getX() && player->getY() < getY())
			fire = nothingInTheWay(player->getX(), player->getY(), *getWorld()->getActors());
		break;
	case left:
		if (player->getX() < getX() && player->getY() == getY())
			fire = nothingInTheWay(player->getX(), player->getY(), *getWorld()->getActors());
		break;
	case right:
		if (player->getX() > getX() && player->getY() == getY())
			fire = nothingInTheWay(player->getX(), player->getY(), *getWorld()->getActors());
		break;
	}

	return fire;
}

bool Robot::canMove(const int& x, const int& y) const	//returns true if the movement to (x,y) is unobstructed
{
	if (x > 15 || x < 0 || y > 15 || y < 0)
		return false;

	Player* player = getWorld()->getPlayer();
	vector<Actor*>* actors = getWorld()->getActors();

	if (player->getX() == x && player->getY() == y)
		return false;
	for (int i = 0; i < actors->size(); i++)
	{
		if (actors->at(i)->getX() == x && actors->at(i)->getY() == y)
		{
			if (!actors->at(i)->canBeSteppedOn())
				return false;
		}
	}

	return true;
}

/////////////////////////////////////////////////
//////////DERIVED CLASS IMPLEMENTATIONS//////////
/////////////////////////////////////////////////
//BULLET IMPLEMENTATIONS//
bool Bullet::doBullet()	//returns false if dead, otherwise checks for actors that can be hit at current location and hits them
{
	vector<Actor*>* actors = getWorld()->getActors();

	for (int i = 0; i < actors->size(); i++)
	{
		Wall* wall = dynamic_cast<Wall*>(actors->at(i));
		Entity* organism = dynamic_cast<Entity*>(actors->at(i));
		Factory* factory = dynamic_cast<Factory*>(actors->at(i));

		if (actors->at(i)->getX() == getX() && actors->at(i)->getY() == getY())
		{
			if (organism != nullptr)
			{
				organism->onHit();
				Die();
				return false;
			}
			else if (wall != nullptr)
			{
				Die();
				return false;
			}
			else if (factory != nullptr)
			{
				factory->damageBotOnTop();
				Die();
				return false;
			}
		}
	}

	Player* player = getWorld()->getPlayer();

	if (player->getX() == getX() && player->getY() == getY())
	{
		player->onHit();
		Die();
		return false;
	}
	return true;
}

void Bullet::doSomething()	//if alive, moves and hits any actors; if still alive, tries to hit actors again
{
	if (!isAlive())
		return;
	
	bool doAgain = doBullet();
	moveTo(getX() + getDx(), getY() + getDy());

	if (doAgain)
		doBullet();
}

//HOLE IMPLEMENTATION//
void Hole::doSomething()	//if alive, checks for a boulder at current location to swallow
{
	if (!isAlive())
		return;

	vector<Actor*>* actors = getWorld()->getActors();

	for (int i = 0; i < actors->size(); i++)
	{
		if (actors->at(i)->getX() == getX() && actors->at(i)->getY() == getY())
		{
			Boulder* boulder = dynamic_cast<Boulder*>(actors->at(i));
			if (boulder != nullptr)
			{
				boulder->Die();
				Die();
			}
		}
	}
}

//EXIT IMPLEMENTATION//
void Exit::doSomething()	//if the Player is on the Exit and the Exit is visible, wins the level; else, makes itself visible if no Jewels remain
{
	if (getWorld()->getJewels() > 0)
		return;
	else if (!isVisible())
	{
		setVisible(true);
		getWorld()->playSound(SOUND_REVEAL_EXIT);
	}

	if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
	{
		getWorld()->increaseScore(2000);
		getWorld()->playSound(SOUND_FINISHED_LEVEL);
		getWorld()->setAdvanceLevel();
	}
}

//FACTORY IMPLEMENTATIONS//
void Factory::doSomething()	//if < 3 Kleptobots in 3x3 box around this and no Kleptobots are on the same space as this, has a 1/50 chance of spawning
{							//a Kleptobot, depending on its type (angry/normal)
	vector<Actor*>* actors = getWorld()->getActors();
	int kleptoKount = 0;
	bool kleptoOnTop = false;

	for (int i = 0; i < actors->size(); i++)
	{
		if (actors->at(i)->getX() <= getX() + 3 && actors->at(i)->getX() >= getX() - 3 &&
			actors->at(i)->getY() <= getY() + 3 && actors->at(i)->getY() >= getY() - 3)
		{

			Kleptobot* klepto = dynamic_cast<Kleptobot*>(actors->at(i));

			if (klepto != nullptr)
			{
				kleptoKount++;

				if (klepto->getX() == getX() && klepto->getY() == getY())
					kleptoOnTop = true;
			}
		}
	}

	int spawnChance = rand() % 50;

	if (kleptoKount < 3 && !kleptoOnTop && spawnChance == 0)
	{
		getWorld()->playSound(SOUND_ROBOT_BORN);

		switch (m_angry)
		{
		case true: getWorld()->createKlepto(getX(), getY(), true); break;
		case false: getWorld()->createKlepto(getX(), getY(), false); break;
		}
	}
}

void Factory::damageBotOnTop() const	//checks for Kleptobots on top of the Factory, damages one if there are any
{
	vector<Actor*>* actors = getWorld()->getActors();

	for (int i = 0; i < actors->size(); i++)
	{
		if (actors->at(i)->getX() == getX() && actors->at(i)->getY() == getY())
		{
			Entity* entity = dynamic_cast<Entity*>(actors->at(i));

			if (entity != nullptr)
				entity->onHit();
		}
	}
}

//PLAYER IMPLEMENTATIONS//
void Player::shoot()	//shoots a bullet and decrements ammo, if possible
{
	if (m_ammo < 1)
		return;

	getWorld()->playSound(SOUND_PLAYER_FIRE);
	m_ammo--;
	getWorld()->createBullet(getX() + getDx(), getY() + getDy(), getDirection());
}

void Player::doSomething()	//checks for key input then does nothing, moves, shoots, or resets the level
{
	if (!isAlive())
		return;

	int key;
	bool moved = false;

	if (getWorld()->getKey(key))
	{
		switch (key)
		{
		case KEY_PRESS_ESCAPE:
			Die();
			break;
		case KEY_PRESS_SPACE:
			shoot();
			break;
		case KEY_PRESS_UP:
			if (getDirection() != up)
			{
				setDirection(up);
				setDs();
			}
			moved = true;
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
			{
				setDirection(down);
				setDs();
			}
			moved = true;
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
			{
				setDirection(left);
				setDs();
			}
			moved = true;
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
			{
				setDirection(right);
				setDs();
			}
			moved = true;
			break;
		default:
			break;
		}

		if (moved && canMove(getX() + getDx(), getY() + getDy()))
			moveTo(getX() + getDx(), getY() + getDy());
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
		Die();
	}
}

bool Player::canMove(const int& x, const int& y) const	//moves to x, y if not stopped by a robot, wall, or unpushable boulder
{	
	if (x > 15 || x < 0 || y > 15 || y < 0)
		return false;

	vector<Actor*>* actors = getWorld()->getActors();

	for (int i = 0; i < actors->size(); i++)
	{
		if (actors->at(i)->getX() == x && actors->at(i)->getY() == y)
		{
			Boulder* boulder = dynamic_cast<Boulder*>(actors->at(i));

			if (boulder != nullptr)
			{
				if (boulder->canMove(x + getDx(), y + getDy()))
				{
					boulder->push(x + getDx(), y + getDy());
					return true;
				}
				else
					return false;
			}
			else if (!actors->at(i)->canBeSteppedOn())
				return false;
		}
	}

	return true;
}

//BOULDER IMPLEMENTATIONS//
void Boulder::push(const int& x, const int& y)
{
	moveTo(x, y);
}

void Boulder::onHit()
{
	getHit();

	if (getHealth() <= 0)
		Die();
}

bool Boulder::canMove(const int& x, const int& y) const	//returns true if (x,y) is empty or contains a hole
{
	if (x > 15 || x < 0 || y > 15 || y < 0)
		return false;

	vector<Actor*>* actors = getWorld()->getActors();

	for (int i = 0; i < actors->size(); i++)
	{
		Hole* hole = dynamic_cast<Hole*>(actors->at(i));

		if (actors->at(i)->getX() == x && actors->at(i)->getY() == y)
		{
			return hole != nullptr;
		}
	}
	return true;
}

//SNARLBOT IMPLEMENTATIONS//
void Snarlbot::reverseDirection()	//switches to opposite Direction from current
{
	switch (getDirection())
	{
	case up:
		setDirection(down);
		setDs();
		break;
	case down:
		setDirection(up);
		setDs();
		break;
	case left:
		setDirection(right);
		setDs();
		break;
	case right:
		setDirection(left);
		setDs();
		break;
	}
}

void Snarlbot::doSomething()	//if alive and not resting, fires at the Player if in line of sight, or attempts to move
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

	if (fired)
	{
		getWorld()->playSound(SOUND_ENEMY_FIRE);
		getWorld()->createBullet(getX() + getDx(), getY() + getDy(), getDirection());
	}
	else
	{
		if (canMove(getX() + getDx(), getY() + getDy()))
			moveTo(getX() + getDx(), getY() + getDy());
		else
			reverseDirection();
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
		getWorld()->increaseScore(getScore());
		Die();
	}
}

//KLEPTOBOT IMPLEMENTATIONS//
void Kleptobot::turn()	//picks a new distance before turning and a new direction to move in and moves one space if possible
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
				setDs();
				return;
			}
			else if (!checked[0] && canMove(getX(), getY() + 1))
			{
				setDirection(up);
				setDs();
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
				setDs();
				return;
			}
			else if (!checked[1] && canMove(getX(), getY() - 1))
			{
				setDirection(down);
				setDs();
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
				setDs();
				return;
			}
			else if (!checked[2] && canMove(getX() - 1, getY()))
			{
				setDirection(left);
				setDs();
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
				setDs();
				return;
			}
			else if (!checked[3] && canMove(getX() + 1, getY()))
			{
				setDirection(right);
				setDs();
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

bool Kleptobot::attemptSteal()	//attempts to steal a Goodie with 1/10 chance if one is on the current space, returns true if successful
{
	if (m_goodie != 'n')
		return false;

	vector<Actor*>* actors = getWorld()->getActors();

	for (int i = 0; i < actors->size(); i++)
	{
		if (actors->at(i)->getX() == getX() && actors->at(i)->getY() == getY())
		{
			int decider = rand() % 10;
			Life* l = dynamic_cast<Life*>(actors->at(i));
			Health* h = dynamic_cast<Health*>(actors->at(i));
			Ammo* a = dynamic_cast<Ammo*>(actors->at(i));

			if (decider == 0 && (l != nullptr || h !=nullptr || a !=nullptr))
			{
				actors->at(i)->Die();
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

void Kleptobot::doSomething()	//if alive and not resting, attempts to steal; if unsuccessful, attempts to move
{
	if (!isAlive())
		return;
	else if (getTicks() < getTickCap())
	{
		setTicks(getTicks() + 1);
		return;
	}
	else if (attemptSteal())
	{
		setTicks(1);
		return;
	}
	setTicks(1);

	if (getDist() < getTurnDist())
	{
		if (canMove(getX() + getDx(), getY() + getDy()))
		{
			moveTo(getX() + getDx(), getY() + getDy());
			setDist(getDist() + 1);
		}
		else
			turn();
	}
	else
		turn();
}

void Kleptobot::onHit()	//drops any carried goodies on death
{
	getHit();

	if (getHealth() > 0)
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
	else
	{
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(getScore());
		getWorld()->dropGoodie(getX(), getY(), m_goodie);
		Die();
	}
}

//ANGRY KLEPTOBOT IMPLEMENTATION//
void AngryKlepto::doSomething()	//same as Kleptobot doSomething() function except also attempts to shoot before attempting to steal or move
{
	if (!isAlive())
		return;

	if (getTicks() < getTickCap())
	{
		setTicks(getTicks() + 1);
		return;
	}

	bool fired = decideToFire();

	if (!fired && attemptSteal())
	{
		setTicks(1);
		return;
	}

	setTicks(1);

	if (fired)
	{
		getWorld()->playSound(SOUND_ENEMY_FIRE);
		getWorld()->createBullet(getX() + getDx(), getY() + getDy(), getDirection());
	}
	else
	{
		if (getDist() < getTurnDist())
		{
			if (canMove(getX() + getDx(), getY() + getDy()))
			{
				moveTo(getX() + getDx(), getY() + getDy());
				setDist(getDist() + 1);
			}
			else
				turn();
		}
		else
			turn();
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
		Die();
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
		Die();
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
		Die();
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
		Die();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->gotAmmo();
	}
}