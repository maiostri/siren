#include "stDynamicTexture.h"

tDynamicTexture::tDynamicTexture()
{
    //int ang, dist,i;
    Serialized = NULL;
    memset(EntropyMatrix,  0, (4*5)    * sizeof(double));
    memset(HomogenMatrix,  0, (4*5)    * sizeof(double));
    memset(UniformMatrix,  0, (4*5)    * sizeof(double));
    memset(Moment3Matrix,  0, (4*5)    * sizeof(double));
    memset(VarianceMatrix, 0, (4*5)    * sizeof(double));
    memset(VarInvMatrix,   0, (4*5)    * sizeof(double));
    memset(RungMatrixx,     0, (4*5*16) * sizeof(double));
}

/**
*  This constructor copies the values of the object pointed by obj
*/
tDynamicTexture::tDynamicTexture(tDynamicType *o)
{
    tDynamicTexture *obj = (tDynamicTexture *)o;
    //int ang, dist,i;
    Serialized = NULL;
    memcpy(EntropyMatrix,  obj->EntropyMatrix,  (4*5)    * sizeof(double));
    memcpy(HomogenMatrix,  obj->HomogenMatrix,  (4*5)    * sizeof(double));
    memcpy(UniformMatrix,  obj->UniformMatrix,  (4*5)    * sizeof(double));
    memcpy(Moment3Matrix,  obj->Moment3Matrix,  (4*5)    * sizeof(double));
    memcpy(VarianceMatrix, obj->VarianceMatrix, (4*5)    * sizeof(double));
    memcpy(VarInvMatrix,   obj->VarInvMatrix,   (4*5)    * sizeof(double));
    memcpy(RungMatrixx,     obj->RungMatrixx,     (4*5*16) * sizeof(double));
}

/**
* Destructor.
*/
tDynamicTexture::~tDynamicTexture()
{
    if (Serialized != NULL)
        {
            delete [] Serialized;
        }
}

tDynamicType *tDynamicTexture::Clone()
{
    tDynamicTexture *clone = new tDynamicTexture(this);
    return (tDynamicType *)clone;
}//end Clone

/**
* Determines if this instance is equal to another instance.
*/
bool tDynamicTexture::IsEqual(tDynamicType * o)
{
    tDynamicTexture *obj = (tDynamicTexture *)o;
    int cmp=0;

    cmp+= memcmp(EntropyMatrix,  obj->EntropyMatrix,  (4*5)    * sizeof(double));
    cmp+= memcmp(HomogenMatrix,  obj->HomogenMatrix,  (4*5)    * sizeof(double));
    cmp+= memcmp(UniformMatrix,  obj->UniformMatrix,  (4*5)    * sizeof(double));
    cmp+= memcmp(Moment3Matrix,  obj->Moment3Matrix,  (4*5)    * sizeof(double));
    cmp+= memcmp(VarianceMatrix, obj->VarianceMatrix, (4*5)    * sizeof(double));
    cmp+= memcmp(VarInvMatrix,   obj->VarInvMatrix,   (4*5)    * sizeof(double));
    cmp+= memcmp(RungMatrixx,     obj->RungMatrixx,     (4*5*16) * sizeof(double));

    return (cmp==0)?true:false;
}

void * tDynamicTexture::Get(stCount idx)
{
    return NULL;

}


stSize tDynamicTexture::GetSize()
{
    return 0;
}




/**
* Gets the serialized size
*/
stSize tDynamicTexture::GetSerializedSize()
{
    return (sizeof(double) * ( (6*(4*5)) + (4*5*16) ) );
}//end GetSerializedSize

/**
* Serialization
*/
const stByte * tDynamicTexture::Serialize()
{
    unsigned long skip;
    unsigned char *start;

    if(Serialized == NULL)
        Serialized = new  unsigned char[GetSerializedSize()];

    skip = (4*5) * sizeof(double);
    start = Serialized;

    memcpy(Serialized, EntropyMatrix,  skip);
    memcpy(Serialized += skip, HomogenMatrix,  skip);
    memcpy(Serialized += skip, UniformMatrix,  skip);
    memcpy(Serialized += skip, Moment3Matrix,  skip);
    memcpy(Serialized += skip, VarianceMatrix, skip);
    memcpy(Serialized += skip, VarInvMatrix,   skip);
    memcpy(Serialized += skip, RungMatrixx,     skip*16);

    Serialized = start;
    return start;
}

/**
* Unserialize
*/
void tDynamicTexture::Unserialize(const stByte * data, stSize datasize)
{
    unsigned long skip;

    skip = (4*5) * sizeof(double);

    memcpy(EntropyMatrix,  data,          skip);
    memcpy(HomogenMatrix,  data += skip,  skip);
    memcpy(UniformMatrix,  data += skip,  skip);
    memcpy(Moment3Matrix,  data += skip,  skip);
    memcpy(VarianceMatrix, data += skip,  skip);
    memcpy(VarInvMatrix,   data += skip,  skip);
    memcpy(RungMatrixx,     data += skip,  skip*16);

    if (Serialized != NULL)
        {
            delete [] Serialized;
            Serialized = NULL;
        }//end if
}


/**
* Extract the texture from a jpeg file
*/
/*void tDynamicTexture::ExtractTextureFromJpegFile(std::string filename) {
    JPGImage *jpg = new JPGImage();
    try {
        jpg->openImage(filename);
    }
    catch(...) {
    }

    BMPImage *bmp = new BMPImage();
    bmp->openImage(filename);
    bmp->setHeight(jpg->getHeight());
    bmp->setWidth(jpg->getWidth());

    ExtractTextureFromBmp(bmp);

    delete bmp;
    delete jpg;
} */

/**
* Extract the texture from a bitmap
*/
void tDynamicTexture::ExtractTexture(const Image *image)
{
    
}

double tDynamicTextureDistanceEvaluator::distance(DescrMatrix feature1, DescrMatrix feature2)
{
    double avgfeature1[4];  //mediaquery
    double avgfeature2[4];  //mediaimage
    double sumquadr, lesser;
    int i,j;

    //initializing
    memset(avgfeature1,0,4*sizeof(double));
    memset(avgfeature2,0,4*sizeof(double));

    //Getting average of descriptor
    for(i=0; i<4; i++)
        {
            for(j=0; j<5; j++)
                {
                    avgfeature1[i] += feature1[i][j]/5.0;
                    avgfeature2[i] += feature2[i][j]/5.0;
                }
        }

    //volta zero
    sumquadr = 0;
    for(i=0; i<4; i++)
        {
            sumquadr += (avgfeature1[i]-avgfeature2[i])*(avgfeature1[i]-avgfeature2[i]);
        }
    lesser = sumquadr;

    //volta um
    sumquadr = 0;
    for(i=1; i<4; i++)
        {
            sumquadr += (avgfeature1[i]-avgfeature2[i-1])*(avgfeature1[i]-avgfeature2[i-1]);
        }
    sumquadr += (avgfeature1[0]-avgfeature2[3])*(avgfeature1[0]-avgfeature2[3]);
    lesser =  sumquadr < lesser ? sumquadr : lesser;

    //volta dois
    sumquadr = 0;
    for(i=2; i<4; i++)
        {
            sumquadr += (avgfeature1[i]-avgfeature2[i-2])*(avgfeature1[i]-avgfeature2[i-2]);
        }
    sumquadr += (avgfeature1[0]-avgfeature2[2])*(avgfeature1[0]-avgfeature2[2]);
    sumquadr += (avgfeature1[1]-avgfeature2[3])*(avgfeature1[1]-avgfeature2[3]);
    lesser =  sumquadr < lesser ? sumquadr : lesser;

    //volta tr�s
    sumquadr = 0;
    for(i=0; i<3; i++)
        {
            sumquadr += (avgfeature1[i]-avgfeature2[i+1])*(avgfeature1[i]-avgfeature2[i+1]);
        }
    sumquadr += (avgfeature1[3]-avgfeature2[0])*(avgfeature1[3]-avgfeature2[0]);
    lesser =  sumquadr < lesser ? sumquadr : lesser;

    // dist�ncia final
    return sqrt(lesser);
}

/**
* Computes the distance between two RungMatrix
*/
double tDynamicTextureDistanceEvaluator::distance(RungMatrix feature1, RungMatrix feature2)
{
    double avgdistfeature1[4][16];
    double avgdistfeature2[4][16];
    int i,j,k;
    double sumquadr;
    double lesser;
    double distpond;

    //initializing
    memset(avgdistfeature1,0,4*16*sizeof(double));
    memset(avgdistfeature2,0,4*16*sizeof(double));

    //vetores para sumarizar gradiente antes de calcular a dist
    for(i=0; i<4; i++)
        {
            for(k=0; k<16; k++)
                {
                    for(j=0; j<5; j++)
                        {
                            avgdistfeature1[i][k] += feature1[i][j][k]/5.0;
                            avgdistfeature2[i][k] += feature2[i][j][k]/5.0;
                        }
                }
        }

    //volta zero
    distpond = 0;
    for(i=0; i<4; i++)
        {
            sumquadr = 0;
            for(k=0; k<16; k++)
                {
                    sumquadr += (k+1)*(k+1)*(avgdistfeature1[i][k]-avgdistfeature2[i][k])*(avgdistfeature1[i][k]-avgdistfeature2[i][k]);
                }
            distpond += sqrt(sumquadr);
        }//end for
    lesser = distpond;

    //volta um
    distpond = 0;
    for(i=1; i<4; i++)
        {
            sumquadr = 0;
            for(k=0; k<16; k++)
                {
                    sumquadr += (k+1)*(k+1)*(avgdistfeature1[i][k]-avgdistfeature2[i-1][k])*(avgdistfeature1[i][k]-avgdistfeature2[i-1][k]);
                }
            distpond += sqrt(sumquadr);
        }
    sumquadr = 0;
    for (k=0; k<16; k++)
        {
            sumquadr += (k+1)*(k+1)*(avgdistfeature1[0][k]-avgdistfeature2[3][k])*(avgdistfeature1[0][k]-avgdistfeature2[3][k]);
        }
    distpond += sqrt(sumquadr);
    lesser =  distpond < lesser ? distpond : lesser;

    //volta 2
    distpond = 0;
    for(i=2; i<4; i++)
        {
            sumquadr = 0;
            for(k=0; k<16; k++)
                {
                    sumquadr += (k+1)*(k+1)*(avgdistfeature1[i][k]-avgdistfeature2[i-2][k])*(avgdistfeature1[i][k]-avgdistfeature2[i-2][k]);
                }
            distpond += sqrt(sumquadr);
        }
    sumquadr = 0;
    for (k=0; k<16; k++)
        {
            sumquadr += (k+1)*(k+1)*(avgdistfeature1[0][k]-avgdistfeature2[2][k])*(avgdistfeature1[0][k]-avgdistfeature2[2][k]);
        }
    distpond += sqrt(sumquadr);
    sumquadr = 0;
    for (k=0; k<16; k++)
        {
            sumquadr += (k+1)*(k+1)*(avgdistfeature1[1][k]-avgdistfeature2[3][k])*(avgdistfeature1[1][k]-avgdistfeature2[3][k]);
        }
    distpond += sqrt(sumquadr);
    lesser =  distpond < lesser ? distpond : lesser;

    //volta 3
    distpond = 0;
    for(i=1; i<4; i++)
        {
            sumquadr = 0;
            for(k=0; k<16; k++)
                {
                    sumquadr += (k+1)*(k+1)*(avgdistfeature1[i][k]-avgdistfeature2[i+1][k])*(avgdistfeature1[i][k]-avgdistfeature2[i+1][k]);
                }
            distpond += sqrt(sumquadr);
        }
    sumquadr = 0;
    for (k=0; k<16; k++)
        {
            sumquadr += (k+1)*(k+1)*(avgdistfeature1[3][k]-avgdistfeature2[0][k])*(avgdistfeature1[3][k]-avgdistfeature2[0][k]);
        }
    distpond += sqrt(sumquadr);
    lesser =  distpond < lesser ? distpond : lesser;

    return  lesser;
}

/**
* Gets the distance between two tDynamicTexture objects
*/
double tDynamicTextureDistanceEvaluator::GetDistance(tDynamicTexture *obj1, tDynamicTexture *obj2)
{
    double dist1,dist2,dist3;

    dist1 = distance(obj1->EntropyMatrix,obj2->EntropyMatrix);
    dist2 = distance(obj1->HomogenMatrix,obj2->HomogenMatrix);
    dist3 = distance(obj1->RungMatrixx,obj2->RungMatrixx);

    return (double)((dist1+dist2+dist3)/3);
}

/**
* Gets the square distance between two tDynamicTexture objects
*/
double tDynamicTextureDistanceEvaluator::GetDistance2(tDynamicTexture *obj1, tDynamicTexture *obj2)
{
    double distance = GetDistance(obj1,obj2);
    return distance * distance;
}



