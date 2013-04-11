/* 
 * File:   SirenResultHeader.h
 * Author: ricardo
 *
 * Created on February 25, 2013, 9:49 PM
 */

#ifndef SIRENRESULTHEADER_H
#define	SIRENRESULTHEADER_H

#include <string>
#include <boost/variant.hpp>
#include <artemis/image/ImageBase.hpp>

#include "db/query/DataTypes.h"

using namespace std;
using namespace boost;
using namespace DataTypes;

typedef variant<int, string, Image> VariantType;

class SirenResultHeader
{
public:
    SirenResultHeader(const string& name, const eDataTypes& type);
    virtual ~SirenResultHeader();
    
    const string getName();
    const eDataTypes getType();
    void setType(const eDataTypes& type);
    
private:
    
    string name;
    
    eDataTypes type;
    
};

#endif	/* SIRENRESULTHEADER_H */

