#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class GhostRacer;
class Actor;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    virtual ~StudentWorld();
    Actor* getNearestCollisionInLaneFront(double x_coord, double y_coord);
    Actor* getNearestCollisionInLaneBack(double x_coord, double y_coord);
    GhostRacer* getGhostRacer() const;
  
    // Add an actor to the world.
  void addActor(Actor* a);

    // Record that a soul was saved.
  void recordSoulSaved();

    // If actor a overlaps some live actor that is affected by a holy water
    // projectile, inflict a holy water spray on that actor and return true;
    // otherwise, return false.  (See Actor::beSprayedIfAppropriate.)
  bool sprayFirstAppropriateActor(Actor* a);

    // Return true if actor a1 overlaps actor a2, otherwise false.
  bool overlaps(const Actor* a1, const Actor* a2) const;

    // If actor a overlaps this world's GhostRacer, return a pointer to the
    // GhostRacer; otherwise, return nullptr
  GhostRacer* getOverlappingGhostRacer(Actor* a) const;

private:
    GhostRacer* m_racer;
    std::list<Actor*> m_actorList;
    Actor* m_lastWhiteBorderAdded;
    int m_soulsToSave;
    int m_bonus;
};

#endif // STUDENTWORLD_H_
