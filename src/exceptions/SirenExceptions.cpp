/* 
 * File:   SirenExceptions.cpp
 * Author: ricardo
 * 
 * Created on February 16, 2013, 10:09 AM
 */

#include "SirenExceptions.h"


ErrorReadingBlobException::ErrorReadingBlobException(const string& arg) :
		runtime_error("Error on reading the blob from database: " + arg) {
}

FileNotFoundException::FileNotFoundException(const string& arg) :
		runtime_error("File not found: " + arg) {
}

IdentifierExpectedException::IdentifierExpectedException() :
		runtime_error("Identifier expected.") {
}

IncorrectEndOfStatementException::IncorrectEndOfStatementException() :
		runtime_error("Incorrect end of statement.") {
}

IntegerExpectedException::IntegerExpectedException() :
		runtime_error("Integer expected.") {
}

InvalidIdentifierException::InvalidIdentifierException(const string& arg, const string& arg2) :
		runtime_error("Expected " + arg + " found " + arg2 + ".") {
}

InvalidComplexAttributeException::InvalidComplexAttributeException(const string& arg) :
		runtime_error("Invalid complex attribute: " + arg) {
}

InvalidCommandException::InvalidCommandException(const string& arg) :
		runtime_error("Invalid command: " + arg) {
}

InvalidExtractorException::InvalidExtractorException() :
		runtime_error("Missing or invalid extractor.") {
}

InvalidMetricException::InvalidMetricException(const string& arg) :
		runtime_error("Invalid metric: " + arg) {
}

InvalidOperatorException::InvalidOperatorException(const string& arg) :
		runtime_error("Invalid operator: " + arg) {
}

InvalidParameterReferenceException::InvalidParameterReferenceException(const string& arg) :
		runtime_error("Invalid parameter reference: " + arg) {
}

InvalidParameterTypeException::InvalidParameterTypeException(const string& arg, const string& arg2) :
		runtime_error("Invalid parameter type. Expected: " + arg + ". Found: " + arg2) {
}

InvalidParameterTypeException::InvalidParameterTypeException() :
		runtime_error("Invalid parameter type.") {
}

InvalidPredicateException::InvalidPredicateException(const string& arg) :
		runtime_error("Invalid predicate: " + arg) {
}

InvalidSimilarityGroupingTypeException::InvalidSimilarityGroupingTypeException(const string& arg) :
		runtime_error("Invalid similarity grouping: " + arg) {
}

MissingOrExtraParticulateMetricParameterException::MissingOrExtraParticulateMetricParameterException() :
		runtime_error("Missing or extra particulate metric parameter(s)") {
}

MissingOrInvalidParameterNameException::MissingOrInvalidParameterNameException() :
		runtime_error("Missing or invalid parameter name.") {
}

MissingOrInvalidParameterTypeException::MissingOrInvalidParameterTypeException() :
		runtime_error("Missing or invalid parameter type.") {
}

MissingParameterException::MissingParameterException(const string& arg) :
		runtime_error("Missing parameter: " + arg) {
}

MissingParticleForParticulateAttributeException::MissingParticleForParticulateAttributeException(const string& arg,
		const string& arg2) :
		runtime_error("The particle " + arg + " of the metric " + arg2 + " must be informed.") {
}

MissingStopAfterOrRangeException::MissingStopAfterOrRangeException() :
		runtime_error("Missing 'stop after' or 'range' clausule in the statement.") {
}

NoDefaultMetricDefinedException::NoDefaultMetricDefinedException(const string& arg, const string& arg2) :
		runtime_error("There isn't a default metric defined for " + arg + "." + arg2 + ".") {
}

OperatorWithoutComplexAttributeException::OperatorWithoutComplexAttributeException(const string& arg) :
		runtime_error("The operator: " + arg + " can't be used without a complex attribute") {
}

OperatorExpectedException::OperatorExpectedException() :
		runtime_error("Operator expected.") {
}

QueryReturnedZeroRecordsException::QueryReturnedZeroRecordsException(const string& arg) :
		runtime_error("Query returned zero records: " + arg) {
}

