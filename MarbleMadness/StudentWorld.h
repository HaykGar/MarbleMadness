#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

const int LEVEL_START_BONUS = 1000;


class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    void UpdateGameText();
    
    void AddActor(Actor* a);
    
    void RemoveDead();
    
    bool AttackSquare(double x, double y);
    void FireFrom(Actor* a);
    
    void DecCrystals();
    bool CrystalsLeft();
    bool SamePosAsPlayer(Actor* a);
    void GivePlayerPeas(unsigned int amount);
    void RestorePlayerHealth();
    
    
    bool CanWalk(Actor* a);
    void ConvertCoords(double x, double y, int& row, int& col);
    void GetLevelFileName(std::string& s);
    
    // GameMap wrappers essentially
    bool SquareWalkable(double x, double y);
    void LeaveSquare(Actor* a);
    void OccupySquare(Actor* a);
    bool SquarePushable(double x, double y);
    bool TryToPush();
    bool MarbleWithPit(Pit* p);
    bool SquareAttackable(double x, double y);

private:
    std::vector<Actor*> m_Actors;
    Player* m_player;
    int m_nCrystals;
    unsigned int m_Bonus;

    struct GameMap
    {
        GameMap();
        ~GameMap();
        void CleanUpMap();
        
        bool InvalidCoords(int col, int row);
        bool InvalidStatus(int status);
        bool InvalidStatusOrCoords(int col, int row, int status);
        
        void OccupySquare(int col, int row, int status);
        void LeaveSquare(int col, int row, int status);
        bool SquareWalkable(int col, int row);
        bool SquarePushable(int col, int row);
        bool MarbleWithPit(int col, int row);
        bool SquareAttackable(int col, int row);
        std::list<int>* m_occupancyMap[VIEW_HEIGHT][VIEW_WIDTH];
        /*
         empty list at a position means that square is empty, otherwise status code of objects in lists tell about walk/push/shoot-ability
         */
    };
    GameMap m_grid;
    
    void SwallowMarble(double x, double y);
};

inline void StudentWorld::DecCrystals()
{
    m_nCrystals--;
}

inline bool StudentWorld::CrystalsLeft()
{
    return m_nCrystals > 0;
}

inline bool StudentWorld::SamePosAsPlayer(Actor *a)
{
    return (AreEqual(a->getX(), m_player->getX()) && AreEqual(a->getY(), m_player->getY()));
}

inline void StudentWorld::GivePlayerPeas(unsigned int amount)
{
    m_player->AddPeas(amount);
}

inline void StudentWorld::RestorePlayerHealth()
{
    m_player->SetHealth(START_PLAYER_HEALTH);
}

#endif // STUDENTWORLD_H_
