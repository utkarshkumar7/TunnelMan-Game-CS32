#include "StudentWorld.h"
#include <string>
#include <cmath>
#include <iostream> // defines the overloads of the << operator
#include <sstream> // defines the type std::ostringstream
#include <iomanip> // defines the manipulator setw
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::~StudentWorld(){}

//Returns true if any digging is done by the TunnelMan
bool StudentWorld::digEarth()
{
    bool dug = false;
    //Check if the TunnelMan’s 4x4 image currently overlaps any Earth objects
    int curX = m_TunnelMan->getX();
    int curY = m_TunnelMan->getY();
    
    for(int x = curX; x < curX + 4; x++ ){
        for(int y = curY; y < curY + 4; y++){
            if (m_Earth[x][y] != nullptr){ //Ensures there is an existing Earth object
                if(m_Earth[x][y]->getStatus()) //Ensure the earth object is existent
                {
                m_Earth[x][y]->setDead();
                m_Earth[x][y]->setVisible(false);
                dug = true;
                }
            }
        }
    }
    return dug;
}

//Calculates the radial distance between the centers of two Actor objects
//Actors can be passed in in any order (first/second)
int StudentWorld::radialDistance(int firstX, int firstY, int secondX, int secondY)
{
    //Change to the center coordinates
    firstX += 2;
    firstY += 2;
    secondX += 2;
    secondY += 2;
    
    double dx = firstX - secondX; //Difference in x coordinates
    double dy = firstY - secondY; //Difference in y coordinates
    
    //Calculating rSquared first
    double rSquared = dx*dx + dy*dy;
    
    return sqrt(rSquared); //Use cmath to calculate the square root
}

bool StudentWorld::checkBelowBoulder(int x, int y)
{
    int curY = y-1;
    int i = x;
    while( i < x + 4)
    {
        if (m_Earth[i][curY] != nullptr){ //Ensures there is an existing Earth object
            if(m_Earth[i][curY]->getStatus()) //If the earth is alive
            {
                return true; //i.e. there is Earth
            }
        }
        i++;
    }
    return false; //i.e. there is no Earth below
}

int StudentWorld::getTicks() { return m_ticks; }

TunnelMan* StudentWorld::getTunnelMan() { return m_TunnelMan;}

//Insert coordinates of Boulder to see if it will collide with TunnelMan
bool StudentWorld::collisionTunnelMan(Actor* boulder)
{
    int tunnelX = m_TunnelMan->getX();
    int tunnelY = m_TunnelMan->getY();
    int boulderX = boulder->getX();
    int boulderY = boulder->getY();
    double distance = radialDistance(tunnelX, tunnelY, boulderX, boulderY);
    if(distance <= 3 ) return true;
    else return false;
}

void StudentWorld::collisionProtester(int curX, int curY)
{
    int numObjects = (int)gameObjects.size();
    for(int i = 0; i < numObjects; i++)
    {
        if(gameObjects[i]->getObjectType() == protester)
        {
            //Note if the gameObject is not a protester, getLeave will always cause it to return
        if(gameObjects[i]->getLeave()) return;
        int objX = gameObjects[i]->getX();
        int objY = gameObjects[i]->getY();
        if(radialDistance(objX, objY, curX, curY) <= 3.0)
        {
            gameObjects[i] -> decHitPoints(100);
            increaseScore(500);
        }
        }
    }
}

/*
void StudentWorld::shootProtester(int curX, int curY)
{
    int numObjects = (int)gameObjects.size();
    for(int i = 0; i < numObjects; i++)
    {
        if(gameObjects[i]->getObjectType()== protester)
        {
        int objX = gameObjects[i]->getX();
        int objY = gameObjects[i]->getY();
        if(radialDistance(objX, objY, curX, curY) <= 3.0)
        {
            if(gameObjects[i] -> getLeave()) return; //If protester is leaving, we do not do anything
            gameObjects[i] -> decHitPoints(2);
            //If not fully annoyed after decremented hit points, play this sound
            if(gameObjects[i] -> getHitPoints() > 0)
            {
                playSound(SOUND_PROTESTER_ANNOYED);
                int N = fmax(50, 100 - getLevel()*10);
                protesterObjects[i]->setRestTicks(N);
            }
        }
        }
    }
}
 */


bool StudentWorld::checkForObjects(Actor *p, int radius = 6.0)
{
    int curX = p->getX();
    int curY = p->getY();
    
    int numObjects = (int)gameObjects.size();
    for(int i = 0; i < numObjects; i++)
    {
        //Looking for objects other than itself
        if(gameObjects[i] == p) continue;
        int objX = gameObjects[i]->getX();
        int objY = gameObjects[i]->getY();
        if(radialDistance(objX, objY, curX, curY) <= radius) return true;
    }
    return false;
}

bool StudentWorld::checkForObjects(ObjectType type, Actor *p, int radius = 6.0)
{
    int curX = p->getX();
    int curY = p->getY();
    
    int numObjects = (int)gameObjects.size();
    for(int i = 0; i < numObjects; i++)
    {
        if(gameObjects[i] == p) continue;
        if(gameObjects[i]->getObjectType() == type)
        {
        int objX = gameObjects[i]->getX();
        int objY = gameObjects[i]->getY();
        if(radialDistance(objX, objY, curX, curY) <= radius) return true;
        }
    }
    return false;
}

bool StudentWorld::checkForObjects(int curX, int curY, int radius = 6.0)
{
int numObjects = (int)gameObjects.size();
for(int i = 0; i < numObjects; i++)
{
    int objX = gameObjects[i]->getX();
    int objY = gameObjects[i]->getY();
    if(radialDistance(objX, objY, curX, curY) <= radius) return true;
}
return false;
}

bool StudentWorld::checkForBoulder(int curX, int curY)
{
    int numObjects = (int)gameObjects.size();
    for(int i = 0; i < numObjects; i++)
    {
        if(gameObjects[i]->getObjectType() == boulder)
        {
        int objX = gameObjects[i]->getX();
        int objY = gameObjects[i]->getY();
        if(radialDistance(objX, objY, curX, curY) <= 3.0) return true;
        }
    }
    return false;
}

//Checks if Earth exists at the specified coordinates
bool StudentWorld::checkForEarth(int curX, int curY)
{
    for(int x = curX; x < curX + 4; x++){
        for(int y = curY; y< curY + 4; y++)
        {
            if(m_Earth[x][y]!=nullptr){
                if(m_Earth[x][y]->getStatus()) return true;
            }
        }
    }
    return false;
}

//Used by squirt gun and protester, checks for both Earth AND Boulders as well as accounting for out of bounds
bool StudentWorld::pathObstructed(int curX, int curY)
{
    //If out of bounds, return true immediately
    if (curX > 63 || curX < 0 || curY > 63 || curY < 0) return true;
    
    bool earthPresent = checkForEarth(curX, curY);
    bool boulderPresent = checkForBoulder(curX, curY);
    
    return earthPresent || boulderPresent;
}

//Used by TunnelMan
void StudentWorld::sonarUsed(int curX, int curY)
{
    playSound(SOUND_SONAR);
    int numObjects = (int)gameObjects.size();
    for(int i = 0; i < numObjects; i++)
    {
        if(gameObjects[i]->getObjectType() == protester) continue; //Cannot interact with a protester
        int objX = gameObjects[i]->getX();
        int objY = gameObjects[i]->getY();
        if(radialDistance(objX, objY, curX, curY) <= 12.00) gameObjects[i]->setVisible(true);
    }
}

void StudentWorld::bribeProtester(Actor *p)
{
    int curX = p->getX();
    int curY = p->getY();
    
    int numObjects = (int)protesterObjects.size();
    for(int i = 0; i < numObjects; i++)
    {
        int objX = protesterObjects[i]->getX();
        int objY = protesterObjects[i]->getY();
        if(radialDistance(objX, objY, curX, curY) <= 3.0)
        {
            p->setDead();
            playSound(SOUND_PROTESTER_FOUND_GOLD);
            protesterObjects[i]->setBribed();
            return;
        }
    }
}

void StudentWorld::shootProtester(Actor *p)
{
    int curX = p->getX();
    int curY = p->getY();
    
    int numObjects = (int)protesterObjects.size();
    for(int i = 0; i < numObjects; i++)
    {
        int objX = protesterObjects[i]->getX();
        int objY = protesterObjects[i]->getY();
        if(radialDistance(objX, objY, curX, curY) <= 3.0)
        {
            //Don't do anything if protester is leaving
            if(protesterObjects[i]->getLeave()) continue;
            p->setDead();
            protesterObjects[i]->decHitPoints(2);
            if(protesterObjects[i]->getHitPoints() > 0)
            {
                playSound(SOUND_PROTESTER_ANNOYED);
                protesterObjects[i]->setStunned();
            }
            return;
        }
    }
}

//From project warmup
void StudentWorld::removeDeadGameObjects(vector<Actor*>& v)
{
    if (v.empty()) return;
    
    int i = 0;
    while(i < v.size())
    {
        if(v[i]->getStatus() == false)
        {
            //Delete the object pointed to by the pointer
            delete v[i];
            //Delete the pointer itself
            v.erase(v.begin() + i);
        }
        else i++;
    }
}

void StudentWorld::removeDeadProtesters(vector<Protester*>& v)
{
    if (v.empty()) return;
    
    int i = 0;
    while(i < v.size())
    {
        if(v[i]->getStatus() == false)
        {
            //Delete the object pointed to by the pointer
            delete v[i];
            //Delete the pointer itself
            v.erase(v.begin() + i);
        }
        else i++;
    }
}

void StudentWorld::addGoodies()
{
    int goodieProbability = getLevel()*25 + 300;
    int probabilityValue = rand()%goodieProbability + 1; //Generates a number between 1 and G with equal probability
    if(probabilityValue == 1)
    {
        int sonarChance = rand()%5 + 1;
        if(sonarChance == 1) //1 in 4 chance
        {
            //Add a new SonarKit
            SonarKit* G = new SonarKit(0,60,this);
            gameObjects.push_back(G);
        }
        else
        {
            int placeX, placeY;
            WaterPool *W = nullptr;
            
            //Keep randomizing until we find a valid value
            do {
                placeX = rand() % 61; // random number between 0-60
                placeY = rand() % 61; // random number between 0-60
                if(pathObstructed(placeX, placeY)) continue;
                W = new WaterPool(placeX,placeY,this);
            } while ( W == nullptr );

            gameObjects.push_back(W);
        }
    }
}

//If a certain number of ticks have passed since any protester has been added, we add a new protester
//This protester must be added every T ticks and at most there can be only P protesters
//The probabilityOfHardcore determines if the protester added will be hardcore or not
void StudentWorld::addProtesters()
{
    int probabilityOfHardcore = fmin(90, getLevel()*10 + 30);
    numProtesters++;
            
    int probability = 1 + rand() % 100;
    if (probability <= probabilityOfHardcore)
    {
        HardcoreProtester *h = new HardcoreProtester(this);
        gameObjects.push_back(h);
        protesterObjects.push_back(h);
    }
    else
    {
        Protester* p = new Protester(this);
        gameObjects.push_back(p);
        protesterObjects.push_back(p);
    }
}

void StudentWorld::decBarrels() {numBarrels--;}
void StudentWorld::decBoulders() {numBoulders--;}
void StudentWorld::decProtesters() {numProtesters--;}

void StudentWorld::insertObject(Actor* O)
{
    gameObjects.push_back(O);
}

void StudentWorld::initBoulder()
{
    // Initialize the Boulders
    numBoulders = fmin(getLevel()/2 + 2, 9);
    //Populate the vector with the required Boulders
    int i = 0;
    while(i < numBoulders)
    {
        int placeX = rand() % 61; //Gives value between 0-60 inclusive
        int placeY = rand() % 37 + 20; //Gives value between 20-56 inclusive
        
        //CANNOT be within a radius of 6 squared of any other distributed game object
        if(checkForObjects(placeX, placeY)) continue;
        //CANNOT spawn inside of the tunnel
        if (placeX >= 27 && placeX <= 33 && placeY >= 4 && placeY <= 59) continue;
        
        //Create a new Boulder
        Boulder* B = new Boulder(placeX, placeY, this);
        //Remove Earth from that area
        for(int x = placeX; x < placeX + 4; x++ ){
            for(int y = placeY; y < placeY + 4; y++){
                if (m_Earth[x][y] != nullptr){ //Ensures there is an existing Earth object
                    if(m_Earth[x][y]->getStatus()) //Ensure the earth object is existent
                    {
                    m_Earth[x][y]->setDead();
                    m_Earth[x][y]->setVisible(false);
                    }
                }
            }
        }
        
        //Insert the Boulder into the vector of all Actors
        gameObjects.push_back(B);
        i++;
    }
}

void StudentWorld::initBarrel()
{
    //Initialize the Oil Barrels
    numBarrels = fmin(2 + getLevel(), 21);
    
    //Populate the vector with the required Oil Barrels
    int k = 0;
    while(k < numBarrels)
    {
        int placeX = rand() % 61; //Gives value between 0-60 inclusive
        int placeY = rand() % 57; //Gives value between 0-56 inclusive
        
        //CANNOT be within a radius of 6 squared of any other distributed game object
        if(checkForObjects(placeX, placeY)) continue;
        //CANNOT spawn inside of the tunnel
        if (placeX >= 27 && placeX <= 33 && placeY >= 4 && placeY <= 59) continue;
        
        //Create a new Barrel
        Barrel* L = new Barrel(placeX, placeY, this);

        //Insert the Boulder into the vector of all Actors
        gameObjects.push_back(L);
        
        //Continue to iterate
        k++;
    }
}

void StudentWorld::initGold()
{
    //Initialize the Gold Nuggets
    numGoldNuggets = fmax(5 - getLevel()/2, 2);
    
    //Populate the vector with the required Oil Barrels
    int k = 0;
    while(k < numGoldNuggets)
    {
        int placeX = rand() % 61; //Gives value between 0-60 inclusive
        int placeY = rand() % 57; //Gives value between 0-56 inclusive
        
        //CANNOT be within a radius of 6 squared of any other distributed game object
        if(checkForObjects(placeX, placeY)) continue;
        //CANNOT spawn inside of the tunnel
        if (placeX >= 27 && placeX <= 33 && placeY >= 4 && placeY <= 59) continue;
        
        //Create a new Nugget (note: dropped is set to false)
        GoldNugget* G = new GoldNugget(placeX, placeY, false, this);

        //Insert the Boulder into the vector of all Actors
        gameObjects.push_back(G);
        
        //Continue to iterate
        k++;
    }
}

/*
//Use queue-based maze solving algorithm (AKA Breadth first search)
//NOTE: The idea for this code was given by Nima on a GroupMe chat where he suggested inputting the endpoints
//Into the queue first and then instead of marking each point as visited, it was marked according to
//How many steps away it is from the end point, you can then cause the Protester to move to the points with lowest values
void StudentWorld::createExitPath(int maze[61][61], int ex, int ey)
{
    //Fill in the exitMaze array
    for(int x = 0; x < 61; x++)
    {
        for(int y = 0; y < 61; y++)
        {
            if(pathObstructed(x, y) == false) maze[x][y] = -1; //Undiscovered location
            //Obstructed block
            else maze[x][y] = -2;
        }
    }
    
    location end;
    end.x = ex;
    end.y = ey;
    
    //Create a queue to perform BFS
    queue <location> mazeQueue;
    //Insert starting point into queue
    mazeQueue.push(end);
    //Mark point as destination
    exitMaze[end.x][end.y] = 0;
    
    int steps = 1; //This measures how many steps you are from the destination
    int count = 1; //A counter variable
    
    while(!mazeQueue.empty())
    {
        location cur = mazeQueue.front();
        mazeQueue.pop();
        count--;
        
    //If slot to the WEST is open and undiscovered
    if( maze[cur.x-1][cur.y] == -1 )
    {
        //Mark as discovered
        maze[cur.x - 1][cur.y] = steps;
        location west; west.x = cur.x - 1; west.y = cur.y;
        //Add to queue
        mazeQueue.push(west);
    }
    //If slot to the EAST is open and undiscovered
    if(maze[cur.x+1][cur.y] == -1)
    {
        //Mark as discovered
        maze[cur.x + 1][cur.y] = steps;
        location east; east.x = east.x + 1; east.y = cur.y;
        //Add to queue
        mazeQueue.push(east);
    }
    //If slot to the NORTH is open and undiscovered
    if(maze[cur.x][cur.y+1] == -1)
    {
        //Mark as discovered
        maze[cur.x][cur.y+1] = steps;
        location north; north.x = cur.x; north.y = cur.y+1;
        //Add to queue
        mazeQueue.push(north);
    }
    //If slot to the SOUTH is open and undiscovered
     if(maze[cur.x][cur.y-1] == -1)
    {
        //Mark as discovered
        maze[cur.x][cur.y-1] = steps;
        location south; south.x = cur.x; south.y = cur.y-1;
        //Add to queue
        mazeQueue.push(south);
    }
       if(count == 0)
       {
           steps++;
           count = (int)mazeQueue.size();
       }
    }
}
*/

int StudentWorld::init()
{
    // Initialize a TunnelMan object
    m_TunnelMan = new TunnelMan(this);
        
    // Populate the 2-D Earth array (note, there is a tunnel between
    for (int x = 0; x < 64; x++){
        for (int y = 0; y < 60; y++){
            //a single tunnel (occupying columns 30-33 of the oil field) and (occupying rows 4-59)
                if (!(x >= 30 && x <= 33 && y >= 4 && y <= 59))
                    m_Earth[x][y] = new Earth(x, y, this);
        }
    }
    
    initBoulder();
    initBarrel();
    initGold();
    
    //Initializing Protester variables
    m_ticks = 0;
    tickPerProtester = fmax(25, 200 - getLevel()); //So that it spawns at first when move() is called, after that it will be set to T
    numProtesters = 0;
    maxNumProtester = fmin(15, 2 + getLevel() * 1.5);
    
    addProtesters();

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{ //See spec. page 19
    m_ticks++;

    // Update the Game Status Line
    setDisplayText(); // update the score/lives/level text at screen top
    
    //This decides whether to add a Protester or not and then accordingly adds it to gameObjects
    if(m_ticks % tickPerProtester == 0 && numProtesters < maxNumProtester)
    {
        addProtesters();
    }

    addGoodies();
    m_TunnelMan->doSomething();
    
    for (int i = 0; i < gameObjects.size(); i++)
    {
        if(gameObjects[i]->getStatus())
        {
            gameObjects[i]->doSomething();
            if (m_TunnelMan->getHitPoints() <= 0) {
                m_TunnelMan->setDead();
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            
            if (numBarrels == 0) {
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    
    // Remove newly-dead actors after each tick
    removeDeadGameObjects(gameObjects);
    // Remove newly-dead protester after each tick
    //removeDeadProtesters(protesterObjects);
    
    // return the proper result
    if (m_TunnelMan->getHitPoints() <= 0) {
        m_TunnelMan->setDead(); //Kill the TunnelMan
        decLives(); //Decrement a life
        return GWSTATUS_PLAYER_DIED;
    }
    
    // If the player has collected all of the Barrels on the level, then
    // return the result that the player finished the level
    if (numBarrels == 0) {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    // the player hasn’t completed the current level and hasn’t died
    // let them continue playing the current level
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //Remove the TunnelMan object
    delete m_TunnelMan;
    
    //Delete all contents of the Earth array
    for (int x = 0; x < 64; x++)
        for (int y = 0; y < 60; y++)
            if (!(x >= 30 && x <= 33 && y >= 4 && y <= 59))
                delete m_Earth[x][y];
    
    for (vector<Actor*>::iterator itr = gameObjects.begin(); itr != gameObjects.end();)
        {
            delete *itr;
            gameObjects.erase(gameObjects.begin());
        }
        
        for (vector<Protester*>::iterator itr = protesterObjects.begin(); itr != protesterObjects.end(); itr++)
        {
            itr = protesterObjects.erase(itr);
            itr--;
        }
    
    /*
    //Delete all of the game's objects/actors
    for (int i = 0; i < gameObjects.size(); i++)
        {
            //Delete the object pointed to
            delete gameObjects[i];
            //Delete the pointer itself
            gameObjects.erase(gameObjects.begin() + i);
            //Decrement since erase function increments
            i--;
        }
    
    //Delete all of the game's protesters
    for (int i = 0; i < protesterObjects.size(); i++)
        {
            //Delete the pointer itself
            protesterObjects.erase(protesterObjects.begin() + i);
            //Decrement since erase function increments
            i--;
        }
     */
}

string StudentWorld::formatDisplayText(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score)
{
    //Read Stringstreams.pdf
     ostringstream text;
     text << "Lvl: " << setw(2) << level << "  ";
     text << "Lives: " << setw(1) << lives << "  ";
     text << "Hlth: " << setw(3) << health * 10 << "%  ";
     text << "Wtr: " << setw(2) << squirts << "  ";
     text << "Gld: " << setw(2) << gold << "  ";
     text << "Oil Left: " << setw(2) << barrelsLeft << "  ";
     text << "Sonar: " << setw(2) << sonar << "  ";
     text.fill('0');
     text << "Scr: " << setw(6) << score;
    
    //Save it into a string labelled s
    string s = text.str();
    //Return this string to be used by setDisplayText
    return s;
}

void StudentWorld::setDisplayText(){
    
    int level = getLevel();
    int lives = getLives();
    int health = m_TunnelMan->getHitPoints();
    int squirts = m_TunnelMan->getSquirts();
    int gold = m_TunnelMan->getGoldNuggets();
    int barrelsLeft = numBarrels; //TO DO
    int sonar = m_TunnelMan->getSonar();
    int score = getScore();
    // Next, create a string from your statistics, of the form: // Lvl: 52 Lives: 3 Hlth: 80% Wtr: 20 Gld: 3 Oil Left: 2 Sonar: 1 Scr: 321000
    string s = formatDisplayText(level, lives, health, squirts, gold, barrelsLeft, sonar, score);
    // Finally, update the display text at the top of the screen with your // newly created stats
    setGameStatText(s); // calls our provided GameWorld::setGameStatText
}
