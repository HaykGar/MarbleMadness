#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr), m_nCrystals(0), m_Bonus(0), m_grid()
{}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

void StudentWorld::GetLevelFileName(std::string& s)
{
    std::ostringstream currentLevel;

    currentLevel << "level" << setw(2);
    currentLevel.fill('0');
    currentLevel << getLevel() << ".txt";
    s = currentLevel.str();
}

int StudentWorld::init()
{
    if(getLevel() == 99)
        return GWSTATUS_PLAYER_WON;
    
    m_Bonus = LEVEL_START_BONUS;
    Level lev(assetPath());
    string currentLevel;
    GetLevelFileName(currentLevel);
    
    Level::LoadResult result = lev.loadLevel(currentLevel);
    if(result == Level::load_fail_file_not_found)
    {
        cerr << "Could not find " << currentLevel << " data file\n";    // no level files left, won game
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format)
    {
        cerr << "Your level was improperly formatted\n";
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level\n";
        for(int x = 0; x < VIEW_WIDTH; x++)
        {
            for(int y = 0; y < VIEW_HEIGHT; y++)
            {
                Actor* newActor = nullptr;
                Level::MazeEntry ge = lev.getContentsOf(x,y);
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::wall:
                    {
                        newActor = new Wall(this, x, y);
                        AddActor(newActor);
                        break;
                    }
                    case Level::exit:
                        newActor = new Exit(this, x, y);
                        AddActor(newActor);
                        break;
                    case Level::crystal:
                        m_nCrystals++;
                        newActor = new Crystal(this, x, y);
                        AddActor(newActor);
                        break;
                    case Level::ammo:
                        newActor = new AmmoGoodie(this, x, y);
                        AddActor(newActor);
                        break;
                    case Level::extra_life:
                        newActor = new ExtraLifeGoodie(this, x, y);
                        AddActor(newActor);
                        break;
                    case Level::player:
                        m_player = new Player(this, x, y);
                        OccupySquare(m_player);
                        break;
                    default:
                        std::cout << "ankap gyada\n";
                }
            }
        }
    }
    
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    UpdateGameText();

    size_t size = m_Actors.size();  // don't give new peas a chance to move on their first tick
    m_player->doSomething();
    for(size_t i = 0; i < size && !m_player->isDead(); i++) // leave loop if player dies
    {
        int actionResult = m_Actors[i]->doSomething();
        if( actionResult == GWSTATUS_FINISHED_LEVEL)
        {
            increaseScore(m_Bonus);
            return GWSTATUS_FINISHED_LEVEL;    // exit returns level won status
        }
    }
            
    
    if(m_player->isDead())
    {
        decLives();
        cleanUp();
        return GWSTATUS_PLAYER_DIED;
    }
    RemoveDead();
    
    if(m_Bonus > 0)
        m_Bonus--;
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::RemoveDead()
{
    int i = 0;
    while(i < m_Actors.size())
    {
        if(m_Actors[i]->isDead())
        {
            delete m_Actors[i];
            m_Actors[i] = m_Actors[m_Actors.size()-1];
            m_Actors.pop_back();
        }
        else
            i++;
    }
}

void StudentWorld::cleanUp()
{
    long int last = m_Actors.size() - 1;
    while(last >= 0)
    {
        delete m_Actors[last];
        m_Actors.pop_back();
        last--;
    }
    delete m_player;
    m_player = nullptr;
    m_nCrystals = 0;
    m_grid.CleanUpMap();
}

void StudentWorld::AddActor(Actor* a)
{
    m_Actors.push_back(a);
    OccupySquare(a);
}

void StudentWorld::UpdateGameText() // inefficient, fix later
{
//    if(m_player == nullptr)
//        return;
    std::ostringstream text;
    text << "Score: " << setw(7);
    text.fill('0');
    text << getScore() << "  " << "Level: " << setw(2) << getLevel() << "  ";
    text.fill(' ');
    text << "Lives: " << setw(2) << getLives() << "  ";
    
    double hpPercent = ((double) m_player->GetHealth() / START_PLAYER_HEALTH) * 100;
    text << setw(3) << "Health: " << hpPercent << '%' << "  ";
    text << "Ammo: " << setw(3) << m_player->GetCurrentAmmo() << "  ";
    text << "Bonus: " << setw(4) << m_Bonus << "  ";
    
    setGameStatText(text.str());
}

void StudentWorld::ConvertCoords(double x, double y, int& row, int& col)
{
    col = std::round(x);
    row = VIEW_HEIGHT-1-std::round(y);
}

bool StudentWorld::CanWalk(Actor* a)
{
    double x = a->getX();
    double y = a->getY();
    int dir = a->getDirection();
    
    switch(dir)
    {
        case Actor::up:
            return SquareWalkable(x, y+1);
        case Actor::down:
            return SquareWalkable(x, y-1);
        case Actor::right:
            return SquareWalkable(x+1, y);
        case Actor::left:
            return SquareWalkable(x-1, y);
        default:
            std::cerr << "walking error, invalid direction\n";
            return false;
    }
}

bool StudentWorld::AttackSquare(double x, double y)
{
    
    if(!SquareAttackable(x, y))
        return false;   // no point iterating in next step
    
    for(int i = 0; i < m_Actors.size(); i++)
    {
        if(AreEqual(m_Actors[i]->getX(), x) && AreEqual(m_Actors[i]->getY(), y) && (m_Actors[i]->GetOcStatus() > Actor::OC_BARRIER_NON_SHOTSTOP && m_Actors[i]->GetOcStatus() < Actor::END_NOT_A_STATUS) /* <-- shotstopper */)    // account for factories
        {
            m_Actors[i]->getAttacked();
            return true;
        }
    }
    std::cerr << "Unexpected behavior, no actors attacked\n";
    return false;
}

void StudentWorld::FireFrom(Actor* a)
{
    double x = a->getX();
    double y = a->getY();
    int dir = a->getDirection();
    
    Pea* pea = nullptr;
    switch(dir)
    {
        case Actor::left:
            pea = new Pea(this, x-1, y, dir);
            break;
        case Actor::right:
            pea = new Pea(this, x+1, y, dir);
            break;
        case Actor::up:
            pea = new Pea(this, x, y+1, dir);
            break;
        case Actor::down:
            pea = new Pea(this, x, y-1, dir);
            break;
        default:
            std::cerr << "invalid direction for new pea\n";
            return;
    }
    
    AddActor(pea);
}

// GameMap wrappers

void StudentWorld::LeaveSquare(Actor* a)
{
    int row, col;
    double x = a->getX();
    double y = a->getY();
    int status = a->GetOcStatus();
    
    ConvertCoords(x, y, row, col);
    m_grid.LeaveSquare(col, row, status);
}

void StudentWorld::OccupySquare(Actor* a)
{
    int row, col;
    double x = a->getX();
    double y = a->getY();
    int status = a->GetOcStatus();
    
    ConvertCoords(x, y, row, col);
    m_grid.OccupySquare(col, row, status);
}

bool StudentWorld::SquareWalkable(double x, double y)
{
    int row, col;
    ConvertCoords(x, y, row, col);
    return m_grid.SquareWalkable(col, row);
}

bool StudentWorld::SquareAttackable(double x, double y)
{
    int row, col;
    ConvertCoords(x, y, row, col);
    return m_grid.SquareAttackable(col, row);
}

// GameMap struct implementations

StudentWorld::GameMap::GameMap()
{
    for(int i = 0; i < VIEW_HEIGHT; i++)
        for(int j = 0; j < VIEW_WIDTH; j++)
            m_occupancyMap[i][j] = new std::list<int>;
}

StudentWorld::GameMap::~GameMap()
{
    for(int i = 0; i < VIEW_HEIGHT; i++)
        for(int j = 0; j < VIEW_WIDTH; j++)
            delete m_occupancyMap[i][j];
}

void StudentWorld::GameMap::CleanUpMap()
{
    for(int i = 0; i < VIEW_HEIGHT; i++)
        for(int j = 0; j < VIEW_WIDTH; j++)
            m_occupancyMap[i][j]->clear();
}

bool StudentWorld::GameMap::InvalidCoords(int col, int row)
{
    if ( col < 0 || col >= VIEW_WIDTH || row < 0 || row >= VIEW_HEIGHT)
    {
        std::cerr << "invalid coordinates\n";
        return true;
    }
    return false;
}

bool StudentWorld::GameMap::InvalidStatus(int status)
{
    if (status <= Actor::OC_ERROR || status >= Actor::END_NOT_A_STATUS)
    {
        std::cerr << "invalid status\n";
        return true;
    }
    return false;
}

bool StudentWorld::GameMap::InvalidStatusOrCoords(int col, int row, int status)
{
    return InvalidStatus(status) || InvalidCoords(col, row);
}

void StudentWorld::GameMap::LeaveSquare(int col, int row, int status)
{
    if ( InvalidStatusOrCoords(col, row, status) )
    {
        std::cerr << "Error leaving square";
        return;
    }
    
    std::list<int>::iterator i = m_occupancyMap[row][col]->begin();
    for(; i != m_occupancyMap[row][col]->end(); i++)    // won't enter loop if list empty
    {
        if(*i == status)
        {
            m_occupancyMap[row][col]->erase(i);
            return;
        }
    }
    
    std::cerr << "Error, leaving empty square\n";
}

void StudentWorld::GameMap::OccupySquare(int col, int row, int status)
{
    if ( InvalidStatusOrCoords(col, row, status) )
    {
        std::cerr << "Error occupying square\n";
        return;
    }
    
    m_occupancyMap[row][col]->push_back(status);
}

bool StudentWorld::GameMap::SquareWalkable(int col, int row)
{
    if ( InvalidCoords(col, row) )
    {
        std::cerr << "Error walking\n";
        return false;
    }
    
    std::list<int>::iterator i = m_occupancyMap[row][col]->begin();
    for(; i != m_occupancyMap[row][col]->end(); i++)
    {
        if(*i != Actor::OC_NON_BARRIER)
            return false;
    }
    return true;    // loop didn't execute or only goodies
}

bool StudentWorld::GameMap::SquareAttackable(int col, int row)
{
    if ( col < 0 || col >= VIEW_WIDTH || row < 0 || row >= VIEW_HEIGHT)
    {
        std::cerr << "invalid coordinates passed to SquareAttackable()\n";
        return false;
    }
    
    list<int>::iterator i = m_occupancyMap[row][col]->begin();
    for(; i != m_occupancyMap[row][col]->end(); i++)
    {
        if (*i == Actor::OC_KILLABLE_SHOTSTOP || *i == Actor::OC_UNKILLABLE_SHOTSTOP)
            return true;
    }
    return false;
}
