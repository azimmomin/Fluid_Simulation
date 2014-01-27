#include "source.h"
#include <ctime>
#include <cstdlib>

using namespace std;

#define LIMIT 240

Source::Source(Grid *g, Point& p, Vector& v, float m) {
  seed();
  pos = p;
  grid = g;
  vel = v;
  mass = m;
  setRate(0.1f);
  setMinScatter(-1.0f, -1.0f, -1.0f);
  setMaxScatter(1.0f, 1.0f, 1.0f);
  setMinDistance(0.0f, 0.0f, 0.0f);
  setMaxDistance(0.0f, 0.0f, 0.0f);
  currentRate = 0.0f;
  setDelay(0);
  setLimit(0);
}

Source::Source(Grid *g, Point *p, Vector *v, float m) {
  seed();
  pos = *p;
  grid = g;
  vel = *v;
  setRate(0.1f);
  setMinScatter(-1.0f, -1.0f, -1.0f);
  setMaxScatter(1.0f, 1.0f, 1.0f);
  setMinDistance(0.0f, 0.0f, 0.0f);
  setMaxDistance(0.0f, 0.0f, 0.0f);
  currentRate = 0.0f;
  setDelay(0);
  setLimit(LIMIT);
  mass = m;
}

void Source::seed() {
  static bool seeded = false;
  if (!seeded) {
    srand (static_cast <unsigned> (time(0)));
    seeded = true;
  }
}

void Source::update(vector<Particle>& particles) {
  if (delay > 0) {
    delay--;
    return;
  }
  if (-delay > limit) {
    return;
  }
  delay--;
  currentRate += rate;
  while (currentRate >= 1.0f) {
    float fmax = static_cast <float> (RAND_MAX);
    float xShift = static_cast <float> (rand()) / fmax * (xMaxDistance - xMinDistance) + xMinDistance;
    float yShift = static_cast <float> (rand()) / fmax * (yMaxDistance - yMinDistance) + yMinDistance;
    float zShift = static_cast <float> (rand()) / fmax * (zMaxDistance - zMinDistance) + zMinDistance;
    Point newPos = pos + Vector(xShift, yShift, zShift);
    if (!grid->canContain(newPos)) {
       currentRate -= 1.0f;
       continue;
    }
    xShift = static_cast <float> (rand()) / fmax * (xMaxScatter - xMinScatter) + xMinScatter;
    yShift = static_cast <float> (rand()) / fmax * (yMaxScatter - yMinScatter) + yMinScatter;
    zShift = static_cast <float> (rand()) / fmax * (zMaxScatter - zMinScatter) + zMinScatter;
    Vector newVel = Vector(xShift, yShift, zShift) + vel;
    particles.push_back(Particle(newPos, newVel, mass));
    currentRate -= 1.0f;
  }
}

void Source::setMinScatter(float x, float y, float z) {
  xMinScatter = x;
  yMinScatter = y;
  zMinScatter = z;
}

void Source::setMaxScatter(float x, float y, float z) {
  xMaxScatter = x;
  yMaxScatter = y;
  zMaxScatter = z;
}

void Source::setMinDistance(float x, float y, float z) {
  xMinDistance = x;
  yMinDistance = y;
  zMinDistance = z;
}

void Source::setMaxDistance(float x, float y, float z) {
  xMaxDistance = x;
  yMaxDistance = y;
  zMaxDistance = z;
}

void Source::setRate(float r) {
  rate = r;
}

void Source::setDelay(int d) {
  delay = d;
}

void Source::setLimit(int l) {
  limit = l;
}

void Source::setGrid(Grid* g) {
  grid = g;
}

Grid* Source::getGrid() {
  return grid;
}

InitializeSource::InitializeSource(Grid* g, float m, Point& p, float w, float h, float d, int n) : Source(g, &p, new Vector(), m) {
  setRate(0.0f);
  setMinScatter(0.0f, 0.0f, 0.0f);
  setMaxScatter(0.0f, 0.0f, 0.0f);
  setMinDistance(-w/2.0f, -h/2.0f, -d/2.0f);
  setMaxDistance(w/2.0f, h/2.0f, d/2.0f);
  setDelay(0);
  setLimit(0);
  currentRate = static_cast<float>(n);
}
