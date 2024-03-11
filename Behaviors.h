#ifndef PACMAN_BEHAVIORS_H
#define PACMAN_BEHAVIORS_H

class Pacman;
class Ghost;
class Board;
class Entity;

// FIXME: remove that header
#include <stdio.h>

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

// TODO: different ghost behaviors
// TODO:
class RedGhostBehavior : public Behavior {
  public:
    virtual void move(Board &b, Entity &e) override {
        // chase pacman // MOSG aggressive
    }
};

// INFO: testing purpose only
// moves in random direction
class DumbGhostBehavior : public Behavior {
  public:
    DumbGhostBehavior(Ghost &g) : ghost(g) {}

    virtual void move(Board &b, Entity &e) override;

  private:
    Ghost &ghost;
};

// INFO: all ghost behave the same when running away from pacman
// TODO:
class GhostRunAwayBehavior : public Behavior {
  public:
    virtual void move(Board &b, Entity &e) override {
        // run away from pacman
    }
};

// TODO:
class GhostDeathBehavior : public Behavior {
  public:
    GhostDeathBehavior(Ghost &g) : ghost{g} {}

    virtual void move(Board &b, Entity &e) override ;

  private:
    Ghost &ghost;
};

#endif // !PACMAN_BEHAVIORS_H