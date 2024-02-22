#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


// Actor Implementations:

Actor::Actor(StudentWorld* sp, int imageID, double startX, double startY, int dir, double size) : GraphObject(imageID, startX, startY, dir, size), m_world(sp)
{}


// KillableActor Implementations:

KillableActor::KillableActor(int health,StudentWorld* sp, int imageID, double startX, double startY, int dir, double size) : Actor(sp, imageID, startX, startY, dir, size), m_health(health) {}

// SentientActor Implementations:

SentientActor::SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int dir, double size) : KillableActor(health, sp, imageID, startX, startY) {}


// Player Implementations:

Player::Player(StudentWorld* sp, double startX, double startY, int dir, double size) : SentientActor(START_PLAYER_HEALTH, sp, IID_PLAYER, startX, startY, dir, size), m_nPeas(PLAYER_START_PEAS) {}
    // update dir to face right


void Player::doSomething()      // fix me!
{
    if(isDead())
        return;
    
    std::cout << "xeris!\n";
}




// Wall Implementations:

Wall::Wall(StudentWorld* sp, double startX, double startY, double size) : Actor(sp, IID_WALL, startX, startY, none, size) {}

