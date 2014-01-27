#include "grid.h"
#include <cmath>
#include <cstdlib>

Grid::Grid(float width, float height, float depth, float div)
     : w(width), h(height), d(depth), divisions(div),
       hdivisions(ceil(height/div)), wdivisions(ceil(width/div)), ddivisions(ceil(depth/div)){

	buckets = new vector<Particle*>* [hdivisions * wdivisions * ddivisions];
	//Null out all pointers.
	vector<Particle*> **cur = buckets, **last = buckets + hdivisions * wdivisions * ddivisions;
	while(cur < last){
		*cur = NULL;
		cur++;
	}
};

Grid::~Grid(){
	clear();
	delete[] buckets;
}

bool Grid::canContain(const Point& p) {
	return (p.x >= 0.0f && p.x <= w
		&& p.y >= 0.0f && p.y <= h
		&& p.z >= 0.0f && p.z <= d);
}

void Grid::addParticle(Particle* p) {
	int x = floor(p->pos.x/divisions); if(x == wdivisions){ x--; }
	int y = floor(p->pos.y/divisions); if(y == hdivisions){ y--; }
	int z = floor(p->pos.z/divisions); if(z == ddivisions){ z--; }
	//Check to see that the bucket is initalized.
	if(NULL == buckets[x + y * (wdivisions) + z * (hdivisions) * (wdivisions)]){
		buckets[x + y * (wdivisions) + z * (hdivisions) * (wdivisions)] = new vector<Particle*>;
	}
	buckets[x + y * (wdivisions) + z * (hdivisions) * (wdivisions)]->push_back(p);
}

void Grid::getParticleSubsection(const Particle* origin, vector<Particle*>& output) const{
	//Get the bucket.
	int x = floor(origin->pos.x/divisions); if(x == wdivisions){ x--; }
	int y = floor(origin->pos.y/divisions); if(y == hdivisions){ y--; }
	int z = floor(origin->pos.z/divisions); if(z == ddivisions){ z--; }
	//Loop through 3x3x3 around that bucket.
	vector<Particle*>* target;
	vector<Particle*>::iterator iter;
	for(int dx = -1;dx <= 1;dx++){
		for(int dy = -1;dy <= 1;dy++){
			for(int dz = -1;dz <= 1;dz++){
				if((x + dx) < wdivisions && 0 <= (x + dx) && (y + dy) < hdivisions && 0 <= (y + dy) && (z + dz) < ddivisions && 0 <= (z + dz)){
					target = buckets[(x + dx) + (y + dy) * (wdivisions) + (z + dz) * (hdivisions) * (wdivisions)];
					if(NULL == target){ continue; }
					//Copy contents of target into subsection.
					iter = target->begin();
					while(iter != target->end()){
						output.push_back(*iter);
						iter++;
					}
				}
			}
		}
	}
}

void Grid::clear(){
	vector<Particle*> **cur = buckets, **last = buckets + hdivisions * wdivisions * ddivisions;
	while(cur < last){
		if(NULL != *cur){
			delete *cur;
			*cur = NULL;
		}
		cur++;
	}
}

int Grid::getXDivisions() const{ return(wdivisions); }

int Grid::getYDivisions() const{ return(hdivisions); }

int Grid::getZDivisions() const{ return(ddivisions); }

Point Grid::getIndexedPosition(int xIndex, int yIndex, int zIndex) const{
	return Point(divisions*xIndex, divisions*yIndex, divisions*zIndex);
}

float Grid::getW() {
  return w;
}

float Grid::getH() {
  return h;
}

float Grid::getD() {
  return d;
}

#include <iostream>
using namespace std;

void Grid::setup(vector<Particle>& particles) {
	clear();
	vector<Particle>::iterator iter = particles.begin();
	while(iter != particles.end()){
		addParticle( &(*iter) );
		iter++;
	}
}
