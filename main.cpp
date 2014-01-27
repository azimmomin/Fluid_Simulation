// CS184 Final Project
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#include <GL/glut.h>
#include <GL/glu.h>

#include <math.h>
#include <ctime>

#ifdef _WIN32
#endif
// Our files.
#include "scene.h"
#include "FrameSaver.hpp"

using namespace std;

int viewWidth = 1024, viewHeight = 960;
string filename, output;
Scene *scene;

const float panDelta = 0.25f;
const float angleDelta = 2.0f;
float frame = 0;
void myReshape(int w, int h) {
	viewWidth = w;
	viewHeight = h;

	glViewport(0, 0, viewWidth, viewHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//----------- setting the projection -------------------------
	// glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


	// glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
	// glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

	//glOrtho(-1, 1, -1, 1, 1, -1);		// resize type = stretch
	gluPerspective(45.0f, (double)viewWidth / (double)viewHeight, 1.0f, 200.0f);
	//------------------------------------------------------------
}

void myDisplay() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	scene->Draw();

	glFlush();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	float direction = 0.0f;
	switch(key){
	case 27:
		exit(0);
	case '-': case 'q': scene->zDistance += panDelta; break;
	case '=': case 'e': scene->zDistance -= panDelta; break;
	case 'w': scene->yaw += angleDelta; break;
	case 's': scene->yaw -= angleDelta; break;
	case 'a': scene->pitch += angleDelta; break;
	case 'd': scene->pitch -= angleDelta; break;
	case 'D': direction += 90.0f;
	case 'S': direction += 90.0f;
	case 'A': direction += 90.0f;
	case 'W':
		scene->xOffset += -panDelta*sin(direction * M_PI / 180.0f);
		scene->yOffset +=  panDelta*cos(direction * M_PI / 180.0f);
		break;
	case 'p'://Prints out current camera position.
		cout<<"\"camera "<<(scene->xOffset)<<" "<<(scene->yOffset)<<" "<<(scene->zDistance);
		cout<<" "<<(scene->pitch)<<" "<<(scene->yaw)<<"\"\n";
		break;
	case ' ':
		scene->Update(++frame);
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[]) {
	int counter = 1;
	string filename;
	if (argc < 2) {
		cout << "Please specify a valid path to a test file." << endl;
		exit(1);
	}
	filename = argv[counter];
	counter++;
	if (argc == 3) {
		output = argv[counter];
	} else {
		output = "output.mov";
	}
	//Create a new scene.
	scene = new Scene(filename, output);
	viewWidth = scene->screenWidth;
	viewHeight = scene->screenHeight;

	//This initializes glut
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

	glutInitWindowSize(viewWidth, viewHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("CS184 Final Project");

	myReshape(viewWidth, viewHeight);

	glLineWidth(2.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
       	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	float params[4];
	float params1[4];
	params[0] = 11.0f; params[1] = 11.0f; params[2] = 11.0f; params[3] = 0.0f;
	params1[0] = 0.3f; params1[1] = 0.3f; params1[2] = 0.3; params1[3] = 0.3f;
	glLightfv(GL_LIGHT0, GL_POSITION, params);
	glLightfv(GL_LIGHT1, GL_AMBIENT, params1); 
	if(scene->runtime > 0){//Write images to file as fast as possible.
		int t = time(NULL);
		FrameSaver fs(scene->screenWidth, scene->screenHeight, scene->outputDirectory.c_str());
		cout<<"Finished frame: ";
		for(;frame < scene->runtime;frame++){
			scene->Update(frame);
			cout<<frame<<" "; cout.flush();
			myDisplay();
			fs.SaveFrame();
		}
		t = time(NULL) - t;
		float perFrame = ((float)t) / ((float)scene->runtime);
		float realTimeRatio = perFrame * ((float)scene->fps);
		int seconds = t % 60; t /= 60;
		int minutes = t % 60; t /= 60;
		int hours = t;
		cout<<" Done.\nTime taken: "<<hours<<":"<<minutes<<":"<<seconds<<". "<<perFrame<<" seconds per frame. Real time ratio: "<<realTimeRatio<<".\nNow entering interactive mode.\n";
	}
	//Run interactively.
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return(0);
}
