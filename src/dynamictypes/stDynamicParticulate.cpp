#include "stDynamicParticulate.h"

tDynamicParticulate::tDynamicParticulate()
{
    Size = 0;
    Data = NULL;
}

tDynamicParticulate::tDynamicParticulate(stSize n, const double * data)
{
    Size = n;
    Data = new double[n];
    if (data != NULL)
    {
        for (stSize i = 0; i < n; i++)
            Data[i] = data[i];
    }
}

tDynamicParticulate::~tDynamicParticulate()
{
    if (Data != NULL)
    {
        delete[] Data;
    }
}

tDynamicParticulate * tDynamicParticulate::Clone()
{
    tDynamicParticulate *clone = new tDynamicParticulate(this->Size, this->Data);
    return (tDynamicParticulate *) clone;
}

stSize tDynamicParticulate::GetSerializedSize()
{
    return sizeof (double) * Size;
}

const stByte * tDynamicParticulate::Serialize()
{
    return (unsigned char *) Data;
}

void tDynamicParticulate::Unserialize(const stByte * data, stSize dataSize)
{
    // Resize data
    Size = dataSize / sizeof (double);
    if (Data != NULL)
        delete Data;
    Data = new double[Size];
    memcpy(Data, data, dataSize);
}

bool tDynamicParticulate::IsEqual(tDynamicType * obj)
{
    bool equal;
    bool stop;
    stSize i;

    if (Size != ((tDynamicParticulate *) obj)->Size) return false;

    equal = true;
    i = 0;
    stop = (i >= Size);
    while (!stop)
    {
        if (Data[i] != ((tDynamicParticulate *) obj)->Data[i])
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

void tDynamicParticulate::SetSize(stSize n)
{
    Size = n;
    if (Data != NULL)
        delete Data;
    Data = new double[n];
    for (stSize i = 0; i < n; i++)
        Data[i] = 0;
}

stSize tDynamicParticulate::GetSize()
{
    return Size;
}

double tDynamicParticulate::GetData(stSize index)
{
    if (index < Size)
        return Data[index];
    else
        return -1;
}

void tDynamicParticulate::SetData(stSize n, const double * data)
{
    Size = n;
    Data = new double[n];
    if (data != NULL)
    {
        for (stSize i = 0; i < n; i++)
            Data[i] = data[i];
    }
}

void tDynamicParticulate::SetParticle(stSize position, double data)
{
    if (position < Size)
        Data[position] = data;
}