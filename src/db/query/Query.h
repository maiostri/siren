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

using namespace std;

class Query
{
public:

  Query ();

  virtual ~Query () = 0;

  /**
   * Dumps the blob field in the query to a array of char.
   * @param field
   *        The blob's field name.
   * @return 
   *        The array of char.
   */
  virtual char * blobFieldDumpToCharByName (const string& field) = 0;

  virtual void createSequence (const string& sequenceName, const int& startWtih) = 0;

  /**
   * 
   * @param table
   * @return 
   */
  virtual int countRows (const string& table) = 0;

  /**
   * 
   * @param sql
   */
  virtual double DoubleFieldByIndex (const int& index) = 0;

  virtual void dropSequence (const string& sequence) = 0;

  virtual void dropTable (const string& table) = 0;

  virtual int Eof () = 0;

  virtual string FieldNameByIndex (const int& index) = 0;

  virtual int FieldsCount () = 0;

  virtual void First () = 0;

  virtual float FloatFieldByIndex (const int& index) = 0;

  virtual int IntFieldByIndex (const int& index) = 0;

  /**
   * Mounts the PL/SQL insert statement when there are blob fields.
   * @param tableName
   * @param fieldNames
   * @param tDynamicObjectCharVector
   */
  virtual void mountInsert (const string& tableName, const vector<string>& fieldNames, vector<char *>&
                            tDynamicObjectCharVector) = 0;

  virtual int Next () = 0;

  virtual void Open (const string& sql) = 0;

  virtual void Run (const string& sql) = 0;

  virtual void RunPLSQL (const string& sql) = 0;
  
  /**
   * Returns the next value of the sequence.
   * @param sequenceName
   *        The name of the sequence.
   */
  virtual int runSequence(const string& sequenceName) = 0;

  virtual string StringFieldByIndex (const int& index) = 0;

  virtual int FieldIsNullByIndex (const int& index) = 0;

};

#endif /*__QUERY_H*/

