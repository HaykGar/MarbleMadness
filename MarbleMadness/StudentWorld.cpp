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

std::function<void(double&, double&)> modifyParamFunction(int dir)
{
    switch (dir)
    {
        case Actor::left:
            return [&](double& x, double& y) -> void { x--; };
        case Actor::right:
            return [&](double& x, double& y) -> void { x++; };
        case Actor::up:
            return  [&](double& x, double& y) -> void { y++; };
        case Actor::down:
            return [&](double& x, double& y) -> void { y--; };
        default:
            std::cerr << "Error, invalid direction, cannot create appropriate function\n";  // maybe exception better?
            return [&](double& x, double& y) -> void { };
    }
}

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
                        std::cout << "unexpected actor, none of the above\n";
                }
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    UpdateGameText();

    for(size_t i = 0; i < m_Actors.size() && !m_player->isDead(); i++) // leave loop if player dies
    {
        int actionResult = m_Actors[i]->doSomething();
        if( actionResult == GWSTATUS_FINISHED_LEVEL)
        {
            increaseScore(m_Bonus);
            return GWSTATUS_FINISHED_LEVEL;    // exit returns level won status
        }
    }
    m_player->doSomething();
            
    
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

void StudentWorld::UpdateGameText() 
{
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

bool StudentWorld::PlayerTryToPush()
{
    double x = m_player->getX();
    double y = m_player->getY();
    std::function<void(double&, double&)> modifyParams = modifyParamFunction(m_player->getDirection());
    modifyParams(x, y);
    
    for(long int i = m_Actors.size() - 1; i >= 0 ; i--)
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
    std::function<void(double&, double&)> modifyParams = modifyParamFunction(a->getDirection());
    modifyParams(x, y);
    
    return SquareWalkable(x, y);
}

Actor* StudentWorld::GetGoodieHere(double x, double y)
{
    if(HasOccupantWithStatus(x, y, Actor::OC_NON_BARRIER))    // goodie or crystal
    {
        for(long int i = m_Actors.size() - 1; i >= 0 ; i--)
        {
            if( AreEqual(x, m_Actors[i]->getX()) &&
                AreEqual(m_Actors[i]->getY(), y) && m_Actors[i]->isStealableGoodie() )
            {
                Actor* stolen = nullptr;
                if(randInt(1, 10) == 1)     // 10% probability
                {
                    stolen = m_Actors[i];
                    LeaveSquare(m_Actors[i]);
                    m_Actors[i]->SetPos(0 ,0);
                    m_Actors[i]->setVisible(false);
                    m_Actors.erase(m_Actors.begin()+i);
                }
                return stolen;
            }
        }
    }
    return nullptr;
}


bool StudentWorld::PlayerInLOS(Actor *a) const
{
    if(a == nullptr)
    {
        std::cerr << "Error, nullptr passed to PlayerInLOS\n";
        return false;
    }
    // Helper function to avoid redundant dir check
    std::function<void(double&, double&)> modifyParams = modifyParamFunction(a->getDirection());
    return PathToPlayer(a->getX(), a->getY(), modifyParams, 0);
}

bool StudentWorld::PathToPlayer(double x, double y, std::function<void(double&, double&)> modifyParams, int depth) const
{
    modifyParams(x, y);
    depth++;
    if(depth > std::max(VIEW_WIDTH, VIEW_HEIGHT)){
        std::cerr << "terminated recursion, depth limit reached\n";
        return false;
    }
    if (AreEqual(m_player->getX(), x) && AreEqual(m_player->getY(), y))
        return true;
    else if (SquareAttackable(x, y))
        return false;
    else
        return PathToPlayer(x, y, modifyParams, depth);
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
    for(long int i = m_Actors.size() - 1; i >= 0 ; i--)
    {
        if(AreEqual(m_Actors[i]->getX(), x) && AreEqual(m_Actors[i]->getY(), y) && (m_Actors[i]->GetOcStatus() > Actor::OC_BARRIER_NON_SHOTSTOP && m_Actors[i]->GetOcStatus() < Actor::END_NOT_A_STATUS) /* <-- shotstopper */)
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
    std::function<void(double&, double&)> modifyParams = modifyParamFunction(dir);
    modifyParams(x, y);
    Actor* pea = new Pea(this, x, y, dir);
    m_Actors.insert(m_Actors.begin(), pea);
    OccupySquare(pea);
}


void StudentWorld::HandleThiefBotDeath(ThiefBot* t)
{
    Actor* stolen = t->StolenGoodie();
    if(stolen == nullptr)
        return;
    else{
        stolen->SetPos(t->getX(), t->getY());
        stolen->setVisible(true);
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
                if(j >= 0 && j < VIEW_HEIGHT && HasOccupantWithStatus(i, j, Actor::OC_KILLABLE_SHOTSTOP))
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

bool StudentWorld::PushMarble(Marble* m, int dir)
{
    int row, col;
    double x = m->getX();
    double y = m->getY();
    std::function<void(double&, double&)> modifyParams = modifyParamFunction(dir);
    modifyParams(x, y);
    
    ConvertCoords(x, y, row, col);  // repetitive
    int pushability = m_grid.SquarePossiblyPushable(col, row);
    
    
    if( pushability == -1)
        return false;
    else if(pushability != 0)   // square not empty
    {
        for(long int i = m_Actors.size() - 1; i >= 0; i--)
        {
            if( m_Actors[i]->CanPushInto() && AreEqual(x, m_Actors[i]->getX()) && AreEqual(y, m_Actors[i]->getY()) )
            {
                m_Actors[i]->HandleDeath(); // pit disappears
                m->HandleDeath();           // marble dissapears
                return true;
            }
        }
        std::cerr << "error, expected to find pit\n";
        return false;
    }
    
    m->setDirection(dir);
    m->MoveOne();
    m->setDirection(Actor::none);
    return true;    // empty square, so marble moves
}


bool StudentWorld::SquareAttackable(double x, double y) const
{
    int row, col;
    ConvertCoords(x, y, row, col);
    return m_grid.SquareAttackable(col, row);
}

bool StudentWorld::HasOccupantWithStatus(double x, double y, int status) const
{
    int row, col;
    ConvertCoords(x, y, row, col);
    return m_grid.HasOccupantWithStatus(col, row, status);
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

int StudentWorld::GameMap::SquarePossiblyPushable(int col, int row) const
{
    if ( InvalidCoords(col, row) )
    {
        std::cerr << "Error pushing, invalid coordinates\n";
        return -1;
    }
    
    std::list<int>::iterator i = m_occupancyMap[row][col]->begin();
    if (i == m_occupancyMap[row][col]->end())   //empty
        return 0;   // definitely pushable
    
    for(; i != m_occupancyMap[row][col]->end(); i++)
    {
        if(*i != Actor::OC_BARRIER_NON_SHOTSTOP)    // pit
            return -1;
    }
    return 1;   // might be pushable
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

bool StudentWorld::GameMap::HasOccupantWithStatus(int col, int row, int status) const
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
