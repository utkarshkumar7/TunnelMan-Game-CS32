#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>

const int POINTS_FOUND_OIL = 1000;
const int POINTS_FOUND_GOLD = 10;
const int POINTS_FOUND_SONARKIT = 75;
const int POINTS_FOUND_WATERPOOL = 100;


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}
    virtual ~StudentWorld();

    //Functions required to be implemented
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    //Functions used within init() to initialize
    void initBoulder();
    void initBarrel();
    void initGold();
    
    //Functions used within move()
    void removeDeadGameObjects(std::vector<Actor*>& v);
    void addGoodies();
    void addProtesters();
    
    //Additional functions used by Actor objects
    bool digEarth();
    bool checkBelowBoulder(int, int);
    bool collisionTunnelMan(Actor* boulder);
    void collisionProtester(int curX, int curY);
    void sonarUsed(int curX, int curY);
    void shootProtester(int curX, int curY);
    
    //Queue based maze solving algorithm
    void createExitPath(int maze[61][61], int ex, int ey);
    void createPathToTunnelMan(int maze[61][61], int tunnelX, int tunnelY);
    
    //Used to calculate euclidean distance between two given coordinates
    int radialDistance(int firstX, int firstY, int secondX, int secondY);
    
    //Check for Objects returns true if a specified object is present in the specified radius value given coordinates
    //We have two versions: one version checks for a SPECIFIED objectType, the other scans for any and all objects
    bool checkForObjects(Actor *p, int radius); //by default the radius will be 6.0
    bool checkForObjects(ObjectType type, Actor *p, int radius); //by default the radius will be 6.0
    bool checkForObjects(int curX, int curY, int radius); //by default the radius will be 6.0
    bool checkForEarth(int curX, int curY); //Basically sees if the 4x4 grid surrounding the coordinate is free of Earth
    bool checkForBoulder(int curX, int curY);
    bool pathObstructed(int curX, int curY);
    void bribeProtester(Actor *p);
    void shootProtester(Actor *p);
    
    //Accessor/mutator functions
    int getTicks();
    TunnelMan* getTunnelMan();
    void decBarrels();
    void decBoulders();
    void decProtesters();
    void insertObject(Actor* O);
    void setProtesterTick();
    
private:
    struct location
    {
        int x,y;
    };
    
    void setDisplayText();
    std::string formatDisplayText(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score);
    //We keep track of the TunnelMan and Earth object via private members for each (Part 1)
    TunnelMan* m_TunnelMan;
    Earth* m_Earth[64][64];
    //A single STL container to track all objects
    std::vector<Actor*> gameObjects;
    //An STL container to track Protesters
    std::vector<Protester*> protesterObjects;
    void removeDeadProtesters(std::vector<Protester*>& v);
    
    //Counting the number of ticks
    int m_ticks;
    int maxNumProtester; // int P = min(15, 2 + current_level_number * 1.5)
    int tickPerProtester; //int T = fmax(25, 200 - getLevel());
    int numBoulders; //int B = min(current_level_number / 2 + 2, 9)
    int numGoldNuggets; //int G = max(5-current_level_number / 2, 2)
    int numBarrels; //int L = min(2 + current_level_number, 21)
    int numProtesters;
    
    int exitMaze[61][61];
    int pathToTunnelMan[61][61];
};

#endif // STUDENTWORLD_H_
