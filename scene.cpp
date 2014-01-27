#include "scene.h"
#include "vector.h"
#include "point.h"
#include "field.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <GL/glut.h>
#include <GL/glu.h>
#define _USE_MATH_INCLUDES
#include <cmath>

extern float PRESSURE_K;
extern float PRESSURE_REST_DENSITY;
extern float VISCOSITY_MU;

/** Constructs a scene by parsing in arguements from
 *	a file called FILENAME. */
Scene::Scene(std::string filename, std::string output){
        grid = NULL; simulator = NULL; renderer = NULL;
	outputDirectory = "output";
	runtime = 0;
	screenWidth = 640; screenHeight = 480;
	xOffset = 0.0f; yOffset = 0.0f; zDistance = 30.0f;
	pitch = -45.0f; yaw = 30.0f;

	float width = 0.0f, height = 0.0f, depth = 0.0f, division = 0.0f;
	int subdivisions = 1;
	fps = 24; substeps = 1;
	float mass = 1.0f;
	vector<FieldBox*> gravity;

	ifstream inpfile(filename.c_str());
	if(!inpfile.is_open()){
		std::cout << "UNABLE TO OPEN FILE." << std::endl;
		abort();
	}else{
		std::string line;
		while(inpfile.good()){
			std::vector<std::string> splitline;
			std::string buf;
			
			std::getline(inpfile, line);
			std::stringstream ss(line);
			while(ss >> buf){ splitline.push_back(buf); }
			if(splitline.size() == 0){ continue; }//Ignore blank lines
			
			if(splitline[0][0] == '#'){ continue; }//Ignore comments

			if(!splitline[0].compare("size")){
				//The size of the region to put particles in. (uses same parameters as grid).
				if(grid != NULL){ cerr<<"Error: Multiple 'size' parameters specified. Ignoring later.\n"; continue; }
				if(splitline.size() != 5){ cerr<<"Error: The 'size' parameter has wrong number of arguments.\n"; continue; }
				width = atof(splitline[1].c_str());
				height = atof(splitline[2].c_str());
				depth = atof(splitline[3].c_str());
				division = atof(splitline[4].c_str());
				grid = new Grid(width, height, depth, division);
			}else if(!splitline[0].compare("subdivision")){
				//The subdivision argument for Renderer. (Default is 1.)
				if(splitline.size() != 2){ cerr<<"Error: The 'subdivision' parameter has wrong number of arguments.\n"; continue; }
				subdivisions = atoi(splitline[1].c_str());
			}else if(!splitline[0].compare("fps")) {
				//The frame rate of playback. (Default is 24.)
				if(splitline.size() != 2){ cerr<<"Error: The 'fps' parameter has wrong number of arguments.\n"; continue; }
				fps = atof(splitline[1].c_str());
			}else if(!splitline[0].compare("substep")) {
				//Number of updates the time between frames is split into. (Default is 1.)
				if(splitline.size() != 2){ cerr<<"Error: The 'substep' parameter has wrong number of arguments.\n"; continue; }
				substeps = atof(splitline[1].c_str());
			}else if(!splitline[0].compare("mass")){
				//The mass of particles to be created.
				if(splitline.size() != 2){ cerr<<"Error: The 'mass' parameter has wrong number of arguments.\n"; continue; }
				mass = atof(splitline[1].c_str());
			}else if(!splitline[0].compare("obj")){
				//The path to the OBJ file as well as the visible parameter which determines whether or not the object should be drawn.
			  if(splitline.size() != 5){ cerr<<"Error: The 'obj' parameter has wrong number of arguments.\n"; continue; }
			  //second parameter is visible term, third is drawing term, last is the remove term.
			  objects.push_back(Object(splitline[1], atoi(splitline[2].c_str()), atoi(splitline[3].c_str()), atoi(splitline[4].c_str())));
			  /*}else if(!splitline[0].compare("visible")){
			  // specify whether or not to draw objects parsed in from OBJ file (true by default.) 
			  if(splitline.size() != 2) { cerr<<"Error: The 'visible' paramter has wrong number of arguments. \n"; continue; }
			  visible = atoi(splitline[1].c_str());*/
			}else if(!splitline[0].compare("output")){
			        //The output directory file name (no slashes!).
			        if(splitline.size() != 2){ cerr<<"Error: The 'output' parameter has wrong number of arguments.\n"; continue; }
			        outputDirectory = splitline[1];
			}else if(!splitline[0].compare("runtime")){
			        //The number of seconds (an integer) to run for.
			        if(splitline.size() != 2){ cerr<<"Error: The 'runtime' parameter has wrong number of arguments.\n"; continue; }
			        runtime = atoi(splitline[1].c_str()) * fps;
			}else if(!splitline[0].compare("screen")){
				//The initial size of the screen (integer/measures in pixels). (Default 640x480.)
			  if(splitline.size() != 3){ 
			    cerr<<"Error: The 'screen' parameter has wrong number of arguments.\n"; continue; }
				screenWidth = atoi(splitline[1].c_str());
				screenHeight = atoi(splitline[2].c_str());
			}else if(!splitline[0].compare("camera")){
				//The initial position of the camera.
				if(splitline.size() != 6){ cerr<<"Error: The 'camera' parameter has wrong number of arguments.\n"; continue; }
				xOffset = atof(splitline[1].c_str());
				yOffset = atof(splitline[2].c_str());
				zDistance = atof(splitline[3].c_str());
				pitch = atof(splitline[4].c_str());
				yaw = atof(splitline[5].c_str());
			}else if(!splitline[0].compare("params")){
				//Set the force parameters/constants.
				if(splitline.size() != 4){ cerr<<"Error: The 'params' parameter has wrong number of arguments.\n"; continue; }
				PRESSURE_K = atof(splitline[1].c_str());
				PRESSURE_REST_DENSITY = atof(splitline[2].c_str());
				VISCOSITY_MU = atof(splitline[3].c_str());
			}else if(!splitline[0].compare("block")){
				//Create a block of particles.
				if(grid == NULL){ cerr<<"Error: Cannot add particles before 'size'.\n"; continue; }
				if(splitline.size() != 8){ cerr<<"Error: The 'block' parameter has wrong number of arguments.\n"; continue; }
				Point bPos;
				float bwidth = atof(splitline[1].c_str());
				float bheight = atof(splitline[2].c_str());
				float bdepth = atof(splitline[3].c_str());
				bPos.x = atof(splitline[4].c_str());
				bPos.y = atof(splitline[5].c_str());
				bPos.z = atof(splitline[6].c_str());
				float div = atof(splitline[7].c_str());
				Vector vel, rnd;
				Point pos(0.0f, 0.0f, 0.0f);
				for (float i = 0.0f; i < bwidth; i += div) {
					for (float j = 0.0f; j < bheight; j += div) {
						for (float k = 0.0f; k < bdepth; k += div) {
							rnd.x = ((float)rand()) / ((float)RAND_MAX) * 0.5f * div;
							rnd.y = ((float)rand()) / ((float)RAND_MAX) * 0.5f * div;
							rnd.z = ((float)rand()) / ((float)RAND_MAX) * 0.5f * div;
							pos = bPos + Vector(i, j, k) + rnd;
							if(grid->canContain(pos)){
								particles.push_back(Particle(pos, vel, mass));
							}
						}
					}
				}
			}else if(!splitline[0].compare("source")){
				//Creates a fluid source.
				if(grid == NULL){ cerr<<"Error: Cannot specify 'source' before 'size'.\n"; continue; }
				if(splitline.size() != 22){ cerr<<"Error: The 'source' parameter has wrong number of arguments.\n"; continue; }
				Point pos;//Position
				pos.x = atof(splitline[1].c_str());
				pos.y = atof(splitline[2].c_str());
				pos.z = atof(splitline[3].c_str());
				Vector vel;//Velocity
				vel.x = atof(splitline[4].c_str());
				vel.y = atof(splitline[5].c_str());
				vel.z = atof(splitline[6].c_str());
				float r = atof(splitline[7].c_str());//Rate (per substep)
				Vector minDisp;//Minimum displacement from source (position).
				minDisp.x = atof(splitline[8].c_str());
				minDisp.y = atof(splitline[9].c_str());
				minDisp.z = atof(splitline[10].c_str());
				Vector maxDisp;//maximum displacement from source (position).
				maxDisp.x = atof(splitline[11].c_str());
				maxDisp.y = atof(splitline[12].c_str());
				maxDisp.z = atof(splitline[13].c_str());
				Vector minScatter;//Minimum scatter (velocity).
				minScatter.x = atof(splitline[14].c_str());
				minScatter.y = atof(splitline[15].c_str());
				minScatter.z = atof(splitline[16].c_str());
				Vector maxScatter;//Maximum scatter (velocity).
				maxScatter.x = atof(splitline[17].c_str());
				maxScatter.y = atof(splitline[18].c_str());
				maxScatter.z = atof(splitline[19].c_str());
				int delay = atoi(splitline[20].c_str());//How long to wait to begin making particles (in substeps);
				int limit = atoi(splitline[21].c_str());//How long to run after delay (in substeps).
				Source* s = new Source(grid, pos, vel, mass);
				s->setRate(r); s->setDelay(delay); s->setLimit(limit);
				s->setMinDistance(minDisp.x, minDisp.y, minDisp.z);
				s->setMaxDistance(maxDisp.x, maxDisp.y, maxDisp.z);
				s->setMinScatter(minScatter.x, minScatter.y, minScatter.z);
				s->setMaxScatter(maxScatter.x, maxScatter.y, maxScatter.z);
				sources.push_back(s);
			}else if(!splitline[0].compare("gravity")){
				//Creates a gravity field vector
				float spx = atof(splitline[1].c_str());
				float spy = atof(splitline[2].c_str());
				float spz = atof(splitline[3].c_str());
				float svx = atof(splitline[4].c_str());
				float svy = atof(splitline[5].c_str());
				float svz = atof(splitline[6].c_str());
				float rapx = atof(splitline[7].c_str());
				float rapy = atof(splitline[8].c_str());
				float rapz = atof(splitline[9].c_str());
				float r = atof(splitline[10].c_str());
				gravity.push_back(new FieldBox(Point(spx, spy, spz), Vector(svx, svy, svz), Vector(rapx, rapy, rapz), r));
			}else{
				std::cerr << "UNKNOWN COMMAND: " << splitline[0] << std::endl;
			}
		}
		inpfile.close();
		if(grid == NULL){
			cerr<<"Error: The 'size' parameter not specified... aborting.\n";
			abort();
		}
		cout<<"Particle count: "<<particles.size()<<";\n";
		grid->setup(particles);
		simulator = new Simulator(width, height, depth, division, 1.0f/((float)(fps*substeps)), *grid, objects, gravity);
		simulator->calculateDensity(particles);
		renderer = new Renderer(width, height, depth, division, subdivisions, grid);
		renderer->SetupRender();
	}
}

Scene::~Scene() {
	vector<Source*>::iterator iter = sources.begin();
	while(iter != sources.end()){
		delete *iter;
		iter++;
	}
	delete(grid);
	delete(simulator);
	delete(renderer);
}

void Scene::Draw(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(xOffset, yOffset, -zDistance);
	glRotatef(pitch, 0.0f, 1.0f, 0.0f);
	glRotatef(yaw, cos(pitch * M_PI / 180.0f), 0.0f, sin(pitch * M_PI / 180.0f));

	//Draw bounding box based on grid parameters.
	float width = grid->getW();
	float height = grid->getH();
	float depth = grid->getD();
	glColor3f(1, 0, 1);
	//Back Face
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(width, 0.0f, 0.0f);
	glVertex3f(width, height, 0.0f);
	glVertex3f(0.0f, height, 0.0f);
	glEnd();
	
	//Front Face
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0f, 0.0f, depth);
	glVertex3f(width, 0.0f, depth);
	glVertex3f(width, height, depth);
	glVertex3f(0.0f, height, depth);
	glEnd();

	//Other Edges
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, depth);
	glVertex3f(width, 0.0f, 0.0f);
	glVertex3f(width, 0.0f, depth);
	glVertex3f(width, height, 0.0f);
	glVertex3f(width, height, depth);
	glVertex3f(0.0f, height, 0.0f);
	glVertex3f(0.0f, height, depth);
	glEnd();

	//Draw only if the VISIBLE term in the object is true.
	//For each Object, load the obj file and draw it if it is within the bounding box.
	vector<Object>::iterator iter = objects.begin();
	while(iter != objects.end()){
	  if (iter->visible) {
	    iter->Draw();
	  }
	  iter++;
	}
	renderer->Render();
}

void Scene::Update(int curFrame){
  bool changed = false;
  for (int j = 0; j < objects.size(); j++) {
    if (objects[j].removeAt == curFrame) {
      objects.erase(objects.begin()+j);
      changed = true;
    }
  }
  if (changed) {
    simulator->rebuildTree(objects);
  }
  //Go through the sources and have them update the particle list.
  vector<Source*>::iterator iter = sources.begin();
  while(iter != sources.end()){
    (*iter)->update(particles);
    iter++;
  }
  for(int i = 0;i < substeps;i++){
    grid->setup(particles);
    //Update the particle positions.
    simulator->calculateDensity(particles);
    simulator->update(particles);
  }
  cout << "Particles: " << particles.size() << endl;
  //Prep to draw what was just updated.
  renderer->SetupRender();
}

