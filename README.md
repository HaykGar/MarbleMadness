Start by cloning the repository by entering the following command on the command line ($ simply represents the beginning of the line).
$ git clone https://github.com/HaykGar/MarbleMadness.git

This game should run fine on Windows, however, MacOS and Linux users will need to take a couple of extra steps to run the game. 

macOS:
  Install XQuartz:
  Go to https://www.xquartz.org/ then, after selecting dowload, open the downloaded .pkg file, XQuartz.pkg, and follow the installation instructions.
  Log out and log back in again. (It's possible that you might not have to do this step, but we're not sure.)
  To verify the installation, open a Terminal window and run the command echo $DISPLAY. That should produce one line of output that ends with org.xquartz:0.

Linux:
  Install OpenGL and freeGLUT (already installed on the SEASnet Linux servers cs32.seas.ucla.edu, lnxsrv06.seas.ucla.edu, lnxsrv07.seas.ucla.edu, and lnxsrv09.seas.ucla.edu):
  Ubuntu: sudo apt-get install freeglut3 freeglut3-dev
  Fedora: sudo yum install freeglut freeglut-devel


Game Details:

In Marble Madness, the player starts out a new game with three lives and continues to play until all of his/her lives have been exhausted. There are multiple levels in Marble Madness,
beginning with level 0, and each level has its own maze. During each level, theplayer must gather all of the blue crystals within the current maze before the exit is revealed and they may use it to move on to the next level. Upon starting each level, the player’s avatar is placed in a maze filled with one or more blue crystals, marbles, pits, robots, robot factories and other goodies. The player may use the arrow keys to move their avatar (the Indiana Jones-looking character) left, right, up and down through the maze. They may walk on any square so long as it doesn’t have a wall, a marble, a factory, a robot, or a pit on it. The player may walk onto a square containing a marble if they are able to push it out of the way first. In addition to walking around the maze, the player may also shoot their pea cannon – but beware, it has only limited peas. Peas can destroy robots as well as marbles, but it takes more than one shot to do so.

There are three different types of goodies distributed throughout the maze that the player can collect in addition to blue crystals. If the player’s avatar steps upon the same square as an extra life goodie, it instantly gives the player an extra life. If the avatar steps onto the same square as a restore health goodie, it will restore the player to full health (in case of having been injured by shots from the robots). Finally, if the avatar steps onto the same square as an ammo goodie, it will give the player 20 additional peas. 

There are four major types of robots in Marble Madness: Horizontal RageBots, Vertical RageBots, Regular ThiefBots and Mean ThiefBots. As mentioned, RageBots fall into two categories – Horizontal RageBots and Vertical RageBots. Horizonal RageBots simply move back and forth on a row of the screen (only reversing course when they run into an obstacle), shooting at the player’s avatar if he/she ever walks in their line of sight. Vertical RageBots are identical to their Horizontal cousins, but move up and down within a single column of the maze. RageBots can start out anywhere in the maze (depending on where the designer of the maze choses to put them). In contrast to the RageBot, the ThiefBots are a bit nastier. These robots wander around
the maze looking for goodies to steal. If they happen to step onto a goodie (an extra life goodie, a restore health goodie, or an ammo goodie) and they don’t already hold one, they may pick it up for themselves. As with RageBots, there are two types of ThiefBots: Regular ThiefBots and Mean ThiefBots. Regular ThiefBots simply wander aimlessly around the maze looking for, and picking up, goodies. They are otherwise harmless and will not fire upon the player’s avatar. In contrast, in addition to picking up goodies, Mean ThiefBots will fire a pea anytime the player steps in their path. So beware! When any type of ThiefBot dies, if it previously picked up a goodie, it will drop this object upon its square in the maze. 

ThiefBots are created by ThiefBot factories, of which there are two types – one that produces Regular ThiefBots and one that produces Mean ThiefBots. ThiefBots never start out in the maze; they are added only by factories. Once the player has collected all of the blue crystals within the current maze, an exit will appear. The exit is invisible and unusable until all of the crystals have been collected from the level. Once the exit has been revealed, the player must direct their avatar to the exit in order to advance to the next level. The player will be granted 2000 points for exiting a level. The player will also be given a bonus for completing the level quickly, if they did so fast enough. The game is complete once the player has used the exit on the last level.

If the player’s health reaches zero (the player loses health when shot), their avatar dies and loses one “life.” If, after losing a life, the player has one or more remaining lives left, they are placed back on the current level and they must again solve the entire level from scratch (with the level starting as it was at the beginning of the first time it was attempted). The player will restart the level with full health points, as well as 20 peas (regardless of how many they had when they died). If the avatar dies and has no lives left,
then the game is over.

Disclaimer: this is a project I completed for computer science course at UCLA. The game is not my idea, and all of the code I wrote is in Actor.h, Actor.cpp, StudentWorld.h, and StudentWorld.cpp.
