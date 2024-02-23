#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

bool AreEqual(double d1, double d2)
{
    return abs(d1 - d2) < EPSILON;
}

// Actor Implementations:

Actor::Actor(StudentWorld* sp, int imageID, double startX, double startY, bool pushable, int dir) : GraphObject(imageID, startX, startY, dir), m_world(sp), m_isPushable(pushable), m_isDead(false)
{}

bool Actor::getAttacked()
{
    return false;
}

void Actor::HandleDeath()
{
    Die();
    PlayDeadSound();
    GetWorld()->increaseScore(GetXPValue());
    SpecificDeathAction();
}

void Actor::doSomething()
{
    if(isDead())
        return;
    
    doSomethingSpecific();
}

// KillableActor Implementations:

KillableActor::KillableActor(int health,StudentWorld* sp, int imageID, double startX, double startY, bool pushable, int dir) : Actor(sp, imageID, startX, startY, pushable, dir), m_health(health) {}

bool KillableActor::getAttacked()
{
    m_health -= PEA_DAMAGE;
    SpecificGetAttacked();
    
    if(m_health <= 0)
        HandleDeath();
    
    return true;
}

// SentientActor Implementations:

SentientActor::SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int dir) : KillableActor(health, sp, imageID, startX, startY, false, dir) {}

void SentientActor::SpecificGetAttacked()
{
    // IMPLEMENT ME
    std::cerr << "will implement soon ... \n";
}

bool SentientActor::Move()
{
    if(GetWorld()->CanWalk(getX(), getY(), getDirection())){
        moveForward();
        return true;
    }
    return false;
}

void SentientActor::Attack()
{
    GetWorld()->FireFrom(getX(), getY(), getDirection());
    PlayFireSound();
}

// Player Implementations:

Player::Player(StudentWorld* sp, double startX, double startY) : SentientActor(START_PLAYER_HEALTH, sp, IID_PLAYER, startX, startY, right), m_nPeas(PLAYER_START_PEAS) {}
    // update dir to face right


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
                Attack();
                break;
            case KEY_PRESS_ESCAPE:
                Die();
                return;
            default:
                break;
        }
    }
}


void Player::PlayAttackedSound()
{
    GetWorld()->playSound(SOUND_PLAYER_IMPACT);
}

void Player::PlayDeadSound()
{
    GetWorld()->playSound(SOUND_PLAYER_DIE);
}

void Player::PlayFireSound()
{
    GetWorld()->playSound(SOUND_PLAYER_FIRE);
}


// Wall Implementations:

Wall::Wall(StudentWorld* sp, double startX, double startY) : Actor(sp, IID_WALL, startX, startY, none) {}


// Pea Implementations

Pea::Pea(StudentWorld* sp, double startX, double startY, int dir) : Actor(sp, IID_PEA, startX, startY, dir)
{}

void Pea::doSomethingSpecific()
{
    if(GetWorld()->AttackSquare(getX(), getY()))
    {
        HandleDeath();
        return;
    }
    moveForward();
    if(GetWorld()->AttackSquare(getX(), getY()) )
        HandleDeath();
}

bool Pea::isBarrier()
{
    return false;
}
