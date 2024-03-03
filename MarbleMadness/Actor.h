#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>
#include <iostream>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

const int START_PLAYER_HEALTH = 20;
const int MARBLE_HP_START = 10;
const int RAGEBOT_HP_START = 10;
const int REG_THIEFBOT_HP_START = 5;
const int MEAN_THIEFBOT_HP_START = 8;

const int PLAYER_START_PEAS = 20;
const int PEA_DAMAGE = 2;
const int AMMO_EXTRA_PEAS = 20;

const int LEVEL_FINISH_XP = 2000;
const int AMMO_XP = 100;
const int EXTRA_LIFE_XP = 1000;
const int RESTORE_HEALTH_XP = 500;
const int CRYSTAL_XP = 50;
const int RAGEBOT_XP = 100;
const int REG_THIEFBOT_XP = 10;
const int MEAN_THIEFBOT_XP = 20;




const double EPSILON = 1e-10;
bool AreEqual(double d1, double d2);


class StudentWorld;
class Marble;

// Actor

class Actor : public GraphObject
{
    public:
        Actor(StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp);
        virtual ~Actor() {}
        
        void SetPos(double x, double y);
        int doSomething();  // make void, use bool member variable in student world to keep track of game ending
        virtual void getAttacked() {}
        void MoveOne();
            
        int GetOcStatus() const;
        StudentWorld* GetWorld() const;
        virtual int GetXPValue() const;
        bool isDead() const;
        void Die();
        void HandleDeath();
    
        virtual bool Pushable() const;
        virtual bool Push(int dir);
        virtual bool CanPushInto() const;
    
        virtual bool isStealableGoodie() const;
        virtual bool isThief() const;
        virtual Actor* StolenGoodie() const;
    
    
        enum OcStatus    // important: everything after non_shotstop stops peas
        {
            OC_ERROR = -1,
            OC_NON_BARRIER,             // goodies, exit, peas
            OC_BARRIER_NON_SHOTSTOP,    // pit (can push into),
            // shot stoppers:
            OC_KILLABLE_SHOTSTOP,       // marbles, player, robots
            OC_UNKILLABLE_SHOTSTOP,     // walls and factories
            END_NOT_A_STATUS            // end of enum, not a valid status
        };
        
    private:
        StudentWorld* m_world;
        bool m_isDead;
        int m_occupancyStatus;
        int m_XPVal;
    
        virtual int doSomethingSpecific() = 0;
        virtual void PlayDeadSound() const {}

};



inline bool Actor::isThief() const
{
    return false;
}

inline bool Actor::isStealableGoodie() const
{
    return false;
}

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



// Wall

class Wall : public Actor
{
public:
    Wall(StudentWorld* sp, double startX, double startY);
    
private:
    virtual int doSomethingSpecific();
};

inline int Wall::doSomethingSpecific()
{
    return GWSTATUS_CONTINUE_GAME;
}

// Pea

class Pea : public Actor
{
public:
    Pea(StudentWorld* sp, double startX, double startY, int dir);
    
private:
    virtual int doSomethingSpecific();
};

// Exit

class Exit : public Actor
{
    public:
    
    Exit(StudentWorld* sp, double startX, double startY);
    virtual void PlayDeadSound() const;
private:
    virtual int doSomethingSpecific();
};

// Goodie

class Goodie : public Actor
{
public:
    Goodie(StudentWorld* sp, int imageID, double startX, double startY, int xp);
    virtual ~Goodie() {}
    virtual void PlayDeadSound() const;
    virtual bool isStealableGoodie() const;
    
private:
    virtual void GiveSpecificBenefit() = 0;
    virtual int doSomethingSpecific();
};

inline bool Goodie::isStealableGoodie() const
{
    return true;
}

// Ammo Goodie

class AmmoGoodie : public Goodie
{
public:
    AmmoGoodie(StudentWorld* sp, double startX, double startY);
private:
    virtual void GiveSpecificBenefit();
};


// Extra Life

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(StudentWorld* sp, double startX, double startY);
private:
    virtual void GiveSpecificBenefit();
};


// Restore Health

class RestoreHealthGoodie : public Goodie
{
public:
    RestoreHealthGoodie(StudentWorld* sp, double startX, double startY);
private:
    virtual void GiveSpecificBenefit();
};

// Crystal

class Crystal : public Goodie
{
public:
    Crystal(StudentWorld* sp, double startX, double startY);
    virtual bool isStealableGoodie() const;
private:
    virtual void GiveSpecificBenefit();
};

inline bool Crystal::isStealableGoodie() const
{
    return false;
}

// Pit

class Pit : public Actor
{
public:
    Pit(StudentWorld* sp, double startX, double startY);
    virtual bool CanPushInto() const;
private:
    virtual int doSomethingSpecific();
};

inline bool Pit::CanPushInto() const // use function instead of Oc stat
{
    return true;
}


// KillableActor

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

// Marble

class Marble : public KillableActor
{
public:
    Marble(StudentWorld* sp, double startX, double startY);
    virtual bool Pushable() const;
    virtual bool Push(int dir);
private:
    virtual int doSomethingSpecific();
};

inline bool Marble::Pushable() const
{
    return true;
}

inline int Marble::doSomethingSpecific()
{
    return GWSTATUS_CONTINUE_GAME;
}

// SentientActor

class SentientActor : public KillableActor  // player and Robots... specific shared behaviors
{
    public:
        SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp);
        virtual ~SentientActor() {}
    
        void Attack();          
        bool Move();
        virtual bool WalkCondition();
        virtual void getAttacked();
    
    private:
        virtual void PlayAttackedSound() const = 0;
        virtual void PlayFireSound() const = 0;
};

// Player

class Player : public SentientActor     // Handle Player Death
{
public:
    Player(StudentWorld* sp, double startX, double startY);
    
    virtual void PlayDeadSound() const;
    void AddPeas(int amount);
    void DecPeas();
    int GetCurrentAmmo() const;
    void PlayerFire();
    
    virtual bool WalkCondition();
    
private:
    int m_nPeas;
    virtual int doSomethingSpecific();
    virtual void PlayAttackedSound () const;
    virtual void PlayFireSound() const;

};

inline void Player::DecPeas()
{
    m_nPeas--;
}

inline int Player::GetCurrentAmmo() const
{
    return m_nPeas;
}

inline void Player::AddPeas(int amount)
{
    if(amount > 0)
        m_nPeas += amount;
}

// Robot

class Robot : public SentientActor
{
public:
    Robot(int health, StudentWorld* sp, int imageID, double startX, double startY, int dir, int xp);
    
    const int MIN_TICKS_PER_ACTION = 3;
    
    bool NeedsToRest() const;
    bool ShootPlayer();
    
    virtual void PlayDeadSound() const;
    
    
private:
    unsigned int m_ticksWaited;
    int m_ticksPerAction;
    
    virtual void PlayAttackedSound() const;
    virtual void PlayFireSound() const;
    virtual int doSomethingSpecific();
    virtual void SpecialRobotAction() = 0;
};

inline
bool Robot::NeedsToRest() const
{
    return m_ticksWaited < m_ticksPerAction - 1;
}

// RageBot:

class RageBot : public Robot
{
public:
    RageBot(StudentWorld* sp, double startX, double startY, int dir);
    
private:
    bool m_dirIndex;
    int m_dirs[2];
    virtual void SpecialRobotAction();
    void ResetDirection();  // get rid of function
};

inline void RageBot::ResetDirection()
{
    setDirection(m_dirs[m_dirIndex]);
}

// ThiefBot

class ThiefBot : public Robot
{
public:
    ThiefBot(int health, StudentWorld* sp, double startX, double startY, int imageID, int xp);
    ThiefBot(StudentWorld* sp, double startX, double startY);
    
    virtual bool isThief() const;
    
    int GetTravelled() const;
    int GetToTurn() const;
    
    void SetTravelled(int amt);
    void SetToTurn(int amt);
    
    virtual Actor* StolenGoodie() const;
    
    virtual void getAttacked();
    
    bool StealGoodie();
    void ChangeDirection();
    virtual void SpecialRobotAction();

private:
    int m_dTravelled;
    int m_dToTurn;
    int dirs[4];
    Actor* m_stolenGoodie;
    
    void ShuffleDirs();
    
};

inline bool ThiefBot::isThief() const
{
    return true;
}


inline int ThiefBot::GetTravelled() const
{
    return m_dTravelled;
}
inline int ThiefBot::GetToTurn() const
{
    return m_dToTurn;
}
inline void ThiefBot::SetTravelled(int amt)
{
    m_dTravelled = amt;
}
inline void ThiefBot::SetToTurn(int amt)
{
    m_dToTurn = amt;
}


// Mean ThiefBot:

class MeanThiefBot : public ThiefBot
{
public:
    MeanThiefBot(StudentWorld* sp, double startX, double startY);
private:
    virtual void SpecialRobotAction();
};

// Factory

class Factory : public Actor
{
public:
    Factory(StudentWorld* sp, double startX, double startY);
private:
    virtual Actor* NewActor() const = 0;
    virtual int doSomethingSpecific();
    void ManufactureActor() const;
};

// RegFactory

class RegFactory : public Factory
{
public:
    RegFactory(StudentWorld* sp, double startX, double startY) : Factory(sp, startX, startY) {}    
private:
    virtual Actor* NewActor() const;
};

inline Actor* RegFactory::NewActor() const
{
    return new ThiefBot(GetWorld(), getX(), getY());
}

// MeanFactory

class MeanFactory : public Factory
{
public:
    MeanFactory(StudentWorld* sp, double startX, double startY) : Factory(sp, startX, startY) {}
private:
    virtual Actor* NewActor() const;
};

inline Actor* MeanFactory::NewActor() const
{
    return new MeanThiefBot(GetWorld(), getX(), getY());
}


#endif // ACTOR_H_
