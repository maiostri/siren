/* 
 * File:   stDynamicHistogram.h
 * Author: ricardo
 *
 * Created on February 17, 2013, 11:48 AM
 */

#ifndef STDYNAMICHISTOGRAM_H
#define	STDYNAMICHISTOGRAM_H

#include "stDynamicType.h"

/**
 * Normalized histogram
 * @author Humberto Razente
 * @author Maria Camila Barioni
 */
class tDynamicHistogram : public tDynamicType
{
public:

  /**
   * Constructor.
   */
  tDynamicHistogram ();

  /**
   * Constructor.
   */
  tDynamicHistogram (stSize n, const double *data);

  /**
   * Destructor.
   */
  virtual ~tDynamicHistogram ();

  /**
   * Clones the object.
   */
  virtual tDynamicHistogram * Clone ();

  /**
   * Gets the object serialized size
   */
  virtual stSize GetSerializedSize ();

  /**
   * Returns the object serialization
   */
  virtual const stByte * Serialize ();

  /**
   * Restores the serialized object.
   */
  virtual void Unserialize (const stByte * data, stSize dataSize);

  /**
   * Tests if one object is equal to another.
   */
  virtual bool IsEqual (tDynamicType * obj);
  /**
   * Extract the histogram from a bitmap
   */
  void ExtractHistogram (Image * src);

  /**
   * Returns the size of the histogram
   */
  stSize GetSize ();

  /**
   * Returns a value of the histogram
   */
  double GetData (stSize index);
  
  /**
   * Returns the histogram
   */
  double * GetData ();

private:

  /**
   * Sizes or resizes the histogram
   */
  void SetSize (stSize n);

};



#endif	/* STDYNAMICHISTOGRAM_H */

