/*
 * ZernikeDistances.h
 *
 *  Created on: May 14, 2013
 *      Author: ricardo
 */

#ifndef ZERNIKEDISTANCES_H_
#define ZERNIKEDISTANCES_H_

#include <math.h>
#include "../dynamictypes/stDynamicObject.h"
#include "../dynamictypes/stDynamicZernike.h"

using namespace std;

class ZernikeDistance {
public:
	static double calculateDistance(tDynamicZernike *obj1, tDynamicZernike *obj2, const int metric);
};

#endif /* ZERNIKEDISTANCES_H_ */
