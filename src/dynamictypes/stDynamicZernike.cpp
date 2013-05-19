#include "stDynamicZernike.h"

tDynamicZernike::tDynamicZernike()
{
    Size = 256;
    Data = new float[Size];
}

/**
 * Constructor.
 */
tDynamicZernike::tDynamicZernike(const float * data)
{
    Size = 256;
    Data = new float[Size];
    for (stSize i = 0; i < Size; i++)
        Data[i] = data[i];
}

/**
 * Destructor.
 */
tDynamicZernike::~tDynamicZernike()
{
    delete Data;
}

/**
 * Clones the object.
 */
tDynamicZernike *tDynamicZernike::Clone()
{
    tDynamicZernike *clone = new tDynamicZernike(this->Data);
    return (tDynamicZernike *) clone;
}

/**
 * Gets the object serialized size
 */
stSize tDynamicZernike::GetSerializedSize()
{
    return sizeof (float) * Size;
}

/**
 * Returns the object serialization
 */
const stByte * tDynamicZernike::Serialize()
{
    return (unsigned char *) Data;
}

/**
 * Restores the serialized object.
 */
void tDynamicZernike::Unserialize(const stByte * data, stSize dataSize)
{
    // Resize data
    Size = dataSize / sizeof (float);
    if (Data != NULL)
        delete Data;
    Data = new float[Size];
    memcpy(Data, data, dataSize);
}

/**
 * Tests if one object is equal to another.
 */
bool tDynamicZernike::IsEqual(tDynamicType * obj)
{
    bool equal;
    bool stop;
    stSize i;

    if (Size != ((tDynamicZernike *) obj)->Size) return false;

    equal = true;
    i = 0;
    stop = (i >= Size);
    while (!stop)
    {
        if (Data[i] != ((tDynamicZernike *) obj)->Data[i])
        {
            stop = true;
            equal = false;
        }
        else
        {
            i++;
            stop = (i >= Size);
        }//end if
    }//end while
    return equal;
}

double tDynamicZernike::zer_pol_R(char n, char m, float r2)
{

    char i, s, sign, /*exp,*/ lim;
    double a, // (n-s)!
            b, //   s!
            c, // [(n+|m|)/2-s]!
            d; // [(n-|m|)/2-s]!

    double result;
    float ep;

    m = abs(m);

    sign = a = b = c = d = 1;
    lim = (n - m) >> 1;

    // Set initial values for s=0
    for (i = n; i > 1; i--)
        a *= i; // a = n!  &   b = 0! = 1
    for (i = (n + m) >> 1; i > 1; i--)
        c *= i; // c = [(n+|m|)/2]!
    for (i = lim; i > 1; i--)
        d *= i; // d = [(n-|m|)/2-s]!

    result = ((float) a / (float) (b * c * d)) * (!n ? 1 : pow(r2, (float) n / 2.0));

    for (s = 1; s <= lim; s++)
    {

        sign *= -1;
        a /= n - (s - 1);
        b *= s;
        c /= ((n + m) >> 1) - (s - 1);
        d /= ((n - m) >> 1) - (s - 1);

        ep = ((float) n / 2.0) - s;
        // ep = n - 2*s;   // original
        result += (float) sign * ((float) a / (float) (b * c * d)) * (!ep ? 1 : pow(r2, ep));
    }

    return result;
}

inline void tDynamicZernike::zer_pol(short n, short m, float x, float y, TNComplex *result)
{

    double R, arg;
    float r2;

    r2 = SQR(x) + SQR(y);

    if (!r2)
    {
        result->re = 0;
        result->im = 0;
        return;
    }

    R = zer_pol_R(n, m, r2);
    //if (R>1) ShowMessage(IntToStr(n) + " : " + IntToStr(m) + " : " + FloatToStr(R));
    arg = m * atan2(y, x);
    result->re = R * cos(arg);
    result->im = R * sin(arg);
}

inline void tDynamicZernike::zer_mom(BMPImage *bitmap, short n, short m, short xc, short yc, float xscale, float yscale, TNComplex *result)
{

    int h = 0;
    int w = 0;
    short l, c;
    float x, y;
    TNComplex zerp;
    //unsigned char *p;
    double z;

    result->re = 0;
    result->im = 0;
    Pixel pixel;
    h = bitmap->getHeight();
    w = bitmap->getWidth();

    unsigned char d;
    for (l = 0; l < h; l++)
    {

        for (c = 0; c < w; c++)
        {
            y = (float) (l - yc) / yscale;
            x = (float) (c - xc) / xscale;

            pixel = bitmap->getPixel(l, c);
            d += pixel.getRedPixelValue();
            d += pixel.getRedPixelValue();
            d += pixel.getRedPixelValue();

            if ((SQR(x) + SQR(y)) <= 1)
            {
                zer_pol(n, m, x, y, &zerp);
                result->re += d * zerp.re;
                result->im += d * (-zerp.im);
            }
        }
    }

    z = (float) (n + 1) / (float) M_PI;

    result->re *= z;
    result->im *= z;
}

inline TNComplex *tDynamicZernike::ZernikeMoments(short n, BMPImage *bitmap, short *moments_count)
{

    short m, _n;
    //double mass;
    short count, num_moments;
    float xc, yc, xscale, yscale;
    TNComplex *result;

    // The center of the circle/elipse
    //xc = bitmap->getWidth()  >> 1;
    //yc = bitmap->getHeight() >> 1;

    // The radii of the circle/elipse
    //  xscale = (bitmap->getWidth()  -1) >> 1;
    //  yscale = (bitmap->getHeight() -1) >> 1;

    // Determines the number of moments of n_th order and allocates memory for output vector "result"
    num_moments = 0;
    for (m = n; m >= 0; m--)
        num_moments += (m >> 1) + 1;
    result = new TNComplex[num_moments];

    count = 0;

    for (_n = 0; _n <= n; _n++)
        for (m = _n; m >= 0; m -= 2)
            zer_mom(bitmap, _n, m, xc, yc, xscale, yscale, &(result[count++]));

    *moments_count = num_moments;
    return result;
}

inline std::vector<TNComplex> *tDynamicZernike::ZernikeRealPolynomial(char n, char m)
{
    std::vector<TNComplex> *series;
    TNComplex temp;
    float r2;
    for (r2 = 0; r2 <= 1; r2 += 0.001)
        temp.im = r2;
    temp.re = zer_pol_R(n, m, r2);
    series->push_back(temp);

    return series;
}

inline void tDynamicZernike::ZernikeFeatures(BMPImage *bitmap, float *zernike)
{
    TNComplex *moments;
    short count, i;
    double z;

    moments = ZernikeMoments(30, bitmap, &count);

    z = moments[0].re;

    for (i = 0; i < count; i++)
        zernike[i] = sqrt(SQR(moments[i].re) + SQR(moments[i].im)) / z;

    delete moments;
}
