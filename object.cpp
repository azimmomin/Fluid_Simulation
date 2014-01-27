#include "object.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cfloat>
#include <string>
#include <cstring>
#include <stdio.h>
#include <cstdlib>
#include <GL/glut.h>
#include <GL/glu.h>

using namespace std;

Object::Object(std::string filename, int v, int l, int rem) {
        removeAt = rem;
        visible = v;
	lines = l;
	ifstream inpfile(filename.c_str());
	if(!inpfile.is_open()) {
		cout << "UNABLE TO OPEN FILE." << endl;
	} else {
		string line;
		while (inpfile.good()) {
			vector<string> splitline;
			string buf;
			getline(inpfile, line);
			stringstream ss(line);
			while (ss >> buf) { splitline.push_back(buf); }
			//Ignore blank lines
			if (splitline.size() == 0) { continue; }
			//Ignore comments
			if (splitline[0][0] == '#') { continue; }
			//Parse in vertex
			if (!splitline[0].compare("v")) {
				float x = atof(splitline[1].c_str());
				float y = atof(splitline[2].c_str());
				float z = atof(splitline[3].c_str());
				vertices.push_back(Point(x, y, z));
			}
			//Parse in vertex normal
			else if (!splitline[0].compare("vn")) {
				normals.push_back(Normal(atof(splitline[1].c_str()), atof(splitline[2].c_str()), atof(splitline[3].c_str())));
			}
			//Parse in faces. Note that the faces should all be triangles.
			else if (splitline[0][0] == 'f') {
				Face face;
				for (int i = 1; i < 4; i++) {
					stringstream lss(splitline[i]);
					string s;
					getline(lss, s, '/');
					face.vertexIndex[i-1] = atoi(s.c_str()) - 1;
					face.normalIndex[i-1] = -1;
					if(getline(lss, s, '/')){
						face.normalIndex[i-1] = atoi(s.c_str()) - 1;
					}
					if(getline(lss, s, '/')){
						face.normalIndex[i-1] = atoi(s.c_str()) - 1;
					}
				}
				faces.push_back(face);
			} else {//Ignore everything else
				continue;
			}
		}
	}
	inpfile.close();
}

void Object::Draw() const{
  vector<Face>::const_iterator iter = faces.begin();
  while(iter != faces.end()){
    Normal n1, n2, n3;
    Point p1 = vertices[iter->vertexIndex[0]];
    Point p2 = vertices[iter->vertexIndex[1]];
    Point p3 = vertices[iter->vertexIndex[2]];
    if(iter->normalIndex[0] < 0){
      n1 = n2 = n3 = (p2 - p1).cross(p3 - p1);
    }else{
      n1 = normals[iter->normalIndex[0]];
      n2 = normals[iter->normalIndex[1]];
      n3 = normals[iter->normalIndex[2]];
    }
    if (!lines) {
      glBegin(GL_TRIANGLES);
      glNormal3f(n1.x, n1.y, n1.z);
      glVertex3f(p1.x, p1.y, p1.z);
      glNormal3f(n2.x, n2.y, n2.z);
      glVertex3f(p2.x, p2.y, p2.z);
      glNormal3f(n3.x, n3.y, n3.z);
      glVertex3f(p3.x, p3.y, p3.z);
      glEnd();
    } else {
      glBegin(GL_LINES);
      glVertex3f(p1.x, p1.y, p1.z);
      glVertex3f(p2.x, p2.y, p2.z);
      glEnd();
      glBegin(GL_LINES);
      glVertex3f(p2.x, p2.y, p2.z);
      glVertex3f(p3.x, p3.y, p3.z);
      glEnd();
    }
    iter++;
  }
}
