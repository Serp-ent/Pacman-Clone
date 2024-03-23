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

class RedGhostBehavior : public Behavior {
  public:
    RedGhostBehavior(Ghost &g) : ghost(g) {}
    virtual void move(Board &b, Entity &e) override;

    void resetPath() { path.clear(); }
    const std::vector<Graph::BoxNode *> &getPath() { return path; }

  private:
    Ghost &ghost;
    std::vector<Graph::BoxNode *> path;
};

class PinkGhostBehavior : public Behavior {
  public:
    PinkGhostBehavior(Ghost &g) : ghost(g) {}
    virtual void move(Board &b, Entity &e) override;

    void resetPath() { path.clear(); }

  private:
    Ghost &ghost;
    std::vector<Graph::BoxNode *> path;
};

class OrangeGhostBehavior : public Behavior {
  public:
    OrangeGhostBehavior(Ghost &g) : ghost(g) {}
    virtual void move(Board &b, Entity &e) override;

    void resetPath() { path.clear(); }

  private:
    Ghost &ghost;
    std::vector<Graph::BoxNode *> path;
};

class CyanGhostBehavior : public Behavior {
  public:
    CyanGhostBehavior(Ghost &g) : ghost(g) {}
    virtual void move(Board &b, Entity &e) override;

    void resetPath() { path.clear(); }

  private:
    Ghost &ghost;
    std::vector<Graph::BoxNode *> path;
};

// INFO: testing purpose only moves in random direction
class DumbGhostBehavior : public Behavior {
  public:
    DumbGhostBehavior(Ghost &g) : ghost(g) {}

    virtual void move(Board &b, Entity &e) override;

  private:
    Ghost &ghost;
};

// INFO: all ghost behave the same when running away from pacman
class GhostRunAwayBehavior : public Behavior {
  public:
    GhostRunAwayBehavior(Ghost &g) : ghost{g} {}
    virtual void move(Board &b, Entity &e) override;

  private:
    Ghost &ghost;
    std::vector<Graph::BoxNode *> path;
};

class GhostDeathBehavior : public Behavior {
  public:
    GhostDeathBehavior(Ghost &g) : ghost{g} {};

    void loadPathToHome(Board &b);

    virtual void move(Board &b, Entity &e) override;

  private:
    Ghost &ghost;
    std::vector<Graph::BoxNode *> path;
};

#endif // !PACMAN_BEHAVIORS_H
