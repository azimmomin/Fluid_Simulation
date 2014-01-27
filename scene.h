#ifndef SCENE_H
#define SCENE_H

#include "grid.h"
#include "Renderer.hpp"
#include "particle.h"
#include "simulator.h"
#include "source.h"
#include "object.h"
#include "point.h"
#include <string>
#include <vector>

class Scene {
public:
	vector<Object> objects;
	vector<Particle> particles;
	vector<Source*> sources;
	Grid* grid;
	Simulator *simulator;
	Renderer *renderer;
	string outputDirectory;
	int screenWidth, screenHeight;
	float xOffset, yOffset, zDistance;
	float pitch, yaw;//In degrees...
	int runtime;//Seconds to run if in commandline mode.
	int fps;
	int substeps;//Number of updates needed to complete one frame.
	Scene(std::string filename, std::string output);
	~Scene();

	void Draw();

	void Update(int curFrame);
};

#endif
