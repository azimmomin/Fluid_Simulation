#include "simulator.h"
#include "field.h"
#include <iostream>
#include <omp.h>
 
Simulator::Simulator(float width, float height, float depth, float div, float step, Grid& gridi, const vector<Object>& objects, const vector<FieldBox*>& gravity)
         : timestep(step), kernel(div) {
    grid = &gridi;
    w = width;
    d = depth;
    h = height;
    tree = new BoundingTree(width, height, depth, objects);
    forces = new Forces(div, gravity);
    firstUpdate = true;
}
Simulator::~Simulator() {
    delete(tree);
    delete(forces);
}
 
void Simulator::rebuildTree(const vector<Object>& objectList) {
  tree = new BoundingTree(w, h, d, objectList);
}
/** Calculates the acceleration and force for each particle in
 *  PARTICLES. It also updates each particle's' velocity and
 *  position by calling updateVelcity and updatePosition. */
void Simulator::update(vector<Particle>& particles) {
    Vector f, a;
    vector<Particle*> section;
/********************************************************************************NOTE
 * This has changed! Now actually does leapfrog integration. */
	if(firstUpdate){
		//Move velocity foreward half a step.
		#pragma omp parallel for private(section, f)
		for(unsigned int i = 0;i < particles.size();i++){
		    Particle* cur = &(particles[i]);
		    if (cur->density > 0.0f) {
		    section.clear();
		    grid->getParticleSubsection(cur, section);
		    f = forces->applyTo(section, cur);
		    cur->vel = cur->vel + f * (1.0f / cur->density) * 0.5f * timestep;
		    } else { cur->vel = cur->vel + Vector(0.0f, -9.8f, 0.0f) * 0.5f * timestep; }
		}
		firstUpdate = false;
	}
	#pragma omp parallel for
	for(unsigned int i = 0;i < particles.size();i++){
	    Particle& cur = particles[i];
	    updateVelocity(cur, cur.accel);
	    Point newPos = getNewPosition(cur);
	    tree->Bound(cur, newPos, 0.6f);
	}
	#pragma omp parallel for private(section, f)
	for(unsigned int i = 0;i < particles.size();i++){
	    Particle* cur = &(particles[i]);
		    if (cur->density > 0.0f) {
	    section.clear();
	    grid->getParticleSubsection(cur, section);
	    f = forces->applyTo(section, cur);
	    cur->accel = f * (1.0f / cur->density);
		    } else { cur->vel = cur->vel + Vector(0.0f, -9.8f, 0.0f) * 0.5f * timestep; }
	}
}
     
/** Uses leapfrog iteration to update the velocity of particle P
 *  according to acceleration A and the constant timestep. */
void Simulator::updateVelocity(Particle& p, const Vector& a) {
    p.vel = p.vel + (a * timestep);
}
 
/** Returns the new position so you can use it and the old position for
 *  collision calculations. */
Point Simulator::getNewPosition(const Particle& p) {
    return p.pos + (p.vel * timestep);
}
 
/** As the name suggests, this updates the density of each particle in
 *  PARTICLES and stores them within the individual particle. */
void Simulator::calculateDensity(vector<Particle>& particles) {
    //calculate densities for each particle.
    float currentDensity;//, allDensity = 0;
    vector<Particle*> subsection;
    vector<Particle*>::iterator other;
    #pragma omp parallel for private(subsection, other, currentDensity)
    for(unsigned int i = 0;i < particles.size();i++){
        Particle* cur = &(particles[i]); subsection.clear();
        grid->getParticleSubsection(cur, subsection);
        currentDensity = 0.0f;
        other = subsection.begin();
        while(other != subsection.end()) {
            currentDensity += (*other)->mass * kernel(cur->pos - (*other)->pos);
            other++;
        }
        cur->density = currentDensity;
    }
    //allDensity /= ((float)particles.size());
    //cout<<"Average Density:"<<allDensity<<";\n"; cout.flush();
}
