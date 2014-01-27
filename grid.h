#ifndef GRID_H
#define GRID_H

#include "particle.h"
#include "point.h"
#include <vector>

class Grid {
public:
	/**Constructor.
	 * @param width The width of the region the particles will be in.
	 * @param height The height of the region the particles will be in.
	 * @param depth The depth of the region the particles will be in.
	 * @param div The width/height/depth of each bucket.
	 */
	Grid(float width, float height, float depth, float div);
	~Grid();

	/**Checks if the given point is within the grid.
	 * @param p The point to check.
	 */
	bool canContain(const Point& p);

	/**Adds a particle to the grid.
	 * @note All particles must be within region between origin and point (width, height, depth).
	 * @param p The particle to add.
	 */
	void addParticle(Particle* p);
	/**Returns and iterator that goes over all particles within a bucket distance of the given particle.
	 * @param origin The particle to be iterated around.
	 * @param output The vector to store the particles in.
	 */
	void getParticleSubsection(const Particle* origin, vector<Particle*>& output) const;

	/**Clears out all particles, but leaves parameters the same. */
	void clear();

	/**Gets the number of buckets along x-axis.
	 * @return The number of buckets along x-axis.
	 */
	int getXDivisions() const;
	/**Gets the number of buckets along y-axis.
	 * @return The number of buckets along y-axis.
	 */
	int getYDivisions() const;
	/**Gets the number of buckets along z-axis.
	 * @return The number of buckets along z-axis.
	 */
	int getZDivisions() const;
	/**Takes the index of the corner of a bucket and makes it into a point in global space.
	 * @param xIndex The x-index of the bucket's corner.
	 * @param yIndex The y-index of the bucket's corner.
	 * @param zIndex The z-index of the bucket's corner.
	 * @return The corner's point in global space.
	 */
	Point getIndexedPosition(int xIndex, int yIndex, int zIndex) const;
	/** Returns the width of this grid.
	 *  @return Width.
	*/
	float getW();
	/** Returns the height of this grid.
	 *  @return Height.
	 */
	float getH();
	/** Returns the depth of this gird.
	 *  @return Depth.
	 */
	float getD();
	/** Setup the grid with particles
	 *  @param particles The particles to setup with.
	 */
	void setup(vector<Particle>& particles);
private:
	const float w, h, d,;//Size of the region.
	const float divisions;//The width/height/depth of each bucket.
	const int hdivisions, wdivisions, ddivisions;//The number of buckets along each axis.
	vector<Particle*> **buckets;
};

#endif
