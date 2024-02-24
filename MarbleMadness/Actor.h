#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

const int START_PLAYER_HEALTH = 20;
const int PLAYER_START_PEAS = 20;
const int PEA_DAMAGE = 2;

enum OcStatus
{
    OC_ERROR = -1,
    OC_NON_BARRIER,             // goodies, exit, peas
    OC_BARRIER_NON_SHOTSTOP,    // pit, pushable
    OC_KILLABLE_SHOTSTOP,       // marbles, player, robots
    OC_UNKILLABLE_SHOTSTOP,     // walls and factories
    END_NOT_A_STATUS            // end of enum, not a valid status
};

const double EPSILON = 1e-10;
bool AreEqual(double d1, double d2);


class StudentWorld;

class Actor : public GraphObject
{
    public:
        Actor(StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir);
        
        virtual void doSomething();   // needs more functionality
        virtual void doSomethingSpecific() = 0;
        virtual bool getAttacked();     // make void?
        void MoveOne();
            
        int GetOcStatus() const;
        StudentWorld* GetWorld() const;
        virtual int GetXPValue() const;
        bool isDead() const;
        void Die();
        virtual void HandleDeath();
        virtual void PlayDeadSound() const {}
        virtual void SpecificDeathAction() {}
        
    

    private:
        StudentWorld* m_world;
        bool m_isDead;
        int m_occupancyStatus;
        int m_XPVal;

        
        //direction, image ID, visibility provided in graph object
};


class KillableActor : public Actor      // Actors that can be "killed" by peas, player, robots, and marble are killable
{
    public:
    KillableActor(int health,StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir);
        
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
        SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir);
    
        void Attack();          
        bool Move();

        virtual void SpecificGetAttacked();
    
        virtual void PlayAttackedSound() const = 0;
        virtual void PlayFireSound() const = 0;
        
};


class Player : public SentientActor     // Handle Player Death
{
public:
    Player(StudentWorld* sp, double startX, double startY);
    
    virtual void doSomethingSpecific();
    virtual void PlayAttackedSound () const;
    virtual void PlayDeadSound() const;
    virtual void PlayFireSound() const;
    
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
