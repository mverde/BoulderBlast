#include "StudentWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <stack>
#include <cmath>
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{ return new StudentWorld(assetDir); }

string StudentWorld::formatLevel(const int& lev)	//used for loading each new level
{
	string dat = "level";
	if (lev < 10)
	{
		dat += '0';
		dat += lev + TO_CHAR;
	}
	else
	{
		dat += (lev / 10) + TO_CHAR;
		dat += lev % 10 + TO_CHAR;
	}
	dat += ".dat";

	return dat;
}

string StudentWorld::formatNum(const int& num, const int& digits, const bool& spaces)	//used for displaying stats
{
	stack<char> chars;
	string str;
	int upper = (int)pow(10, digits + 1);

	for (int mod = 10, div = 1; mod < upper && div < upper; mod *= 10, div *= 10)	//format by converting each digit to char
	{
		chars.push(((num % mod) / div) + TO_CHAR);
	}
	for (int i = 0; i < digits; i++)
	{
		str += chars.top();
		chars.pop();
	}

	if (spaces)		//change leading 0s to leading spaces if specified
	{
		bool g0Found = false;
		int i = 0;

		while (!g0Found)
		{
			if (str[i] > '0' && str[i] <= '9' || i == str.length() - 1)
				g0Found = true;
			else
				str[i] = ' ';
			i++;
		}
	}

	return str;
}

string StudentWorld::formatDisplayText(const int& score, const int& level, const int& lives, const int& health, const int& ammo, const int& bonus)
{
	string display = "Score: ";
	string next = formatNum(score, 7, false);

	display += next + "  Level: ";
	next = formatNum(level, 2, false);

	display += next + "  Lives: ";
	next = formatNum(lives, 2, true);

	display += next + "  Health: ";
	double healthPD = (double)health / 20 * 100;
	int healthPI = (int)healthPD;
	next = formatNum(healthPI, 3, true);

	display += next + "%  Ammo: ";
	next = formatNum(ammo, 3, true);

	display += next + "  Bonus: ";
	next = formatNum(m_bonus, 4, true);
	display += next;

	return display;
}

void StudentWorld::setDisplayText()	//gets stats, formats display text, and displays it
{
	int score = getScore();
	int level = getLevel();
	int lives = getLives();
	int health = m_player->getHealth();
	int ammo = m_player->getAmmo();

	string str = formatDisplayText(score, level, lives, health, ammo, m_bonus);
	setGameStatText(str);
}

void StudentWorld::deleteDead()
{
	for (int i = 0; i < m_actors.size(); i++)
	{
		if (!m_actors[i]->isAlive())
		{
			delete m_actors[i];
			m_actors.erase(m_actors.begin() + i);
			i--;
		}
	}
}

void StudentWorld::createBullet(const int& x, const int& y, StudentWorld* world, const GraphObject::Direction& dir)
{
	m_actors.push_back(new Bullet(x, y, this, dir));
}

int StudentWorld::init()	//loads current level and creates starting actors
{							//TODO: create all actors after implementing them
	m_bonus = 1000;
	Level lev(assetDirectory());
	Level::LoadResult result = lev.loadLevel(formatLevel(getLevel()));

	if (result == Level::load_fail_file_not_found || result == Level::load_fail_bad_format)
		return GWSTATUS_LEVEL_ERROR;	//bad level

	for (int x = 0; x < VIEW_WIDTH; x++)	//use switch to allocate all Actors and store in m_actors
	{
		for (int y = 0; y < VIEW_HEIGHT; y++)
		{
			Level::MazeEntry item = lev.getContentsOf(x, y);

			switch (item)
			{
			case Level::wall:
				m_actors.push_back(new Wall(x, y, this));
				break;
			case Level::player:
				m_player = new Player(x, y, this);
				break;
			case Level::boulder:
				m_actors.push_back(new Boulder(x, y, this));
				break;
			case Level::hole:
				m_actors.push_back(new Hole(x, y, this));
				break;
			default:
				break;
			}
		}
	}

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()	//plays a tick
{							//TODO: implement exiting the level when all jewels are collected
	setDisplayText();

	m_player->doSomething();	//allow all actors to doSomething

	for (int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->isAlive())
			m_actors[i]->doSomething();

		if (!m_player->isAlive())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
	}

	deleteDead();
	if (m_bonus > 0)
		m_bonus--;

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()	//necessary to deallocate memory when a level is finished but the game is not over
{
	delete m_player;
	for (int i = 0; i < m_actors.size();)
	{
		delete m_actors[i];
		m_actors.erase(m_actors.begin());
	}
}

StudentWorld::~StudentWorld()
{
	delete m_player;
	for (int i = 0; i < m_actors.size();)
	{
		delete m_actors[i];
		m_actors.erase(m_actors.begin());
	}
}

