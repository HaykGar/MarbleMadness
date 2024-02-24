#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    void RemoveDead();
    bool AttackSquare(double x, double y);
    void FireFrom(double x, double y, int dir);

    
    bool CanWalk(double x, double y, int dir);
    void ConvertCoords(double x, double y, int& row, int& col);
    
    // GameMap wrappers essentially
    bool SquareWalkable(double x, double y);
    void LeaveSquare(double x, double y, int status);
    void OccupySquare(double x, double y, int status);
    bool SquareAttackable(double x, double y);
        
private:
    std::vector<Actor*> m_Actors;
    Player* m_player;      //change to Player*
    // need to update move methods accordingly
    struct GameMap      // CAREFUL to convert row col to x y appropriately, HANDLE MOVE UPDATING GRID
    {
        GameMap();
        ~GameMap();
        void CleanUpMap();
        
        bool InvalidCoords(int col, int row);
        bool InvalidStatus(int status);
        bool InvalidStatusOrCoords(int col, int row, int status);
        
        void OccupySquare(int x, int y, int status);
        void LeaveSquare(int x, int y, int status);
        bool SquareWalkable(int x, int y);
        bool SquareAttackable(int x, int y);
        std::list<int>* m_occupancyMap[VIEW_HEIGHT][VIEW_WIDTH];
        /*
         empty list at a position means that square is empty, otherwise status code of objects in lists tell about walk/push/shoot-ability
         */
    };
    GameMap m_grid;
};

#endif // STUDENTWORLD_H_
