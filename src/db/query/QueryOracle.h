/**
 * @file
 *
 * This file defines the queries operations for Oracle.
 *
 * @version 1.0
 * @author Ricardo Pereira Maiostri (maiostri@gmail.com)
 * @date 11-03-2011
 */

//@TODO Create doxygen. Use the same order that .cpp file.

#ifndef __QUERY_ORACLE_H
#define __QUERY_ORACLE_H


#include "db/query/Query.h"
#include "db/conn/Conn.h"

#include <fstream>
#include <map>

#include <boost/lexical_cast.hpp>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

using namespace oracle::occi;
using namespace log4cpp;
using namespace boost;
using namespace std;

class QueryOracle : public Query {
private:
		Connection* conn;
		Statement* stmt;
		ResultSet *rs;
		Category *root;

		// This map is used to get the value of field of the result set by its
		// name.
		map<string, int> *mapFieldsIndex;

public:

		QueryOracle(Connection* conn);

		~QueryOracle();

		void Open(const string& sql);

		void Run(const string& sql);

		void First();

		int Next();

		int Eof();

		string StringFieldByIndex(const int& index);

		int IntFieldByIndex(const int& index);

		float FloatFieldByIndex(const int& index);

		double DoubleFieldByIndex(const int& index);

		void BlobFieldByIndex(const int& index);

		void RunPLSQL(const string& sql);

		int FieldsCount();

		string FieldNameByIndex(const int& index);

		int FieldIsNullByIndex(const int& index);

		int countRows(const string& table);

		int countRows();

		void dropTable(const string& table);

		void createSequence(const string& sequenceName, const int& startWith);

		void dropSequence(const string& sequence);

		void createOrReplaceDirectory(const string& directoryName, const string& location);

		void dumpBlob(Blob& blob, const string& file);

		void populateBlob(Blob& blob, char* data);

		void populateBlob(Blob& blob, const string& file);

		char * dumpBlobToChar(Blob& blob);

		int runSequence(const string& sequenceName);

		void mountInsert(const string& tableName, const vector<string>& fieldNames, vector<char *>&
						tDynamicObjectCharVector);

		void mountMap();

		Blob blobFieldByName(const string& field);

		char * blobFieldDumpToCharByName(const string& field);
        
        


};
#endif /*__QUERY_ORACLE_H*/


