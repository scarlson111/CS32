#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ACTOR IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Actor::Actor(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int depth)
:GraphObject(imageID,x,y,dir,size,depth)
{
    m_isDead = false;
    m_sw = sw;
}


bool Actor::isDead() const
{
    return m_isDead;
}

void Actor::setDead()
{
    m_isDead = true;
}

StudentWorld* Actor::world() const
{
    return m_sw;
}

double Actor::getVerticalSpeed() const
{
    return m_verticalSpeed;
}

void Actor::setVerticalSpeed(double speed)
{
    m_verticalSpeed = speed;
}

bool Actor::beSprayedIfAppropriate()
{
    return false;
}

bool Actor::isCollisionAvoidanceWorthy() const
{
    return false;
}

bool Actor::moveRelativeToGhostRacerVerticalSpeed(double dx)
{
    double newX = getX() + dx;
    double newY = getY() + m_verticalSpeed - m_sw->getGhostRacer()->getVerticalSpeed();
    moveTo(newX,newY);
    if(getX()<0 || getY() <0 || getX()>VIEW_WIDTH || getY()>VIEW_HEIGHT)
    {
        setDead();
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BORDER LINE IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BorderLine::BorderLine(StudentWorld* sw, double x, double y, bool isYellow)
:Actor(sw,(isYellow ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE),x,y,2.0,0,2)
{
    setVerticalSpeed(-4);
}

void BorderLine::doSomething()
{
    moveRelativeToGhostRacerVerticalSpeed(0);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//AGENT IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Agent::Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp)
:Actor(sw,imageID,x,y,size,dir,0)
{
    m_hp = hp;
}

bool Agent::isCollisionAvoidanceWorthy() const
{
    return true;
}

int Agent::getHP() const
{
    return m_hp;
}

void Agent::increaseHP(int hp)
{
    m_hp += hp;
}

bool Agent::takeDamageAndPossiblyDie(int hp)
{
    m_hp -= hp;
    if(m_hp <= 0)
    {
        setDead();
        if(soundWhenDie()!=0)
            world()->playSound(soundWhenDie());
        return true;
    }
    if(soundWhenHurt()!=0)
        world()->playSound(soundWhenHurt());
    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GHOSTRACER IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GhostRacer::GhostRacer(StudentWorld* sw, double x, double y)
:Agent(sw,IID_GHOST_RACER,x,y,4.0,90,100)
{
    m_numSprays = 10;
    setVerticalSpeed(0);
}

void GhostRacer::doSomething()
{
    if(isDead())
    {
        return;
    }
    
    const int LEFT_EDGE = ROAD_CENTER-ROAD_WIDTH/2;
    const int RIGHT_EDGE = ROAD_CENTER+ROAD_WIDTH/2;
    const double PI = 3.14159265359;
    
    if(getX()<=LEFT_EDGE)
    {
        if(getDirection()>90)
            if(takeDamageAndPossiblyDie(10))
                return;
        setDirection(82);
        world()->playSound(SOUND_VEHICLE_CRASH);
    }
    else if(getX()>=RIGHT_EDGE)
    {
        if(getDirection()<90)
            if(takeDamageAndPossiblyDie(10))
                return;
        setDirection(98);
        world()->playSound(SOUND_VEHICLE_CRASH);
    }
    else
    {
        int ch;
        if(world()->getKey(ch))
        {
            switch(ch)
            {
                case KEY_PRESS_SPACE:
                    if(m_numSprays>0)
                    {
                    double delta_x;
                    double delta_y;
                    if(getDirection() >= 90)
                    {
                        delta_x = -SPRITE_HEIGHT*cos((180-getDirection())*PI/180);
                        delta_y = -SPRITE_HEIGHT*sin((180-getDirection())*PI/180);
                    }
                    else
                    {
                        delta_x = SPRITE_HEIGHT*cos(getDirection()*PI/180);
                        delta_y = SPRITE_HEIGHT*sin(getDirection()*PI/180);
                    }
                    world()->addActor(new Spray(world(),getX()+delta_x,getY()+delta_y,getDirection()));
                    world()->playSound(SOUND_PLAYER_SPRAY);
                        m_numSprays--;
                    }
                    //implement spray functionality later
                    break;
                case KEY_PRESS_LEFT:
                    if(getDirection()<114)
                        setDirection(getDirection()+8);
                    break;
                case KEY_PRESS_RIGHT:
                    if(getDirection()>66)
                        setDirection(getDirection()-8);
                    break;
                case KEY_PRESS_UP:
                    if(getVerticalSpeed()<5)
                        setVerticalSpeed(getVerticalSpeed()+1);
                    break;
                case KEY_PRESS_DOWN:
                    if(getVerticalSpeed() > -1)
                        setVerticalSpeed(getVerticalSpeed()-1);
                    break;
            }
        }
    }
    
    //GHOST RACER MOVEMENT ALGORITHM

    double max_shift_per_tick = 4.0;
    double delta_x = cos(getDirection()*PI/180)*max_shift_per_tick;
    moveTo(getX()+delta_x,getY());
}


int GhostRacer::soundWhenDie() const
{
    return SOUND_PLAYER_DIE;
}

int GhostRacer::soundWhenHurt() const
{
    return 0;
}


int GhostRacer::getNumSprays() const
{
    return m_numSprays;
}

void GhostRacer::increaseSprays(int amt)
{
    m_numSprays += amt;
}

void GhostRacer::spin()
{
    int rand = randInt(5,20);
    if(randInt(1,2)==1)
        rand *= -1;
    int newDirection = getDirection() + rand;
    if(newDirection < 60)
        setDirection(60);
    else if(newDirection > 120)
        setDirection(120);
    else
        setDirection(newDirection);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PEDESTRIAN IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size)
:Agent(sw,imageID,x,y,size,0,2)
{
    m_horizontalSpeed = 0;
    m_movementPlanDistance = 0;
    setVerticalSpeed(-4);
}

int Pedestrian::soundWhenHurt() const
{
     return SOUND_PED_HURT;
}

int Pedestrian::soundWhenDie() const
{
    return SOUND_PED_DIE;
}


int Pedestrian::getHorizSpeed() const
{
    return m_horizontalSpeed;
}

void Pedestrian::setHorizSpeed(int s)
{
    m_horizontalSpeed = s;
}

void Pedestrian::moveAndPossiblyPickPlan()
{
    if(!moveRelativeToGhostRacerVerticalSpeed(m_horizontalSpeed))
        return;
    
    m_movementPlanDistance--;
    
    if(m_movementPlanDistance > 0)
    {
        return;
    }
    else
    {
        int randSpeed = 0;
        while(randSpeed == 0)
        {
            randSpeed = randInt(-3,3);
        }
        setHorizSpeed(randSpeed);
        m_movementPlanDistance = randInt(4,32);
        if(randSpeed<0)
            setDirection(180);
        else
            setDirection(0);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HUMAN PEDESTRIAN IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y)
:Pedestrian(sw,IID_HUMAN_PED,x,y,2.0)
{
    
}

void HumanPedestrian::doSomething()
{
    if(isDead())
        return;
    
    if(world()->getOverlappingGhostRacer(this) != nullptr)
    {
        world()->getGhostRacer()->setDead();
        return;
    }
    
    moveAndPossiblyPickPlan();
    
}

bool HumanPedestrian::beSprayedIfAppropriate()
{
    setHorizSpeed(-1*getHorizSpeed());
    if(getDirection()==0)
        setDirection(180);
    else
        setDirection(0);
    world()->playSound(soundWhenHurt());
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ZOMBIE PEDESTRIAN IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y)
:Pedestrian(sw,IID_ZOMBIE_PED,x,y,3.0)
{
    m_ticksUntilGrunt = 0;
}

void ZombiePedestrian::doSomething()
{
    if(isDead())
        return;
    
    
    if(world()->getOverlappingGhostRacer(this) != nullptr)
    {
        world()->getGhostRacer()->takeDamageAndPossiblyDie(5);
        takeDamageAndPossiblyDie(2);
        return;
    }
    
    if(abs(getX()-world()->getGhostRacer()->getX())<=30 && getY()>world()->getGhostRacer()->getY())
    {
        setDirection(270);
        
        if(getX()>world()->getGhostRacer()->getX())
        {
            //ZOMBIE PED IS TO THE RIGHT OF GHOST RACER
            setHorizSpeed(-1);
        }
        else if(getX()< world()->getGhostRacer()->getX())
        {
            //ZOMBIE PED IS TO THE LEFT OF GHOST RACER
            setHorizSpeed(1);
        }
        else
        {
            //ZOMBIE PED IS DIRECTLY IN FRONT OF GHOST RACER
            setHorizSpeed(0);
        }
        
        m_ticksUntilGrunt--;
        if(m_ticksUntilGrunt <= 0)
        {
            world()->playSound(SOUND_ZOMBIE_ATTACK);
            m_ticksUntilGrunt = 20;
        }
    }
    
    moveAndPossiblyPickPlan();
}

bool ZombiePedestrian::beSprayedIfAppropriate()
{
    takeDamageAndPossiblyDie(1);
    return true;
}

bool ZombiePedestrian::takeDamageAndPossiblyDie(int hp)
{
    if(Agent::takeDamageAndPossiblyDie(hp))
    {
        world()->increaseScore(150);
        if(world()->getOverlappingGhostRacer(this) == nullptr)
            if(randInt(1,5) == 1)
                world()->addActor(new HealingGoodie(world(),getX(),getY()));
        return true;
    }
    else
        return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ZOMBIE CAB IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ZombieCab::ZombieCab(StudentWorld* sw, double x, double y)
:Agent(sw,IID_ZOMBIE_CAB,x,y,4.0,90,3)
{
    if(y == SPRITE_HEIGHT/2)
        setVerticalSpeed(world()->getGhostRacer()->getVerticalSpeed() + randInt(2,4));
    else
        setVerticalSpeed(world()->getGhostRacer()->getVerticalSpeed() - randInt(2,4));
    m_hasDamagedRacer = false;
    m_horizontalSpeed = 0;
    m_movementPlanDistance = 0;
}

void ZombieCab::doSomething()
{
    if(isDead())
        return;
    
    if(world()->getOverlappingGhostRacer(this) != nullptr)
    {
        if(!m_hasDamagedRacer)
        {
            world()->playSound(SOUND_VEHICLE_CRASH);
            world()->getGhostRacer()->takeDamageAndPossiblyDie(20);
            if(getX() <= world()->getGhostRacer()->getX())
            {
                m_horizontalSpeed = -5;
                setDirection(120 + randInt(0,19));
            }
            else
            {
                m_horizontalSpeed = 5;
                setDirection(60 - randInt(0,19));
            }
            m_hasDamagedRacer = true;
        }
    }
    
    if(!moveRelativeToGhostRacerVerticalSpeed(m_horizontalSpeed))
        return;
    
    //adjust vertical speed based on placement of collision avoidance worthy actors
    if(getVerticalSpeed() > world()->getGhostRacer()->getVerticalSpeed())
    {
        Actor* ptr = world()->getNearestCollisionInLaneFront(getX(),getY());
        if(ptr != nullptr && ptr->getY()-96 < getY())
        {
        setVerticalSpeed(getVerticalSpeed()-0.5);
        return;
        }
    }
    if(getVerticalSpeed() <= world()->getGhostRacer()->getVerticalSpeed())
    {
        Actor* ptr = world()->getNearestCollisionInLaneBack(getX(),getY());
        if(ptr != nullptr && ptr->getY() + 96 > getY())
        {
            setVerticalSpeed(getVerticalSpeed()+0.5);
            return;
        }
    }
    
    m_movementPlanDistance--;
    if(m_movementPlanDistance > 0)
        return;
    //zombie cab picks new movement plan
    m_movementPlanDistance = randInt(4,32);
    setVerticalSpeed(getVerticalSpeed()+randInt(-2,2));
}

bool ZombieCab::beSprayedIfAppropriate()
{
    takeDamageAndPossiblyDie(1);
    return true;
}

bool ZombieCab::takeDamageAndPossiblyDie(int hp)
{
   if(Agent::takeDamageAndPossiblyDie(hp))
   {
       if(randInt(1,5)==1)
       {
           world()->addActor(new OilSlick(world(),getX(),getY()));
       }
       //only increase score if zombie cab was not damaged by collision with racer
       if(world()->getOverlappingGhostRacer(this) == nullptr)
           world()->increaseScore(200);
       return true;
   }
    else
        return false;
}

int ZombieCab::soundWhenDie() const
{
    return SOUND_VEHICLE_DIE;
}

int ZombieCab::soundWhenHurt() const
{
    return SOUND_VEHICLE_HURT;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SPRAY IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Spray::Spray(StudentWorld* sw, double x, double y, int dir)
:Actor(sw,IID_HOLY_WATER_PROJECTILE,x,y,1.0,dir,1)
{
    m_maxTravelDistance = 160;
}

void Spray::doSomething()
{
    if(isDead())
        return;
    
    if(world()->sprayFirstAppropriateActor(this))
    {
        setDead();
        return;
    }
    
    moveForward(SPRITE_HEIGHT);
    m_maxTravelDistance -= SPRITE_HEIGHT;
    
    if(getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
    {
        setDead();
        return;
    }
    
    if(m_maxTravelDistance <= 0)
        setDead();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GHOSTRACER ACTIVATED OBJECT IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir)
:Actor(sw,imageID,x,y,size,dir,2)
{
    setVerticalSpeed(-4);
}

bool GhostRacerActivatedObject::beSprayedIfAppropriate()
{
    if(isSprayable())
    {
        setDead();
        return true;
    }
    return false;
}

void GhostRacerActivatedObject::doSomething()
{
    if(!moveRelativeToGhostRacerVerticalSpeed(0))
        return;
    
    if(world()->getOverlappingGhostRacer(this)!= nullptr)
    {
        world()->playSound(getSound());
        doActivity(world()->getGhostRacer());
        world()->increaseScore(getScoreIncrease());
        
        if(selfDestructs())
            setDead();
    }
}

int GhostRacerActivatedObject::getSound() const
{
    return SOUND_GOT_GOODIE;
}

int GhostRacerActivatedObject::getScoreIncrease() const
{
    return 0;
}

bool GhostRacerActivatedObject::selfDestructs() const
{
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OIL SLICK IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


OilSlick::OilSlick(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw,IID_OIL_SLICK,x,y,randInt(2,5),0)
{
    
}

void OilSlick::doActivity(GhostRacer *gr)
{
    gr->spin();
}

int OilSlick::getSound() const
{
    return SOUND_OIL_SLICK;
}

bool OilSlick::selfDestructs() const
{
    return false;
}

bool OilSlick::isSprayable() const
{
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HEALING GOODIE IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw,IID_HEAL_GOODIE,x,y,1.0,0)
{
    
}

int HealingGoodie::getScoreIncrease() const
{
    return 250;
}

void HealingGoodie::doActivity(GhostRacer *gr)
{
    gr->increaseHP(10);
}

bool HealingGoodie::isSprayable() const
{
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HOLY WATER GOODIE IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 2.0, 90)
{
    
}

int HolyWaterGoodie::getScoreIncrease() const
{
    return 50;
}

void HolyWaterGoodie::doActivity(GhostRacer* gr)
{
    gr->increaseSprays(10);
}

bool HolyWaterGoodie::isSprayable() const
{
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SOUL GOODIE IMPLEMENTATIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw,IID_SOUL_GOODIE,x,y,4.0,0)
{

}

void SoulGoodie::doSomething()
{
    GhostRacerActivatedObject::doSomething();
    setDirection(getDirection()-10);
}

void SoulGoodie::doActivity(GhostRacer* gr)
{
    world()->recordSoulSaved();
}

int SoulGoodie::getScoreIncrease() const
{
    return 100;
}

int SoulGoodie::getSound() const
{
    return SOUND_GOT_SOUL;
}

bool SoulGoodie::isSprayable() const
{
    return false;
}
