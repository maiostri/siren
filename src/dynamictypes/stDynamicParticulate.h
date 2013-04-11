/* 
 * File:   stDynamicParticulate.h
 * Author: ricardo
 *
 * Created on February 17, 2013, 11:51 AM
 */

#ifndef STDYNAMICPARTICULATE_H
#define	STDYNAMICPARTICULATE_H

#include "stDynamicType.h"

/**
 * Particulate Object
 * @author Humberto Razente
 * @author Maria Camila Barioni
 */
class tDynamicParticulate : public tDynamicType
{
public:

  /**
   * Constructor.
   */
  tDynamicParticulate ();

  /**
   * Constructor.
   */
  tDynamicParticulate (stSize n, const double * data);

  /**
   * Destructor.
   */
  virtual ~tDynamicParticulate ();

  /**
   * Clones the object.
   */
  virtual tDynamicParticulate * Clone ();

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
   * Sizes or resizes the object
   */
  void SetSize (stSize n);

  /**
   * Returns the size of the histogram
   */
  stSize GetSize ();

  /**
   * Returns an attribute value
   */
  double GetData (stSize index);

  /**
   * Sets the attributes values
   */
  void SetData (stSize n, const double * data = NULL);

  /**
   * Sets the attributes values
   */
  void SetParticle (stSize position, double data);
};

#endif	/* STDYNAMICPARTICULATE_H */

