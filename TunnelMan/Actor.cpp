#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Implementing the ACTOR class

//Constructors
Actor::Actor(int imageID, int startX, int startY, StudentWorld* world, ObjectType type, int health): GraphObject(imageID, startX, startY)
{
    myWorld = world;
    m_type = type;
    m_hitPoints = health;
    m_status = true;
}

Actor::Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world, ObjectType type, int health):
GraphObject(imageID, startX, startY, dir, size, depth)
{
    myWorld = world;
    m_type = type;
    m_hitPoints = health;
    m_status = true;
}

//Destructor
Actor::~Actor() {}

bool Actor::getStatus(){ return m_status; }

void Actor::setDead(){ m_status = false; }

StudentWorld* Actor::getWorld(){ return myWorld; }

ObjectType Actor::getObjectType(){ return m_type; }

int Actor::getHitPoints(){ return m_hitPoints; }
void Actor::decHitPoints(int annoyance){ m_hitPoints -= annoyance; }
bool Actor::getLeave(){ return true; }

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the TUNNELMAN class

//Constructor
TunnelMan::TunnelMan(StudentWorld* myWorld): Actor(TID_PLAYER, 30, 60, myWorld, tunnelman, 10)
{
    //see spec page 27.
    m_squirts = 5;
    m_sonarCharges = 1;
    m_goldNuggets = 0;
    setVisible(true);
}

//Destructor does nothing since no dynamically allocated memory present
TunnelMan::~TunnelMan() {}

//TO DOs
void TunnelMan::doSomething()
{   //See spec pages 27-28
 
    //If TunnelMan is dead, return immediately
    if (!getStatus()) return;
    
    //If TunnelMan's overlaps any Earth object, remove/destroy Earth objects from the 4x4 area occupied by the TunnelMan
    //Make a digging sound by playing the SOUND_DIG
    if(getWorld()->digEarth()) getWorld()->playSound(SOUND_DIG);
    
    //Otherwise use doSomething to get input from user and move accordingly
    int ch;
    
    //TO EDIT: checkForBoulders specifically, not ALL OBJECTS
    if(getWorld() -> getKey(ch) == true)
    {
        switch(ch){
            case KEY_PRESS_LEFT:
                if (getDirection() == left)
                {
                    if(getX() == 0) return;
                    //Cannot occupy a square less than 3 radius away from center of boulder
                    else if(getWorld()->checkForBoulder(getX()-1, getY())) return;
                    else moveTo(getX()-1, getY());
                }
                else setDirection(left);
                break;
                
            case KEY_PRESS_RIGHT:
                if (getDirection() == right)
                {
                    if(getX() == 60) return;
                    //Cannot occupy a square less than 3 radius away from center of boulder
                    else if(getWorld()->checkForBoulder(getX()+1, getY())) return;
                    else moveTo(getX()+1, getY());
                }
                else setDirection(right);
                break;
                
            case KEY_PRESS_SPACE: //Shoots water
                
                if(getSquirts() > 0)
                {
                    int setX = getX();
                    int setY = getY();
                    
                    //Adjust the direction passed into the Squirt object
                    switch (getDirection()) {
                        case up:
                            setY += 4;
                            break;
                            
                        case down:
                            setY -= 4;
                            break;
                            
                        case left:
                            setX -= 4;
                            break;
                            
                        case right:
                            setX += 4;
                            break;
                        //The below should never be the case
                        case none:
                            break;
                    }
                    
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    
                    if(getWorld()->pathObstructed(setX, setY) == false)
                    {
                        Squirt* S = new Squirt(setX, setY, getDirection(), getWorld());
                        getWorld()->insertObject(S);
                    }
                    m_squirts--;
                }
                break;
                
            case KEY_PRESS_UP:
                if (getDirection() == up)
                {
                    if(getY() == 60) return;
                    //Cannot occupy a square less than 3 radius away from center of boulder
                    else if(getWorld()->checkForBoulder(getX(), getY()+1)) return;
                    else moveTo(getX(), getY()+1);
                }
                else setDirection(up);
                break;
                
            case KEY_PRESS_DOWN:
                if (getDirection() == down)
                {
                    if(getY() == 0) return;
                    //Cannot occupy a square less than 3 radius away from center of boulder
                    else if(getWorld()->checkForBoulder(getX(), getY()-1)) return;
                    else moveTo(getX(), getY()-1);
                }
                else setDirection(down);
                break;
                
            case KEY_PRESS_ESCAPE:
                setDead(); //Inherited from Actor class
                break;
                
            case KEY_PRESS_TAB: //Implemented dropping feature for goldNugget
            {
                if(m_goldNuggets != 0){
                GoldNugget* g = new GoldNugget(getX(), getY(), true, getWorld());
                getWorld()->insertObject(g);
                m_goldNuggets--;
                }
            }
                break;
                
            case 'Z':
            case 'z':
                if (m_sonarCharges != 0)
                {
                    getWorld()->sonarUsed(getX(), getY());
                    m_sonarCharges--;
                }
                break;
        }
    }
}

//Adding accessor + mutator one line definitions
int TunnelMan::getSquirts() {return m_squirts;}
int TunnelMan::getSonar() {return m_sonarCharges;}
int TunnelMan::getGoldNuggets() {return m_goldNuggets;}
void TunnelMan::incSquirts() {m_squirts += 5;} //Note: each time a waterpool is picked up, 5 squirts are added
void TunnelMan::incSonar() {m_sonarCharges++;}
void TunnelMan::incGoldNuggets() {m_goldNuggets++;}

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the EARTH class

Earth::Earth(int startX, int startY, StudentWorld* myWorld): Actor(TID_EARTH, startX, startY, right, 0.25, 3, myWorld, earth)
{
    setVisible(true);
}

Earth::~Earth(){};

void Earth::doSomething(){};

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the BOULDER class

Boulder::Boulder(int startX, int startY, StudentWorld* myWorld): Actor(TID_BOULDER, startX, startY, down, 1, 1, myWorld, boulder)
{
    boulderState = stable;
    setVisible(true);
}

Boulder::~Boulder(){}

void Boulder::doSomething()
{
    if(!getStatus()) return;
    
    BoulderState state = boulderState;
    
    switch(state)
    {
        case stable: {
            //Check the 4 squares below the Boulder
            if(!getWorld()->checkBelowBoulder(getX(),getY())) //If no Earth below boulder
            {
                boulderState = waiting;
                countdown = getWorld()->getTicks();
            }
            break;}
            
        case waiting:{
            int fall = getWorld()->getTicks() - countdown;
            if(fall == 30){
                boulderState = falling;
                getWorld()->playSound(SOUND_FALLING_ROCK);
            }
            break;}
            
        case falling:{
            if(!getWorld()->checkBelowBoulder(getX(),getY())) //If no Earth below boulder
            {
                //Once we have reached the bottom, set boulder to dead
                if(getY() == 0) { setDead(); getWorld()->decBoulders(); return;}
                //If there is a TunnelMan, kill it
                if(getWorld()->collisionTunnelMan(this)) {getWorld()->getTunnelMan()->decHitPoints(100); return;}
                //If we encounter the top of a Boulder below current position when falling, set boulder to dead
                if(getWorld()->checkForObjects(boulder, this, 3.0)) {setDead(); getWorld()->decBoulders(); return;}
                //If we hit a protester, decrement its points BUT continue to fall (Boulder does not die upon hitting a protester)
                getWorld()->collisionProtester(getX(), getY());
                //Otherwise continue falling
                moveTo(getX(), getY() - 1);
            }
            else
            {
                //If we hit Earth
                setDead();
                getWorld()->decBoulders();
                return;
            }
            break;}
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the SQUIRT class

Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld* myWorld):Actor(TID_WATER_SPURT, startX, startY, dir, 1, 1, myWorld, squirt)
{
    travelDistance = 4;
    setVisible(true);
}

Squirt::~Squirt(){}

void Squirt::doSomething()
{
    if(!getStatus()) return;
    
    //Checks if protester is in a 3.0 radius, if it is then the Squirt sets itself to dead
    //if(getWorld()->checkForObjects(protester, getX(),getY(), 3.0)) { setDead(); return; }
    
    if(travelDistance == 0) { setDead(); return; }
    if(getWorld()->pathObstructed(getX(), getY())) { setDead(); return; }
    getWorld()->shootProtester(this);
    
    travelDistance--;
    
    switch (getDirection()) {
            case up:
                if (getWorld()->pathObstructed(getX(), getY()+1) == false)
                    moveTo(getX(), getY()+1);
                else
                    setDead();
                break;
                
            case down:
                if (getWorld()->pathObstructed(getX(), getY()-1) == false)
                    moveTo(getX(), getY()-1);
                else
                    setDead();
                break;
                
            case right:
                if (getWorld()->pathObstructed(getX()+1, getY()) == false)
                    moveTo(getX()+1, getY());
                else
                    setDead();
                break;
                
            case left:
                if (getWorld()->pathObstructed(getX()-1, getY()) == false)
                    moveTo(getX()-1, getY());
                else
                    setDead();
                break;
                
           default:
               break;
        }
    
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the GOODIE class

Goodie::Goodie(int imageID, int startX, int startY, StudentWorld* myWorld, ObjectType type): Actor(imageID, startX, startY, right, 1, 2, myWorld, type)
{
    ticksElapsed = 0;
}

Goodie::~Goodie() {}

int Goodie::getTicksElapsed(){ return ticksElapsed; }

void Goodie::incTicks(){ ++ticksElapsed; }
void Goodie::setVisibility(bool set){ visibility = set; }
bool Goodie::getVisibility(){ return visibility; }


//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the BARREL class

Barrel::Barrel(int startX, int startY, StudentWorld* myWorld): Goodie(TID_BARREL, startX, startY, myWorld, barrel)
{
    setVisible(false); //starts out as invisible
    setVisibility(false);
}

Barrel::~Barrel() {}

void Barrel::doSomething()
{
    int tunnelX = getWorld()->getTunnelMan()->getX();
    int tunnelY = getWorld()->getTunnelMan()->getY();
    int proximity = getWorld()->radialDistance(tunnelX, tunnelY, getX(), getY());
    
    if(getStatus() == false) return;
   
    else if( !getVisibility() && proximity <= 4.0) { setVisible(true); setVisibility(true); return;}
    else if( proximity <= 3.0)
    {
        setDead();
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(POINTS_FOUND_OIL);
        getWorld()->decBarrels();
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the GOLDNUGGET class

GoldNugget::GoldNugget(int startX, int startY, bool dropped, StudentWorld* myWorld): Goodie(TID_GOLD, startX, startY, myWorld, goldnugget)
{
    m_dropped = dropped;
    setVisible(dropped); // if it has yet to be picked up, it is invisible; else, visible
    setVisibility(dropped);
}

GoldNugget::~GoldNugget() {}

void GoldNugget::doSomething()
{
    int tunnelX = getWorld()->getTunnelMan()->getX();
    int tunnelY = getWorld()->getTunnelMan()->getY();
    int proximity = getWorld()->radialDistance(tunnelX, tunnelY, getX(), getY());
    
    if(!getStatus()) return;
    
    if(!getVisibility() && proximity <= 4.0)
    {
        setVisible(true);
        setVisibility(true);
        return;
    }
    //Pickup-able by the TunnelMan
    else if( !m_dropped && proximity <= 3.0)
    {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(POINTS_FOUND_GOLD);
        getWorld()->getTunnelMan()->incGoldNuggets();
    }
    
    else if (m_dropped == true)
    {
        getWorld()->bribeProtester(this);

        if(getTicksElapsed() == 100) setDead();
        incTicks();
    }
        
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the SONARKIT class

SonarKit::SonarKit(int startX, int startY, StudentWorld* myWorld): Goodie(TID_SONAR, startX, startY, myWorld, sonarkit)
{
    setVisible(true);
    setVisibility(true);
}

SonarKit::~SonarKit(){}

void SonarKit::doSomething()
{
    int lifetime = fmax(100, 300 - 10*(getWorld()->getLevel()));
    int tunnelX = getWorld()->getTunnelMan()->getX();
    int tunnelY = getWorld()->getTunnelMan()->getY();
    int proximity = getWorld()->radialDistance(tunnelX, tunnelY, getX(), getY());
    
    if(!getStatus()) return;
    else if(proximity <= 3.0)
    {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getTunnelMan()->incSonar();
        getWorld()->increaseScore(POINTS_FOUND_SONARKIT);
    }
    
    if(getTicksElapsed() == lifetime) setDead();
    
    incTicks();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the WATERPOOL class

WaterPool::WaterPool(int startX, int startY, StudentWorld* myWorld): Goodie(TID_WATER_POOL, startX, startY, myWorld, waterpool)
{
    setVisible(true);
    setVisibility(true);
}

WaterPool::~WaterPool(){}

void WaterPool::doSomething()
{
    int lifetime = fmax(100, 300 - 10*(getWorld()->getLevel()));
    int tunnelX = getWorld()->getTunnelMan()->getX();
    int tunnelY = getWorld()->getTunnelMan()->getY();
    int proximity = getWorld()->radialDistance(tunnelX, tunnelY, getX(), getY());
    
    if(!getStatus()) return;
    else if(proximity <= 3.0)
    {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getTunnelMan()->incSquirts();
        getWorld()->increaseScore(POINTS_FOUND_WATERPOOL);
    }
    
    if(getTicksElapsed() == lifetime) setDead();
    
    incTicks();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the PROTESTOR class

Protester::Protester(StudentWorld* myWorld, bool hardcore, int health, int imageID) : Actor(imageID, 60, 60, left, 1, 0, myWorld, protester, health)
{
    numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
    ticksToWaitBetweenMoves = fmax(0, 3 - getWorld()->getLevel()/4);
    restTicks = 0;
    shoutTicks = 0;
    perpTicks = 0;
    stunnedTicks = fmax(50, 100 - getWorld()->getLevel() * 10);
    isHardcore = hardcore;
    bribed = false;
    stunned = false;
    
    setVisible(true);
}

Protester::~Protester() {}

//Settors and gettors
int Protester::getShoutTicks(){ return shoutTicks; }
void Protester::incShoutTicks(){ ++shoutTicks; }
void Protester::zeroShoutTicks(){ shoutTicks = 0; };

int Protester::getPerpTicks(){ return perpTicks; }
void Protester::incPerpTicks(){ ++perpTicks; }
void Protester::setPerpTicks(int set) { perpTicks = set; }

int Protester::getNumInCurDir(){ return numSquaresToMoveInCurrentDirection; }
void Protester::decNumInCurDir(){ --numSquaresToMoveInCurrentDirection; }
void Protester::setNumInCurDir(){ numSquaresToMoveInCurrentDirection = rand() % 53 + 8; }
void Protester::zeroNumInCurDir(){ numSquaresToMoveInCurrentDirection = 0; }

bool Protester::getLeave(){ return leaveState; }
void Protester::setLeave(){ leaveState = true; }

void Protester::setBribed(){ bribed = true; }
void Protester::setStunned(){ stunned = true; }
bool Protester::isStunned(){ return stunned; }

void Protester::gotBribed()
{
    getWorld()->increaseScore(25); //25 for regular, 50 for hardcore
    setLeave();
    setDead();
}

//void Protester::getShot(){ return; }

//Note: this does not check if the direction being moved to is valid
//since the functions that call this helper will be checking that themselves
void Protester::moveInDirection(Direction dir)
{
    if (dir == up) // move up
    {
        setDirection(up);
        moveTo(getX(), getY()+1);
        return;
    }
    else if (dir == down) // move down
    {
        setDirection(down);
        moveTo(getX(), getY()-1);
        return;
    }
    else if (dir == left) // move left
    {
        setDirection(left);
        moveTo(getX()-1, getY());
        return;
    }
    else if (dir == right) // move right
    {
        setDirection(right);
        moveTo(getX()+1, getY());
        return;
    }
}

bool Protester::horizontalLOS(int curX, int curY, Direction& dir)
{
    //TunnelMan's coordinates
    int tunnelY = getWorld()->getTunnelMan()->getY();
    int tunnelX = getWorld()->getTunnelMan()->getX();
    
    //Must be more than 4 units away
    int proximity = getWorld()->radialDistance(tunnelX, tunnelY, curX, curY);
    
    if(proximity <= 4) return false;
    if(abs(tunnelY - curY) != 0) return false; //Must be at the same vertical level
    
    //Check if path rightward is clear (given tunnelMan is to the right of Protester)
    if(tunnelX > curX)
    {
        dir = right; //TunnelMan is to the right
        for (int x = curX; x < tunnelX; x++)
                {
                    if (getWorld()->pathObstructed(x, curY)) return false;
                }
    }
    else if(tunnelX < curX)
    {
        dir = left; //TunnelMan is to the left
        for (int x = tunnelX; x < curX; x++)
                {
                    if (getWorld()->pathObstructed(x, curY)) return false;
                }
    }
    return true;
}

bool Protester::verticalLOS(int curX, int curY, Direction& dir)
{
    //TunnelMan's coordinates
    int tunnelY = getWorld()->getTunnelMan()->getY();
    int tunnelX = getWorld()->getTunnelMan()->getX();
    
    //Must be more than 4 units away
    int proximity = getWorld()->radialDistance(tunnelX, tunnelY, curX, curY);
    if(proximity <= 4) return false;
    if(abs(tunnelX - curX) != 0) return false; //Must be at the same horizontal level
    
    //Check if path up is clear (given tunnelMan is to the up of Protester)
    if(tunnelY > curY)
    {
        dir = up;
        for (int y = curY; y < tunnelY; y++)
                {
                    if (getWorld()->pathObstructed(curX, y)) return false;
                }
    }
    else if(tunnelY < curY)
    {
        dir = down;
        for (int y = tunnelY; y < curY; y++)
                {
                    if (getWorld()->pathObstructed(curX, y)) return false;
                }
    }
    return true;
}

void Protester::perpendicularHelper()
{
    bool upAvailable = !getWorld()->pathObstructed(getX(), getY()+1);
    bool downAvailable = !getWorld()->pathObstructed(getX(), getY()-1);
    bool rightAvailable = !getWorld()->pathObstructed(getX()+1, getY());
    bool leftAvailable = !getWorld()->pathObstructed(getX()-1, getY());
    //If facing left or right
    if(getDirection() == left || getDirection() == right )
    {
        if(upAvailable || downAvailable)
        {
            if(upAvailable && downAvailable)
            {
                //Pick a random direction
                int choose = rand()%2;
                if(choose == 0)
                {
                    setDirection(up);
                    setNumInCurDir();
                    setPerpTicks(0);
                }
                else if(choose == 1)
                {
                    setDirection(down);
                    setNumInCurDir();
                    setPerpTicks(0);
                }
            }
            else if(upAvailable)
            {
                setDirection(up);
                setNumInCurDir();
                setPerpTicks(0);
            }
            else
            {
                setDirection(down);
                setNumInCurDir();
                setPerpTicks(0);
            }
        }
    }
    else if(getDirection() == up || getDirection() == down )
    {
        if(rightAvailable || leftAvailable)
        {
            if(rightAvailable && leftAvailable)
            {
                //Pick a random direction
                int choose = rand()%2;
                if(choose == 0)
                {
                    setDirection(left);
                    setNumInCurDir();
                    setPerpTicks(0);
                }
                else if(choose == 1)
                {
                    setDirection(right);
                    setNumInCurDir();
                    setPerpTicks(0);
                }
            }
            else if(rightAvailable)
            {
                setDirection(right);
                setNumInCurDir();
                setPerpTicks(0);
            }
            else
            {
                setDirection(left);
                setNumInCurDir();
                setPerpTicks(0);
            }
        }
    }
}

//The below function checks whether the TunnelMan is within the shouting range of the Protester
bool Protester::inShoutingRange(int curX, int curY)
{
    //TunnelMan's coordinates
    int tunnelX = getWorld()->getTunnelMan()->getX();
    int tunnelY = getWorld()->getTunnelMan()->getY();
    bool checkRight = (tunnelX - curX == 4) && getDirection() == right;
    bool checkLeft = (tunnelX - curX == -4) && getDirection() == left;
    bool checkUp = (tunnelY - curY == 4) && getDirection() == up;
    bool checkDown = (tunnelY - curY == -4) && getDirection() == down;
    
    //Make sure the distance between the x and y values individually are within 4
    if(abs(tunnelX - curX) <= 4 && abs(tunnelY - curY) <= 4){
        
        return checkRight || checkLeft || checkUp || checkDown;
    }
    else
        return false;
}

void Protester::doSomething()
{
    //1. Check if protester is alive
    if(!getStatus()) return;
    
    //2. If protester is in a "rest state" during current tick, do nothing and just update resting tick count
    if(bribed)
    {
        gotBribed();
        bribed = false; //Set bribed to false after it has been bribed
    }
    
    if(stunned)
    {
        if(restTicks == stunnedTicks)
        {
            stunned = false; //Gets out of stunned state
            return;
        }
        else if(restTicks < stunnedTicks)
        {
            ++restTicks;
            return;
        }
    }
    else
    {
        if(restTicks < ticksToWaitBetweenMoves)
        {
            ++restTicks;
            return;
        }
    }
    
    //Reset restTick count and continue with doSomething()
    restTicks = 0;
    
    if(getHitPoints() <= 0 && !getLeave())
    {
        //Play sound
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        //Adjust point totals
        if (isHardcore) getWorld()->increaseScore(250);
        else getWorld()->increaseScore(100);
        //Make it leave
        setLeave();
        setDead();
        //Return immediately
        return;
    }
    
    /*
    //3. Otherwise if in leave the OilField state
    if(getLeave())
    {
        //If protester at exit point, kill it
        if(getX() == 60 && getY() == 60)
        {
            setDead();
            getWorld()->decProtesters();
        }
        else //MOVE ONE STEP CLOSER TO EXIT (TO DO)
        {
            //Move one step based on exit algorithm
            //Move RIGHT if there is a path to exit (etc. for all dirs)
            //Must return immediately after performing a move
            return;
        }
    }*/
    
    //If it is alive, i.e. above code did not execute, we increment our tick values
    incShoutTicks();
    incPerpTicks();
    
    //4. Regular protester checks to see if within shouting distance
    if(inShoutingRange(getX(), getY()))
    {
        //AND if it has not shouted in its last 15 non-resting ticks
        if(getShoutTicks() > 15)
        {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->getTunnelMan()->decHitPoints(2);
            zeroShoutTicks();
            return;
        }
    }
   
    //5. Check if the TunnelMan is in a horizontal or vertical line of sight
    Direction dir;
    if (verticalLOS(getX(), getY(), dir))
    {
        moveInDirection(dir);
        zeroNumInCurDir();
        return;
    }
    else if( horizontalLOS(getX(), getY(), dir))
    {
        moveInDirection(dir);
        zeroNumInCurDir();
        return;
    }
    
    //6. The Protester cannot directly see the TunnelMan so take a step in a random direction with no blockage
    decNumInCurDir();
    
    //If the protester has finished walking the steps in current direction, pick a new direction
    //Change direction function
    if(getNumInCurDir() <= 0)
    {
        //Use randomizer to find a new, valid direction
        bool pathClear = false;
        while (pathClear == false)
        {
            //Randomize to a new direction
            int dir = rand() % 4 + 1;
            //enum Direction { none, up, down, left, right };
            if (dir == 1 && getWorld()->pathObstructed(getX(), getY()+1) == false)
            {
                pathClear = true;
                setDirection(up);
                setNumInCurDir();
            }
            else if (dir == 2 && getWorld()->pathObstructed(getX(), getY()-1) == false)
            {
                pathClear = true;
                setDirection(down);
                setNumInCurDir();
            }
            else if (dir == 3 && getWorld()->pathObstructed(getX()-1, getY()) == false)
            {
                pathClear = true;
                setDirection(left);
                setNumInCurDir();
            }
            else if (dir == 4 && getWorld()->pathObstructed(getX()+1, getY()) == false)
            {
                pathClear = true;
                setDirection(right);
                setNumInCurDir();
            }
        }
    }
    
    //7. Otherwise, check perpendicular paths
    
    else if (getPerpTicks() > 200)
    {
        perpendicularHelper();
    }
    
    //8. Take one step in facing direction
    switch (getDirection()) {
        case up:
            if (getWorld()->pathObstructed(getX(), getY()+1) == false)
            {
                moveTo(getX(), getY()+1);
            }
            else
               zeroNumInCurDir();
            break;
            
        case down:
            if (getWorld()->pathObstructed(getX(), getY()-1) == false)
            {
                moveTo(getX(), getY()-1);
            }
            else
               zeroNumInCurDir();
            break;
            
        case left:
            if (getWorld()->pathObstructed(getX()-1, getY()) == false)
            {
                moveTo(getX()-1, getY());
            }
            else
               zeroNumInCurDir();
            break;
            
        case right:
            if (getWorld()->pathObstructed(getX()+1, getY()) == false)
            {
                moveTo(getX()+1, getY());
            }
            else
               zeroNumInCurDir();
            break;
        case none:
            zeroNumInCurDir();
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

// Implementing the HARDCORE PROTESTOR class

HardcoreProtester::HardcoreProtester(StudentWorld* myWorld): Protester(myWorld, true, 20, TID_HARD_CORE_PROTESTER) {}

HardcoreProtester::~HardcoreProtester(){}

void HardcoreProtester::gotBribed()
{
    getWorld()->increaseScore(50); //25 for regular, 50 for hardcore
    setStunned();
}
