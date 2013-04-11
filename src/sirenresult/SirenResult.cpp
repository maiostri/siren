#include "SirenResult.h"

SirenResult::SirenResult(Query& query) {
	this->fieldsCount = query.FieldsCount();

	arrayHeaders = new vector<SirenResultHeader*>();

	// Create the headers.
	for (unsigned int i = 0; i < this->fieldsCount; i++) {
		SirenResultHeader *sirenResultHeader = new SirenResultHeader(query.FieldNameByIndex(i), query.getFieldType(i));
		arrayHeaders->push_back(sirenResultHeader);
	}

	matrixValues = new vector<vector<VariantType>*>();
	// Populate the data map
	while (!query.Eof()) {
		vector<VariantType> *arrayRow = new vector<VariantType>();
		for (unsigned int i = 0; i < fieldsCount - 1; i++) {
			if (arrayHeaders->at(i)->getType() == INTEGER) {
				arrayRow->push_back(query.IntFieldByIndex(i));
			} else if (arrayHeaders->at(i)->getType() == STRING) {
				arrayRow->push_back(query.StringFieldByIndex(i));
			} else if (arrayHeaders->at(i)->getType() == NUMBER) {
				arrayRow->push_back(query.DoubleFieldByIndex(i));
			} else if (arrayHeaders->at(i)->getType() == BLOB) {
				arrayRow->push_back(query.blobFieldDumpToCharByName(arrayHeaders->at(i)->getName()));
			}
		}
		matrixValues->push_back(arrayRow);
	}
}

SirenResult::~SirenResult() {
	delete this->matrixValues;
	delete this->arrayHeaders;
}

vector<SirenResultHeader *> * SirenResult::getSirenResultHeader() {
	return this->arrayHeaders;
}

unsigned int SirenResult::getNumberOfFields() {
	return this->matrixValues->size();
}

unsigned int SirenResult::getResultCount() {
	return this->matrixValues->size();
}
