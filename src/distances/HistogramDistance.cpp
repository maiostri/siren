/*
 * HistogramDistance.cpp
 *
 *  Created on: May 14, 2013
 *      Author: ricardo
 */

#include "HistogramDistance.h"

double HistogramDistance::calculateDistance(tDynamicHistogram *obj1, tDynamicHistogram *obj2, const int metric) {
	double distance = 0;

	// L2 or Euclidean distance function
	if (metric == 2) {
		stBasicArrayObject<double, int> *descriptor1 = new stBasicArrayObject<double, int>(obj1->getSize(),
				obj1->getData());
		stBasicArrayObject<double, int> *descriptor2 = new stBasicArrayObject<double, int>(obj2->getSize(),
				obj2->getData());

		EuclidianDistance *m1 = new EuclidianDistance();
		stDistance d1 = m1->getDistance<stDistance>(descriptor1, descriptor2);
		distance = d1;
	}
	// L1 or Manhatan distance function
	else if (metric == 1) {
		for (unsigned long i = 0; i < obj1->getSize(); i++) {
			distance += fabs(obj1->getData(i) - obj2->getData(i));
		} //end for
	}
	// L0 or LInfinity distance function
	else if (metric == 0) {
		double tmp;
		for (unsigned long i = 0; i < obj1->getSize(); i++) {
			tmp = fabs(obj1->getData(i) - obj2->getData(i));
			if (tmp > distance) {
				distance = tmp;
			}
		} //end for
	}
	// generic Lp distance function
	else {
		double tmp;
		for (unsigned long i = 0; i < obj1->getSize(); i++) {
			tmp = fabs(obj1->getData(i) - obj2->getData(i));
			distance += pow(tmp, metric);
		} //end for
		distance = pow(distance, 1.0 / double(metric));
	}

	return distance;
}

