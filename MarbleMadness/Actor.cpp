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


int Actor::doSomething()
{
    if(isDead())
        return GWSTATUS_CONTINUE_GAME;
    
    return doSomethingSpecific();
}



void Actor::HandleDeath()
{
    GetWorld()->LeaveSquare(this);
    Die();
    PlayDeadSound();
    GetWorld()->increaseScore(GetXPValue());
}

void Actor::MoveOne()
{
    GetWorld()->LeaveSquare(this);
    moveForward();
    GetWorld()->OccupySquare(this);
}

// KillableActor Implementations:

KillableActor::KillableActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp) : Actor(sp, imageID, startX, startY, ocStat, dir, xp), m_health(health) {}

void KillableActor::getAttacked()
{
    m_health -= PEA_DAMAGE;
    
    if(m_health <= 0)
        HandleDeath();
}


// Marble Implementations

Marble::Marble(StudentWorld* sp, double startX, double startY) : KillableActor(MARBLE_HP_START, sp, IID_MARBLE, startX, startY, OC_KILLABLE_SHOTSTOP, none, 0)
{}

void Marble::MarbleMove(int dir)    // find nicer way?
{
    if(dir != left && dir != right && dir != up && dir != down){
        std::cerr << "invalid direction for marble movement\n";
        return;
    }
    setDirection(dir);
    MoveOne();
    setDirection(none);
}

bool Marble::Push(int dir)
{
    double x = getX();
    double y = getY();
    
    switch(dir)
    {
        case left:
            x--;
            break;
        case right:
            x++;
            break;
        case up:
            y++;
            break;
        case down:
            y--;
            break;
    }
    if(!GetWorld()->SquarePushable(x, y))
        return false;
    else{
        MarbleMove(dir);
        return true;
    }
}


// SentientActor Implementations:

SentientActor::SentientActor(int health, StudentWorld* sp, int imageID, double startX, double startY, int ocStat, int dir, int xp) : KillableActor(health, sp, imageID, startX, startY, ocStat, dir, xp) {}

void SentientActor::getAttacked()
{
    KillableActor::getAttacked();
    if(!isDead())
        PlayAttackedSound();
}

bool SentientActor::WalkCondition()
{
    return GetWorld()->CanWalk(this);
}

bool SentientActor::Move()
{
    if( WalkCondition() )
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

bool Player::WalkCondition()
{
    return SentientActor::WalkCondition() || GetWorld()->TryToPush();
}

int Player::doSomethingSpecific()
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
                HandleDeath();
                return GWSTATUS_PLAYER_DIED;
            default:
                break;
        }
    }
    return GWSTATUS_CONTINUE_GAME;
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


// Robot Implementations

Robot::Robot(int health, StudentWorld* sp, int imageID, double startX, double startY, int dir, int xp) : SentientActor(health, sp, imageID, startX, startY, OC_KILLABLE_SHOTSTOP, dir, xp), m_ticksWaited(0)
{
    m_ticksPerAction = ( 28 - GetWorld()->getLevel() ) / 4;
    if(m_ticksPerAction < MIN_TICKS_PER_ACTION)
        m_ticksPerAction = MIN_TICKS_PER_ACTION;
}

int Robot::doSomethingSpecific()
{
    if(NeedsToRest())
    {
        m_ticksWaited++;
        return GWSTATUS_CONTINUE_GAME;
    }
    m_ticksWaited = 0;
    SpecialRobotAction();
    return GWSTATUS_CONTINUE_GAME;
}

bool Robot::ShootPlayer()
{
    if(GetWorld()->PlayerInLOS(this))
    {
        Attack();
        return true;
    }
    return false;
}

void Robot::PlayAttackedSound() const
{
    GetWorld()->playSound(SOUND_ROBOT_IMPACT);
}

void Robot::PlayDeadSound() const
{
    GetWorld()->playSound(SOUND_ROBOT_DIE);
}

void Robot::PlayFireSound() const
{
    GetWorld()->playSound(SOUND_ENEMY_FIRE);
}


// RageBot Implementations

RageBot::RageBot(StudentWorld* sp, double startX, double startY, int dir) : Robot(RAGEBOT_HP_START, sp, IID_RAGEBOT, startX, startY, dir, RAGEBOT_XP), m_dirIndex(false)
{
    switch (dir) // given right or down based on vertical or horizontal RageBot
    {
        case right:
            m_dirs[0] = right;
            m_dirs[1] = left;
            break;
        case down:
            m_dirs[0] = down;
            m_dirs[1] = up;
        default:
            std::cerr << "Error, invalid dir parameter for ragebot\n";
            break;
    }
}


void RageBot::SpecialRobotAction()
{
    if(ShootPlayer())
        return;
    if(!Move())
    {
        m_dirIndex = !m_dirIndex;
        ResetDirection();       // change dir to opposite direction
    }
}

// ThiefBot Implementations:

ThiefBot::ThiefBot(int health, StudentWorld* sp, double startX, double startY, int imageID, int xp) : Robot(health, sp, imageID, startX, startY, right, xp), m_dTravelled(0), m_StolenGoodie(NOT_GOODIE)
{
    m_dToTurn = randInt(1, 6);
    dirs[0] = left;
    dirs[1] = right;
    dirs[2] = up;
    dirs[3] = down;
}

void ThiefBot::ShuffleDirs()
{
    for(int i = 0; i < 4; i++)
    {
        std::swap(dirs[i], dirs[randInt(i, 3)]);
    }
}

void ThiefBot::SpecialRobotAction()
{
    if(m_StolenGoodie == NOT_GOODIE)
    {
        int goodieFound = GetWorld()->GoodieHere(getX(), getY());
        if(goodieFound != NOT_GOODIE)
        {
            m_StolenGoodie = goodieFound;
            GetWorld()->playSound(SOUND_ROBOT_MUNCH);
            return;
        }
    }
    if( !(m_dTravelled < m_dToTurn && Move()) ) // failed to move
    {
        m_dToTurn = randInt(1, 6);
        m_dTravelled = 0;
        
        int prevDir = getDirection();
        ShuffleDirs();
        for(int i = 0; i < 4; i++)
        {
            setDirection(dirs[i]);
            if(Move())
            {
                m_dTravelled++;
                return;
            }
        }
        setDirection(prevDir);  // could not move in any direction
    }
    else
        m_dTravelled++;
}

void ThiefBot::getAttacked()
{
    SentientActor::getAttacked();
    if(isDead())
        GetWorld()->HandleThiefBotDeath(this);
}

// Wall Implementations:

Wall::Wall(StudentWorld* sp, double startX, double startY) : Actor(sp, IID_WALL, startX, startY, OC_UNKILLABLE_SHOTSTOP, none, 0) {}

// Pea Implementations

Pea::Pea(StudentWorld* sp, double startX, double startY, int dir) : Actor(sp, IID_PEA, startX, startY, OC_NON_BARRIER, dir, 0)
{}

int Pea::doSomethingSpecific()
{
    if(GetWorld()->AttackSquare(getX(), getY()))
    {
        HandleDeath();
        return GWSTATUS_CONTINUE_GAME;
    }
    MoveOne();
    if(GetWorld()->AttackSquare(getX(), getY()) )
        HandleDeath();
    
    return GWSTATUS_CONTINUE_GAME;
}

// Exit Implementations

Exit::Exit(StudentWorld* sp, double startX, double startY) : Actor(sp, IID_EXIT, startX, startY, OC_NON_BARRIER, none, LEVEL_FINISH_XP)
{
    setVisible(false);
}

void Exit::PlayDeadSound() const
{
    GetWorld()->playSound(SOUND_FINISHED_LEVEL);
}

int Exit::doSomethingSpecific()
{
    if(!isVisible() && !GetWorld()->CrystalsLeft()) // no crystals left
    {
        setVisible(true);
        GetWorld()->playSound(SOUND_REVEAL_EXIT);
    }
    if( isVisible() && GetWorld()->SamePosAsPlayer(this) )
    {
        HandleDeath();
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

// Goodie Implementations
Goodie::Goodie(StudentWorld* sp, int imageID, double startX, double startY, int xp) : Actor(sp, imageID, startX, startY, OC_NON_BARRIER, none, xp)
{}

int Goodie::doSomethingSpecific()
{
    if(GetWorld()->SamePosAsPlayer(this))
    {
        HandleDeath();  // die and give XP points to player
        GiveSpecificBenefit();
    }
    return GWSTATUS_CONTINUE_GAME;
}

void Goodie::PlayDeadSound() const
{
    GetWorld()->playSound(SOUND_GOT_GOODIE);
}

// Ammo Goodie Implementations

AmmoGoodie::AmmoGoodie(StudentWorld* sp, double startX, double startY) : Goodie(sp, IID_AMMO, startX, startY, AMMO_XP)
{}

void AmmoGoodie::GiveSpecificBenefit()
{
    GetWorld()->GivePlayerPeas();
}

// Extra Life Goodie Implementations
ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* sp, double startX, double startY) : Goodie(sp, IID_EXTRA_LIFE, startX, startY, EXTRA_LIFE_XP)
{}

void ExtraLifeGoodie::GiveSpecificBenefit()
{
    GetWorld()->incLives();
}

// Restore Health Implementations

RestoreHealthGoodie::RestoreHealthGoodie(StudentWorld* sp, double startX, double startY) : Goodie(sp, IID_RESTORE_HEALTH, startX, startY, RESTORE_HEALTH_XP)
{}

void RestoreHealthGoodie::GiveSpecificBenefit()
{
    GetWorld()->RestorePlayerHealth();
}

// Crystal Implementations

Crystal::Crystal (StudentWorld* sp, double startX, double startY) : Goodie(sp, IID_CRYSTAL, startX, startY, CRYSTAL_XP)
{}

void Crystal::GiveSpecificBenefit()
{
    GetWorld()->DecCrystals();  // let world know there are one fewer crystals
}

// Pit Implementations
Pit::Pit(StudentWorld* sp, double startX, double startY) : Actor(sp, IID_PIT, startX, startY, OC_BARRIER_NON_SHOTSTOP, none, 0)
{}

int Pit::doSomethingSpecific()
{
    if(GetWorld()->MarbleWithPit(this)){
        HandleDeath();
    }
    
    return GWSTATUS_CONTINUE_GAME;
}
