/* 
 * File:   SirenResult.h
 * Author: ricardo
 *
 * Created on February 24, 2013, 10:24 PM
 */

#ifndef SIRENRESULT_H
#define	SIRENRESULT_H

#include <vector>

#include <boost/functional/hash.hpp>
#include <boost/variant.hpp>

#include "db/query/Query.h"
#include "db/query/DataTypes.h"
#include "SirenResultHeader.h"
#include "utils/sirenUtils.h"

using namespace boost;
using namespace std;

class SirenResult
{
private:
	typedef variant<int, string, char*> VariantType;
	vector<vector<VariantType >* > *matrixValues;
    vector<SirenResultHeader*> *arrayHeaders;
    unsigned int fieldsCount;

public:

    /**
     * Constructor.
     * @param query
     * 			Query to build the Siren Result struct.
     */
    SirenResult(Query& query);

    /**
     * Destructor.
     */
    virtual ~SirenResult();
    

    /**
     * Gets the header with fields info.
     * @return
     * 			The SirenResultHeader instance.
     */
    vector<SirenResultHeader*> * getSirenResultHeader();

    /**
     * Get the result count.
     * @return
     * 			The result count.
     */
    unsigned int getResultCount();

    /**
     * Get the number of the fields in the result.
     * @return
     * 			Number of fields.
     */
    unsigned int getNumberOfFields();

    /**
     * Gets the value of a field, based on its index and in a row index.
     * @param rowIndex
     * 			Row index.
     * @param fieldIndex
     * 			Field index.
     * @return
     * 			Field value.
     */
    template <typename T> T getFieldValue(const int& rowIndex, const int& fieldIndex) {
    	return boost::get<T>(this->matrixValues->at(rowIndex)->at(fieldIndex));

    }


};

#endif	/* SIRENRESULT_H */

