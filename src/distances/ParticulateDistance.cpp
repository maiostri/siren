/*
 * ParticulateDistance.cpp
 *
 *  Created on: May 14, 2013
 *      Author: ricardo
 */

#include "ParticulateDistance.h"

double ParticulateDistance::calculateDistance(tDynamicParticulate *obj1, tDynamicParticulate *obj2, const int metric,
		const double* weight) {
	double distance = 0;

	// L2 or Euclidean distance function
	if (metric == 2) {
		double tmp;
		for (unsigned int i = 0; i < obj1->GetSize(); i++) {
			tmp = obj1->GetData(i) - obj2->GetData(i);
			// applying the weight
			distance += (tmp * tmp) * weight[i];
		} //end for
		distance = sqrt(distance);
	}
	// L1 or Manhatan distance function
	else if (metric == 1) {
		for (unsigned int i = 0; i < obj1->GetSize(); i++) {
			distance += fabs(obj1->GetData(i) - obj2->GetData(i)) * weight[i];
		} //end for
	}
	// L0 or LInfinity distance function
	else if (metric == 0) {
		double tmp;
		for (unsigned int i = 0; i < obj1->GetSize(); i++) {
			tmp = fabs(obj1->GetData(i) - obj2->GetData(i));
			if (tmp > distance) {
				distance = tmp * weight[i];
			}
		} //end for
	}
	// generic Lp distance function
	else {
		double tmp;
		for (unsigned int i = 0; i < obj1->GetSize(); i++) {
			tmp = fabs(obj1->GetData(i) - obj2->GetData(i));
			distance += pow(tmp, metric) * weight[i];
		} //end for
		distance = pow(distance, 1.0 / double(metric));
	}

	return distance;
}

