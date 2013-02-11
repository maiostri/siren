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

#include <arboretum/stTypes.h>
#include <arboretum/stUtil.h>
#include <arboretum/stUserLayerUtil.h>

#include "stDynamicType.h"
#include "stDynamicObject.h"


/**
* This class implements the dynamic evaluator.
*
* @author Humberto Razente
* @author Maria Camila Barioni
* @ingroup user
*/

class tDynamicDistanceEvaluator: public stMetricEvaluatorStatistics
{
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

public:
    /**
    * Constructor. Use it to build diferent dlls for the index server,
    * changing the value of the metric.
    */
    tDynamicDistanceEvaluator()
    {
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
        for (int i = 0; i < 256; i++)
            particulate_weight[i] = 1;

        texturedistanceevaluator = new tDynamicTextureDistanceEvaluator();
    }

    /**
    * Destructor.
    */
    ~tDynamicDistanceEvaluator()
    {
        delete texturedistanceevaluator;
    }

    /**
    * Sets the metric
    */
    void SetMetric(int Metric)
    {
        metric = Metric;
    }

    /**
    * Sets the weights for each extrator/parameter.
    */
    void SetParticulateWeight(int particleposition, double weight)
    {
        particulate_weight[particleposition] = weight;
    }

    /**
    * Sets the weights for each extrator/parameter.
    */
    void SetWeight(std::string extractor, std::string parameter, double w)
    {
        // extractor == histogram
        if ((extractor == "histogramext") && (parameter == "histogram"))
            {
                histogram_weight = w;
            }
        // extractor == metric histogram
        else if ((extractor == "metrichistogramext") && (parameter == "metrichistogram"))
            {
                metrichistogram_weight = w;
            }
        // extractor == texture
        else if ((extractor == "textureext") && (parameter == "texture"))
            {
                texture_weight = w;
            }
        // extractor == zernike
        else if ((extractor == "zernikeext") && (parameter == "zernike"))
            {
                zernike_weight = w;
            }
        else if ((extractor == "soundtextureext") && (parameter == "stft"))
            {
                soundtexturestft_weight = w;
            }
        else if ((extractor == "soundtextureext") && (parameter == "mfcc"))
            {
                soundtexturemfcc_weight = w;
            }
    }

    /**
    * Gets the distance between 2 objects with regard to the Metric
    */
    virtual stDistance GetDistance(tDynamicObject * o1, tDynamicObject * o2)
    {
        tDynamicObject *ob1 = (tDynamicObject *)o1;
        tDynamicObject *ob2 = (tDynamicObject *)o2;

        // the distance is supposed to be a value from 0 - 1
        double distance = 0;
        int distancecount = 0;

        // histogram
        if ((ob1->GetHistogram() != NULL) && (ob2->GetHistogram() != NULL))
            {
                distance += histogram_weight * HistogramDistance(ob1->GetHistogram(),ob2->GetHistogram(),metric);
                distancecount++;
            }

        // metric histogram
        if ((ob1->GetMetricHistogram() != NULL) && (ob2->GetMetricHistogram() != NULL))
            {
                distance += metrichistogram_weight * MetricHistogramDistance(ob1->GetMetricHistogram(),ob2->GetMetricHistogram(),metric);
                distancecount++;
            }

        // texture
        if ((ob1->GetTexture() != NULL) && (ob2->GetTexture() != NULL))
            {
                distance += texture_weight * TextureDistance(ob1->GetTexture(),ob2->GetTexture(),metric);
                distancecount++;
            }

        // particulate
        if ((ob1->GetParticulate() != NULL) && (ob2->GetParticulate() != NULL))
            {
                distance += ParticulateDistance(ob1->GetParticulate(),ob2->GetParticulate(),metric);
                distancecount++;
            }

        // zernike
        if ((ob1->GetZernike() != NULL) && (ob2->GetZernike() != NULL))
            {
                distance += zernike_weight * ZernikeDistance(ob1->GetZernike(),ob2->GetZernike(),metric);
                distancecount++;
            }

        // Statistic support
        UpdateDistanceCount();

        // return the distance normilized in the range 0 - 1
        if (distancecount != 0)
            return distance / double(distancecount);
        else
            return 0;
    }

    /**
    * Gets the square of the distance between 2 objects with regard to the Metric
    */
    virtual stDistance GetDistance2(tDynamicObject * o1, tDynamicObject * o2)
    {
        double distance = GetDistance(o1,o2);
        return distance * distance;
    }

    /**
    *  Compute the distance between two tDynamicMetricHistograms
    */
    double MetricHistogramDistance(tDynamicMetricHistogram *obj1, tDynamicMetricHistogram *obj2, int metric)
    {
        double distance = 0;
        return distance;
    }

    /**
    *  Compute the distance between two tDynamicMetricHistograms
    */
    double TextureDistance(tDynamicTexture *obj1, tDynamicTexture *obj2, int metric)
    {
        double distance = texturedistanceevaluator->GetDistance(obj1,obj2);
        return distance;
    }

    /**
    *  Compute the distance between two tDynamicHistograms
    */
    double HistogramDistance(tDynamicHistogram *obj1, tDynamicHistogram *obj2, int metric)
    {
        double distance = 0;

        // L2 or Euclidean distance function
        if (metric == 2)
            {
                double tmp;
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        tmp = obj1->GetData(i) - obj2->GetData(i);
                        distance += (tmp * tmp);
                    }//end for
                distance = sqrt(distance);
            }
        // L1 or Manhatan distance function
        else if (metric == 1)
            {
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        distance += fabs(obj1->GetData(i) - obj2->GetData(i));
                    }//end for
            }
        // L0 or LInfinity distance function
        else if (metric == 0)
            {
                double tmp;
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        tmp = fabs(obj1->GetData(i) - obj2->GetData(i));
                        if (tmp > distance)
                            {
                                distance = tmp;
                            }
                    }//end for
            }
        // generic Lp distance function
        else
            {
                double tmp;
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        tmp = fabs(obj1->GetData(i) - obj2->GetData(i));
                        distance += pow(tmp,metric);
                    }//end for
                distance = pow(distance, 1.0 / double(metric));
            }

        return distance;
    }

    /**
    *  Compute the distance between two tDynamicZernikes
    */
    double ZernikeDistance(tDynamicZernike *obj1, tDynamicZernike *obj2, int metric)
    {
        double distance = 0;

        // L2 or Euclidean distance function
        if (metric == 2)
            {
                double tmp;
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        tmp = obj1->GetData(i) - obj2->GetData(i);
                        distance += (tmp * tmp);
                    }//end for
                distance = sqrt(distance);
            }
        // L1 or Manhatan distance function
        else if (metric == 1)
            {
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        distance += fabs(obj1->GetData(i) - obj2->GetData(i));
                    }//end for
            }
        // L0 or LInfinity distance function
        else if (metric == 0)
            {
                double tmp;
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        tmp = fabs(obj1->GetData(i) - obj2->GetData(i));
                        if (tmp > distance)
                            {
                                distance = tmp;
                            }
                    }//end for
            }
        // generic Lp distance function
        else
            {
                double tmp;
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        tmp = fabs(obj1->GetData(i) - obj2->GetData(i));
                        distance += pow(tmp,metric);
                    }//end for
                distance = pow(distance, 1.0 / double(metric));
            }

        return distance;
    }

    /**
    *  Compute the distance between two Particulates
    */
    double ParticulateDistance(tDynamicParticulate *obj1, tDynamicParticulate *obj2, int metric)
    {
        double distance = 0;

        // L2 or Euclidean distance function
        if (metric == 2)
            {
                double tmp;
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        tmp = obj1->GetData(i) - obj2->GetData(i);
                        // applying the weight
                        distance += (tmp * tmp) * particulate_weight[i];
                    }//end for
                distance = sqrt(distance);
            }
        // L1 or Manhatan distance function
        else if (metric == 1)
            {
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        distance += fabs(obj1->GetData(i) - obj2->GetData(i)) * particulate_weight[i];
                    }//end for
            }
        // L0 or LInfinity distance function
        else if (metric == 0)
            {
                double tmp;
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        tmp = fabs(obj1->GetData(i) - obj2->GetData(i));
                        if (tmp > distance)
                            {
                                distance = tmp * particulate_weight[i];
                            }
                    }//end for
            }
        // generic Lp distance function
        else
            {
                double tmp;
                for (unsigned int i = 0; i < obj1->GetSize(); i++)
                    {
                        tmp = fabs(obj1->GetData(i) - obj2->GetData(i));
                        distance += pow(tmp,metric) * particulate_weight[i];
                    }//end for
                distance = pow(distance, 1.0 / double(metric));
            }

        return distance;
    }

};//end tDynamicDistanceEvaluator

#endif
