/*
 * HistogramDistance.h
 *
 *  Created on: May 14, 2013
 *      Author: ricardo
 */

#ifndef HISTOGRAMDIST_H_
#define HISTOGRAMDIST_H_

#include <arboretum/stTypes.h>
#include <arboretum/stUtil.h>
#include <arboretum/stUserLayerUtil.h>

#include <hermes/MetricHistogramFamily/FamilyMetricHistogram.hpp>
#include <hermes/MinkwoskyFamily/FamilyL.hpp>

#include "../dynamictypes/stDynamicHistogram.h"

class HistogramDistance {
public:
	static double calculateDistance(tDynamicHistogram *obj1, tDynamicHistogram *obj2, const int metric);
};

#endif /* HISTOGRAMDIST_H_ */
