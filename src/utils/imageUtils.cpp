#include "imageUtils.h"

imageUtils::imageUtils()
{

}

imageUtils& imageUtils::getInstance()
{
    static imageUtils instance;
    return instance;
}

Image* imageUtils::createImageObjectByFilename(string filename)
{
    string extension = boost::filesystem::extension(filename);
    Image* image;
    if (extension.compare(".bmp") == 0)
    {
        BMPImage *bmpImage = new BMPImage(filename);
        image = bmpImage;
    }
    else if (extension.compare(".jpg") == 0)
    {

        JPGImage *jpgImage = new JPGImage(filename);
        image = jpgImage;
    }
    else if (extension.compare(".png") == 0)
    {
        PNGImage *pngImage = new PNGImage(filename);
        image = pngImage;
    }
    else
    {
        image = NULL;
    }
    return image;
}

