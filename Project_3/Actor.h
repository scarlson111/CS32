#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int depth);

      // Action to perform for each tick.
    virtual void doSomething() = 0;

      // Is this actor dead?
    bool isDead() const;
    
      // Mark this actor as dead.
    void setDead();

      // Get this actor's world
    StudentWorld* world() const;

      // Get this actor's vertical speed.
    double getVerticalSpeed() const;

      // Set this actor's vertical speed.
    void setVerticalSpeed(double speed);

      // If this actor is affected by holy water projectiles, then inflict that
      // affect on it and return true; otherwise, return false.
    virtual bool beSprayedIfAppropriate();

      // Does this object affect zombie cab placement and speed?
    virtual bool isCollisionAvoidanceWorthy() const;

      // Adjust the x coordinate by dx to move to a position with a y coordinate
      // determined by this actor's vertical speed relative to GhostRacser's
      // vertical speed.  Return true if the new position is within the view;
      // otherwise, return false, with the actor dead.
     bool moveRelativeToGhostRacerVerticalSpeed(double dx);
    

private:
    double        m_verticalSpeed;
    bool          m_isDead;
    StudentWorld* m_sw;
};

class BorderLine : public Actor
{
public:
    BorderLine(StudentWorld* sw, double x, double y, bool isYellow);
    virtual void doSomething();
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp);
    virtual bool isCollisionAvoidanceWorthy() const;

      // Get hit points.
    int getHP() const;

      // Increase hit points by hp.
    void increaseHP(int hp);

      // Do what the spec says happens when hp units of damage is inflicted.
      // Return true if this agent dies as a result, otherwise false.
    virtual bool takeDamageAndPossiblyDie(int hp);

      // What sound should play when this agent is damaged but does not die?
    virtual int soundWhenHurt() const = 0;

      // What sound should play when this agent is damaged and dies?
    virtual int soundWhenDie() const = 0;
    
private:
    int m_hp;
};

class GhostRacer : public Agent
{
public:
    GhostRacer(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual int soundWhenDie() const;
    virtual int soundWhenHurt() const;

      // How many holy water projectiles does the object have?
    int getNumSprays() const;

      // Increase the number of holy water projectiles the object has.
    void increaseSprays(int amt);

      // Spin as a result of hitting an oil slick.
    void spin();
    
    
private:
    int m_numSprays;
};

class Pedestrian : public Agent
{
public:
    Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size);
    virtual int soundWhenHurt() const;
    virtual int soundWhenDie() const;

      // Get the pedestrian's horizontal speed
    int getHorizSpeed() const;

      // Set the pedestrian's horizontal speed
    void setHorizSpeed(int s);

      // Move the pedestrian.  If the pedestrian doesn't go off screen and
      // should pick a new movement plan, pick a new plan.
    void moveAndPossiblyPickPlan();
    
private:
    int m_horizontalSpeed;
    int m_movementPlanDistance;
};

class HumanPedestrian : public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
};

class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
    virtual bool takeDamageAndPossiblyDie(int hp);
private:
    int m_ticksUntilGrunt;
};

class ZombieCab : public Agent
{
public:
    ZombieCab(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
    virtual bool takeDamageAndPossiblyDie(int hp);
    virtual int soundWhenHurt() const;
    virtual int soundWhenDie() const;
private:
    bool m_hasDamagedRacer;
    int  m_horizontalSpeed;
    int  m_movementPlanDistance;
};

class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir);
    virtual void doSomething();
private:
    double m_maxTravelDistance;
};

class GhostRacerActivatedObject : public Actor
{
public:
    GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir);
    virtual bool beSprayedIfAppropriate();
    
    virtual void doSomething();

      // Do the object's special activity (increase health, spin Ghostracer, etc.)
    virtual void doActivity(GhostRacer* gr) = 0;

      // Return the object's increase to the score when activated.
    virtual int getScoreIncrease() const;

      // Return the sound to be played when the object is activated.
    virtual int getSound() const;

      // Return whether the object dies after activation.
    virtual bool selfDestructs() const;

      // Return whether the object is affected by a holy water projectile.
    virtual bool isSprayable() const = 0;
};

class OilSlick : public GhostRacerActivatedObject
{
    
public:
    OilSlick(StudentWorld* sw, double x, double y);
    virtual void doActivity(GhostRacer* gr);
    virtual int getSound() const;
    virtual bool selfDestructs() const;
    virtual bool isSprayable() const;
};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual bool isSprayable() const;
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual bool isSprayable() const;
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
    SoulGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual int getSound() const;
    virtual bool isSprayable() const;
};


























/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//StudentWorld
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, int verticalSpeed, int horizontalSpeed, bool collisionWorthy, StudentWorld* ptr);
    virtual void doSomething() = 0;
    
    //Accessors
    bool checkAlive() const;
    int  getVerticalSpeed() const;
    int  getHorizontalSpeed() const;
    bool getCollisionWorthy() const;
    
    
    //Mutators
    void setAlive(bool isAlive);
    void setVerticalSpeed(int speed);
    void setHorizontalSpeed(int speed);
    
    
    
    bool checkOverlap(const Actor& other) const;
    void move();
    StudentWorld* getPointerToWorld() const;
    
private:
    bool          m_isAlive;
    bool          m_collisionAvoidanceWorthy;
    int           m_verticalSpeed;
    int           m_horizontalSpeed;
    StudentWorld* m_pointerToWorld;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BorderLine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class BorderLine: public Actor
{
public:
    BorderLine(double x_coord,double y_coord, int imageID,StudentWorld* ptr);
    virtual void doSomething();
private:
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GhostRacer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GhostRacer: public Actor
{
public:
    GhostRacer(StudentWorld* ptr);
    virtual void doSomething();
    void doDamage(int damage);
private:
    int m_health;
    int m_numHolyWater;
};




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HumanPedestrian
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HumanPedestrian: public Actor
{
public:
    HumanPedestrian(double x_coord, double y_coord, StudentWorld* ptr);
    virtual void doSomething();
    
private:
    int m_health;
    int m_movementPlanDistance;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ZombiePedestrian
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ZombiePedestrian: public Actor
{
public:
    ZombiePedestrian(double x_coord, double y_coord, StudentWorld* ptr);
    virtual void doSomething();
private:
    int m_health;
    int m_movementPlanDistance;
    int m_ticksUntilGrunt;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ZombieCab
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ZombieCab: public Actor
{
public:
    ZombieCab(double x_coord, double y_coord, int verticalSpeed, StudentWorld* ptr);
    virtual void doSomething();
    
private:
    int m_health;
    int m_movementPlanDistance;
    bool m_hasDamagedGhostRacer;
};
 */

#endif // ACTOR_H_
