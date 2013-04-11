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

#include <fstream>
#include <map>

#include <boost/lexical_cast.hpp>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

#include "db/query/Query.h"
#include "db/conn/Conn.h"
#include "utils/logUtils.h"
#include "exceptions/SirenExceptions.h"

using namespace oracle::occi;
using namespace boost;
using namespace std;

class QueryOracle : public Query
{
private:
  Connection* conn;
  Statement* stmt;
  ResultSet *rs;

  // This map is used to get the value of field of the result set by its
  // name.
  map<string, int> *mapFieldsIndex;

public:

  QueryOracle (Connection* conn);

  ~QueryOracle ();

  void BlobFieldByIndex (const int& index);
  
  Blob blobFieldByName (const string& field);
  
  void blobFieldDumpToFileByIndex (const int& index, const string& filename);
  
  char* blobFieldDumpToCharByName(const string& field);
  
  int countRows (const string& table);

  int countRows ();
  
  void createSequence (const string& sequenceName, const int& startWith);
  
  double DoubleFieldByIndex (const int& index);
  
  void dropSequence (const string& sequence);
  
  void dropTable (const string& table);
  
  void dumpBlob (Blob& blob, const string& file);

  char * dumpBlobToChar (Blob& blob, int& size);

  int Eof ();
  
  int FieldIsNullByIndex (const int& index);
  
  string FieldNameByIndex (const int& index);
  
  int FieldsCount ();
  
  float FloatFieldByIndex (const int& index);
  
  eDataTypes getFieldType(const int& index);
    
  int IntFieldByIndex (const int& index);
  
  void mountSelectBlobs (const vector<string>& fieldNames, const string& tableName, const int& id);
  
  inline int Next ();
  
  void Open (const string& sql);
  
  void populateBlob (Blob& blob, char* data);

  void populateBlob (Blob& blob, const string& file);
  
  void populateBlobs (const vector<char *>& tDynamicObjectCharVector, const int& count, const string& fileName);

  void Run (const string& sql);

  void RunPLSQL (const string& sql);

  int runSequence (const string& sequenceName);

  void mountInsert (const string& tableName, const vector<string>& fieldNames, vector<char *>&
                    tDynamicObjectCharVector, const int& imageid, const string& fileName);

  void mountMap ();

  string StringFieldByIndex (const int& index);

};
#endif /*__QUERY_ORACLE_H*/


