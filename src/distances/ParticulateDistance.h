/*
 * ParticulateDistance.h
 *
 *  Created on: May 14, 2013
 *      Author: ricardo
 */

#ifndef PARTICULATEDISTANCE_H_
#define PARTICULATEDISTANCE_H_

#include "math.h"
#include "../dynamictypes/stDynamicParticulate.h"

class ParticulateDistance {
public:
	static double calculateDistance(tDynamicParticulate *obj1, tDynamicParticulate *obj2, const int metric, const double* weight);
};

#endif /* PARTICULATEDISTANCE_H_ */
