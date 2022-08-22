#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <queue>

enum BoulderState { stable, waiting, falling };
enum ObjectType { tunnelman, earth, boulder, barrel, goldnugget, sonarkit, waterpool, squirt, protester };

class StudentWorld;

class Actor: public GraphObject
{
    //GraphObject(int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0)
public:
    //Constructor for Actor (for objects which have size and depth of 1.0 and 0 and must start facing rightward)
    Actor(int imageID, int startX, int startY, StudentWorld* world, ObjectType type, int health = 0 );
    Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world, ObjectType type, int health = 0);
    
    //Destructor for Actor (virtual due to polymorphism)
    virtual ~Actor();
        
    //Status true means alive, else dead
    bool getStatus();
    
    //Sets status to dead (i.e. false)
    void setDead();
    
    //Pure virtual method used by almost every child
    virtual void doSomething() = 0;
    
    virtual ObjectType getObjectType();
    
    //Accessor function to access + use StudentWorld passed into Actor
    StudentWorld* getWorld();
    
    //Used by only the Protesters and TunnelMan
    int getHitPoints();
    void decHitPoints(int annoyance);
    void setHitPoints(int set);
    bool getLeave();
    
private:
    StudentWorld* myWorld;
    ObjectType m_type;
    bool m_status;
    int m_hitPoints;
};

class TunnelMan: public Actor
{
public:
    //Constructor: Actor(int imageID, int startX, int startY, StudentWorld* world)
    TunnelMan(StudentWorld* myWorld);
    //Destructor
    virtual ~TunnelMan();
    
    //Inherited function from Actor (otherwise TunnelMan becomes ABC)
    virtual void doSomething();
    
    //Accessor and Mutator functions
    int getSquirts();
    int getSonar();
    int getGoldNuggets();
    //The following is done when a goodie is picked up
    void incSquirts();
    void incSonar();
    void incGoldNuggets();

private:
    int m_squirts, m_sonarCharges, m_goldNuggets;
    
};

class Earth: public Actor
{
public:
    Earth(int startX, int startY, StudentWorld* myWorld);
    virtual ~Earth();
    
    //Added to prevent it from being an ABC
    virtual void doSomething();
};

class Boulder: public Actor
{
public:
    Boulder(int startX, int startY, StudentWorld* myWorld);
    virtual ~Boulder();
    virtual void doSomething();
    
private:
    BoulderState boulderState;
    int countdown;
};

class Squirt: public Actor
{
public:
    Squirt(int startX, int startY, Direction dir, StudentWorld* myWorld);
    virtual ~Squirt();
    virtual void doSomething();
    
private:
    int travelDistance;
};

class Goodie: public Actor
{
public:
    //Actor(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, StudentWorld* world);
    Goodie(int imageID, int startX, int startY, StudentWorld* myWorld, ObjectType type);
    virtual ~Goodie();
    int getTicksElapsed();
    void incTicks();
    bool getVisibility();
    void setVisibility(bool set);
    
private:
    int ticksElapsed;
    bool visibility;
};

class Barrel: public Goodie
{
public:
    Barrel(int startX, int startY, StudentWorld* myWorld);
    virtual ~Barrel();
    virtual void doSomething();
};

class GoldNugget: public Goodie
{
public:
    GoldNugget(int startX, int startY, bool dropped, StudentWorld* myWorld);
    virtual ~GoldNugget();
    virtual void doSomething();

    
private:
    bool m_dropped; //Will be true if dropped, false if spawned under oil field

};

class SonarKit: public Goodie
{
public:
    SonarKit(int startX, int startY, StudentWorld* myWorld);
    virtual ~SonarKit();
    virtual void doSomething();
};

class WaterPool: public Goodie
{
public:
    WaterPool(int startX, int startY, StudentWorld* myWorld);
    virtual ~WaterPool();
    virtual void doSomething();
};

class Protester: public Actor
{
public:
    //By default, health will be 5 and image will be Regular, we must specify when constructing hardcore to change these
    Protester(StudentWorld* myWorld, bool hardcore = false, int health = 5, int imageID = TID_PROTESTER);
    virtual ~Protester();
    virtual void doSomething();
    //This is pure virtual since it differs based on Regular or hardcore
    //virtual void pickUpGold() = 0;
    
    //Accessor and mutator functions common to all types of Protesters
    bool getLeave();
    void setLeave();
    
    //Functions used by doSomething
    void moveInDirection(Direction dir);
    bool findExitPath(int curX, int curY);
    //void walkToExit();
    bool inShoutingRange(int curX, int curY);
    bool verticalLOS(int curX, int curY, Direction& dir);
    bool horizontalLOS(int curX, int curY, Direction& dir);
    void perpendicularHelper();
    virtual void gotBribed();
    void setBribed();
    void setStunned();
    bool isStunned();

    //Things that can happen to Protester
    
private:
    int numSquaresToMoveInCurrentDirection;
    bool leaveState;
    int ticksToWaitBetweenMoves, restTicks, shoutTicks, perpTicks, stunnedTicks;
    bool isHardcore;
    bool bribed;
    bool stunned;
            
    //Only used by Protester class
    int getShoutTicks();
    void incShoutTicks();
    void zeroShoutTicks();
    
    int getPerpTicks();
    void incPerpTicks();
    void setPerpTicks(int set);
    
    int getNumInCurDir();
    void decNumInCurDir();
    void setNumInCurDir();
    void zeroNumInCurDir();
};

//LEAVE FOR THE END
class HardcoreProtester: public Protester
{
public:
    HardcoreProtester(StudentWorld* myWorld);
    virtual ~HardcoreProtester();
    virtual void gotBribed();
 
};

#endif // ACTOR_H_
