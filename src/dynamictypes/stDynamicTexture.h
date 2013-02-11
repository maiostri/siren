/**
* @file
*
* This file implements the dynamic texture object
*/
#ifndef __STDYNAMICTEXTURE_H
#define __STDYNAMICTEXTURE_H

#include <artemis/Artemis.hpp>

#include "stDynamicType.h"

typedef double DescrMatrix[4][5];    // [ang][dist]
typedef double RungMatrix[4][5][16]; // [ang][dist][degrau]


/**
* This class implements the Texture object
* Code adapted from Joaquim Felipe's code
*/
class tDynamicTexture : tDynamicType
{

private:
    /**
    * Serialized object
    */
    stByte *Serialized;

public:
    /**
    * Entropy matrix
    */
    DescrMatrix EntropyMatrix;

    /**
    * homogeneity matrix
    */
    DescrMatrix HomogenMatrix;
    /**
    * Uniformity matrix
    */
    DescrMatrix UniformMatrix;

    /**
    * Third order moment or distortion
    */
    DescrMatrix Moment3Matrix;

    /**
    * Variance or contrast matrix
    */
    DescrMatrix VarianceMatrix;

    /**
    * Inverse variance or inverse contrast
    */
    DescrMatrix VarInvMatrix;

    /**
    * degree (gradient) matrix
    */
    RungMatrix RungMatrixx;

    /**
     * Default constructor.
     */
    tDynamicTexture();


    /**
    *  This constructor copies the values of the object pointed by obj
    */
    tDynamicTexture(tDynamicType *o);


    /**
    * Destructor.
    */
    virtual ~tDynamicTexture();


    /**
    * Clone method.
    */
    virtual tDynamicType *Clone();


    /**
    * Determines if this instance is equal to another instance.
    */
    virtual bool IsEqual(tDynamicType * o);

    virtual void * Get(stCount idx);

    virtual stSize GetSize();

    /**
    * Gets the serialized size
    */
    virtual stSize GetSerializedSize();


    /**
    * Serialization
    */
    virtual const stByte * Serialize();



    /**
    * Unserialize
    */
    virtual void Unserialize(const stByte * data, stSize datasize);




    /**
    * Extract the texture from a jpeg file
    */
    //void ExtractTextureFromJpegFile(std::string filename);



    /**
    * Extract the texture from a bitmap
    */
    void ExtractTexture(const Image *image);


};

/**
* This class implements the Texture distance evaluator
* Code adapted from Joaquim Felipe's code
*/
class tDynamicTextureDistanceEvaluator
{
private:

    /**
    * Computes the distance between two DescrMatrix
    */
    double distance(DescrMatrix feature1, DescrMatrix feature2);


    /**
    * Computes the distance between two RungMatrix
    */
    double distance(RungMatrix feature1, RungMatrix feature2);

public:

    /**
    * Gets the distance between two tDynamicTexture objects
    */
    double GetDistance(tDynamicTexture *obj1, tDynamicTexture *obj2);



    /**
    * Gets the square distance between two tDynamicTexture objects
    */
    double GetDistance2(tDynamicTexture *obj1, tDynamicTexture *obj2);


};//end class tDynamicTextureDistanceEvaluator

#endif