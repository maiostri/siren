/**
 * @file
 *
 * This file defines the operations for a query
 *
 * @version 1.0
 * @author Ricardo Pereira Maiostri (maiostri@gmail.com)
 * @date 11-03-2011
 */


#ifndef __QUERY_H
#define __QUERY_H

#include <string>
#include <exception>
#include <vector>

#include "DataTypes.h"

using namespace std;
using namespace DataTypes;

class Query
{
public:

    /**
     * Constructor.
     */
    Query();

    /**
     * Destructor.
     */
    virtual ~Query() = 0;

    /**
     * Dumps a blob to an array of char by its name.
     * @param field
     * @return 
     */
    virtual char* blobFieldDumpToCharByName(const string& field) = 0;

    /**
     * 
     * @param sequenceName
     * @param startWtih
     */
    virtual void createSequence(const string& sequenceName, const int& startWtih) = 0;

    /**
     * 
     * @param table
     * @return 
     */
    virtual int countRows(const string& table) = 0;

    /**
     * 
     * @param sql
     */
    virtual double DoubleFieldByIndex(const int& index) = 0;

    /**
     * 
     * @param sequence
     */
    virtual void dropSequence(const string& sequence) = 0;

    /**
     * 
     * @param table
     */
    virtual void dropTable(const string& table) = 0;

    /**
     * 
     * @return 
     */
    virtual int Eof() = 0;

    /**
     * 
     * @param index
     * @return 
     */
    virtual int FieldIsNullByIndex(const int& index) = 0;

    /**
     * 
     * @param index
     * @return 
     */
    virtual string FieldNameByIndex(const int& index) = 0;

    /**
     * 
     * @return 
     */
    virtual int FieldsCount() = 0;

    /**
     * 
     * @param index
     * @return 
     */
    virtual float FloatFieldByIndex(const int& index) = 0;

    /**
     * This functions returns the type of a result field.
     * @param index
     *            Index of the field in the query.
     * @return 
     *            The field type
     */
    virtual eDataTypes getFieldType(const int& index) = 0;

    virtual int IntFieldByIndex(const int& index) = 0;

    /**
     * Mounts the PL/SQL insert statement when there are blob fields.
     * @param tableName
     * @param fieldNames
     * @param tDynamicObjectCharVector
     */
    virtual void mountInsert(const string& tableName, const vector<string>& fieldNames, vector<char *>&
                             tDynamicObjectCharVector, const int& imageid, const string& fileName) = 0;

    virtual int Next() = 0;

    virtual void Open(const string& sql) = 0;

    virtual void Run(const string& sql) = 0;

    virtual void RunPLSQL(const string& sql) = 0;

    /**
     * Returns the next value of the sequence.
     * @param sequenceName
     *        The name of the sequence.
     */
    virtual int runSequence(const string& sequenceName) = 0;

    virtual string StringFieldByIndex(const int& index) = 0;

};

#endif /*__QUERY_H*/

