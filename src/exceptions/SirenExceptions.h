/* 
 * File:   SirenExceptions.h
 * Author: ricardo
 *
 * Created on February 16, 2013, 10:09 AM
 */

#ifndef SIRENEXCEPTIONS_H
#define	SIRENEXCEPTIONS_H

#include <string>
#include <stdexcept>

using namespace std;

class ErrorReadingBlobException: public runtime_error {
public:
	ErrorReadingBlobException(const string& arg);
};

class FileNotFoundException: public runtime_error {
public:
	FileNotFoundException(const string& arg);
};

class IdentifierExpectedException: public runtime_error {
public:
	IdentifierExpectedException();
};

class IncorrectEndOfStatementException: public runtime_error {
public:
	IncorrectEndOfStatementException();
};

class IntegerExpectedException: public runtime_error {
public:
	IntegerExpectedException();
};

class InvalidComplexAttributeException: public runtime_error {
public:
	InvalidComplexAttributeException(const string& arg);
};

class InvalidExtractorException: public runtime_error {
public:
	InvalidExtractorException();
};

class InvalidCommandException: public runtime_error {
public:
	InvalidCommandException(const string& arg);
};

class InvalidIdentifierException: public runtime_error {
public:
	InvalidIdentifierException(const string& arg, const string& arg2);
};

class InvalidMetricException: public runtime_error {
public:
	InvalidMetricException(const string& arg);
};

class InvalidOperatorException: public runtime_error {
public:
	InvalidOperatorException(const string& arg);
};

class InvalidParameterReferenceException: public runtime_error {
public:
	InvalidParameterReferenceException(const string& arg);
};

class InvalidParameterTypeException: public runtime_error {
public:
	InvalidParameterTypeException(const string& arg, const string& arg2);
	InvalidParameterTypeException();
};

class InvalidPredicateException: public runtime_error {
public:
	InvalidPredicateException(const string& arg);
};

class InvalidSimilarityGroupingTypeException: public runtime_error {
public:
	InvalidSimilarityGroupingTypeException(const string& arg);
};

class MissingOrExtraParticulateMetricParameterException: public runtime_error {
public:
	MissingOrExtraParticulateMetricParameterException();
};

class MissingOrInvalidParameterTypeException: public runtime_error {
public:
	MissingOrInvalidParameterTypeException();
};

class MissingOrInvalidParameterNameException: public runtime_error {
public:
	MissingOrInvalidParameterNameException();
};

class MissingParameterException: public runtime_error {
public:
	MissingParameterException(const string& arg);
};

class MissingParticleForParticulateAttributeException: public runtime_error {
public:
	MissingParticleForParticulateAttributeException(const string& arg, const string& arg2);
};

class MissingStopAfterOrRangeException: public runtime_error {
public:
	MissingStopAfterOrRangeException();
};

class NoDefaultMetricDefinedException: public runtime_error {
public:
	NoDefaultMetricDefinedException(const string& arg, const string& arg2);
};

class OperatorExpectedException: public runtime_error {
public:
	OperatorExpectedException();
};

class OperatorWithoutComplexAttributeException: public runtime_error {
public:
	OperatorWithoutComplexAttributeException(const string& arg);
};

class QueryReturnedZeroRecordsException: public runtime_error {
public:
	QueryReturnedZeroRecordsException(const string& arg);
};

#endif	/* SIRENEXCEPTIONS_H */

