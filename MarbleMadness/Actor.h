#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

const int START_PLAYER_HEALTH = 20;
const int PLAYER_START_PEAS = 20;

class StudentWorld;

class Actor : public GraphObject
{
    public:
        Actor(StudentWorld* sp, int imageID, double startX, double startY, int dir = right, double size = 1.0);
        
        virtual void doSomething() {}
        virtual bool getAttacked() { return false; }
        
        // move implemented in graph object
        
        
        void die()          { m_isDead = true; }
        bool isDead() const { return m_isDead; }
    

    private:
        bool m_isDead;
        StudentWorld* m_world;
        
        //direction, image ID, visibility provided in graph object
};


class KillableActor : public Actor      // Actors that can be "killed" by peas, player, robots, and marble are killable
{
    public:
        KillableActor(int health,StudentWorld* sp, int imageID, double startX, double startY, int dir = right, double size = 1.0);
        
        virtual void SpecificGetAttacked() {}
        
        virtual bool getAttacked()
        {
            m_health -= 2;
            SpecificGetAttacked();
            return true;
        }
        
        void SetHealth(int health){
            m_health = health;
        }
        int GetHealth() const
        {
            return m_health;
        }

    private:
        int m_health;
};

class SentientActor : public KillableActor  // player and Robots... specific shared behaviors
{
    public:
        SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int dir = right, double size = 1.0);
    
};


class Player : public SentientActor
{
public:
    Player(StudentWorld* sp, double startX, double startY, int dir = right, double size = 1.0);
    
    virtual void doSomething();
    
private:
    int m_nPeas;
};

//class ShotStopActor : public Actor  // Walls and Factories
//{
//public:
//    ShotStopActor(StudentWorld* sp, int imageID, double startX, double startY, int dir = right, double size = 1.0);
//    
//    
//};


class Wall : public Actor   // public shotStopper ???
{
public:
    Wall(StudentWorld* sp, double startX, double startY, double size = 1.0);
    
    
    
};

#endif // ACTOR_H_
