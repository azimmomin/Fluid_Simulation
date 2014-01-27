#include "Renderer.hpp"

#include <iostream>
using namespace std;

#include <GL/glut.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <omp.h>

#define POINT1 1
#define POINT2 2
#define POINT3 4
#define POINT4 8

Renderer::Renderer(float width, float height, float depth, float division, int subdivision, Grid* gridi)
        : kern(division){
	grid = gridi;
	subdiv = subdivision;
	int xCount = (grid->getXDivisions()+2)*subdiv, xCountP = xCount + 1;
	int yCount = (grid->getYDivisions()+2)*subdiv, yCountP = yCount + 1;
	int zCount = (grid->getZDivisions()+2)*subdiv, zCountP = zCount + 1;
	int length = xCountP * yCountP * zCountP;
	inside = new bool[length];
	corners = new Point[length];
	Point *cur = corners;
	float xStep = width/((float)xCount - 2.0f*subdiv), yStep = height/((float)yCount - 2.0f*subdiv), zStep = depth/((float)zCount - 2.0f*subdiv);
	for(int zIndex = 0;zIndex < zCountP;zIndex++){
		for(int yIndex = 0;yIndex < yCountP;yIndex++){
			for(int xIndex = 0;xIndex < xCountP;xIndex++){
				cur->x = ((float)xIndex - subdiv)*xStep;
				cur->y = ((float)yIndex - subdiv)*yStep;
				cur->z = ((float)zIndex - subdiv)*zStep;
				cur++;
			}
		}
	}
	xEdges = new Point[xCount * yCountP * zCountP];
	yEdges = new Point[xCountP * yCount * zCountP];
	zEdges = new Point[xCountP * yCountP * zCount];
	xyEdges = new Point[xCount * yCount * zCountP];
	yzEdges = new Point[xCountP * yCount * zCount];
	zxEdges = new Point[xCount * yCountP * zCount];
	crossEdges = new Point[xCount * yCount * zCount];
}

Renderer::~Renderer(){
	delete[] inside;
	delete[] corners;
	delete[] xEdges;
	delete[] yEdges;
	delete[] zEdges;
	delete[] xyEdges;
	delete[] yzEdges;
	delete[] zxEdges;
	delete[] crossEdges;
}

void Renderer::SetupRender(){
	//TODO: Implement "Anisotropic Kernels" by starting with:
	//Do a run through all particles.
		//Shift the points using diffusion smoothing (Extra?)
		//Calculate the particle's neighborhood density distribution matrix
	//Go through all buckets and prerender!
	int xCount = (grid->getXDivisions()+2)*subdiv, xCountP = xCount + 1;
	int yCount = (grid->getYDivisions()+2)*subdiv, yCountP = yCount + 1;
	int zCount = (grid->getZDivisions()+2)*subdiv, zCountP = zCount + 1;
	int levelP = xCountP*yCountP;
	int levelXY = xCount*yCount, levelXP = xCount * yCountP;
	int levelPY = xCountP * yCount, levelPP = xCountP * yCountP;
	bool xNotZero = false, yNotZero, zNotZero, curInside;
#define X(pt) (pt ## x)
#define Y(pt) (pt ## y)
#define Z(pt) (pt ## z)
#define INDEXP(pt) (X(pt) + Y(pt) * xCountP + Z(pt) * levelP)

#define XEDGEI(xInd, yInd, zInd) (xEdges + xInd + yInd * xCount + zInd * levelXP)
#define YEDGEI(xInd, yInd, zInd) (yEdges + xInd + yInd * xCountP + zInd * levelPY)
#define ZEDGEI(xInd, yInd, zInd) (zEdges + xInd + yInd * xCountP + zInd * levelPP)

#define XYEDGEI(xInd, yInd, zInd) (xyEdges + xInd + yInd * xCount + zInd * levelXY)
#define YZEDGEI(xInd, yInd, zInd) (yzEdges + xInd + yInd * xCountP + zInd * levelPY)
#define ZXEDGEI(xInd, yInd, zInd) (zxEdges + xInd + yInd * xCount + zInd * levelXP)
#define CEDGEI(xInd, yInd, zInd) (crossEdges + xInd + yInd * xCount + zInd * levelXY)

#define INSIDE(pt) (inside[INDEXP(pt)])
#define CORNER(pt) (corners + INDEXP(pt))

#define XEDGE(pt) XEDGEI(X(pt), Y(pt), Z(pt))
#define YEDGE(pt) YEDGEI(X(pt), Y(pt), Z(pt))
#define ZEDGE(pt) ZEDGEI(X(pt), Y(pt), Z(pt))

#define XYEDGE(pt) XYEDGEI(X(pt), Y(pt), Z(pt))
#define YZEDGE(pt) YZEDGEI(X(pt), Y(pt), Z(pt))
#define ZXEDGE(pt) ZXEDGEI(X(pt), Y(pt), Z(pt))
#define CEDGE(pt) CEDGEI(X(pt), Y(pt), Z(pt))
	int max = xCountP*yCountP*zCountP;
	int xIndex, yIndex, zIndex;
	
	vector<Particle*> neighborhood;
	Particle tmp;
	#pragma omp parallel for private(neighborhood, tmp, xIndex, yIndex, zIndex)
	for(int index = 0;index < max;index++){
		xIndex = index % xCountP;
		zIndex = (index / levelPP);
		yIndex = (index - zIndex * levelPP) / xCountP;

		#define P0x (xIndex)
		#define P0y (yIndex)
		#define P0z (zIndex)
		tmp.pos = *CORNER(P0);
		grid->getParticleSubsection(&tmp, neighborhood);

		INSIDE(P0) = CheckInside(*CORNER(P0), neighborhood);
		#undef P0x
		#undef P0y
		#undef P0z

		neighborhood.clear();
	}
	#pragma omp parallel for private(neighborhood, tmp, xIndex, yIndex, zIndex, xNotZero, yNotZero, zNotZero, curInside)
	for(int index = 0;index < max;index++){
		xIndex = index % xCountP;
		zIndex = (index / levelPP);
		yIndex = (index - zIndex * levelPP) / xCountP;
		if(yIndex < 0 || yIndex >= yCountP){ cout<<"yIndex: "<<yIndex<<"\n"; }
		xNotZero = (0 != xIndex);
		yNotZero = (0 != yIndex);
		zNotZero = (0 != zIndex);
	
		#define P0x (xIndex)
		#define P0y (yIndex)
		#define P0z (zIndex)

		{
			int xInd = xIndex - 1, yInd = yIndex - 1, zInd = zIndex - 1;
			if(xInd < 0){ xInd = 0; }
			if(yInd < 0){ yInd = 0; }
			if(zInd < 0){ zInd = 0; }
			tmp.pos = corners[xInd + yInd * xCountP + zInd * levelP];
		}
		grid->getParticleSubsection(&tmp, neighborhood);

		#define P1x (xIndex - 1)
		#define P1y (yIndex)
		#define P1z (zIndex)
		//Figure out if that corner is in the isosurface.
		curInside = INSIDE(P0);
		//Figure out any edges connected to that corner.
		//Check the x-edge.
		if(xNotZero && curInside != INSIDE(P1)){
			if(curInside){
				*XEDGE(P1) = GetIntersection(*CORNER(P0), *CORNER(P1), neighborhood);
			}else{
				*XEDGE(P1) = GetIntersection(*CORNER(P1), *CORNER(P0), neighborhood);
			}
		}
		#undef P1x
		#undef P1y
		#undef P1z

		#define P1x (xIndex)
		#define P1y (yIndex - 1)
		#define P1z (zIndex)
		//Check the y-edge.
		if(yNotZero && curInside != INSIDE(P1)){
			if(curInside){
				*YEDGE(P1) = GetIntersection(*CORNER(P0), *CORNER(P1), neighborhood);
			}else{
				*YEDGE(P1) = GetIntersection(*CORNER(P1), *CORNER(P0), neighborhood);
			}
		}
		#undef P1x
		#undef P1y
		#undef P1z

		#define P1x (xIndex)
		#define P1y (yIndex)
		#define P1z (zIndex - 1)
		//Check the z-edge.
		if(zNotZero && curInside != INSIDE(P1)){
			if(curInside){
				*ZEDGE(P1) = GetIntersection(*CORNER(P0), *CORNER(P1), neighborhood);
			}else{
				*ZEDGE(P1) = GetIntersection(*CORNER(P1), *CORNER(P0), neighborhood);
			}
		}
		#undef P1x
		#undef P1y
		#undef P1z

		#define P1x (xIndex - 1)
		#define P1y (yIndex - 1)
		#define P1z (zIndex)
		//Check xy-edge.
		if(xNotZero && yNotZero && curInside != INSIDE(P1)){
			if(curInside){
				*XYEDGE(P1) = GetIntersection(*CORNER(P0), *CORNER(P1), neighborhood);
			}else{
				*XYEDGE(P1) = GetIntersection(*CORNER(P1), *CORNER(P0), neighborhood);
			}
		}
		#undef P1x
		#undef P1y
		#undef P1z

		#define P1x (xIndex)
		#define P1y (yIndex - 1)
		#define P1z (zIndex - 1)
		//Check yz-edge.
		if(yNotZero && zNotZero && curInside != INSIDE(P1)){
			if(curInside){
				*YZEDGE(P1) = GetIntersection(*CORNER(P0), *CORNER(P1), neighborhood);
			}else{
				*YZEDGE(P1) = GetIntersection(*CORNER(P1), *CORNER(P0), neighborhood);
			}
		}
		#undef P1x
		#undef P1y
		#undef P1z

		#define P1x (xIndex - 1)
		#define P1y (yIndex)
		#define P1z (zIndex - 1)
		//Check xz-edge.
		if(xNotZero && zNotZero && curInside != INSIDE(P1)){
			if(curInside){
				*ZXEDGE(P1) = GetIntersection(*CORNER(P0), *CORNER(P1), neighborhood);
			}else{
				*ZXEDGE(P1) = GetIntersection(*CORNER(P1), *CORNER(P0), neighborhood);
			}
		}
		#undef P1x
		#undef P1y
		#undef P1z

		#define P1x (xIndex - 1)
		#define P1y (yIndex - 1)
		#define P1z (zIndex - 1)
		//Check the cross-edge.
		if(xNotZero && yNotZero && zNotZero && curInside != INSIDE(P1)){
			if(curInside){
				*CEDGE(P1) = GetIntersection(*CORNER(P0), *CORNER(P1), neighborhood);
			}else{
				*CEDGE(P1) = GetIntersection(*CORNER(P1), *CORNER(P0), neighborhood);
			}
		}
		#undef P1x
		#undef P1y
		#undef P1z

		#undef P0x
		#undef P0y
		#undef P0z

		neighborhood.clear();
	}
}

void Renderer::Render(){
	//Go through all buckets and render!
	int xCount = (grid->getXDivisions()+2)*subdiv, xCountP = xCount + 1;
	int yCount = (grid->getYDivisions()+2)*subdiv, yCountP = yCount + 1;
	int zCount = (grid->getZDivisions()+2)*subdiv, zCountP = zCount + 1;
	int levelP = xCountP*yCountP;
	int levelXY = xCount*yCount, levelXP = xCount * yCountP;
	int levelPY = xCountP * yCount, levelPP = xCountP * yCountP;
	for(int xIndex = 1;xIndex < xCountP;xIndex++){
		for(int yIndex = 1;yIndex < yCountP;yIndex++){
			for(int zIndex = 1;zIndex < zCountP;zIndex++){
				#define P0x (xIndex)
				#define P0y (yIndex)
				#define P0z (zIndex)

				#define P1x (xIndex - 1)
				#define P1y (yIndex - 1)
				#define P1z (zIndex - 1)
				cTetra.p1 = CORNER(P1);
				#define P2x (xIndex)
				#define P2y (yIndex)
				#define P2z (zIndex)
				cTetra.p2 = CORNER(P2);
				#define P3x (xIndex)
				#define P3y (yIndex - 1)
				#define P3z (zIndex)
				cTetra.p3 = CORNER(P3);
				#define P4x (xIndex - 1)
				#define P4y (yIndex - 1)
				#define P4z (zIndex)
				cTetra.p4 = CORNER(P4);
				cTetra.i12 = CEDGE(P1);
				cTetra.i13 = ZXEDGE(P1);
				cTetra.i14 = ZEDGE(P1);
				cTetra.i23 = YEDGE(P3);
				cTetra.i34 = XEDGE(P4);
				cTetra.i24 = XYEDGE(P4);
				cState = 0;
				if(INSIDE(P1)){ cState |= POINT1; }
				if(INSIDE(P2)){ cState |= POINT2; }
				if(INSIDE(P3)){ cState |= POINT3; }
				if(INSIDE(P4)){ cState |= POINT4; }
				DrawTetrahedron();
				#undef P3x
				#undef P3y
				#undef P3z

				#define P3x (xIndex - 1)
				#define P3y (yIndex)
				#define P3z (zIndex)
				cTetra.p3 = CORNER(P3);
				cTetra.i13 = YZEDGE(P1);
				cTetra.i23 = XEDGE(P3);
				cTetra.i34 = YEDGE(P4);
				if(INSIDE(P3)){ cState |= POINT3; }
				else{ cState &= ~POINT3; }
				DrawTetrahedron();
				#undef P4x
				#undef P4y
				#undef P4z

				#define P4x (xIndex - 1)
				#define P4y (yIndex)
				#define P4z (zIndex - 1)
				cTetra.p4 = CORNER(P4);
				cTetra.i14 = YEDGE(P1);
				cTetra.i34 = ZEDGE(P4);
				cTetra.i24 = ZXEDGE(P4);
				if(INSIDE(P4)){ cState |= POINT4; }
				else{ cState &= ~POINT4; }
				DrawTetrahedron();
				#undef P3x
				#undef P3y
				#undef P3z

				#define P3x (xIndex)
				#define P3y (yIndex)
				#define P3z (zIndex - 1)
				cTetra.p3 = CORNER(P3);
				cTetra.i13 = XYEDGE(P1);
				cTetra.i23 = ZEDGE(P3);
				cTetra.i34 = XEDGE(P4);
				if(INSIDE(P3)){ cState |= POINT3; }
				else{ cState &= ~POINT3; }
				DrawTetrahedron();
				#undef P4x
				#undef P4y
				#undef P4z

				#define P4x (xIndex)
				#define P4y (yIndex - 1)
				#define P4z (zIndex - 1)
				cTetra.p4 = CORNER(P4);
				cTetra.i14 = XEDGE(P1);
				cTetra.i34 = YEDGE(P4);
				cTetra.i24 = YZEDGE(P4);
				if(INSIDE(P4)){ cState |= POINT4; }
				else{ cState &= ~POINT4; }
				DrawTetrahedron();
				#undef P3x
				#undef P3y
				#undef P3z

				#define P3x (xIndex)
				#define P3y (yIndex - 1)
				#define P3z (zIndex)
				cTetra.p3 = CORNER(P3);
				cTetra.i13 = ZXEDGE(P1);
				cTetra.i23 = YEDGE(P3);
				cTetra.i34 = ZEDGE(P4);
				if(INSIDE(P3)){ cState |= POINT3; }
				else{ cState &= ~POINT3; }
				DrawTetrahedron();
				#undef P0x
				#undef P0y
				#undef P0z

				#undef P1x
				#undef P1y
				#undef P1z

				#undef P2x
				#undef P2y
				#undef P2z

				#undef P3x
				#undef P3y
				#undef P3z

				#undef P4x
				#undef P4y
				#undef P4z
			}
		}
	}
#undef X
#undef Y
#undef Z
#undef INDEXP
#undef INSIDE
#undef CORNER
#undef XEDGE
#undef YEDGE
#undef ZEDGE
#undef XYEDGE
#undef YZEDGE
#undef ZXEDGE
#undef CEDGE
}

void Renderer::DrawTetrahedron() {
	glBegin(GL_TRIANGLES);
	//glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 0.0f, 0.0f);
#define VERTEX(ptr) SetNormal(*(cTetra.ptr)); glVertex3f(cTetra.ptr->x, cTetra.ptr->y, cTetra.ptr->z)
		switch(cState){
		case 0: case 15: break;//All or None.
		case 1: case 14://All but P1
			VERTEX(i12); VERTEX(i13); VERTEX(i14);
			break;
		case 2: case 13: //All but P2
			VERTEX(i12); VERTEX(i23); VERTEX(i24);
			break;
		case 4: case 11: //All but P3
			VERTEX(i23); VERTEX(i13); VERTEX(i34);
			break;
		case 8: case 7: //All but P4
			VERTEX(i14); VERTEX(i24); VERTEX(i34);
			break;
		case 3: case 12: //P1 & P2 vs P3 & P4
			VERTEX(i14); VERTEX(i13); VERTEX(i23);
			VERTEX(i24); VERTEX(i14); VERTEX(i23);
			break;
		case 6: case 9: //P2 & P3 vs P1 & P4
			VERTEX(i13); VERTEX(i12); VERTEX(i24);
			VERTEX(i34); VERTEX(i13); VERTEX(i24);
			break;
		case 5: case 10: //P1 & P3 vs P2 & P4
			VERTEX(i14); VERTEX(i12); VERTEX(i23);
			VERTEX(i34); VERTEX(i14); VERTEX(i23);
			break;
		}
	glEnd();
#undef VERTEX
/*#define VERTEX(ptr) glVertex3f(cTetra.ptr->x, cTetra.ptr->y, cTetra.ptr->z)
	glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f, 1.0f);
		switch(cState){
		case 0: case 15: break;//All or None.
		case 1: case 14://All but P1
			VERTEX(i12); VERTEX(i13); VERTEX(i14);
			break;
		case 2: case 13: //All but P2
			VERTEX(i12); VERTEX(i23); VERTEX(i24);
			break;
		case 4: case 11: //All but P3
			VERTEX(i23); VERTEX(i13); VERTEX(i34);
			break;
		case 8: case 7: //All but P4
			VERTEX(i14); VERTEX(i24); VERTEX(i34);
			break;
		case 3: case 12: //P1 & P2 vs P3 & P4
			VERTEX(i14); VERTEX(i13); VERTEX(i23);
			VERTEX(i24); VERTEX(i14); VERTEX(i23);
			break;
		case 6: case 9: //P2 & P3 vs P1 & P4
			VERTEX(i13); VERTEX(i12); VERTEX(i24);
			VERTEX(i34); VERTEX(i13); VERTEX(i24);
			break;
		case 5: case 10: //P1 & P3 vs P2 & P4
			VERTEX(i14); VERTEX(i12); VERTEX(i23);
			VERTEX(i34); VERTEX(i14); VERTEX(i23);
			break;
		}
	glEnd();
#undef VERTEX
/*#define VERTEX(ptr) DrawNormal(*(cTetra.ptr))
	glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);
		switch(cState){
		case 0: case 15: break;//All or None.
		case 1: case 14://All but P1
			VERTEX(i12); VERTEX(i13); VERTEX(i14);
			break;
		case 2: case 13: //All but P2
			VERTEX(i12); VERTEX(i23); VERTEX(i24);
			break;
		case 4: case 11: //All but P3
			VERTEX(i23); VERTEX(i13); VERTEX(i34);
			break;
		case 8: case 7: //All but P4
			VERTEX(i14); VERTEX(i24); VERTEX(i34);
			break;
		case 3: case 12: //P1 & P2 vs P3 & P4
			VERTEX(i14); VERTEX(i13); VERTEX(i23);
			VERTEX(i24); VERTEX(i14); VERTEX(i23);
			break;
		case 6: case 9: //P2 & P3 vs P1 & P4
			VERTEX(i13); VERTEX(i12); VERTEX(i24);
			VERTEX(i34); VERTEX(i13); VERTEX(i24);
			break;
		case 5: case 10: //P1 & P3 vs P2 & P4
			VERTEX(i14); VERTEX(i12); VERTEX(i23);
			VERTEX(i34); VERTEX(i14); VERTEX(i23);
			break;
		}
	glEnd();
#undef VERTEX*/
}
/*
void Renderer::SetupIsovalueNeighborhood(const Point& near){
	tParticle.pos = near;
	neighborhood.clear();
	grid->getParticleSubsection(&tParticle, neighborhood);
}
*/
float Renderer::GetIsovalue(const Point& pt, const vector<Particle*>& neighborhood){
	vector<Particle*>::const_iterator iter = neighborhood.begin();
	float sum = 0.0f;
	while(iter != neighborhood.end()){
		sum += ((*iter)->mass) / ((*iter)->density) * kern((*iter)->pos - pt);
		iter++;
	}
	return(sum);
}

Vector Renderer::GetIsovalueGradient(const Point& pt, const vector<Particle*>& neighborhood){
	vector<Particle*>::const_iterator iter = neighborhood.begin();
	Vector sum;
	while(iter != neighborhood.end()){
		sum = sum + kern.gradient((*iter)->pos - pt) * ( ((*iter)->mass) / ((*iter)->density) );
		iter++;
	}
	return(sum);
}

bool Renderer::CheckInside(const Point& pt, const vector<Particle*>& neighborhood){
	return(GetIsovalue(pt, neighborhood) >= 0.5f);
}

#define ITERATION_MAX 10
#define EPSILON 0.000001f

Point Renderer::GetIntersection(Point inside, Point outside, vector<Particle*>& neighborhood){
	Point guess = (inside + outside) * 0.5f;
	int iteration = 0;
	float value = GetIsovalue(guess, neighborhood);
	while( (fabs(value - 0.5f) > EPSILON) && iteration < ITERATION_MAX){
		if(value < 0.5f){//On outside.
			outside = guess;
		}else{
			inside = guess;
		}
		guess = (inside + outside) * 0.5f;
		value = GetIsovalue(guess, neighborhood);
		iteration++;
	}
	return(guess);
}

void Renderer::SetNormal(const Point& pt){
	Particle tmp; tmp.pos = pt;
	vector<Particle*> neighborhood;
	grid->getParticleSubsection(&tmp, neighborhood);
	Vector n = GetIsovalueGradient(pt, neighborhood).normalize();
	glNormal3f(n.x, n.y, n.z);
}

void Renderer::DrawNormal(const Point& pt, float len){
	Particle tmp; tmp.pos = pt;
	vector<Particle*> neighborhood;
	grid->getParticleSubsection(&tmp, neighborhood);
	Vector n = GetIsovalueGradient(pt, neighborhood).normalize() * len;
	glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(pt.x, pt.y, pt.z);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(pt.x + n.x, pt.y + n.y, pt.z + n.z);
};

