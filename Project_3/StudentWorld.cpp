#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h" 
#include <iostream>
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
#include <string>
#include <vector>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    m_bonus = 5000;
    m_soulsToSave = 2*getLevel() + 5;
    m_racer = new GhostRacer(this,128,32);
    
    const int LEFT_EDGE = ROAD_CENTER-ROAD_WIDTH/2;
    const int RIGHT_EDGE = ROAD_CENTER+ROAD_WIDTH/2;
    
    for(int i = 0; i < VIEW_HEIGHT/SPRITE_HEIGHT; i++)
    {
        addActor(new BorderLine(this,LEFT_EDGE,i*SPRITE_HEIGHT,true));
        addActor(new BorderLine(this,RIGHT_EDGE,i*SPRITE_HEIGHT,true));
    }
    
    for(int i = 0; i < VIEW_HEIGHT/(4*SPRITE_HEIGHT);i++)
    {
        addActor(new BorderLine(this,LEFT_EDGE+ROAD_WIDTH/3,i*4*SPRITE_HEIGHT,false));
        addActor(new BorderLine(this,RIGHT_EDGE-ROAD_WIDTH/3,i*4*SPRITE_HEIGHT,false));
    }
    m_lastWhiteBorderAdded = m_actorList.back();

    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_racer->doSomething();
    
    for(list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        (*it)->doSomething();
        if(m_racer->isDead())
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    
    for(list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end();)
    {
        if((*it)->isDead())
        {
            delete *it;
            it = m_actorList.erase(it);
        }
        else
            it++;
    }

    
    
    //add new borderlines if necessary
    int delta_y = VIEW_HEIGHT-SPRITE_HEIGHT-m_lastWhiteBorderAdded->getY();
    const int LEFT_EDGE = ROAD_CENTER-ROAD_WIDTH/2;
    const int RIGHT_EDGE = ROAD_CENTER+ROAD_WIDTH/2;
    if(delta_y >= SPRITE_HEIGHT)
    {
        addActor(new BorderLine(this,LEFT_EDGE,VIEW_HEIGHT-SPRITE_HEIGHT,true));
        addActor(new BorderLine(this,RIGHT_EDGE,VIEW_HEIGHT-SPRITE_HEIGHT,true));
    }
    if(delta_y >= 4*SPRITE_HEIGHT)
    {
        addActor(new BorderLine(this,LEFT_EDGE+ROAD_WIDTH/3,VIEW_HEIGHT-SPRITE_HEIGHT,false));
        addActor(new BorderLine(this,RIGHT_EDGE-ROAD_WIDTH/3,VIEW_HEIGHT-SPRITE_HEIGHT,false));
        m_lastWhiteBorderAdded = m_actorList.back();
    }

    
    //add human ped if necessary
    if(randInt(0,max(200-getLevel()*10,30)-1)==0)
        addActor(new HumanPedestrian(this,randInt(0,VIEW_WIDTH),VIEW_HEIGHT));
    
    //add zombie ped if necessary
    if(randInt(0,max(100-getLevel()*10,20)-1)==0)
        addActor(new ZombiePedestrian(this,randInt(0,VIEW_WIDTH),VIEW_HEIGHT));

    //add zombie cab if necessary
    if(randInt(0,max(100-getLevel()*10,20)-1)==0)
    {
        vector<int> lanesToChoose = {0,1,2};
        bool laneFound = false;
        while(!lanesToChoose.empty() && !laneFound)
        {
            int rand = randInt(0,lanesToChoose.size()-1);
            switch(lanesToChoose[rand])
            {
                case 0:
                {
                    //LEFT LANE
                    Actor* ptr = getNearestCollisionInLaneFront(LEFT_EDGE + 1,0);
                    if(ptr == nullptr || ptr->getY() > VIEW_HEIGHT/3)
                    {
                        laneFound = true;
                        addActor(new ZombieCab(this,ROAD_CENTER-ROAD_WIDTH/3,SPRITE_HEIGHT/2));
                        break;
                    }
                    ptr = getNearestCollisionInLaneBack(LEFT_EDGE + 1,VIEW_HEIGHT);
                    if(ptr == nullptr || ptr->getY() < 2*VIEW_HEIGHT/3)
                    {
                        laneFound = true;
                        addActor(new ZombieCab(this,ROAD_CENTER-ROAD_WIDTH/3,VIEW_HEIGHT-SPRITE_HEIGHT/2));
                        break;
                    }
                    break;
                }
                case 1:
                {
                    //MIDDLE LANE
                    Actor* ptr = getNearestCollisionInLaneFront(LEFT_EDGE + ROAD_WIDTH/3 + 1,0);
                    if(ptr == nullptr || ptr->getY() > VIEW_HEIGHT/3)
                    {
                        laneFound = true;
                        addActor(new ZombieCab(this,ROAD_CENTER,SPRITE_HEIGHT/2));
                        break;
                    }
                    ptr = getNearestCollisionInLaneBack(LEFT_EDGE + ROAD_WIDTH/3 + 1,VIEW_HEIGHT);
                    if(ptr == nullptr || ptr->getY() < 2*VIEW_HEIGHT/3)
                    {
                        laneFound = true;
                        addActor(new ZombieCab(this,ROAD_CENTER,VIEW_HEIGHT-SPRITE_HEIGHT/2));
                        break;
                    }
                    break;
                }
                case 2:
                {
                    //RIGHT LANE
                    Actor* ptr = getNearestCollisionInLaneFront(LEFT_EDGE + 2*ROAD_WIDTH/3 + 1,0);
                    if(ptr == nullptr || ptr->getY() > VIEW_HEIGHT/3)
                    {
                        laneFound = true;
                        addActor(new ZombieCab(this,ROAD_CENTER+ROAD_WIDTH/3,SPRITE_HEIGHT/2));
                        break;
                    }
                    ptr = getNearestCollisionInLaneBack(LEFT_EDGE + 2*ROAD_WIDTH/3 + 1,VIEW_HEIGHT);
                    if(ptr == nullptr || ptr->getY() < 2*VIEW_HEIGHT/3)
                    {
                        laneFound = true;
                        addActor(new ZombieCab(this,ROAD_CENTER+ROAD_WIDTH/3,VIEW_HEIGHT-SPRITE_HEIGHT/2));
                        break;
                    }
                    break;
                }
            }
            lanesToChoose.erase(lanesToChoose.begin()+rand);
        }
    }
    
    //add ghost racer activated objects if necessary
    
    //add oil slick if necessary
    if(randInt(0,max(150-getLevel()*10,40)-1)==0)
        addActor(new OilSlick(this,randInt(LEFT_EDGE+1,RIGHT_EDGE-1),VIEW_HEIGHT));
    
    //add holy water refill goodie if necessary
    if(randInt(0,100 + 10*getLevel()-1)==0)
        addActor(new HolyWaterGoodie(this,randInt(LEFT_EDGE+1,RIGHT_EDGE-1),VIEW_HEIGHT));
    
    
    //add lost soul goodie if necessary
    if(randInt(0,99)==0)\
        addActor(new SoulGoodie(this,randInt(LEFT_EDGE+1,RIGHT_EDGE-1),VIEW_HEIGHT));
    
    //decrement bonus
    if(m_bonus>0)
        m_bonus--;
    
    //update game status
    ostringstream oss;
    oss << "Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << m_soulsToSave << "  Lives: " << getLives() << "  Health: " << m_racer->getHP() << "  Sprays: " << m_racer->getNumSprays() << "  Bonus: " << m_bonus;
    setGameStatText(oss.str());
    
    //check if game has been won yet
    if(m_soulsToSave <= 0)
    {
        increaseScore(m_bonus);
        return GWSTATUS_FINISHED_LEVEL;
    }
        
   
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_racer;
    Actor* ptr;
    while(!m_actorList.empty())
    {
        ptr = m_actorList.front();
        delete ptr;
        m_actorList.pop_front();
    }
}


GhostRacer* StudentWorld::getGhostRacer() const
{
    return m_racer;
}

bool StudentWorld::sprayFirstAppropriateActor(Actor *a)
{
    for(list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        if(overlaps(a,*it) && (*it)->beSprayedIfAppropriate())
            return true;
    }
    return false;
}

bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const
{
    double delta_x = abs(a1->getX() - a2->getX());
    double delta_y = abs(a1->getY() - a2->getY());
    double radius_sum = a1->getRadius() + a2->getRadius();
    if(delta_x < 0.25*radius_sum && delta_y < 0.6*radius_sum)
        return true;
    return false;
}

GhostRacer* StudentWorld::getOverlappingGhostRacer(Actor *a) const
{
    if(overlaps(m_racer,a))
        return m_racer;
    return nullptr;
}

  
Actor* StudentWorld::getNearestCollisionInLaneFront(double x_coord, double y_coord)
{
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    Actor* closestActor = nullptr;
    if(x_coord<LEFT_EDGE || x_coord > LEFT_EDGE + ROAD_WIDTH)
    {
        //NOT ON THE ROAD
        return nullptr;
    }
    else if(x_coord < LEFT_EDGE + ROAD_WIDTH/3)
    {
        //LEFT LANE
        for(list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end(); it++)
        {
            if((*it)->isCollisionAvoidanceWorthy() && LEFT_EDGE <= (*it)->getX() && (*it)->getX() < LEFT_EDGE + ROAD_WIDTH/3 && y_coord < (*it)->getY())
            {
               if(closestActor == nullptr)
                   closestActor = *it;
                else if((*it)->getY() < closestActor->getY())
                {
                    closestActor = *it;
                }
            }
        }
        //check ghost racer
        if(LEFT_EDGE <= m_racer->getX() && m_racer->getX() < LEFT_EDGE + ROAD_WIDTH/3 && y_coord < m_racer->getY())
        {
            if(closestActor == nullptr)
                closestActor = m_racer;
             else if(m_racer->getY() < closestActor->getY())
             {
                 closestActor = m_racer;
             }
        }
        return closestActor;
    }
    else if(x_coord < LEFT_EDGE + 2*ROAD_WIDTH/3)
    {
        //MIDDLE LANE
        for(list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end(); it++)
        {
            if((*it)->isCollisionAvoidanceWorthy() && LEFT_EDGE + ROAD_WIDTH/3 <= (*it)->getX() && (*it)->getX() < LEFT_EDGE + 2*ROAD_WIDTH/3 && y_coord < (*it)->getY())
            {
                if(closestActor == nullptr)
                    closestActor = *it;
                 else if((*it)->getY() < closestActor->getY())
                 {
                     closestActor = *it;
                 }
            }
        }
        //check ghost racer
        if(LEFT_EDGE + ROAD_WIDTH/3 <= m_racer->getX() && m_racer->getX() < LEFT_EDGE + 2*ROAD_WIDTH/3 && y_coord < m_racer->getY())
        {
            if(closestActor == nullptr)
                closestActor = m_racer;
             else if(m_racer->getY() < closestActor->getY())
             {
                 closestActor = m_racer;
             }
        }
        return closestActor;
    }
    else
    {
        //RIGHT LANE
        for(list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end(); it++)
        {
            if((*it)->isCollisionAvoidanceWorthy() && LEFT_EDGE + 2*ROAD_WIDTH/3 <= (*it)->getX() && (*it)->getX() < LEFT_EDGE + ROAD_WIDTH && y_coord < (*it)->getY())
            {
                if(closestActor == nullptr)
                    closestActor = *it;
                 else if((*it)->getY() < closestActor->getY())
                 {
                     closestActor = *it;
                 }
            }
        }
        //check ghost racer
        if(LEFT_EDGE + 2*ROAD_WIDTH/3 <= m_racer->getX() && m_racer->getX() < LEFT_EDGE + ROAD_WIDTH && y_coord < m_racer->getY())
        {
            if(closestActor == nullptr)
                closestActor = m_racer;
             else if(m_racer->getY() < closestActor->getY())
             {
                 closestActor = m_racer;
             }
        }
        return closestActor;
    }
}

Actor* StudentWorld::getNearestCollisionInLaneBack(double x_coord, double y_coord)
{
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    Actor* closestActor = nullptr;
    if(x_coord<LEFT_EDGE || x_coord > LEFT_EDGE + ROAD_WIDTH)
    {
        //NOT ON THE ROAD
        return nullptr;
    }
    else if(x_coord < LEFT_EDGE + ROAD_WIDTH/3)
    {
        //LEFT LANE
        for(list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end(); it++)
        {
            if((*it)->isCollisionAvoidanceWorthy() && LEFT_EDGE <= (*it)->getX() && (*it)->getX() < LEFT_EDGE + ROAD_WIDTH/3 && y_coord > (*it)->getY())
            {
                if(closestActor == nullptr)
                    closestActor = *it;
                 else if((*it)->getY() > closestActor->getY())
                 {
                     closestActor = *it;
                 }
            }
        }
        return closestActor;
    }
    else if(x_coord < LEFT_EDGE + 2*ROAD_WIDTH/3)
    {
        //MIDDLE LANE
        for(list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end(); it++)
        {
            if((*it)->isCollisionAvoidanceWorthy() && LEFT_EDGE + ROAD_WIDTH/3 <= (*it)->getX() && (*it)->getX() < LEFT_EDGE + 2*ROAD_WIDTH/3 && y_coord > (*it)->getY())
            {
                if(closestActor == nullptr)
                    closestActor = *it;
                 else if((*it)->getY() > closestActor->getY())
                 {
                     closestActor = *it;
                 }
            }
        }
        return closestActor;
    }
    else
    {
        //RIGHT LANE
        for(list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end(); it++)
        {
            if((*it)->isCollisionAvoidanceWorthy() && LEFT_EDGE + 2*ROAD_WIDTH/3 <= (*it)->getX() && (*it)->getX() < LEFT_EDGE + ROAD_WIDTH && y_coord > (*it)->getY())
            {
                if(closestActor == nullptr)
                    closestActor = *it;
                 else if((*it)->getY() > closestActor->getY())
                 {
                     closestActor = *it;
                 }
            }
        }
        return closestActor;
    }
}

void StudentWorld::addActor(Actor *a)
{
    m_actorList.push_back(a);
}

void StudentWorld::recordSoulSaved()
{
    m_soulsToSave--;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

