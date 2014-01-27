#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include "grid.h"
#include "kernel.h"

//TODO: Add in "Anisotropic Kernels"

class Renderer{
public:
	/**Sets up the renderer for rendering.
	 * @param width The width of the region bounding the particles.
	 * @param height The height of the region bounding the particles.
	 * @param depth The depth of the region bounding the particles.
	 * @param division The kernel function's non-zero region size.
	 * @param subdivision The number of times each bucket will be split along each axis. (A bucket will become subdivision^3 cubes.)
	 */
	Renderer(float width, float height, float depth, float division, int subdivision, Grid* grid);
	~Renderer();

	/**Prepares to render the particles. */
	void SetupRender();
	/**Renders the particles. SetupRenderer() must be called first. */
	void Render();
private:
	struct Tetrahedron{
		Point *p1, *p2, *p3, *p4;
		Point *i12, *i13, *i14;
		Point *i23, *i34, *i24;
	} cTetra;
	int cState;
	void DrawTetrahedron();

	Kernel kern;
	/**Gets the isovalue at the given point. SetupIsovalueNeighborhood() must be called first.
	 * @param pt The point to check.
	 * @return The isosurface value at the point.
	 */
	float GetIsovalue(const Point& pt, const vector<Particle*>& neighborhood);
	/**Gets the isovalue gradient at the given point. SetupIsovalueNeighborhood() must be called first.
	 * @param pt The point to check.
	 * @return The gradient of the isosurface value at the point.
	 */
	Vector GetIsovalueGradient(const Point& pt, const vector<Particle*>& neighborhood);

	/**Checks if the given point is within the isosurface.
	 * @param pt The point to check.
	 * @return If the point is in the isosurface.
	 */
	bool CheckInside(const Point& pt, const vector<Particle*>& neighborhood);
	/**Gets a point on the isosurface between two points.
	 * @note Of the given points (p0 and p1), one must be inside the surface, and one must be outside.
	 * @note The points between p0 and p1 must be within the same bucket.
	 * @param inside A point inside the surface.
	 * @param outside A point outside the surface.
	 * @return The point on the isosurface between the given points.
	 */
	Point GetIntersection(Point inside, Point outside, vector<Particle*>& neighborhood);
	/**Sets the OpenGL normal to the one for the given point.
	 * @param pt The point on the isosurface to set the normal of.
	 */
	void SetNormal(const Point& pt);

	void DrawNormal(const Point& pt, float len = 0.05);
	float xMax, yMax, zMax;//The bounds for particle positions.
	int subdiv;//The number of times each bucket will be split along each axis.
	bool* inside;//Array that tells if grid corners are inside isosurface.
	Point* corners;//Array of the corner at (x, y, z) in global space.
	Point* xEdges;//Array of edges from (x, y, z) to (x+1, y, z).
	Point* yEdges;//Array of edges from (x, y, z) to (x, y+1, z).
	Point* zEdges;//Array of edges from (x, y, z) to (x, y, z+1).
	Point* xyEdges;//Array of edges from (x, y, z) to (x+1, y+1, z).
	Point* yzEdges;//Array of edges from (x, y, z) to (x, y+1, z+1).
	Point* zxEdges;//Array of edges from (x, y, z) to (x+1, y, z+1).
	Point* crossEdges;//Array of edges from (x, y, z) to (x+1, y+1, z+1).
	Grid* grid;
};

#endif//RENDERER_HPP
