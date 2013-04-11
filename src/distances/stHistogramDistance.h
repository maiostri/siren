/*
 * stHistogramDistance.h
 *
 *  Created on: Apr 2, 2013
 *      Author: ricardo
 */

#ifndef STHISTOGRAMDISTANCE_H_
#define STHISTOGRAMDISTANCE_H_

class stHistogramDistance: stDistanceElement {
public:
	stHistogramDistance();
	virtual ~stHistogramDistance();
	double calculateDistance(const tDynamicObject *tDynamicObject, const tDynamicObject *tDynamicObject2,
			const int metric) override;

};

#endif /* STHISTOGRAMDISTANCE_H_ */
