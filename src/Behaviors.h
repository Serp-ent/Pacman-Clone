#ifndef PACMAN_BEHAVIORS_H
#define PACMAN_BEHAVIORS_H

#include "Graph.h"

class Pacman;
class Ghost;
class Board;
class Entity;

// Strategy pattern to do different behaviors that can change in run-time
class Behavior {
  public:
    virtual void move(Board &b, Entity &e) = 0;

    virtual ~Behavior() {}
};

class PacmanDefaultBehavior : public Behavior {
  public:
    PacmanDefaultBehavior(Pacman &p) : pacman(p) {}

    virtual void move(Board &b, Entity &e) override;

  private:
    Pacman &pacman;
};

class PacmanSuperPointBehavior : public Behavior {
  public:
    PacmanSuperPointBehavior(Pacman &p) : pacman(p) {}
    virtual void move(Board &b, Entity &e) override;

  private:
    Pacman &pacman;
};

//*********************************************************************************

class GhostAttackBehavior : public Behavior {
  public:
    GhostAttackBehavior(Ghost &g) : ghost{g} {}
    virtual ~GhostAttackBehavior(){};

    void resetPath() { path.clear(); }
    std::vector<Graph::BoxNode *> &getPath() { return path; }

  protected:
    Ghost &ghost;
    std::vector<Graph::BoxNode *> path;
};

class RedGhostBehavior : public GhostAttackBehavior {
  public:
    RedGhostBehavior(Ghost &g) : GhostAttackBehavior{g} {}
    virtual void move(Board &b, Entity &e) override;
};

class PinkGhostBehavior : public GhostAttackBehavior {
  public:
    PinkGhostBehavior(Ghost &g) : GhostAttackBehavior{g} {}
    virtual void move(Board &b, Entity &e) override;
};

class OrangeGhostBehavior : public GhostAttackBehavior {
  public:
    OrangeGhostBehavior(Ghost &g) : GhostAttackBehavior{g} {}
    virtual void move(Board &b, Entity &e) override;

  private:
    bool scared = false;
};

class CyanGhostBehavior : public GhostAttackBehavior {
  public:
    CyanGhostBehavior(Ghost &g) : GhostAttackBehavior{g} {}
    virtual void move(Board &b, Entity &e) override;
};

// INFO: testing purpose only moves in random direction
class DumbGhostBehavior : public GhostAttackBehavior {
  public:
    DumbGhostBehavior(Ghost &g) : GhostAttackBehavior{g} {}

    virtual void move(Board &b, Entity &e) override;
};

//*********************************************************************************

// INFO: all ghost behave the same when running away from pacman
class GhostRunAwayBehavior : public Behavior {
  public:
    GhostRunAwayBehavior(Ghost &g) : ghost{g} {}
    virtual void move(Board &b, Entity &e) override;

    std::vector<Graph::BoxNode *> &getPath() { return path; }

  private:
    Ghost &ghost;
    std::vector<Graph::BoxNode *> path;
};

class GhostDeathBehavior : public Behavior {
  public:
    GhostDeathBehavior(Ghost &g) : ghost{g} {};

    void loadPathToHome(Board &b);

    virtual void move(Board &b, Entity &e) override;
    std::vector<Graph::BoxNode *> &getPath() { return path; }

  private:
    Ghost &ghost;
    std::vector<Graph::BoxNode *> path;
};

#endif // !PACMAN_BEHAVIORS_H
