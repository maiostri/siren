/*
 * stHistogramDistance.cpp
 *
 *  Created on: Apr 2, 2013
 *      Author: ricardo
 */

#include "stHistogramDistance.h"

stHistogramDistance::stHistogramDistance() {
}

stHistogramDistance::~stHistogramDistance() {
}

double stHistogramDistance::calculateDistance(const tDynamicObject *tDynamicObject,
		const tDynamicObject *tDynamicObject2, DistanceFunction *distanceFunction) {

	stBasicArrayObject<double, int> *descriptor1 = new stBasicArrayObject<double, int>(tDynamicObject->GetSize(),
			tDynamicObject->GetData());
	stBasicArrayObject<double, int> *descriptor2 = new stBasicArrayObject<double, int>(tDynamicObject2->GetSize(),
			tDynamicObject2->GetData());

	EuclidianDistance *m1 = (EuclidianDistance *) distanceFunction;
	stDistance d1 = m1->getDistance<stDistance>(descriptor1, descriptor2);

	distance = d1;
	delete m1;
	return distance;
}
