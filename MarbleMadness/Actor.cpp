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

Actor::Actor(StudentWorld* sp, int imageID, double startX, double startY, int dir, double size) : GraphObject(imageID, startX, startY, dir, size), m_world(sp)
{}


// KillableActor Implementations:

KillableActor::KillableActor(int health,StudentWorld* sp, int imageID, double startX, double startY, int dir, double size) : Actor(sp, imageID, startX, startY, dir, size), m_health(health) {}

// SentientActor Implementations:

SentientActor::SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int dir, double size) : KillableActor(health, sp, imageID, startX, startY) {}

void SentientActor::Move()
{    
    if(GetWorld()->CanWalk(getX(), getY(), getDirection()))
        moveForward();
}


// Player Implementations:

Player::Player(StudentWorld* sp, double startX, double startY, int dir, double size) : SentientActor(START_PLAYER_HEALTH, sp, IID_PLAYER, startX, startY, dir, size), m_nPeas(PLAYER_START_PEAS) {}
    // update dir to face right


void Player::doSomething()      // fix me!
{
    if(isDead())
        return;
    
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
                // fire pea, m_nPeas--
                break;
            case KEY_PRESS_ESCAPE:
                Die();
                return;
            default:
                break;
        }
    }
}




// Wall Implementations:

Wall::Wall(StudentWorld* sp, double startX, double startY, double size) : Actor(sp, IID_WALL, startX, startY, none, size) {}

