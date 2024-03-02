#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
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

void StudentWorld::GetLevelFileName(std::string& s) const
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
                Level::MazeEntry ge = lev.getContentsOf(x,y);
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::wall:
                    {
                        AddActor(new Wall(this, x, y));
                        break;
                    }
                    case Level::crystal:
                        m_nCrystals++;
                        AddActor(new Crystal(this, x, y));
                        break;
                    case Level::ammo:
                        AddActor(new AmmoGoodie(this, x, y));
                        break;
                    case Level::marble:
                        AddActor(new Marble(this, x, y));
                        break;
                    case Level::pit:
                        AddActor(new Pit(this, x, y));
                        break;
                    case Level::horiz_ragebot:
                        AddActor(new RageBot(this, x, y, Actor::right));
                        break;
                    case Level::vert_ragebot:
                        AddActor(new RageBot(this, x, y, Actor::down));
                        break;
                    case Level::thiefbot_factory:
                        AddActor(new RegFactory(this, x, y));
                        break;
                    case Level::mean_thiefbot_factory:
                        AddActor(new MeanFactory(this, x, y));
                        break;
                    case Level::exit:
                        AddActor(new Exit(this, x, y));
                        break;
                    case Level::extra_life:
                        AddActor(new ExtraLifeGoodie(this, x, y));
                        break;
                    case Level::restore_health:
                        AddActor(new RestoreHealthGoodie(this, x, y));
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
    
    AddActor(new ThiefBot(this, 1, 10)); 
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
        if(m_Actors[last]->isThief())
            delete m_Actors[last]->StolenGoodie();
        
        delete m_Actors[last];
        m_Actors.pop_back();
        last= m_Actors.size() - 1;
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

bool StudentWorld::TryToPush()
{
    double x, y;
    switch (m_player->getDirection())
    {
        case Actor::right:
            x = m_player->getX() + 1;
            y = m_player->getY();
            break;
        case Actor::left:
            x = m_player->getX() - 1;
            y = m_player->getY();
            break;
        case Actor::up:
            x = m_player->getX();
            y = m_player->getY() + 1;
            break;
        case Actor::down:
            x = m_player->getX();
            y = m_player->getY() - 1;
            break;
        default:
            std::cerr << "Player has invalid direction\n";
            return false;
    }
    
    for(int i = 0; i < m_Actors.size(); i++)
    {
        if(m_Actors[i]->Pushable() && AreEqual(m_Actors[i]->getX(), x) && AreEqual(m_Actors[i]->getY(), y) )
            return m_Actors[i]->Push(m_player->getDirection());
    }
    return false;
}

void StudentWorld::ConvertCoords(double x, double y, int& row, int& col) const
{
    col = std::round(x);
    row = VIEW_HEIGHT-1-std::round(y);
}

bool StudentWorld::CanWalk(Actor* a) const
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

Actor* StudentWorld::GoodieHere(double x, double y)
{
    if(HasSuchOccupant(x, y, Actor::OC_NON_BARRIER))
    {
        for(int i = 0; i < m_Actors.size(); i++)
        {
            if( AreEqual(x, m_Actors[i]->getX()) &&
                AreEqual(m_Actors[i]->getY(), y) && m_Actors[i]->isStealableGoodie() )
            {
                Actor* stolen = nullptr;
                if(randInt(1, 10) == 1)     // 10% probability
                {
                    stolen = m_Actors[i];
                    LeaveSquare(m_Actors[i]);
                    m_Actors[i]->SetPos(-10, -10);    // move offscreen (fix me)
                    m_Actors.erase(m_Actors.begin()+i); // remove this goodie from Actors vector so it will not be part of the game
                }
                return stolen;
            }
        }
    }
    return nullptr;
}


bool StudentWorld::PlayerInLOS(Actor *a) const  // set max depth to guard against infinite recursion?
{
    if(a == nullptr)
    {
        std::cerr << "Error, nullptr passed to PlayerInLOS\n";
        return false;
    }
    // Helper lambda function to avoid redundant dir check
    std::function<void(double&, double&)> modifyParams;
    switch (a->getDirection()) {
        case Actor::left:
            modifyParams = [&](double& x, double& y) -> void
            { x--; };
            break;
        case Actor::right:
            modifyParams = [&](double& x, double& y) -> void
            { x++; };
            break;
        case Actor::up:
            modifyParams = [&](double& x, double& y) -> void
            { y++; };
            break;
        case Actor::down:
            modifyParams = [&](double& x, double& y) -> void
            { y--; };
            break;
        default:
            std::cerr << "Error, invalid direction\n";
            return false;
    }
    
    return PathToPlayer(a->getX(), a->getY(), modifyParams);
}

bool StudentWorld::PathToPlayer(double x, double y, std::function<void(double&, double&)> modifyParams) const
{
    modifyParams(x, y);
    if (AreEqual(m_player->getX(), x) && AreEqual(m_player->getY(), y))
        return true;
    else if (SquareAttackable(x, y))
        return false;
    else{
        return PathToPlayer(x, y, modifyParams);
    }
}

void StudentWorld::SwallowMarble(double x, double y)
{
    for(int i = 0; i < m_Actors.size(); i++)
    {
        if(AreEqual(m_Actors[i]->getX(), x) && AreEqual(m_Actors[i]->getY(), y) && (m_Actors[i]->GetOcStatus() == Actor::OC_KILLABLE_SHOTSTOP) )
            m_Actors[i]->HandleDeath();
    }
}

bool StudentWorld::AttackSquare(double x, double y)
{
    
    if(!SquareAttackable(x, y))
        return false;   // no point iterating in next step
    
    if( AreEqual(x, m_player->getX()) && AreEqual(y, m_player->getY()) )
    {
        m_player->getAttacked();
        return true;
    }
    bool hitSomething = false;
    for(int i = 0; i < m_Actors.size(); i++)
    {
        if(AreEqual(m_Actors[i]->getX(), x) && AreEqual(m_Actors[i]->getY(), y) && (m_Actors[i]->GetOcStatus() > Actor::OC_BARRIER_NON_SHOTSTOP && m_Actors[i]->GetOcStatus() < Actor::END_NOT_A_STATUS) /* <-- shotstopper */)    // account for factories
        {
            if(m_Actors[i]->GetOcStatus() == Actor::OC_UNKILLABLE_SHOTSTOP)
                hitSomething = true;
            else
            {
                m_Actors[i]->getAttacked();
                return true;
            }
            
        }
    }
    return hitSomething;
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
    
    AddActor(pea);  // never add nullptr
}


void StudentWorld::HandleThiefBotDeath(ThiefBot* t)
{
    Actor* stolen = t->StolenGoodie();
    if(stolen == nullptr)
        return;
    else{
        stolen->SetPos(t->getX(), t->getY());
        AddActor(stolen);   // bring stolen goodie back to the game
    }
}

bool StudentWorld::ThreeThievesWithin3(double x, double y) const
{
    x = std::round(x);
    y = std::round(y);
    int possibleThieves = 0;
    std::list<Coord> possiblePos;
    for(int i = x - 3; i <= x + 3; i++ )
    {
        if(i >= 0 && i < VIEW_WIDTH)
        {
            for(int j = y - 3; j <= y + 3; j++)
            {
                if(j >= 0 && j < VIEW_HEIGHT && HasSuchOccupant(i, j, Actor::OC_KILLABLE_SHOTSTOP))
                {
                    possibleThieves++;
                    possiblePos.push_back(Coord(i, j));
                }
            }
        }
        
    }
    if(possiblePos.size() < 3)
        return false;
    
    int thievesFound = 0;
    for(long int i = m_Actors.size() - 1; i >= 0; i--)
    {
        std::list<Coord>::iterator it = possiblePos.begin();
        for(;it != possiblePos.end(); it++)
        {
            if(AreEqual(it->x_c, m_Actors[i]->getX()) &&
               AreEqual(it->y_c, m_Actors[i]->getY())){
                thievesFound++;
                break;
            }
        }
        if(thievesFound == 3)
            return true;
    }
    
    return false;
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

bool StudentWorld::SquareWalkable(double x, double y) const
{
    int row, col;
    ConvertCoords(x, y, row, col);
    return m_grid.SquareWalkable(col, row);
}

bool StudentWorld::SquarePushable(double x, double y) const
{
    int row, col;
    ConvertCoords(x, y, row, col);
    return m_grid.SquarePushable(col, row);
}

bool StudentWorld::MarbleWithPit(Pit* p)
{
    int row, col;
    ConvertCoords(p->getX(), p->getY(), row, col);
    if(m_grid.MarbleWithPit(col, row))
    {
        SwallowMarble(p->getX(), p->getY());
        return true;
    }
    else
        return false;
}


bool StudentWorld::SquareAttackable(double x, double y) const
{
    int row, col;
    ConvertCoords(x, y, row, col);
    return m_grid.SquareAttackable(col, row);
}

bool StudentWorld::HasSuchOccupant(double x, double y, int status) const
{
    int row, col;
    ConvertCoords(x, y, row, col);
    return m_grid.HasSuchOccupant(col, row, status);
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

bool StudentWorld::GameMap::InvalidCoords(int col, int row) const
{
    if ( col < 0 || col >= VIEW_WIDTH || row < 0 || row >= VIEW_HEIGHT)
    {
        std::cerr << "invalid coordinates\n";
        return true;
    }
    return false;
}

bool StudentWorld::GameMap::InvalidStatus(int status) const
{
    if (status <= Actor::OC_ERROR || status >= Actor::END_NOT_A_STATUS)
    {
        std::cerr << "invalid status\n";
        return true;
    }
    return false;
}


void StudentWorld::GameMap::LeaveSquare(int col, int row, int status)
{
    if ( InvalidStatus(status) || InvalidCoords(col, row) )
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
    if ( InvalidStatus(status) || InvalidCoords(col, row) )
    {
        std::cerr << "Error occupying square\n";
        return;
    }
    
    m_occupancyMap[row][col]->push_back(status);
}

bool StudentWorld::GameMap::SquarePushable(int col, int row) const
{
    if ( InvalidCoords(col, row) )
    {
        std::cerr << "Error pushing, invalid coordinates\n";
        return false;
    }
    
    std::list<int>::iterator i = m_occupancyMap[row][col]->begin();
    if (i == m_occupancyMap[row][col]->end())   //empty
        return true;
    
    for(; i != m_occupancyMap[row][col]->end(); i++)
    {
        if(*i == Actor::OC_BARRIER_NON_SHOTSTOP)    // pit
            return true;
    }
    return false;
}

bool StudentWorld::GameMap::MarbleWithPit(int col, int row) const
{
    
    if ( InvalidCoords(col, row) )
    {
        std::cerr << "Error, invalid coordinates\n";
        return false;
    }
    
    std::list<int>::iterator i = m_occupancyMap[row][col]->begin();
    if (i == m_occupancyMap[row][col]->end())
    {
        std::cerr << "invalid function call, no pit at this location\n";
        return false;
    }
    
    for(; i != m_occupancyMap[row][col]->end(); i++)
    {
        if(*i == Actor::OC_KILLABLE_SHOTSTOP)
            return true; // marble only killable actor allowed to share spot with pit
    }
    return false;
}

bool StudentWorld::GameMap::SquareWalkable(int col, int row) const
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

bool StudentWorld::GameMap::SquareAttackable(int col, int row) const
{
    if ( InvalidCoords(col, row) )
        return false;
    
    list<int>::iterator i = m_occupancyMap[row][col]->begin();
    for(; i != m_occupancyMap[row][col]->end(); i++)
    {
        if (*i == Actor::OC_KILLABLE_SHOTSTOP || *i == Actor::OC_UNKILLABLE_SHOTSTOP)
            return true;
    }
    return false;
}

bool StudentWorld::GameMap::HasSuchOccupant(int col, int row, int status) const
{
    if(InvalidCoords(col, row) || InvalidStatus(status))
        return false;
    
    list<int>::iterator i = m_occupancyMap[row][col]->begin();
    for(; i != m_occupancyMap[row][col]->end(); i++)
    {
        if (*i == status)
            return true;
    }
    return false;
}
