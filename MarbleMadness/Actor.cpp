#include "Actor.h"
#include "StudentWorld.h"


// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

bool AreEqual(double d1, double d2)
{
    return abs(d1 - d2) < EPSILON;
}

// Actor Implementations:

Actor::Actor(StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp) : GraphObject(imageID, startX, startY, dir), m_world(sp), m_isDead(false), m_occupancyStatus(ocStat), m_XPVal(xp)
{}

bool Actor::getAttacked()   // virtual
{
    return false;
}

void Actor::doSomething()
{
    if(isDead())
        return;
    
    doSomethingSpecific();
}



void Actor::HandleDeath()
{
    Die();
    PlayDeadSound();
    GetWorld()->increaseScore(GetXPValue());
    SpecificDeathAction();
}

void Actor::MoveOne()
{
    StudentWorld* world = GetWorld();
    world->LeaveSquare(this);
    moveForward();
    world->OccupySquare(this);
}

// KillableActor Implementations:

KillableActor::KillableActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp) : Actor(sp, imageID, startX, startY, ocStat, dir, xp), m_health(health) {}

bool KillableActor::getAttacked()
{
    m_health -= PEA_DAMAGE;
    SpecificGetAttacked();
    
    if(m_health <= 0)
        HandleDeath();
    
    return true;
}

// SentientActor Implementations:

SentientActor::SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp) : KillableActor(health, sp, imageID, startX, startY, ocStat, dir, xp) {}

void SentientActor::SpecificGetAttacked()
{
    // IMPLEMENT ME
    std::cerr << "will implement soon ... \n";
}

bool SentientActor::Move()
{
    if( GetWorld()->CanWalk(this) )
    {
        MoveOne();
        return true;
    }
    return false;
}

void SentientActor::Attack()
{
    GetWorld()->FireFrom(this);
    PlayFireSound();
}

// Player Implementations:

Player::Player(StudentWorld* sp, double startX, double startY) : SentientActor(START_PLAYER_HEALTH, sp, IID_PLAYER, startX, startY, OC_KILLABLE_SHOTSTOP, right, 0), m_nPeas(PLAYER_START_PEAS) {}


void Player::PlayerFire()
{
    if(m_nPeas > 0)
    {
        Attack();
        DecPeas();
    }
    else
        std::cerr << "out of ammo!\n";
}

void Player::doSomethingSpecific()      // fix me!
{
    int ch;
    if(GetWorld()->getKey(ch))
    {
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                setDirection(left);
                Move();
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                Move();
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                Move();
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                Move();
                break;
            case KEY_PRESS_SPACE:
                PlayerFire();
                break;
            case KEY_PRESS_ESCAPE:
                Die();
                return;
            default:
                break;
        }
    }
}

// Do NOT inline the sound functions
void Player::PlayAttackedSound() const
{
    GetWorld()->playSound(SOUND_PLAYER_IMPACT);
}

void Player::PlayDeadSound() const
{
    GetWorld()->playSound(SOUND_PLAYER_DIE);
}

void Player::PlayFireSound() const
{
    GetWorld()->playSound(SOUND_PLAYER_FIRE);
}




// Wall Implementations:

Wall::Wall(StudentWorld* sp, double startX, double startY) : Actor(sp, IID_WALL, startX, startY, OC_UNKILLABLE_SHOTSTOP, none, 0) {}

// Pea Implementations

Pea::Pea(StudentWorld* sp, double startX, double startY, int dir) : Actor(sp, IID_PEA, startX, startY, OC_NON_BARRIER, dir, 0)
{}

void Pea::doSomethingSpecific()
{
    if(GetWorld()->AttackSquare(getX(), getY()))
    {
        HandleDeath();
        return;
    }
    MoveOne();
    if(GetWorld()->AttackSquare(getX(), getY()) )
        HandleDeath();
}
