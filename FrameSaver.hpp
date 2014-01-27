#ifndef FRAMESAVER_HPP
#define FRAMESAVER_HPP

#include <string>
#include <FreeImagePlus.h>

class FrameSaver{
public:
	/**The constructor.
	 * @param width The width of the screen.
	 * @param height the height of the screen.
	 * @param dir The directory (without slashes) to put the output images in.
	 */
	FrameSaver(unsigned int width, unsigned int height, const char* dir);
	/**The destructor. */
	~FrameSaver();

	/**Saves the currently rendered frame.
	 * @return If the save was successful.
	 */
	bool SaveFrame();
private:
	const unsigned int width, height;
	unsigned char* image;
	fipImage img;

	int frame;
	char* directory;
	/**Creates a path to the output file.
	 * @return A path to the output file.
	 */
	std::string GetPath() const;
};

#endif//FRAMESAVER_HPP
