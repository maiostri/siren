/*
 * stDistanceElement.h
 *
 *  Created on: Apr 2, 2013
 *      Author: ricardo
 */

#ifndef STDISTANCEELEMENT_H_
#define STDISTANCEELEMENT_H_

#include <arboretum/stTypes.h>
#include <arboretum/stUtil.h>
#include <arboretum/stUserLayerUtil.h>
#include <arboretum/stBasicObjects.h>

class stDistanceElement {
public:
	stDistanceElement();
	virtual ~stDistanceElement();
	void setDistance(const double distance);
	const double calculateDistance(const tDynamicObject *tDynamicObject, const tDynamicObject *tDynamicObject2,
			DistanceFunction *distanceFunction);
	const double getWeigth();
	const DistanceFunction resolvesDistance(const int metricCode);
	const void setWeigth(const double weigth);

protected:
	double weigth;
};

#endif /* STDISTANCEELEMENT_H_ */
