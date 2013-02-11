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

  void Open (const string& sql);

  void Run (const string& sql);

  void RunPLSQL (const string& sql);

  void First ();

  int Next ();

  int FieldsCount ();

  int Eof ();

  string StringFieldByIndex (const int& index);

  int IntFieldByIndex (const int& index);

  float FloatFieldByIndex (const int& index);

  double DoubleFieldByIndex (const int& index);

  string FieldNameByIndex (const int& index);

  int FieldIsNullByIndex (const int& index);

  int countRows (const string& table);

  void dropTable (const string& table);

  void createSequence (const string& sequenceName, const int& startWith);

  void dropSequence (const string& sequence);

  void mountInsert (const string& tableName, const vector<string>& fieldNames, vector<char *>&
                    tDynamicObjectCharVector);

  char * blobFieldDumpToCharByName (const string& field);
  
  int runSequence(const string& sequenceName);

};

#endif /*__QUERY_POSTGRES_H*/




