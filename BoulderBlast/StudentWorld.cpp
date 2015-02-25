#include "StudentWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <stack>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{ return new StudentWorld(assetDir); }

string StudentWorld::formatLevel(const int& lev)	//used for loading each new level
{
	if (lev > 99)
		return "level100.dat";

	string dat = "level";

	ostringstream oss;
	oss.fill('0');
	oss << setw(2) << lev;
	dat += oss.str();

	dat += ".dat";
	return dat;
}

string StudentWorld::formatDisplayText(const int& score, const int& level, const int& lives, const int& health, const int& ammo, const int& bonus)
{
	string display = "Score: ";

	ostringstream oss; 
	oss.fill('0');
	oss << setw(7) << score << "  Level: " << setw(2) << level << "  Lives: ";
	oss.fill(' ');

	double healthPD = (double)health / 20 * 100;
	int healthPI = (int)healthPD;
	oss << setw(2) << lives << "  Health: " << setw(3) << healthPI << "%  Ammo: " << setw(3) << ammo << "  Bonus: " << setw(4) << bonus;
	
	display += oss.str();
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

void StudentWorld::dropGoodie(const int& x, const int& y, const char& goodie)
{
	switch (goodie)
	{
	case 'n':
		return;
	case 'l':
		m_actors.push_back(new Life(x, y, this));
		break;
	case 'h':
		m_actors.push_back(new Health(x, y, this));
		break;
	case 'a':
		m_actors.push_back(new Ammo(x, y, this));
		break;
	}
}

int StudentWorld::init()	//loads current level and creates starting actors
{							//TODO: create all actors after implementing them
	m_bonus = 1000;
	m_nextLev = false;
	srand(static_cast<unsigned int>(time(nullptr)));
	if (getLevel() == 5) return GWSTATUS_FINISHED_LEVEL;
	Level lev(assetDirectory());
	Level::LoadResult result = lev.loadLevel(formatLevel(getLevel()));

	if (result == Level::load_fail_file_not_found)
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
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
			case Level::jewel:
				m_actors.push_back(new Jewel(x, y, this));
				m_nJewels++;
				break;
			case Level::exit:
				m_actors.push_back(new Exit(x, y, this));
				break;
			case Level::extra_life:
				m_actors.push_back(new Life(x, y, this));
				break;
			case Level::restore_health:
				m_actors.push_back(new Health(x, y, this));
				break;
			case Level::ammo:
				m_actors.push_back(new Ammo(x, y, this));
				break;
			case Level::horiz_snarlbot:
				m_actors.push_back(new Kleptobot(x, y, this));
				break;
			case Level::vert_snarlbot:
				m_actors.push_back(new Snarlbot(x, y, this, GraphObject::down));
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

	for (int i = 0; i < m_actors.size(); i++)		//allow all actors to doSomething
	{
		if (m_actors[i]->isAlive())
			m_actors[i]->doSomething();

		if (!m_player->isAlive())
		{
			decLives();
			m_nJewels = 0;
			return GWSTATUS_PLAYER_DIED;
		}
		else if (m_nextLev)
		{
			increaseScore(2000 + m_bonus);
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}
	}
	m_player->doSomething();

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

