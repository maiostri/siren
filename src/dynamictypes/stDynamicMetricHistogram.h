/* 
 * File:   stDynamicMetricHistogram.h
 * Author: ricardo
 *
 * Created on February 17, 2013, 11:49 AM
 */

#ifndef STDYNAMICMETRICHISTOGRAM_H
#define	STDYNAMICMETRICHISTOGRAM_H

#include "stDynamicType.h"

/**
 * Metric histogram
 * @author Humberto Razente
 * @author Maria Camila Barioni
 */
class tDynamicMetricHistogram: public tDynamicType {
public:

	/**
	 * Constructor.
	 */
	tDynamicMetricHistogram();

	/**
	 * Destructor.
	 */
	virtual
	~tDynamicMetricHistogram();

	/**
	 * Clones the object.
	 */
	virtual tDynamicMetricHistogram * Clone();

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
	 * Extract the metric histogram from a jpeg file
	 */
	void
	ExtractMetricHistogram(const Image *img);

};

#endif	/* STDYNAMICMETRICHISTOGRAM_H */

