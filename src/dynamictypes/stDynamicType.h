/**
 * @file
 *
 * This file implements the dynamic object types
 */
#ifndef __STDYNAMICTYPE_H
#define __STDYNAMICTYPE_H

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <math.h>

#include <arboretum/stBasicObjects.h>
#include <artemis/Artemis.hpp>

/**
 * Pure abstract class tDynamicType defines the Distance, Clone and other methods needed
 * for a dynamic type.
 * @author Humberto Razente
 * @author Maria Camila Barioni
 */
class tDynamicType
{
protected:
    /**
     * Pointer to the data vector
     */
    double * Data;

    /**
     * Size of the data vector
     */
    stSize Size;


public:
    /**
     * Clones the object.
     */
    virtual tDynamicType *Clone() = 0;

    /**
     * Gets the object serialized size
     */
    virtual stSize GetSerializedSize() = 0;

    /**
     * Returns the object serialization
     */
    virtual const stByte * Serialize() = 0;

    /**
     * Restores the serialized object.
     */
    virtual void Unserialize(const stByte * data, stSize dataSize) = 0;

    /**
     * Tests if one object is equal to another.
     */
    virtual bool IsEqual(tDynamicType * obj) = 0;

    /**
     * Destructor.
     */
    virtual ~tDynamicType() = 0;

    /**
     * Returns a pointer to char with the data. Used to storage in database.
     */
    virtual char * convertDataToChar();

    virtual void setData(double *data);

};


#endif
