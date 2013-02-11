/**
* @file
*
* This file implements the dynamic Zernike object.
*/
#ifndef __STDYNAMICZERNIKE_H
#define __STDYNAMICZERNIKE_H

#include <math.h>

#include <artemis/Artemis.hpp>
#include "stDynamicType.h"


#define SQR(a) (a*a)

typedef struct
{
    double im, re;
} TNComplex;

std::vector<TNComplex> *ZernikeRealPolynomial(char n, char m);
TNComplex *ZernikeMoments(short n, BMPImage *bitmap, short *moments_count);
void ZernikeFeatures(BMPImage *bitmap, float *zernike);

/**
* This class implements the Zernike object.
* Code adapted from Andre Balan's code
*/
class tDynamicZernike : public tDynamicType
{
private:
    /**
    * Pointer to the data vector
    */
    float * Data;

    /**
    * Size of the data vector
    */
    stSize Size;



public:
    /**
    * Default constructor.
    */
    tDynamicZernike();

    /**
    * Constructor.
    */
    tDynamicZernike(const float * data);

    /**
    * Destructor.
    */
    virtual ~tDynamicZernike();

    /**
    * Clones the object.
    */
    virtual tDynamicType *Clone();

    /**
    * Gets the object serialized size
    */
    virtual stSize GetSerializedSize();

    /**
    * Returns the object serialization
    */
    virtual const stByte * Serialize();

    /**
    * Restores the serialized object.
    */
    virtual void Unserialize(const stByte * data, stSize dataSize);

    /**
    * Tests if one object is equal to another.
    */
    virtual bool IsEqual(tDynamicType * obj);


    /**
    * Extract the zernike from a jpeg file
    */
    // void ExtractZernikeFromJpegFile(std::string filename);

    /**
    * Extract the zernike from a jpeg blob
    */
    /* void ExtractZernikeFromJpegBlob(TBlobField *blobfield) {
         TBlobStream *stream = new TBlobStream(blobfield,bmRead);
         JPGImage *jpg = new JPGImage();
         try {
             jpg->LoadFromStream(stream);
         }
         catch(...) {
         }

         BMPImage *bmp = new BMPImage();
         bmp->Assign(jpg);
         bmp->setHeight(jpg->getHeight());
         bmp->setWidth(jpg->getWidth());

         ExtractZernikeFromBmp(bmp);

         delete bmp;
         delete jpg;
         delete stream;
     } */

    /**
    * Extract the zernike from a bitmap
    */
    //void ExtractZernikeFromBmp(BMPImage *src);

    stSize GetSize()
    {
        return tDynamicZernike::Size;
    }

    /**
    * Returns a value
    */
    float GetData(stSize index)
    {
        if (index < Size)
            return Data[index];
        else
            return -1;
    }


    double zer_pol_R(char n, char m, float  r2);

    void zer_pol(short n, short m, float x, float y, TNComplex *result);

    void zer_mom(BMPImage *bitmap, short n, short m, short xc, short yc, float xscale, float yscale, TNComplex *result);

    TNComplex *ZernikeMoments(short n, BMPImage *bitmap, short *moments_count);

    std::vector<TNComplex> *ZernikeRealPolynomial(char n, char m);

    void ZernikeFeatures(BMPImage *bitmap, float *zernike);

};
#endif
