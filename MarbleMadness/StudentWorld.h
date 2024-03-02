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
    
    
    bool AttackSquare(double x, double y);
    void FireFrom(Actor* a);
    
    void DecCrystals();
    bool CrystalsLeft() const;
    Actor* GoodieHere(double x, double y);
    bool SamePosAsPlayer(Actor* a) const;
    bool PlayerInLOS(Actor* a) const;
    
    void GivePlayerPeas();
    void RestorePlayerHealth();
    
    bool CanWalk(Actor* a) const;
    
    void HandleThiefBotDeath(ThiefBot* t);
    
    // GameMap wrappers essentially
    bool SquareWalkable(double x, double y) const;
    void LeaveSquare(Actor* a);
    void OccupySquare(Actor* a);
    bool SquarePushable(double x, double y) const;
    bool TryToPush();
    bool MarbleWithPit(Pit* p);
    bool SquareAttackable(double x, double y) const;

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
        
        bool InvalidCoords(int col, int row) const;
        bool InvalidStatus(int status) const;
        
        void OccupySquare(int col, int row, int status);
        void LeaveSquare(int col, int row, int status);
        bool SquareWalkable(int col, int row) const;
        bool SquarePushable(int col, int row) const;
        bool MarbleWithPit(int col, int row) const;
        bool SquareAttackable(int col, int row) const;
        bool HasSuchOccupant(int col, int row, int status) const;
        std::list<int>* m_occupancyMap[VIEW_HEIGHT][VIEW_WIDTH];
        /*
         empty list at a position means that square is empty, otherwise status code of objects in lists tell about walk/push/shoot-ability
         */
    };
    GameMap m_grid;
    
    void SwallowMarble(double x, double y);
    void ConvertCoords(double x, double y, int& row, int& col) const;
    void GetLevelFileName(std::string& s) const;
    void RemoveDead();
    void UpdateGameText();
    void AddActor(Actor* a);
    
    bool PathToPlayer(double x, double y, std::function<void(double&, double&)> ) const;
};

inline void StudentWorld::DecCrystals()
{
    m_nCrystals--;
}

inline bool StudentWorld::CrystalsLeft() const
{
    return m_nCrystals > 0;
}

inline bool StudentWorld::SamePosAsPlayer(Actor *a) const
{
    return (AreEqual(a->getX(), m_player->getX()) && AreEqual(a->getY(), m_player->getY()));
}

inline void StudentWorld::GivePlayerPeas()
{
    m_player->AddPeas(AMMO_EXTRA_PEAS);
}

inline void StudentWorld::RestorePlayerHealth()
{
    m_player->SetHealth(START_PLAYER_HEALTH);
}

#endif // STUDENTWORLD_H_
