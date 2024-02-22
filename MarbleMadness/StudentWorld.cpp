#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{}

StudentWorld::~StudentWorld()
{
    long int last = m_Actors.size() - 1;
    while(last >= 0)
    {
        delete m_Actors[last];
        m_Actors.pop_back();
        last--;
    }
}


int StudentWorld::init()
{
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel("level00.txt");
    if (result == Level::load_fail_file_not_found)
            cerr << "Could not find level00.txt data file\n";
    else if (result == Level::load_fail_bad_format)
        cerr << "Your level was improperly formatted\n";
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level\n";
        for(int x = 0; x < VIEW_WIDTH; x++)
        {
            for(int y = 0; y < VIEW_HEIGHT; y++)
            {
                Level::MazeEntry ge = lev.getContentsOf(x,y);
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::exit:
                        // make exit
                        break;
                    case Level::player:
                        m_Actors.push_back(new Player(this, x, y));
                        break;
                    case Level::wall:
                        m_Actors.push_back(new Wall(this, x, y));
                        break;
                    default:
                        std::cout << "kliri glux\n";
                }
            }
        }
    }
    
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q

    setGameStatText("Game will end when you type q");
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
}



//void StudentWorld::someFunc()
//{
//    Level lev(assetPath());
//    Level::LoadResult result = lev.loadLevel("level00.txt");
//    if (result == Level::load_fail_file_not_found)
//            cerr << "Could not find level00.txt data file\n";
//    else if (result == Level::load_fail_bad_format)
//        cerr << "Your level was improperly formatted\n";
//    else if (result == Level::load_success)
//    {
//        cerr << "Successfully loaded level\n";
//        Level::MazeEntry ge = lev.getContentsOf(5,10); // x=5, y=10
//        switch (ge)
//        {
//            case Level::empty:
//                cout << "5,10 is empty\n";
//                break;
//            case Level::exit:
//                cout << "5,10 is where the exit is\n";
//                break;
//            case Level::player:
//                cout << "5,10 is where the player starts\n";
//                break;
//            case Level::horiz_ragebot:
//                cout << "5,10 starts with a horiz. RageBot\n";
//                break;
//            case Level::vert_ragebot:
//                cout << "5,10 starts with a vertical RageBot\n";
//                break;
//            case Level::thiefbot_factory:
//                cout << "5,10 holds a ThiefBot factory\n";
//                break;
//            case Level::enraged_thiefbot_factory:
//                cout << "5,10 holds an enraged ThiefBot factory\n";
//                break;
//            case Level::wall:
//                22
//                cout << "Location 5,10 holds a wall\n";
//                break;
//        }
//    }
//}
