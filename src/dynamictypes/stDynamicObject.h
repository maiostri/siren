/**
 * @file
 *
 * This file implements the dynamic object
 */
#ifndef __STDYNAMICBJECT_H
#define __STDYNAMICBJECT_H

#include <vector>

#include <arboretum/stTypes.h>
#include <arboretum/stObject.h>
#include <arboretum/stException.h>

#include "stDynamicType.h"
#include "stDynamicTexture.h"
#include "stDynamicZernike.h"

/**
 * This class implements the dynamic object.
 * In order to add new characteristics to this object,
 * the methods Clone() and IsEqual() must be altered.
 *
 * @author Humberto Razente
 * @author Maria Camila Nardini Barioni
 */
class tDynamicObject : public stObject
{
private:
  /**
   * Pointer to a histogram.
   */
  tDynamicHistogram *histogram;

  /**
   * Pointer to a metric histogram.
   */
  tDynamicMetricHistogram *metrichistogram;

  /**
   * Pointer to texture feature.
   */
  tDynamicTexture *texture;

  tDynamicTexture *sample;

  /**
   * Pointer to particulate feature.
   */
  tDynamicParticulate *particulate;

  /**
   * Pointer to texture feature.
   */
  tDynamicZernike *zernike;

  /**
   * Pointer to a sound texture stft.
   */
  tDynamicHistogram *soundtexturestft;

  /**
   * Pointer to a sound texture stft.
   */
  tDynamicHistogram *soundtexturemfcc;

  /**
   * Object identification.
   */
  long OID;

  /**
   * Multimedia file source
   */
  std::string FileName;


public:

  /**
   * Constructor.
   */
  tDynamicObject ()
  {
    OID = 0;
    histogram = NULL;
    metrichistogram = NULL;
    texture = NULL;
    particulate = NULL;
    zernike = NULL;
    FileName = "";
  }//end tDynamicObject

  /**
   * Destructor.
   */
  virtual
  ~tDynamicObject ()
  {

    if (histogram != NULL)
      delete histogram;

    if (metrichistogram != NULL)
      delete metrichistogram;

    if (texture != NULL)
      delete texture;

    if (particulate != NULL)
      delete particulate;

    if (zernike != NULL)
      delete zernike;
  }//end ~tDynamicObject

  /**
   * Sets multimedia file source name
   */
  void
  SetFileName (std::string fn)
  {
    FileName = fn;
  }

  /**
   * Gets multimedia file source name
   */
  std::string
  GetFileName ()
  {
    return FileName;
  }

  virtual void *
  Get (stCount idx)
  {
    return NULL;
  }

  virtual stSize
  GetSize ()
  {
    return 0;
  }

  /**
   * Returns the tDynamicType feature
   */
  tDynamicType *
  GetExtraction (const std::string& extractor)
  {
    // extractor == histogram
    if (extractor.compare ("histogramext") == 0)
      {
        //if (parameter.compare("histogram") == 0)
        return (tDynamicType *) GetHistogram ();
      }// extractor == metric histogram
    else if (extractor.compare ("metrichistogramext") == 0)
      {
        //if (parameter.compare("metrichistogram") == 0)
        return (tDynamicType *) GetMetricHistogram ();
      }// extractor == texture
    else if (extractor.compare ("textureext") == 0)
      {
        //if (parameter.compare("texture") == 0)
        return (tDynamicType *) GetTexture ();
      }// extractor == zernike
    else if (extractor.compare ("zernikeext") == 0)
      {
        //if (parameter.compare("zernike") == 0)
        return (tDynamicZernike *) GetZernike ();
      }
    return NULL;
  }

  /**
   * This function receives the data of the extractor as a char
   * and copy it to a double which will be attributed in the tDynamicObject.
   * @param data
   *				The extractor data.
   */
  double *
  getDataDynamicType (char *data)
  {
    double *dataDouble = new double();
    memcpy (dataDouble, data, sizeof (data));
    return dataDouble;
  }

  /**
   * Here we set the DynamicType of the DynamicObject based on the parameter.
   * @param parameter
   * @param data
   */
  void
  setData (std::string parameter, char *data)
  {
    tDynamicType *tDynamicTypeObject;
    double *dataDouble = getDataDynamicType (data);

    if (parameter.compare ("histogram") == 0)
      {
        tDynamicTypeObject = new tDynamicHistogram ();
        tDynamicTypeObject->setData (dataDouble);
        this->SetHistogram ((tDynamicHistogram *) tDynamicTypeObject);
      }
    else if (parameter.compare ("metrichistogram") == 0)
      {
        tDynamicTypeObject = new tDynamicMetricHistogram ();
        tDynamicTypeObject->setData (dataDouble);
        this->SetMetricHistogram ((tDynamicMetricHistogram *) tDynamicTypeObject);
        // @TODO - That is important.
        // The specifications of the texture extractor are different.
        // Let's wait until the new extractor is implemented.
      }
    else if (parameter.compare ("texture") == 0)
      {
        //tDynamicTypeObject = new tDynamicTexture();												
        //tDynamicTypeObject->setData(dataDouble);
        //this->SetTexture((tDynamicTexture *) tDynamicTypeObject);
      }
    else if (parameter.compare ("zernike") == 0)
      {
        tDynamicTypeObject = new tDynamicZernike ();
        tDynamicTypeObject->setData (dataDouble);
        this->SetZernike ((tDynamicZernike *) tDynamicTypeObject);
      }
  }

  /**
   * Extracts the features of an image
   */
  void
  Extract (const std::string& extractor, const std::string& parameter, Image* src)
  {
    // extractor == histogram
    if (extractor.compare ("histogramext") == 0)
      {
        if (parameter.compare ("histogram") == 0)
          {
            tDynamicHistogram *h = new tDynamicHistogram ();
            h->ExtractHistogram (src);
            if (this->GetHistogram () == NULL)
              this->SetHistogram (h);
            delete h;
          }
      }// extractor == metric histogram
    else if (extractor.compare ("metrichistogramext") == 0)
      {
        if (parameter.compare ("metrichistogram") == 0)
          {
            tDynamicMetricHistogram *m = new tDynamicMetricHistogram ();
            m->ExtractMetricHistogram (src);
            if (this->GetMetricHistogram () == NULL)
              this->SetMetricHistogram (m);
            delete m;
          }
      }// extractor == texture
    else if (extractor.compare ("textureext") == 0)
      {
        if (parameter.compare ("texture") == 0)
          {
            tDynamicTexture *t = new tDynamicTexture ();
            t->ExtractTexture (src);
            if (this->GetTexture () == NULL)
              this->SetTexture (t);
            delete t;
          }
      }// @TODO This extractor isn't implemented yet.
    else if (extractor.compare ("zernikeext") == 0)
      {
        if (parameter.compare ("zernike") == 0)
          {
          }
      }
  }

  /**
   * Sets the histogram.
   */
  void
  SetHistogram (tDynamicHistogram *h)
  {
    this->histogram = h;
  }

  /**
   * Sets the histogram.
   */
  void
  SetHistogram (stSize n, double* data = NULL)
  {
    if (histogram == NULL)
      {
        histogram = new tDynamicHistogram (n, data);
      }
    else
      {
        delete histogram;
        histogram = new tDynamicHistogram (n, data);
      }
  }

  /**
   * Gets the histogram.
   */
  tDynamicHistogram *
  GetHistogram ()
  {
    return histogram;
  }

  /**
   * Sets the metric histogram.
   */
  void
  SetMetricHistogram (tDynamicMetricHistogram *m)
  {
    this->metrichistogram = m;
  }

  /**
   * Gets the metric histogram.
   */
  tDynamicMetricHistogram *
  GetMetricHistogram ()
  {
    return metrichistogram;
  }

  /**
   * Sets the texture.
   */
  void
  SetTexture (tDynamicTexture *m)
  {
    this->texture = m;
  }

  /**
   * Gets the texture.
   */
  tDynamicTexture *
  GetTexture ()
  {
    return texture;
  }

  /**
   * Sets the particulate.
   */
  void
  SetParticulate (tDynamicParticulate *p)
  {
    if (p != NULL)
      {
        if (particulate == NULL)
          {
            particulate = (tDynamicParticulate *) p->Clone ();
          }
        else
          {
            delete particulate;
            particulate = (tDynamicParticulate *) p->Clone ();
          }
      }
  }

  /**
   * Gets the particulate.
   */
  tDynamicParticulate *
  GetParticulate ()
  {
    return particulate;
  }

  /**
   * Sets the zernike.
   */
  void
  SetZernike (tDynamicZernike *m)
  {
    this->zernike = m;
  }

  /**
   * Gets the zernike.
   */
  tDynamicZernike *
  GetZernike ()
  {
    return zernike;
  }

  /**
   * Clones the object.
   */
  virtual tDynamicObject *
  Clone ()
  {
    // instantiate the new object
    tDynamicObject * tmp = new tDynamicObject ();

    // set identification
    tmp->SetOID (this->GetOID ());
    tmp->SetFileName (this->GetFileName ());

    // set the histogram
    tmp->SetHistogram (this->GetHistogram ());

    // set the metric histogram
    tmp->SetMetricHistogram (this->GetMetricHistogram ());

    // set the texture
    tmp->SetTexture (this->GetTexture ());

    // set the particulate
    tmp->SetParticulate (this->GetParticulate ());

    // set the zernike
    tmp->SetZernike (this->GetZernike ());

    // return new object
    return tmp;
  }//end Clone

  /**
   * Tests if one object is equal to another.
   */
  virtual bool
  IsEqual (stObject * obj)
  {
    bool histogramIsEqual = true;
    bool metrichistogramIsEqual = true;
    bool textureIsEqual = true;
    bool particulateIsEqual = true;
    bool zernikeIsEqual = true;
    bool soundtexturestftIsEqual = true;
    bool soundtexturemfccIsEqual = true;

    // test 1: tests if each data type is equal
    if (this->GetHistogram () != NULL)
      histogramIsEqual = this->GetHistogram ()->IsEqual (((tDynamicObject *) obj)->GetHistogram ());
    if (this->GetMetricHistogram () != NULL)
      metrichistogramIsEqual = this->GetMetricHistogram ()->IsEqual (((tDynamicObject *) obj)->GetMetricHistogram ());
    if (this->GetTexture () != NULL)
      //				textureIsEqual = this->GetTexture()->IsEqual(((tDynamicObject *)obj)->GetTexture());
      if (this->GetParticulate () != NULL)
        particulateIsEqual = this->GetParticulate ()->IsEqual (((tDynamicObject *) obj)->GetParticulate ());
    if (this->GetZernike () != NULL)
      zernikeIsEqual = this->GetZernike ()->IsEqual (((tDynamicObject *) obj)->GetZernike ());

    // test 2: tests if a data type not present in this object is present on the other
    if ((this->GetHistogram () == NULL) && (((tDynamicObject *) obj)->GetHistogram () != NULL))
      histogramIsEqual = false;
    if ((this->GetMetricHistogram () == NULL) && (((tDynamicObject *) obj)->GetMetricHistogram () != NULL))
      metrichistogramIsEqual = false;
    if ((this->GetTexture () == NULL) && (((tDynamicObject *) obj)->GetTexture () != NULL))
      textureIsEqual = false;
    if ((this->GetParticulate () == NULL) && (((tDynamicObject *) obj)->GetParticulate () != NULL))
      particulateIsEqual = false;
    if ((this->GetZernike () == NULL) && (((tDynamicObject *) obj)->GetZernike () != NULL))
      zernikeIsEqual = false;
    // join individual tests
    if (histogramIsEqual && metrichistogramIsEqual &&
        textureIsEqual && particulateIsEqual && zernikeIsEqual &&
        soundtexturestftIsEqual && soundtexturemfccIsEqual)
      return true;
    else
      return false;
  }//end IsEqual

  /**
   * Returns the object indentification
   */
  virtual long
  GetOID () const
  {
    return OID;
  }//end GetOID

  /**
   * Sets the object indentification
   */
  void
  SetOID (long oid)
  {
    OID = oid;
  }//end SetOID

  stSize
  GetSerializedSize ()
  {
    return 0;
  }

  const stByte *
  Serialize ()
  {
    // @TODO Fix it.
    stByte *serialized = new stByte[256];
    return serialized;
  }

  void
  Unserialize (const stByte *data, stSize datasize) { }

}; //end tDynamicObject

#endif
