#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

const int START_PLAYER_HEALTH = 20;
const int MARBLE_HP = 10;

const int PLAYER_START_PEAS = 20;
const int PEA_DAMAGE = 2;
const int AMMO_EXTRA_PEAS = 20;

const int LEVEL_FINISH_XP = 2000;
const int AMMO_XP = 100;
const int EXTRA_LIFE_XP = 1000;
const int CRYSTAL_XP = 50;



const double EPSILON = 1e-10;
bool AreEqual(double d1, double d2);


class StudentWorld;

class Actor : public GraphObject
{
    public:
        Actor(StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp);
        virtual ~Actor() {}
        
        int doSomething();
        virtual int doSomethingSpecific() = 0;
        virtual void getAttacked() {}
        void MoveOne();
            
        int GetOcStatus() const;
        StudentWorld* GetWorld() const;
        virtual int GetXPValue() const;
        bool isDead() const;
        void Die();
        virtual void HandleDeath();
        virtual void PlayDeadSound() const {}
        virtual void SpecificDeathAction() {}
    
        virtual bool Pushable() const;
        virtual bool Push(int dir = none);
    
        enum OcStatus    // important: everything after non_shotstop stops peas
        {
            OC_ERROR = -1,
            OC_NON_BARRIER,             // goodies, exit, peas
            OC_BARRIER_NON_SHOTSTOP,    // pit (can push into)
            OC_KILLABLE_SHOTSTOP,       // marbles, player, robots
            OC_UNKILLABLE_SHOTSTOP,     // walls and factories
            END_NOT_A_STATUS            // end of enum, not a valid status
        };
        
    

    private:
        StudentWorld* m_world;
        bool m_isDead;
        int m_occupancyStatus;
        int m_XPVal;
};

inline int Actor::GetOcStatus() const
{
    return m_occupancyStatus;
}

inline int Actor::GetXPValue() const
{
    return m_XPVal;
}

inline StudentWorld* Actor::GetWorld() const
{
    return m_world;
}

inline bool Actor::isDead() const
{
    return m_isDead;
}

inline void Actor::Die()
{
    m_isDead = true;
}

inline bool Actor::Pushable() const
{
    return false;
}

inline bool Actor::Push(int dir)
{
    return false;
}


class KillableActor : public Actor      // Actors that can be "killed" by peas, player, robots, and marble are killable
{
    public:
    KillableActor(int health,StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp);
    virtual ~KillableActor() {}
        
    virtual void getAttacked();
        
    void SetHealth(int health);
    int GetHealth() const;

    private:
        int m_health;
};


inline void KillableActor::SetHealth(int health)
{
    m_health = health;
}

inline int KillableActor::GetHealth() const
{
    return m_health;
}


class Marble : public KillableActor
{
public:
    Marble(StudentWorld* sp, double startX, double startY);
    virtual int doSomethingSpecific();
    virtual bool Pushable() const;
    virtual bool Push(int dir);
private:
    void MarbleMove(int dir);
};

inline bool Marble::Pushable() const
{
    return true;
}

inline int Marble::doSomethingSpecific()
{
    return GWSTATUS_CONTINUE_GAME;
}


class SentientActor : public KillableActor  // player and Robots... specific shared behaviors
{
    public:
        SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp);
        virtual ~SentientActor() {}
    
        void Attack();          
        bool Move();
        virtual bool WalkCondition();

        virtual void getAttacked();
    
        virtual void PlayAttackedSound() const = 0;
        virtual void PlayFireSound() const = 0;
        
};


class Player : public SentientActor     // Handle Player Death
{
public:
    Player(StudentWorld* sp, double startX, double startY);
    
    virtual int doSomethingSpecific();
    virtual void PlayAttackedSound () const;
    virtual void PlayDeadSound() const;
    virtual void PlayFireSound() const;
    void AddPeas(unsigned int amount);
    void DecPeas();
    int GetCurrentAmmo() const;
    void PlayerFire();
    
    virtual bool WalkCondition();
    
private:
    int m_nPeas;
};

inline void Player::DecPeas()
{
    m_nPeas--;
}

inline int Player::GetCurrentAmmo() const
{
    return m_nPeas;
}

inline void Player::AddPeas(unsigned int amount)
{
    m_nPeas += amount;
}


class Wall : public Actor   // public shotStopper ???
{
public:
    Wall(StudentWorld* sp, double startX, double startY);
    
    virtual int doSomethingSpecific();
};

inline int Wall::doSomethingSpecific()
{
    return GWSTATUS_CONTINUE_GAME;
}


class Pea : public Actor    // change parent class later
{
public:
    Pea(StudentWorld* sp, double startX, double startY, int dir);
    
    virtual int doSomethingSpecific();
};


class Exit : public Actor
{
    public:
    
    Exit(StudentWorld* sp, double startX, double startY);
    
    virtual int doSomethingSpecific();
    virtual void PlayDeadSound() const;
};

class Goodie : public Actor
{
public:
    Goodie(StudentWorld* sp, int imageID, double startX, double startY, int xp);
    virtual ~Goodie() {}
    
    virtual void GiveSpecificBenefit() = 0;
    virtual int doSomethingSpecific();
    virtual void PlayDeadSound() const;
};

class AmmoGoodie : public Goodie
{
public:
    AmmoGoodie(StudentWorld* sp, double startX, double startY);
    virtual void GiveSpecificBenefit();
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(StudentWorld* sp, double startX, double startY);
    virtual void GiveSpecificBenefit();
};

class Crystal : public Goodie
{
public:
    Crystal(StudentWorld* sp, double startX, double startY);
    virtual void GiveSpecificBenefit();
};

class Pit : public Actor
{
public:
    Pit(StudentWorld* sp, double startX, double startY);
    virtual int doSomethingSpecific();
};

class Robot : public SentientActor
{
public:
    Robot(int health, StudentWorld* sp, int imageID, double startX, double startY, int dir, int xp);
    
    const int MIN_TICKS_PER_ACTION = 3;
    
    bool NeedsToRest() const;
    virtual int doSomethingSpecific();
    virtual void SpecialRobotAction() = 0;
    
    virtual void PlayDeadSound() const;
    virtual void PlayAttackedSound() const;
    virtual void PlayFireSound() const;
    
private:
    unsigned int m_ticksWaited;
    int m_ticksPerAction;
};

inline
bool Robot::NeedsToRest() const
{
    return m_ticksWaited < m_ticksPerAction - 1;
}

#endif // ACTOR_H_
