#include "FrameSaver.hpp"

#include <GL/glut.h>

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

#include <sys/stat.h>
#include <cstring>

FrameSaver::FrameSaver(unsigned int widthi, unsigned int heighti, const char* dir)
          : width(widthi), height(heighti), img(FIT_BITMAP, widthi, heighti, 24){
	//Make the directory.
	mkdir(dir, 0777);
	//Save the prefix.
	unsigned int lenp = strlen(dir) + 1;
	directory = new char[lenp];
	memcpy(directory, dir, lenp);
	//Setup image data buffer.
	image = new unsigned char[width*height*3];
	//Defaults
	frame = 0;
}

FrameSaver::~FrameSaver(){
	delete[] directory;
	delete[] image;
}

bool FrameSaver::SaveFrame(){
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
	unsigned char *cur;
	RGBQUAD px;
	for(unsigned int y = 0;y < height;y++){
		for(unsigned int x = 0;x < width;x++){
			cur = image + (x + y * width) * 3;
			px.rgbRed = *cur;
			px.rgbGreen = *(cur+1);
			px.rgbBlue = *(cur+2);
			img.setPixelColor(x, y, &px);
		}
	}
	img.save(GetPath().c_str(), PNG_Z_NO_COMPRESSION);
	frame++;
	return(true);
}

#include <stdio.h>
/*
bool FrameSaver::SavePNG() const{
	//Open the file.
	const char* fName = GetPath().c_str();
	FILE* file = fopen(fName, "w");
	if(!file){ cerr<<"ERROR: Couldn't open output file \""<<fName<<"\".\n"; return(false); }
	//Prepare for writing to PNG file.
	png_structp pStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!pStruct){ cerr<<"ERROR: Couldn't create png_struct.\n"; return(false); }
	png_infop pInfo = png_create_info_struct(pStruct);
	if(!pInfo){
		png_destroy_write_struct(&pStruct, NULL);
		fclose(file);
		cerr<<"ERROR: Couldn't create png_info.\n";
		return(false);
	}
	if(setjmp(png_jmpbuf(pStruct))){
		png_destroy_write_struct(&pStruct, &pInfo);
		fclose(file);
		cerr<<"ERROR: Coulnd't init_io.\n";
		return(false);
	}
	png_init_io(pStruct, file);
	//Write the header.
	if(setjmp(png_jmpbuf(pStruct))){
		png_destroy_write_struct(&pStruct, &pInfo);
		fclose(file);
		cerr<<"ERROR: Coulnd't write file header.\n";
		return(false);
	}
	png_set_IHDR(pStruct, pInfo, width, height, 8, PNG_COLOR_TYPE_RGB,
	             PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
	png_set_sBIT(pStruct, pInfo, );
	png_write_info(pStruct, pInfo);
	//Write the data.
	if(setjmp(png_jmpbuf(pStruct))){
		png_destroy_write_struct(&pStruct, &pInfo);
		fclose(file);
		cerr<<"ERROR: Coulnd't write image data.\n";
		return(false);
	}
	unsigned char *cur = image + width*height*3;
	while(image <= cur){
		png_write_row(pStruct, cur);
		cur -= width*3;
	}
	//Finish.
	if(setjmp(png_jmpbuf(pStruct))){
		png_destroy_write_struct(&pStruct, &pInfo);
		fclose(file);
		cerr<<"ERROR: Coulnd't finish writing.\n";
		return(false);
	}
	png_write_end(pStruct, pInfo);
	png_destroy_write_struct(&pStruct, &pInfo);
	fclose(file);
	return(true);
}*/

string FrameSaver::GetPath() const{
	stringstream ss;
	ss<<directory<<"/img"<<setfill('0')<<setw(3)<<frame<<".png";
	return(ss.str());
}
