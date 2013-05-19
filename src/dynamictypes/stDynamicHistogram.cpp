#include "stDynamicHistogram.h"

tDynamicHistogram::tDynamicHistogram()
{
    Size = 0;
    Data = NULL;
}

tDynamicHistogram::tDynamicHistogram(stSize n, const double *data)
{
    Size = n;
    Data = new double[Size];
    if (data != NULL)
        for (stSize i = 0; i < n; i++) Data[i] = data[i];
}

tDynamicHistogram::~tDynamicHistogram()
{
    if (Data != NULL)
    {
        delete[] Data;
    }
}

tDynamicHistogram * tDynamicHistogram::Clone()
{
    tDynamicHistogram *clone = new tDynamicHistogram(this->Size, this->Data);
    return (tDynamicHistogram *) clone;
}

stSize tDynamicHistogram::GetSerializedSize()
{
    return sizeof (double) * Size;
}

const stByte * tDynamicHistogram::Serialize()
{
    return (unsigned char *) Data;
}

void tDynamicHistogram::Unserialize(const stByte * data, stSize dataSize)
{
    // Resize data
    Size = dataSize / sizeof (double);    
    if (Data != NULL)
        delete Data;
    Data = new double[Size];
    memcpy(Data, data, dataSize);
}

bool tDynamicHistogram::IsEqual(tDynamicType * obj)
{
    bool equal;
    bool stop;
    stSize i;

    if (Size != ((tDynamicHistogram *) obj)->Size) return false;

    equal = true;
    i = 0;
    stop = (i >= Size);
    while (!stop)
    {
        if (Data[i] != ((tDynamicHistogram *) obj)->Data[i])
        {
            //((tDynamicHistogram *)obj)->(*Data[i].GetData())) {
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

void tDynamicHistogram::ExtractHistogram(Image * src)
{
    // width and heightS
    NormalizedHistogramExtractor<stBasicArrayObject<double> > *extractor =
            new NormalizedHistogramExtractor<stBasicArrayObject<double> >();

    try
    {
        stBasicArrayObject<double> *Se = new stBasicArrayObject<double>(256);
        extractor->generateSignature(*src, *Se);
        if (Data != NULL) {
            delete Data;
        }    
        Data = new double[Se->GetSize()];
        Size = Se->GetSize();
        for (unsigned int i = 0; i < Se->GetSize(); i++) Data[i] = *(double*) Se->Get(i);
        delete Se;
    }
    catch (NotAvaliableException* ne)
    {
        printf("Erro no descritor: Erro nos recursos do extrator\n");
        delete extractor;
        delete Data;
    }
    catch (artemis::FullHeapException* fe)
    {
        printf("Erro no descritor: Erro na alocação de memória de recursos\n");
        delete extractor;
    }
}

double tDynamicHistogram::getData(stSize index)
{
    if (index < Size)
        return Data[index];
    else
        return -1;
}

double * tDynamicHistogram::getData() {
	return tDynamicType::getData();
}

void tDynamicHistogram::SetSize(stSize n)
{
    Size = n;
    if (Data != NULL)
        delete Data;
    Data = new double[Size];
    for (stSize i = 0; i < n; i++)
        Data[i] = 0;
}
