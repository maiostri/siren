/**
 * @file
 *
 * This file defines the queries operations for PostgreSQL.
 *
 * @version 1.0
 * @author Ricardo Pereira Maiostri (maiostri@gmail.com)
 * @date 11-03-2011
 */

#ifndef __QUERYPOSTGRES_H
#define __QUERYPOSTGRES_H

#include "db/query/Query.h"
#include "db/conn/Conn.h"

using namespace std;

class QueryPostgres : public Query
{
private:
  pqxx::result ret;
  pqxx::result::const_iterator row;
  const char *connection;
public:
  QueryPostgres (const char *connection);

  ~QueryPostgres () { };
  
  void blobFieldDumpToFileByIndex(const int& index, const string& filename);
  
  char* blobFieldDumpToCharByName(const string& field);
  
  void createSequence (const string& sequenceName, const int& startWith);
  
  int countRows(const string& table);
  
  void dropTable (const string& table);

  double DoubleFieldByIndex (const int& index);
  
  void dropSequence (const string& sequence);
  
  int Eof ();
  
  int FieldsCount ();
  
  int FieldIsNullByIndex (const int& index);

  string FieldNameByIndex (const int& index);

  float FloatFieldByIndex (const int& index);
  
  eDataTypes getFieldType(const int& index);
  
  int IntFieldByIndex (const int& index);

  void mountInsert (const string& tableName, const vector<string>& fieldNames, vector<char *>&
                    tDynamicObjectCharVector, const int& imageid, const string& fileName);

  int Next ();
  
  void Open (const string& sql);

  void Run (const string& sql);

  void RunPLSQL (const string& sql);
  
  int runSequence (const string& sequenceName);

  string StringFieldByIndex (const int& index);


};

#endif /*__QUERY_POSTGRES_H*/




