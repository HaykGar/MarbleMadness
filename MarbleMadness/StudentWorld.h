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
    
    bool CanWalk(double x, double y, int dir);
    bool SquareWalkable(double x, double y);
    
private:
    std::vector<Actor*> m_Actors;
    Actor* m_player;        //change to Player*
};

#endif // STUDENTWORLD_H_
