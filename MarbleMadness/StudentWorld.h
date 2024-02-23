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
    
    bool CanWalk(double x, double y, int dir);
    bool SquareWalkable(double x, double y);
    
    bool AttackSquare(double x, double y);
    void FireFrom(double x, double y, int dir);
    
private:
    std::vector<Actor*> m_Actors;
    Player* m_player;      //change to Player*
};

#endif // STUDENTWORLD_H_
