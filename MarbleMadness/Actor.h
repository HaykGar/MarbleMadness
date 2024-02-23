#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

const int START_PLAYER_HEALTH = 20;
const int PLAYER_START_PEAS = 20;

const double EPSILON = 1e-10;

const int PEA_DAMAGE = 2;

bool AreEqual(double d1, double d2);


class StudentWorld;

class Actor : public GraphObject
{
    public:
        Actor(StudentWorld* sp, int imageID, double startX, double startY, bool pushable, int dir = right);
        
        virtual void doSomething();   // needs more functionality
        virtual void doSomethingSpecific() = 0;
        virtual bool getAttacked();     // make void?
            
        virtual int GetXPValue() { return 0; }
        
        void Die()          { m_isDead = true; }
        bool isDead() const { return m_isDead; }
        virtual void HandleDeath();
        virtual void PlayDeadSound() {}
        virtual void SpecificDeathAction() {}
    
        virtual bool isBarrier() { return true; }
        
    
        StudentWorld* GetWorld() { return m_world; }
        
    

    private:
        StudentWorld* m_world;
        bool m_isDead;
        bool m_isPushable;

        
        //direction, image ID, visibility provided in graph object
};


class KillableActor : public Actor      // Actors that can be "killed" by peas, player, robots, and marble are killable
{
    public:
        KillableActor(int health,StudentWorld* sp, int imageID, double startX, double startY, bool pushable, int dir = right);
        
    virtual void SpecificGetAttacked() {}
    virtual bool getAttacked();
        
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
        SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int dir = right);
    
        void Attack();          
        bool Move();

        virtual void SpecificGetAttacked();
    
        virtual void PlayAttackedSound() = 0;
        virtual void PlayFireSound() = 0;
        
};


class Player : public SentientActor     // Handle Player Death
{
public:
    Player(StudentWorld* sp, double startX, double startY);
    
    virtual void doSomethingSpecific();
    virtual void PlayAttackedSound ();
    virtual void PlayDeadSound ();
    virtual void PlayFireSound();
    
private:
    int m_nPeas;
};


// class unkillable actor?


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
    Wall(StudentWorld* sp, double startX, double startY);
    
    virtual void doSomethingSpecific() {}
};


class Pea : public Actor    // change parent class later
{
public:
    Pea(StudentWorld* sp, double startX, double startY, int dir);
    
    virtual void doSomethingSpecific();
    virtual bool isBarrier();
};

#endif // ACTOR_H_
