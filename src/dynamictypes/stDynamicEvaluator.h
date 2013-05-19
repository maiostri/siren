/**
 * @file
 *
 * This file implements the dynamic evaluator.
 *
 * @author Humberto Razente
 * @author Maria Camila Barioni
 * @todo Documentation review and tests.
 */
#ifndef __STDYNAMICEVALUATOR_H
#define __STDYNAMICEVALUATOR_H

#include <stdlib.h>
#include <values.h>

#include "stDynamicType.h"
#include "stDynamicObject.h"
#include "../distances/HistogramDistance.h"
#include "../distances/ZernikeDistance.h"
#include "../distances/ParticulateDistance.h"

using namespace std;

/**
 * This class implements the dynamic evaluator.
 *
 * @author Humberto Razente
 * @author Maria Camila Barioni
 * @ingroup user
 */

class tDynamicDistanceEvaluator: public stMetricEvaluatorStatistics {
private:
	/**
	 * The metric
	 */
	int metric;

	/**
	 * The weight for the histogram
	 */
	double histogram_weight;

	/**
	 * The weight for the metric histogram
	 */
	double metrichistogram_weight;

	/**
	 * The weight for the texture
	 */
	double texture_weight;

	/**
	 * The weight for the zernike
	 */
	double zernike_weight;

	/**
	 * The weight for the particulate
	 */
	double *particulate_weight;

	/**
	 * The weight for the soundtexturestft
	 */
	double soundtexturestft_weight;

	/**
	 * The weight for the soundtexturemfcc
	 */
	double soundtexturemfcc_weight;

	/**
	 * To compute the texture distance
	 */
	tDynamicTextureDistanceEvaluator *texturedistanceevaluator;

	DistanceFunction *distanceFunction;

public:

	/**
	 * Constructor. Use it to build diferent dlls for the index server,
	 * changing the value of the metric.
	 */
	tDynamicDistanceEvaluator() {

		// initializing the default metric
		metric = 2; // L2

		// initializing weights
		histogram_weight = 1;
		metrichistogram_weight = 1;
		texture_weight = 1;
		zernike_weight = 1;
		soundtexturestft_weight = 1;
		soundtexturemfcc_weight = 1;

		// the particulate is limited to 256 particles
		particulate_weight = new double[256];
		for (int i = 0; i < 256; i++) {
			particulate_weight[i] = 1;
		}

		texturedistanceevaluator = new tDynamicTextureDistanceEvaluator();
	}

	/**
	 * Destructor.
	 */
	virtual ~tDynamicDistanceEvaluator() {
		delete texturedistanceevaluator;
	}

	/**
	 * Sets the metric
	 */
	void setMetric(int metric) {
		this->metric = metric;
	}

	/**
	 * Sets the weights for each extrator/parameter.
	 */
	void SetParticulateWeight(int particleposition, double weight) {
		particulate_weight[particleposition] = weight;
	}

	/**
	 * Sets the weights for each extrator/parameter.
	 */
	void SetWeight(string extractor, string parameter, double w) {
		// extractor == histogram
		if ((extractor == "histogramext") && (parameter == "histogram")) {
			histogram_weight = w;
		}
		// extractor == metric histogram
		else if ((extractor == "metrichistogramext") && (parameter == "metrichistogram")) {
			metrichistogram_weight = w;
		}
		// extractor == texture
		else if ((extractor == "textureext") && (parameter == "texture")) {
			texture_weight = w;
		}
		// extractor == zernike
		else if ((extractor == "zernikeext") && (parameter == "zernike")) {
			zernike_weight = w;
		} else if ((extractor == "soundtextureext") && (parameter == "stft")) {
			soundtexturestft_weight = w;
		} else if ((extractor == "soundtextureext") && (parameter == "mfcc")) {
			soundtexturemfcc_weight = w;
		}
	}

	/**
	 * Gets the distance between 2 objects with regard to the Metric
	 */
	virtual stDistance GetDistance(tDynamicObject * ob1, tDynamicObject * ob2) {

		// the distance is supposed to be a value from 0 - 1
		double distance = 0;
		int distancecount = 0;

		// histogram
		if ((ob1->GetHistogram() != nullptr) && (ob2->GetHistogram() != nullptr)) {
			distance += histogram_weight
					* HistogramDistance::calculateDistance(ob1->GetHistogram(), ob2->GetHistogram(), metric);
			distancecount++;
		}

		// metric histogram
		if ((ob1->GetMetricHistogram() != nullptr) && (ob2->GetMetricHistogram() != nullptr)) {
			distance += metrichistogram_weight
					* MetricHistogramDistance(ob1->GetMetricHistogram(), ob2->GetMetricHistogram());
			distancecount++;
		}

		// texture
		if ((ob1->GetTexture() != nullptr) && (ob2->GetTexture() != nullptr)) {
			distance += texture_weight * TextureDistance(ob1->GetTexture(), ob2->GetTexture());
			distancecount++;
		}

		// particulate
		if ((ob1->GetParticulate() != nullptr) && (ob2->GetParticulate() != nullptr)) {
			distance += ParticulateDistance::calculateDistance(ob1->GetParticulate(), ob2->GetParticulate(), metric,
					particulate_weight);
			distancecount++;
		}

		// zernike
		if ((ob1->GetZernike() != nullptr) && (ob2->GetZernike() != nullptr)) {
			distance += zernike_weight
					* ZernikeDistance::calculateDistance(ob1->GetZernike(), ob2->GetZernike(), metric);
			distancecount++;
		}

		// Statistic support
		UpdateDistanceCount();

		// return the distance normilized in the range 0 - 1
		if (distancecount != 0) {
			return distance / double(distancecount);
		} else {
			return 0;
		}

	}

	/**
	 *  Compute the distance between two tDynamicMetricHistograms
	 *  In case metrics added, this should be rewritten in a proper class.
	 */
	double MetricHistogramDistance(tDynamicMetricHistogram *obj1, tDynamicMetricHistogram *obj2) {
		double distance = 0;
		return distance;
	}

	/**
	 *  Compute the distance between two tDynamicTexture
	 *  In case metrics added, this should be rewritten in a proper class.
	 */
	double TextureDistance(tDynamicTexture *obj1, tDynamicTexture *obj2) {
		double distance = texturedistanceevaluator->GetDistance(obj1, obj2);
		return distance;
	}
};
//end tDynamicDistanceEvaluator

#endif
