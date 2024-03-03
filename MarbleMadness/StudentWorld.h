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
    
    void AddActor(Actor* a);
    
    bool AttackSquare(double x, double y);
    void FireFrom(Actor* a);
    
    void DecCrystals(Crystal* c);   // check if crystal is dead then decrement
    bool AreCrystalsLeft() const;
    Actor* GetGoodieHere(double x, double y);
    bool SamePosAsPlayer(Actor* a) const;
    bool PlayerInLOS(Actor* a) const;
    
    void GivePlayerPeas(AmmoGoodie* a);
    void RestorePlayerHealth(RestoreHealthGoodie* r);
    
    bool CanWalk(Actor* a) const;
    
    void HandleThiefBotDeath(ThiefBot* t);
    
    bool ThreeThievesWithin3(double x, double y) const;
    
    // GameMap wrappers essentially
    bool SquareWalkable(double x, double y) const;
    void LeaveSquare(Actor* a);
    void OccupySquare(Actor* a);
    bool PushMarble(Marble* m, int dir);
    bool PlayerTryToPush();
    bool SquareAttackable(double x, double y) const;
    bool HasSuchOccupant(double x, double y, int status) const;

private:
    std::vector<Actor*> m_Actors;
    Player* m_player;
    int m_nCrystals;
    unsigned int m_Bonus;
    
    struct Coord
    {
        Coord(double x, double y) : x_c(x), y_c(y) {}
        double x_c;
        double y_c;
    };
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
        int SquarePossiblyPushable(int col, int row) const;
        bool SquareAttackable(int col, int row) const;
        bool HasSuchOccupant(int col, int row, int status) const;
        std::list<int>* m_occupancyMap[VIEW_HEIGHT][VIEW_WIDTH];
        /*
         empty list at a position means that square is empty, otherwise status code of objects in lists tell about walk/push/shoot-ability
         */
    };
    GameMap m_grid;
    
    void ConvertCoords(double x, double y, int& row, int& col) const;
    void GetLevelFileName(std::string& s) const;
    void RemoveDead();
    void UpdateGameText();
    
    bool PathToPlayer(double x, double y, std::function<void(double&, double&)> ) const;
};

inline void StudentWorld::DecCrystals(Crystal* c)
{
    if(c->isDead())
        m_nCrystals--;
    else{
        std::cerr << "nCrystals should only be decremented when on is removed from the game\n";
    }
}

inline bool StudentWorld::AreCrystalsLeft() const
{
    return m_nCrystals > 0;
}

inline bool StudentWorld::SamePosAsPlayer(Actor *a) const
{
    return (AreEqual(a->getX(), m_player->getX()) && AreEqual(a->getY(), m_player->getY()));
}

inline void StudentWorld::GivePlayerPeas(AmmoGoodie* a)
{
    if(a->isDead())
        m_player->AddPeas(AMMO_EXTRA_PEAS);
    else
        std::cerr << "Error giving ammo, goodie is still alive\n";
}

inline void StudentWorld::RestorePlayerHealth(RestoreHealthGoodie* r)
{
    if(r->isDead())
        m_player->SetHealth(START_PLAYER_HEALTH);
    else
        std::cerr << "Error restoring health, goodie is still alive\n";
}

#endif // STUDENTWORLD_H_
