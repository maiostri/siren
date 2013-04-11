/* 
 * File:   imageUtils.h
 * Author: ricardo
 * Email:  rmaiostri@grad.icmc.usp.br
 *
 * This header contains definitions on various functions to manipulate images.
 */

#ifndef IMAGEUTILS_H
#define	IMAGEUTILS_H

#include <cstring>
#include <boost/filesystem.hpp>
#include <iostream>
#include <artemis/image/bmp/BmpLib.hpp>
#include <artemis/image/png/PngLib.hpp>
#include <artemis/image/jpg/JpgLib.hpp>

using namespace std;

class imageUtils
{
private:
  imageUtils (imageUtils const& copy);
  imageUtils& operator= (imageUtils const& copy);

  /**
   * Private constructor.
   */
  imageUtils ();


public:

  static imageUtils& getInstance ();

  /**
   * This function switch the possible extensions for the file and
   * instantiate the Artemis Image object.
   * @param filename
   *            the filename.
   * @return 
   *            the artemis object.
   */
  Image* createImageObjectByFilename (string filename);

};

#endif	/* IMAGEUTILS_H */

