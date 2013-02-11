#include "imageUtils.h"

Image* imageUtils::createImageObjectByFilename(string filename)
{
    string extension = boost::filesystem::extension(filename);
    string filenames = filename;
    Image* image;
    if (extension.compare(".bmp") == 0) {
        BMPImage *bmpImage = new BMPImage(filenames);
        image = bmpImage;
    } else if (extension.compare(".jpg") == 0) {
        JPGImage *jpgImage = new JPGImage(filenames);
        //JPGImage *jpgImage = new JPGImage(filename);
        image = jpgImage;
    } else if (extension.compare(".png") == 0) {
        PNGImage *pngImage = new PNGImage(filenames);
        image = pngImage;
    } else {
        image = NULL;
    }
    return image;
}
