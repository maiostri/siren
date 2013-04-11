/* 
 * File:   SirenResultHeader.cpp
 * Author: ricardo
 * 
 * Created on February 25, 2013, 9:49 PM
 */

#include "SirenResultHeader.h"

SirenResultHeader::SirenResultHeader(const string& name, const eDataTypes& type)
{
    this->name = name;
    this->type = type;
}

SirenResultHeader::~SirenResultHeader() {
    
}

const string SirenResultHeader::getName() {
    return this->name;
}

const eDataTypes SirenResultHeader::getType() {
    return this->type;
}

void SirenResultHeader::setType(const eDataTypes& type) {
    this->type = type;
}
