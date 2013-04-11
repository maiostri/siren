#include "stDynamicMetricHistogram.h"

/**
 * Constructor.
 */
tDynamicMetricHistogram::tDynamicMetricHistogram()
{
}

/**
 * Destructor.
 */
tDynamicMetricHistogram::~tDynamicMetricHistogram()
{
}

/**
 * Clones the object.
 */
tDynamicMetricHistogram * tDynamicMetricHistogram::Clone()
{
    tDynamicMetricHistogram *clone = new tDynamicMetricHistogram();
    return clone;
}

/**
 * Gets the object serialized size
 */
stSize tDynamicMetricHistogram::GetSerializedSize()
{
    return 0;
}

/**
 * Returns the object serialization
 */
const stByte * tDynamicMetricHistogram::Serialize()
{
    return 0;
}

/**
 * Restores the serialized object.
 */
void tDynamicMetricHistogram::Unserialize(const stByte * data, stSize dataSize)
{
}

/**
 * Tests if one object is equal to another.
 */
bool tDynamicMetricHistogram::IsEqual(tDynamicType * obj)
{
    return 0;
}

/**
 * Extract the metric histogram from a jpeg file
 */
void tDynamicMetricHistogram::ExtractMetricHistogram(const Image *img)
{
}
