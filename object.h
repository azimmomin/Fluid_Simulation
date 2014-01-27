#ifndef OBJECT_H
#define OBJECT_H

#include "point.h"
#include "vector.h"
#include "normal.h"
#include "face.h"
#include <string>
#include <vector>

class Object {
public:
	vector<Point> vertices;
	vector<Normal> normals;
	vector<Face> faces;
	int visible; //Determines whether or not an object should be drawn.
	int lines; //Draw in lines or triangles.
	int removeAt; //Frame to remove this object. Never removed if this is -1.
	Object(std::string filename, int v, int l, int rem);
	void Draw() const;
};
#endif
