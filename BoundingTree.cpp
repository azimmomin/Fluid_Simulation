#include "BoundingTree.hpp"

#include <iostream>
using namespace std;
#include <cmath>
#define EPSILON 0.00001

#include "Matrix.hpp"

BoundingTree::BoundingTree(float width, float height, float depth, const vector<Object>& objs)
            : xMaximum(width), yMaximum(height), zMaximum(depth){
	cout<<"Max: ("<<width<<","<<height<<","<<depth<<")\n";
	root = NULL;//\nJust in case...
	//First create a bounding box for each face.
	vector<BoundingBox*> leaves;
	{
		vector<Object>::const_iterator object = objs.begin();
		while(object != objs.end()){
			vector<Face>::const_iterator iter = object->faces.begin();
			while(iter != object->faces.end()){
				leaves.push_back(new BoundingBox(object->vertices[iter->vertexIndex[0]], object->vertices[iter->vertexIndex[1]],
				                 object->vertices[iter->vertexIndex[2]]));
				iter++;
			}
			object++;
		}
	}
	//Merge the two boxes that would form the smallest bounding box until all are merged. (There might be a better way? This is an R-tree...)
	unsigned int bFirst = 0, bNext = 0; float bSize = 0.0f, cSize = 0.0f;//Don't worry these will be set to first valid pairing.
	bool isFirst;
	BoundingBox *fNode, *nNode;
	for(int remaining = leaves.size();remaining > 1;remaining--){
		isFirst = true;
		for(unsigned int first = 0;first < leaves.size()-1;first++){
			fNode = leaves[first];
			if(NULL != fNode){
				for(unsigned int next = first+1;next < leaves.size();next++){
					nNode = leaves[next];
					if(NULL != nNode){
						cSize = (*fNode + *nNode).Size();
						//Check if best (or if a best hasn't already been found).
						if(isFirst || cSize < bSize){
							bFirst = first; bNext = next;
							bSize = cSize;
							isFirst = false;
						}
					}
				}
			}
		}
		//Replace first with combined.
		fNode = leaves[bFirst]; nNode = leaves[bNext];
		leaves[bFirst] = new BoundingBoxNode(fNode, nNode);
		leaves[bNext] = NULL;//Remove at bNext.
	}
	//Set root to last remaining node.
	{
		vector<BoundingBox*>::iterator iter = leaves.begin();
		while(iter != leaves.end()){
			if(NULL != *iter){ root = *iter; break; }
			iter++;
		}
	}
}

BoundingTree::~BoundingTree(){
	if(NULL != root){ delete root; }
}

void BoundingTree::Bound(Particle& particle, Point& newPos, float mu) const{
	bool needCheck[2];
	float t; Vector dir;
	Point xIntersect, yIntersect, zIntersect, bIntersect;
	Vector xNormal, yNormal, zNormal, bNormal;
	float xClose, yClose, zClose, bClose;
	bool xChange, yChange, zChange;
	Particle tParticle[2];
	Point nPos[2];
	bool nudged;
	needCheck[0] = needCheck[1] = true;
	while(needCheck[0] || needCheck[1]){
		needCheck[0] = needCheck[1] = false;
		tParticle[0] = tParticle[1] = particle;
		nPos[0] = nPos[1] = newPos;
		if(NULL != root){ needCheck[1] = root->Bound(tParticle[1], nPos[1], mu); }
		if( (!needCheck[1]) && nPos[1] != newPos){
			nPos[0] = nPos[1];
			nudged = true;
		}
		//Begin forced bounding in the region.
		dir = nPos[0] - tParticle[0].pos;
#define ATLEAST(var) if(var < 0.0f){ var = 0.0f; }
#define ATMOST(var, most) if(var > most){ var = most; }

#define FSHIFT(var, varMax) \
		if(dir.var > -EPSILON && dir.var < EPSILON){ \
			if(nPos[0].var < 0.0f){ \
				tParticle[0].pos.var -= nPos[0].var; \
				ATMOST(tParticle[0].pos.var, varMax); \
				nPos[0].var = 0.0f; \
				needCheck[0] = true; \
			}else if(nPos[0].var > varMax){ \
				tParticle[0].pos.var -= nPos[0].var - varMax; \
				ATLEAST(tParticle[0].pos.var); \
				nPos[0].var = varMax; \
				needCheck[0] = true; \
			} \
		}
		FSHIFT(x, xMaximum);//Check if nPos[0].x is out of bounds and requires shifting.
		FSHIFT(y, yMaximum);//Check if nPos[0].y is out of bounds and requires shifting.
		FSHIFT(z, zMaximum);//Check if nPos[0].z is out of bounds and requires shifting.
#undef FSHIFT
		dir = nPos[0] - tParticle[0].pos;
		xChange = yChange = zChange = false;
		xIntersect = yIntersect = zIntersect = nPos[0];
		//See if the nPos[0].x is out of bounds.
		if(nPos[0].x < 0.0f){
			//Scale it so that its at that point.
			t = (0.0f - tParticle[0].pos.x)/dir.x;
			xIntersect.x = 0.0f;
			xIntersect.y = tParticle[0].pos.y*(1.0f - t) + nPos[0].y*t;
			xIntersect.z = tParticle[0].pos.z*(1.0f - t) + nPos[0].z*t;
			xNormal = Vector(1.0f, 0.0f, 0.0f);
			xChange = true;
		}else if(nPos[0].x > xMaximum){
			//Scale it so that its at that point.
			t = (xMaximum - tParticle[0].pos.x)/dir.x;
			xIntersect.x = xMaximum;
			xIntersect.y = tParticle[0].pos.y*(1.0f - t) + nPos[0].y*t;
			xIntersect.z = tParticle[0].pos.z*(1.0f - t) + nPos[0].z*t;
			xNormal = Vector(-1.0f, 0.0f, 0.0f);
			xChange = true;
		}
		//See if the nPos[0].y is out of bounds.
		if(nPos[0].y < 0.0f){
			//Scale it so that its at that point.
			t = (0.0f - tParticle[0].pos.y)/dir.y;
			yIntersect.x = tParticle[0].pos.x*(1.0f - t) + nPos[0].x*t;
			yIntersect.y = 0.0f;
			yIntersect.z = tParticle[0].pos.z*(1.0f - t) + nPos[0].z*t;
			yNormal = Vector(0.0f, 1.0f, 0.0f);
			yChange = true;
		}else if(nPos[0].y > yMaximum){
			//Scale it so that its at that point.
			t = (yMaximum - tParticle[0].pos.y)/dir.y;
			yIntersect.x = tParticle[0].pos.x*(1.0f - t) + nPos[0].x*t;
			yIntersect.y = yMaximum;
			yIntersect.z = tParticle[0].pos.z*(1.0f - t) + nPos[0].z*t;
			yNormal = Vector(0.0f, -1.0f, 0.0f);
			yChange = true;
		}
		//See if the nPos[0].z is out of bounds.
		if(nPos[0].z < 0.0f){
			//Scale it so that its at that point.
			t = (0.0f - tParticle[0].pos.z)/dir.z;
			zIntersect.x = tParticle[0].pos.x*(1.0f - t) + nPos[0].x*t;
			zIntersect.y = tParticle[0].pos.y*(1.0f - t) + nPos[0].y*t;
			zIntersect.z = 0.0f;
			zNormal = Vector(0.0f, 0.0f, 1.0f);
			zChange = true;
		}else if(nPos[0].z > zMaximum){
			//Scale it so that its at that point.
			t = (zMaximum - tParticle[0].pos.z)/dir.z;
			zIntersect.x = tParticle[0].pos.x*(1.0f - t) + nPos[0].x*t;
			zIntersect.y = tParticle[0].pos.y*(1.0f - t) + nPos[0].y*t;
			zIntersect.z = zMaximum;
			zNormal = Vector(0.0f, 0.0f, -1.0f);
			zChange = true;
		}
		if(xChange || yChange || zChange){
			xClose = (xIntersect - tParticle[0].pos).magnitude();
			yClose = (yIntersect - tParticle[0].pos).magnitude();
			zClose = (zIntersect - tParticle[0].pos).magnitude();
			bool xyClose = (fabs(xClose - yClose) < EPSILON);
			bool yzClose = (fabs(yClose - zClose) < EPSILON);
			bool zxClose = (fabs(zClose - xClose) < EPSILON);
			if(xyClose && yzClose && zxClose){
				bIntersect = (xIntersect + yIntersect + zIntersect) * (1.0f / 3.0f);
				bNormal = (xNormal + yNormal + zNormal).normalize();
			}else{
				if(xyClose){//They're close then merge.
					bIntersect = (xIntersect + yIntersect) * 0.5f;
					bNormal = (xNormal + yNormal).normalize();
					bClose = (xClose + yClose) * 0.5f;
				}else if(xClose < yClose){
					bIntersect = xIntersect; bNormal = xNormal; bClose = xClose;
				}else{
					bIntersect = yIntersect; bNormal = yNormal; bClose = yClose;
				}
				if(fabs(bClose - zClose) < EPSILON){//They're close then merge.
					bIntersect = (bIntersect + zIntersect) * 0.5f;
					bNormal = (bNormal + zNormal).normalize();
				}else if(bClose > zClose){
					bIntersect = zIntersect; bNormal = zNormal;
				}
			}
			//Apply the intersection...
			ATLEAST(bIntersect.x); ATMOST(bIntersect.x, xMaximum);
			ATLEAST(bIntersect.y); ATMOST(bIntersect.y, yMaximum);
			ATLEAST(bIntersect.z); ATMOST(bIntersect.z, zMaximum);
			tParticle[0].pos = bIntersect;
			nPos[0] = bIntersect + (nPos[0] - bIntersect).reflect(bNormal)*(-mu);
			tParticle[0].vel = tParticle[0].vel.reflect(bNormal)*(-mu);
			needCheck[0] = true;
		}
		//Get the closest of them.
		if(needCheck[0] && needCheck[1]){
			float d0 = (tParticle[0].pos - particle.pos).magnitude();
			float d1 = (tParticle[1].pos - particle.pos).magnitude();
			if(d0 < d1){
				particle = tParticle[0]; newPos = nPos[0];
			}else{
				particle = tParticle[1]; newPos = nPos[1];
			}
		}else if(needCheck[0]){
			particle = tParticle[0]; newPos = nPos[0];
		}else if(needCheck[1]){
			particle = tParticle[1]; newPos = nPos[1];
		}else{
			if(nudged){ newPos = nPos[1]; }
		}
	}
	particle.pos = newPos;
}

float minf(float a, float b, float c){
	if(a < b){ return((c < a)?(c):(a)); }
	else{ return((c < b)?(c):(b)); }
}

float maxf(float a, float b, float c){
	if(a > b){ return((c > a)?(c):(a)); }
	else{ return((c > b)?(c):(b)); }
}

BoundingTree::BoundingBox::BoundingBox(const Point& p0, const Point& p1, const Point& p2)
                         : xMin(minf(p0.x, p1.x, p2.x)), yMin(minf(p0.y, p1.y, p2.y)),
                           zMin(minf(p0.z, p1.z, p2.z)), xMax(maxf(p0.x, p1.x, p2.x)),
                           yMax(maxf(p0.y, p1.y, p2.y)), zMax(maxf(p0.z, p1.z, p2.z)),
                           v12(p1 - p0), v13(p2 - p0), norm(((p1 - p0).cross(p2 - p0)).normalize()), pt0(p0) { }

BoundingTree::BoundingBox::BoundingBox(const BoundingBox& l, const BoundingBox& r)
                         : xMin((l.xMin < r.xMin)?(l.xMin):(r.xMin)), yMin((l.yMin < r.yMin)?(l.yMin):(r.yMin)),
                           zMin((l.zMin < r.zMin)?(l.zMin):(r.zMin)), xMax((l.xMax > r.xMax)?(l.xMax):(r.xMax)),
                           yMax((l.yMax > r.yMax)?(l.yMax):(r.yMax)), zMax((l.zMax > r.zMax)?(l.zMax):(r.zMax)) { }

BoundingTree::BoundingBox BoundingTree::BoundingBox::operator+(const BoundingBox& r) const{
	return(BoundingBox(*this, r));
}

float BoundingTree::BoundingBox::Size() const{ return((xMax - xMin)*(yMax - yMin)*(zMax - zMin)); }

bool BoundingTree::BoundingBox::Bound(Particle& particle, Point& newPos, float mu) const{
	//if(CheckIntersection(particle.pos, newPos)){
		Vector direction = newPos - particle.pos;
		//Check if line is perpendicular to the surface.
		float dot = direction.normalize() * norm;
		if(EPSILON > dot && -EPSILON < dot){ return(false); }
		//Setup a linear system and solve.
		Matrix intersect(v12, v13, direction*-1.0f);
		Vector param = intersect.Solve(particle.pos - pt0);
		//Check that the intersection is within the triangle.
		if(-EPSILON > param.x || -EPSILON > param.y || 1.0f+EPSILON < param.x + param.y){ return(false); }
		//Check if the intersection is on the ray (not behind or at starting point).
		float zEpsilon = EPSILON/fabs(dot);//Length of (EPSILON*normal projected along direction.normalize()).
		if(0 > param.z || 1.0f+zEpsilon < param.z){ return(false); }
		//Then found the intersection at!
		Point intersection = particle.pos + direction*param.z;//Equivalently (vertex1+i*param.x+j*param.y)...
		intersection = intersection + direction.normalize()*(-zEpsilon);//Move it back to one EPSILON above.
		if(param.z <= 1.0f){//Check if it was actually an intersection.
			//Adjust the path.
			Vector normal = (dot < 0)?(norm):(norm.negate());
			direction = (newPos - intersection).reflect(normal)*(-mu);
			particle.pos = intersection;
			newPos = intersection + direction;
			particle.vel = particle.vel.reflect(normal)*(-mu);
			return(true);
		}else{
			//Push the endpoint back.
			newPos = intersection;
		}
	//}
	return(false);
}

void BoundingTree::BoundingBox::PushBack(Point& start, Point& end) const{
	//Find the distance to the end point.
}

bool BoundingTree::BoundingBox::CheckIntersection(const Point& start, const Point& end) const{
#define ISINSIDE(x, y, z) (x <= xMax && x >= xMin && y <= yMax && y >= yMin && z <= zMax && z >= zMin)
	if(ISINSIDE(start.x, start.y, start.z) || ISINSIDE(end.x, end.y, end.z)){ return(true); }
#undef ISINSIDE
	//Does the line go through the bounding box?
	float d, t, x, y, z;
	//For checking x planes.
#define CHECK(var) \
	t = (var - start.x)/d; \
	if(0.0f <= t && 1.0f >= t){ \
		y = start.y*(1.0f - t) + end.y*t; \
		z = start.z*(1.0f - t) + end.z*t; \
		if(y >= yMin - EPSILON && y <= yMax + EPSILON && z >= zMin - EPSILON && z <= zMax + EPSILON){ \
			return(true); \
		} \
	}
	d = (end.x - start.x);
	if(d < -EPSILON || d > EPSILON){
		CHECK(xMin)//Check xMin Plane.
		CHECK(xMax)//Check xMax Plane.
	}
#undef CHECK
	//For checking y planes.
#define CHECK(var) \
	t = (var - start.y)/d; \
	if(0.0f <= t && 1.0f >= t){ \
		x = start.x*(1.0f - t) + end.x*t; \
		z = start.z*(1.0f - t) + end.z*t; \
		if(x >= xMin - EPSILON && x <= xMax + EPSILON && z >= zMin - EPSILON && z <= zMax + EPSILON){ \
			return(true); \
		} \
	}
	d = (end.y - start.y);
	if(d < -EPSILON || d > EPSILON){
		CHECK(yMin)//Check yMin Plane.
		CHECK(yMax)//Check yMax Plane.
	}
#undef CHECK
	//For checking z planes.
#define CHECK(var) \
	t = (var - start.z)/d; \
	if(0.0f <= t && 1.0f >= t){ \
		x = start.x*(1.0f - t) + end.x*t; \
		y = start.y*(1.0f - t) + end.y*t; \
		if(x >= xMin - EPSILON && x <= xMax + EPSILON && y >= yMin - EPSILON && y <= yMax + EPSILON){ \
			return(true); \
		} \
	}
	d = (end.z - start.z);
	if(d < -EPSILON || d > EPSILON){
		CHECK(zMin)//Check zMin Plane.
		CHECK(zMax)//Check zMax Plane.
	}
#undef CHECK
	return(false);
}

BoundingTree::BoundingBoxNode::BoundingBoxNode(const BoundingBox* left, const BoundingBox* right)
                             : BoundingBox(*left, *right){
	children[0] = left; children[1] = right;
}

BoundingTree::BoundingBoxNode::~BoundingBoxNode(){
	delete children[0]; delete children[1];
}

bool BoundingTree::BoundingBoxNode::Bound(Particle& particle, Point& newPos, float mu) const{
	bool ret[2] = {false, false};
	if(CheckIntersection(particle.pos, newPos)){
		Particle tmp[2]; tmp[0] = tmp[1] = particle;
		Point nPos[2]; nPos[0] = nPos[1] = newPos;
		int nudged = -1;
		ret[0] = children[0]->Bound(tmp[0], nPos[0], mu);
		if( (!ret[0]) && nPos[0] != newPos){
			nPos[1] = nPos[0];
			nudged = 0;
		}
		ret[1] = children[1]->Bound(tmp[1], nPos[1], mu);
		if( (!ret[1]) && nPos[1] != newPos){ nudged = 1; }
		if(ret[0] && ret[1]){
			float d0 = (tmp[0].pos - particle.pos).magnitude();
			float d1 = (tmp[1].pos - particle.pos).magnitude();
			if(d0 < d1){
				particle = tmp[0]; newPos = nPos[0];
			}else{
				particle = tmp[1]; newPos = nPos[1];
			}
		}else if(ret[0]){
			particle = tmp[0]; newPos = nPos[0];
		}else if(ret[1]){
			particle = tmp[1]; newPos = nPos[1];
		}else{
			if(nudged > -1){ newPos = nPos[nudged]; }
		}
	}
	return(ret[0] || ret[1]);
}
