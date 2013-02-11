/* 
 * File:   imageUtils.h
 * Author: ricardo
 * Email:  rmaiostri@grad.icmc.usp.br
 *
 * This header contains definitions on various functions to manipulate image.
 */

#ifndef IMAGEUTILS_H
#define	IMAGEUTILS_H

#include <cstring>
#include <boost/filesystem.hpp>
#include <artemis/image/bmp/BmpLib.hpp>
#include <artemis/image/png/PngLib.hpp>
#include <artemis/image/jpg/JpgLib.hpp>


using namespace std;

class imageUtils {
private:
		
		
public:
		/**
		 * This function switch the possible extensions for the file and
		 * instantiate the Artemis Image object.
		 * @param filename
		 * @return 
		 */
		static Image* createImageObjectByFilename(string filename);


};

#endif	/* IMAGEUTILS_H */

