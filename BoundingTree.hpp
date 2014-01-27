#ifndef BOUNDINGTREE_HPP
#define BOUNDINGTREE_HPP

#include <vector>

using namespace std;

#include "object.h"
#include "particle.h"

class BoundingTree{
public:
	/**Sets up a bounding box tree.
	 * @note There is an implicit boundary box around everything that will reflect all trajectories back inward.
	 * @param width The width of the implicit boundary.
	 * @param height The height of the implicit boundary.
	 * @param depth The depth of the implicit boundary.
	 * @param A vector of Objects, whos surfaces will serve as boundaries.
	 */
	BoundingTree(float width, float height, float depth, const vector<Object>& obj);
	/**The destructor. */
	~BoundingTree();

	/**Bounds a particle's path.
	 * @param particle The particle with prior position, and a current velocity.
	 * @param newPos The particle's next position.
	 * @param mu The percent of distance and velocity preserved after a collision.
	 */
	void Bound(Particle& particle, Point& newPos, float mu) const;
private:
	float xMaximum, yMaximum, zMaximum;

	//Bounding Box (for a triangle).
	class BoundingBox{
	public:
		BoundingBox(const Point& p0, const Point& p1, const Point& p2);

		/**Merges with another bounding box.
		 * @param right The bouding box to merge with.
		 * @return The merged bounding box.
		 */
		BoundingBox operator+(const BoundingBox& right) const;
		/**Gets the bounding box's volume.
		 * @return The bounding box's volume.
		 */
		float Size() const;
		/**Bounds the particle's path with the triangle if necessary.
		 * @param particle The particle with prior position, and a current velocity.
		 * @param newPos The particle's next position.
		 * @param mu The percent of distance and velocity preserved after a collision.
		 * @return If a path reflection occured.
		 */
		virtual bool Bound(Particle& particle, Point& newPos, float mu) const;
	protected:
		/**Creates a bounding box that contains two others. */
		BoundingBox(const BoundingBox& l, const BoundingBox& r);

		/**Keeps a given end point at least EPSILON above the triangle.
		 * @param start The particle's current position.
		 * @param end The particle's future position.
		 */
		void PushBack(Point& start, Point& end) const;

		/**Checks if a path interacts with this bounding box.
		 * @param start The particle's current position.
		 * @param end The particle's future position.
		 * @return If this bounding box interacts with the given path.
		 */
		bool CheckIntersection(const Point& start, const Point& end) const;
	private:
		const float xMin, yMin, zMin;
		const float xMax, yMax, zMax;

		const Vector v12, v13, norm;//v12 = v2 - v1, v13 = v3 - v1, norm = normalize(v12 cross v13)
		const Point pt0;//pt0 = v1
	};

	class BoundingBoxNode : public BoundingBox{
	public:
		BoundingBoxNode(const BoundingBox* left, const BoundingBox* right);
		~BoundingBoxNode();

		/**Bounds the particle's path using its children if necessary.
		 * @param particle The particle with prior position, and a current velocity.
		 * @param newPos The particle's next position.
		 * @param mu The percent of distance and velocity preserved after a collision.
		 * @return If a path reflection occured.
		 */
		bool Bound(Particle& particle, Point& newPos, float mu) const;
	private:
		const BoundingBox* children[2];
	};

	BoundingBox* root;
};

#endif//BOUNDINGTREE_HPP
