#ifndef PACMAN_BEHAVIORS_H
#define PACMAN_BEHAVIORS_H

class Pacman;
class Board;

// Strategy pattern to do different behaviors that can change in run-time
class Behavior {
  public:
    virtual void move(Board &b) = 0;

    virtual ~Behavior() {}
};

class PacmanDefaultBehavior : public Behavior {
  public:
    PacmanDefaultBehavior(Pacman &p) : pacman(p) {}

    virtual void move(Board &b) override;

  private:
    Pacman &pacman;
};

class PacmanSuperPointBehavior : public Behavior {
  public:
    PacmanSuperPointBehavior(Pacman &p) : pacman(p) {}
    virtual void move(Board &b) override {
        // eat points
        // if ghost encoutered eat him and add multiple points
    }

  private:
    Pacman &pacman;
};

// TODO: different ghost behaviors
// TODO:
class RedGhostBehavior : public Behavior {
  public:
    virtual void move(Board &b) override {
        // chase pacman // MOSG aggressive
    }
};

// INFO: all ghost behave the same when running away from pacman
// TODO:
class GhostRunAwayBehavior : public Behavior {
  public:
    virtual void move(Board &b) override {
        // run away from pacman
    }
};

// TODO:
class GhostDeathBehavior : public Behavior {
  public:
    virtual void move(Board &b) override {
        // move to ghost spawn
    }
};

#endif // !PACMAN_BEHAVIORS_H
