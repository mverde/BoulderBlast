#include "StudentWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{ return new StudentWorld(assetDir); }

int StudentWorld::init()
{
	m_bonus = 1000;
	string curlevel = "level00.dat";	//placeholder for level progression
	Level lev(assetDirectory());
	Level::LoadResult result = lev.loadLevel(curlevel);

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
			default:
				break;
			}
		}
	}

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	string stats = "Score: 0321000  Level: 05  Lives:  3  Health:  70%  Ammo:  20  Bonus:  724";	//placeholder for stats update
	setGameStatText(stats);	//TODO: parse these values in correct format
	m_player->doSomething();	//allow all actors to doSomething
	if (m_bonus > 0)
		m_bonus--;

	vector<Actor*>::iterator it;
	it = m_actors.begin();
	while (it != m_actors.end())
	{
		if ((*it)->isAlive())
			(*it)->doSomething();

		if (!m_player->isAlive())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		it++;
	}

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
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

