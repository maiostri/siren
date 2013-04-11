/**
 * @file
 *
 * This file implements the data dictionary access.
 * @image html datadictionary.png "Data Dictionary Schema"
 *
 * @version 1.0
 *
 * @author Maria Camila Nardini Barioni (mcamila@icmc.usp.br)
 * @todo Documentation review and tests.
 */
//--------------------------------------------------------------------------
//
#ifndef __DATA_DICTIONARY_ACCESS_H
#define __DATA_DICTIONARY_ACCESS_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <artemis/image/ImageBase.hpp>

#include "exceptions/SirenExceptions.h"

#include "utils/logUtils.h"
#include "utils/imageUtils.h"

#include "db/conn/ConnOracle.h"
#include "db/conn/ConnPostgres.h"

#include "db/query/QueryPostgres.h"
#include "db/query/QueryOracle.h"

#include "Lex.h"
#include "MetricTreeManager.h"
#include "Parser.h"
#include "sirenqueries/SirenQueries.h"
#include "sirenresult/SirenResult.h"


#define MASTER 0
#define MICRO_PER_SECOND 1000000

//class MetricTreeManager;
class Parser;
//class Token;
class tDynamicObject;
class tDynamicDistanceEvaluator;

class Lex;
//mudei aqui

using namespace Connections;
using namespace std;
using namespace log4cpp;

/**
 * This class implements the data dictionary access.
 *
 * @author Maria Camila Nardini Barioni (mcamila@icmc.usp.br)
 */
class DataDictionaryAccess
{
public:
  /**
   * DataDictionaryAccess constructor!!!
   */
  DataDictionaryAccess (eConnections connectionsEnum, const string& username, const string& password,
                        MetricTreeManager *TreeManager, Parser *p);

  /**
   * DataDictionaryAccess destructor!!!
   */
  ~DataDictionaryAccess ();

  /**
   * Drops the database schema.
   */
  void DropSchema (vector<string> *commandlist);

  /**
   * Creates the schema in the database.
   */
  void CreateSchema (vector<string> *commandlist);

  /**
   * Verifies the existence of an extractor.
   * @param extractorname name of the extractor
   * @return 0 if the extractor does not exist, > 1 = ExtractorCode
   */
  int IsExtractor (string extractorname);

  /**
   * Verifies if the extractor is valid for the metric type.
   * @param extractorname name of the extractor
   * @param tokenlist token list
   * @return true or false
   */
  bool IsValidExtractor (string extractorname, Token *tokenlist);

  /**
   * Verifies the existence of an extractor parameter.
   * @param extractorcode code that identifies an extractor
   * @param parametername name of a features vector parameter
   * @returns true if the parameter exists and false if not
   */
  bool IsExtractorParameter (int extractorcode, string parametername);

  /**
   * Verifies the existence of a metric.
   * @param metricname name of the metric
   * @return 0 if the metric does not exist, > 1 = MetricCode
   */
  int IsMetric (string metricname);

  /**
   * Checks if a metric is a particulate metric.
   * @param metricname name of the metric
   * @return boolean
   */
  bool IsParticulateMetric (string metricname);

  /**
   * Verifies if attribname is an image attribute. It is used in the create table statement.
   * @param tokenlist token list
   * @param attribname attribute name
   * @return true if the attribute is an image attribute and false if it is not
   */
  bool IsStillimageAttribute (Token *tokenlist, string attribname);

  /**
   * Verifies if attribname is a particulate attribute. It is used in the create table statement.
   * @param tokenlist token list
   * @param attribname attribute name
   * @return true if the attribute is a particulate attribute and false if it is not
   */
  bool IsParticulateAttribute (Token *tokenlist, string attribname);

  /**
   * Verifies if attribname is an audio attribute. It is used in the create table statement.
   * @param tokenlist token list
   * @param attribname attribute name
   * @return true if the attribute is an audio attribute and false if it is not
   */
  bool IsAudioAttribute (Token *tokenlist, string attribname);

  /**
   * Verifies if attribname is a complex attribute reference. It is used in the create index and in the select statements.
   * @param tablename table name
   * @param attribname attribute name
   * @return if the attribute is complex then the procedure returns the attribute type else it returns ""
   */
  string IsComplexAttributeReference (string tablename, string attribname);

  /**
   * Verifies if imgattribname is an image attribute.
   * @param tablename table name
   * @param imgattribname image attribute name
   * @return true if the attribute is an image attribute and false if it is not
   */
  bool IsStillimageAttributeReference (string tablename, string imgattribname);

  /**
   * Verifies if attribname is an image attribute. It is used in the where clause of the select statement.
   * @param attribname attribute name
   * @return true if the attribute is an image attribute and false if it is not
   */
  bool IsStillimageAttributeReference (string attribname);

  /**
   * Verifies if tablenameref is an IPV$Table. It is used in the procedure "NearAndFarPredicates".
   * @param tablenameref table name reference
   * @return true if the tablenameref is an IPV$Table and false if it is not
   */
  bool IsIPVTableNameReference (string tablenameref);

  /**
   * Verifies if a given metric is valid.
   * @param metricname metric name
   * @param tablename table name
   * @param attribname attribute name
   * @return "0" if the metric is invalid and the "metric_code" if it is valid
   */
  int IsValidMetric (string metricname, string tablename, string attribname);

  /**
   * Verifies if a given index is an index for a complex attribute.
   * @param caindex index name
   * @return "0" if the index is a complex attribute index and ">0" if it is
   */
  bool IsComplexAttribIndex (string caindex);

  /**
   * Gets the default metric.
   * @param tablename table name
   * @param attribname attribute name
   * @return "" if doesn't exists a default metric and the "metric_name" if it exists
   */
  string GetDefaultMetric (string tablename, string attribname);

  /**
   * Gets the type of a metric.
   * @param metriccode metric code
   * @returns the type of a metric
   */
  string GetMetricType (int metriccode);

  /**
   * Gets the metriccode of a metric.
   * @param metricname metric name
   * @returns metric code
   */
  int GetMetricCode (string metricname);

  /**
   * Gets the metrics.
   * @param tablename table name
   * @param attribname attribute name
   * @param metricreflist list of metrics associated with a complex attribute
   * @param complexattriblist list of complex attributes associated with some metric
   * @return "" if doesn't exists a default metric and the "metric_name" if it exists
   */

  void GetMetrics (string tablename, string attribname, vector<string> *complexattriblist,
                   vector<string> *metricreflist);

  /**
   * Gets the index file name.
   * @param tablename table name
   * @param attribname image attribute name
   * @param metricname metric name
   * @return the index file name
   */
  string GetIndexFile (string tablename, string attribname, string metricname);

  /**
   * Gets the image attributes of a table.
   * @param tablename table name
   * @return an image attribute list of table name
   */

  vector<string> * GetStillimageAttribute (string tablename);

  /**
   * Gets the particulate attributes of a table.
   * @param tablename table name
   * @return a particulate attribute list of table name
   */
  vector<string> * GetParticulateAttribute (string tablename);

  /**
   * Gets all the attributes of a table.
   * @param tablename table name
   * @return a attribute name list for the tablename
   */
  vector<string> * GetAttributes (string tablename);

  /**
   * Check if the attribute is an complex attribute in the tables in the statement.
   * @param tokenlist token list
   * @param attribname image attribute name
   * @return 
   *      Complex or not.
   */
  bool verifyComplexFieldStatement (Token *tokenlist, const string& attribname);


  /**
   * Check if the attribute is an complex attribute in the tables in the statement.
   * @param tokenlist token list
   * @param attribname image attribute name
   * @return 
   *      The table name.
   */
  string getTableNameFromTokenList (Token *tokenlist, string attribname);

  /**
   * Gets the P of a Lp distance function.
   * @param tablename table name
   * @param attribname image attribute name
   * @param metricname metric name
   * @return the P of a Lp distance function
   */
  int GetDistanceFunction (string tablename, string attribname, string metricname);

  /**
   * Gets the parameter names and parameter types of a given metric
   * @param metriccode metric code
   * @param a list of parameter names and parameter types
   */
  void GetParameters (int metriccode, vector<string> *attriblist);

  

  /**
   * Drop the index built for a complex attribute.
   * @param indexname index name
   */
  void DropComplexIndex (string indexname);

  /**
   * Sets "CDD$ComplexAttribMetric.IsDefault = false" for all the metrics of an specified image attribute.
   * @param tablename table name
   * @param imageattr image attribute name
   */
  void UpdateTableComplexAttribMetric (string tablename, string imageattr);

  /**
   * Starts a transaction.
   */
  //void StartTransaction ();
  /**
   * Commits the transaction.
   */
  void CommitTransaction ();

  /**
   * Rolls back the transaction.
   */
  void RollbackTransaction ();

  /**
   * Runs an SQL statement mounted on a string.
   */
  string RunSQL (string sql);

  /**
   * Runs an SQL statement from the tokenlist
   */
  string RunSQL (Token *tokenlist);

  /**
   * Runs a SQL statement and passes de filename as the first parameter of the sql.
   */
  string RunInsertFile (string sql, string filename);

  /**
   * Create the IPV$ tables.
   */
  void CreateTableStillimage (Token *tokenlist, vector<string> *createlist);

  /**
   * Create the APV$ tables.
   */
  void CreateTableAudio (Token *tokenlist, vector<string> *createlist);

  /**
   * Create the insert statement for the CDD$ComplexAttribute.
   * @param commandlist command list
   * @param tokenlist token list. It is used to get the table name
   * @param complexattribname complex attribute name
   * @param complexattribtype complex attribute type (stillimage or particulate)
   */
  void CreateTableInsertComplexAttribute (vector<string> *commandlist, Token *tokenlist,
                                          string complexattribname, string complexattribtype);

  /**
   * Create the insert statement for the CDD$ComplexAttribMetric.
   * @param tokenlist token list
   * @param tablename table name
   * @param commandlist command list
   * @param complexattribname complex attribute name
   * @param metriccode code that identifies a metric
   * @param isdefault value that specifies if a metric is default or not
   */
  void CreateTableInsertComplexAttribMetric (Token *tokenlist, string tablename,
                                             vector<string> *commandlist, string complexattribname, int metriccode, bool isdefault);

  /**
   * Alters the IPV$ or APV$ table. Adds one attribute for each extractor of the specified metric.
   * @param attribtype attribute type (stillimage or audio)
   * @param tablename table name
   * @param commandlist command list
   * @param complexattribname complex attribute name
   * @param metriccode code that identifies a metric
   * @param complexattriblist list of complex attributes associated with some metric
   * @param metricreflist list of all metrics associated with a complex attribute
   */
  void CreateTableAlterPVTable (string attribtype, string tablename, vector<string> *commandlist,
                                string complexattribname, int metriccode, vector<string> *complexattriblist,
                                vector<string> *metricreflist);

  /**
   * Alters the "select *" statement when the table(s) listed in the from clause has image attributes.
   * @param tokenlist token list
   * @param scope scope
   * @return true if the select statement has an "*" in the select list and false if it has not
   */
  bool SelectAsteriskStatement (Token *tokenlist, int scope);

  /**
   * Alters the "select ColumnRefList" statement when the table(s) listed in the from clause has image attributes.
   * @param tokenlist token list
   * @param scope scope
	 * @return "" if the command is correct and an error message if not
   */
  string SelectColumnRefListStatement (Token *tokenlist, int scope);

  /**
   * Alters the where clause according to the predicate used ("near" or "far").
   * @param tokenlist token list
   * @param activetokenlist active token list
   * @return "" if the command is correct and an error message if not
   */
  void NearAndFarPredicates (Token **tokenlist, int activetokenlist);

  /**
   * Build selection objects.
   * @param activelisttail active list tail
   * @param filequeryobject file query object
   * @param predicate predicate type (NEAR or FAR)
   * @param tablename table name
   * @param attribname image attribute name
   * @param metricname metric name
   * @param filename image file name
   * @param attribtype attribute type (stillimage or particulate)
   * @param parameterlist parameter list (used when the query object is a particulate object)
   * @param parametervaluelist parameter value list (used when the query object is a particulate object)
   * @param paramcount number of parameters
   * @param parametertypelist parameter type list (used when the query object is a particulate object)
   * @param returncount number of objects in the return
   * @return list of tDynamicObject
   */
  vector<tDynamicObject *> *BuildSelectionObjects (string tablename, string attribname, string metricname,
                                                   string filename, string subselect, string attribtype,
                                                   vector<string> * parameterlist, vector<string> parametervaluelist, int parametercount,
                                                   int &returncount);

  /**
   * Processes the range and nearest neighbour queries.
   * @param activelisttail active list tail
   * @param filequeryobject file query object
   * @param predicate predicate type (NEAR or FAR)
   * @param tablename table name
   * @param attribname image attribute name
   * @param sgtype similarity grouping type
   * @param metricname metric name
   * @param imagefilename image file name
   * @param knn k value for the knn query
   * @param range range value for the range query
   * @param tielist specifies if a tie list must be used
   * @param obj object list
   * @param numberofobjects number of objects in parameter obj
   * @return "" if the command is correct and an error message if not
   */
  void ProcessNearAndFarPredicates (Token **activelisttail, Token *filequeryobject, string predicate,
                                    string tablename, string attribname, string sgtype, string metricname,
                                    string imagefilename, string knn, string range, bool tielist, vector<tDynamicObject*> * obj,
                                    int numberofobjects);

  /**
   * Processes the range and nearest neighbour join queries.
   * @param activelisttail active list tail
   * @param filequeryobject file query object
   * @param predicate predicate type (NEAR or FAR)
   * @param tablename table name
   * @param tablename2 second table name
   * @param attribname image attribute name
   * @param attribname2 second image attribute name
   * @param sgtype similarity grouping type
   * @param metricname metric name
   * @param imagefilename image file name
   * @param jpgimage jpg image list
   * @param jpgimagecount jpg image list count
   * @param knn k value for the knn query
   * @param range range value for the range query
   * @param tielist specifies if a tie list must be used
   * @param attribtype attribute type (stillimage or particulate)
   * @param paramcount number of parameters
   * @param parameterlist parameter list (used when the query object is a particulate object)
   * @param parametervaluelist parameter value list (used when the query object is a particulate object)
   * @param parametertypelist parameter type list (used when the query object is a particulate object)
   * @return "" if the command is correct and an error message if not
   */
  string ProcessJoinQueries (Token **activelisttail, int activetokenlist, string predicate,
                             string tablename, string tablename2, string attribname, string attribname2,
                             string sgtype, string metricname, string knn, string range, bool tielist,
                             string attribtype, vector<string> * parameterlist,
                             vector<string> * parametervaluelist, vector<string> * parametertypelist);

  /**
   * Build a particulate object based on its parameters.
   * @param partid object id
   * @param UserParamList the list of params informed by the user in the sentence
   * @param UserParamValueList the values of the params informed by the user in the sentence
   * @param MetricParamList the list of the params in the metric
   * @param MetricParamTypes the list of types of the params in the metric
   * @return the object
   */

  tDynamicObject * BuildParticulateObject (const int& partid, const vector<string>& UserParamList,
                                           const vector<string>& UserParamValueList, const vector<string>& MetricParamList,
                                           const vector<string>& MetricParamTypes);

  /**
   * Verifyes if some attribute is missing in a select statement.
   * @param tablename name of the table
   * @param InsertColList list of columns passed in an insert
   * @param metricname name of the metric
   * @return the name of the missing attribute or "" if its ok
   */
  //string CheckSelectParticles(string tablename, vector<string> *InsertColList);
  string CheckSelectParticles (string tablename, string metricname,
                               const vector<string>& InsertColList);

  /**
   * Verifyes if some attribute is missing in a insert statement.
   * @param tablename name of the table
   * @param InsertColList list of columns passed in an insert
   * @return the name of the missing attribute or "" if its ok
   */
  string CheckInsertParticles (string tablename, vector<string> *InsertColList);

  /**
   * Identified the particulate objects and inserts them in the metric trees. Used by the Parser insert statement.
   * @param tablename name of the table
   * @param partid the id of the object particulate
   * @param InsertColList list of columns passed in an insert
   * @param InsertValueList list of values to be inserted
   * @param commandlist pointer to the string list that will receive the sql strings
   */
  void InsertParticulate (const string& tablename, const int& partid, const vector<string>& InsertColList,
                          const vector<string>& InsertValueList);

  

  /**
   * Insert a given particulate object in the metric trees.
   * @param tablename name of the table
   * @param partid the id of the object particulate
   * @param particulatename name of the particulate attribute
   * @param obj particulate object
   * @param commandlist pointer to the string list that will receive the sql strings
   */
  void IndexParticulateAttribute (const string& tablename, const int& partid, const string& particulatename,
                                  const vector<string> &InsertColList, const vector<string>& InsertValueList);

  /**
   * Insert a given image in the appropriate IPV$ table. Used by the Parser insert statement.
   * @param tablename name of the table
   * @param stillimagename name of the stillimage attribute
   * @param imagefile image file
   * @param imageid image id		 
   */
  int InsertImageIPV (string tablename, string stillimagename, string imagefile, long imageid);

  /**
   * Updates IPV Table storing the stillimage feature extracted
   */
  void UpdateIPVTableSetCharacteristic (string tablename, string complexattribname, string metricname,
                                        tDynamicObject *obj, vector<string> *commandlist);

  /**
   * Updates APV Table storing the audio feature extracted
   */
  void UpdateAPVTableSetCharacteristic (string tablename, string complexattribname, string metricname,
                                        tDynamicObject *obj, vector<string> *commandlist);

  /**
   * Builds the index files. Used by the create index statement.
   * @param CIAList current image attribute list
   * @param CM current metric
   * @param tablename name of the table
   */
  void BuildStillImageIndexFile (vector<string> *CIAList, string CM, string tablename,
                                 vector<string> *commandlist);

  /**
   * Builds the index files. Used by the create index statement.
   * @param CAAList current audio attribute list
   * @param CM current metric
   * @param tablename name of the table
   */
  void BuildAudioIndexFile (vector<string> *CAAList, string CM, string tablename,
                            vector<string> *commandlist);

  /**
   * Builds a tDynamicObject with regard to the table name, attribute name and a metric from a JPEG file.
   * @param tablename table name
   * @param complexattribname complex attribute name
   * @param metricname metric name
   * @param imagefilename JPG file name
   * @param oid object id
   * @returns an instance of tDynamicObject
   */
  tDynamicObject * BuildObjectFromJpgFile (string tablename, string complexattribname,
                                           string metricname, string imagefilename, long oid);

  /**
   * Builds a tDynamicObject with regard to the table name, attribute name and a metric from a JPEG in memory.
   * @param tablename table name
   * @param complexattribname stillimage attribute name
   * @param metricname metric name
   * @param jpg a JPG in main memory
   * @param oid object id
   * @returns an instance of tDynamicObject
   */
  tDynamicObject * BuildObjectFromJpg (string tablename, string complexattribname, string metricname,
                                       JPGImage *jpg, long oid);

  /**
   * Builds a tDynamicObject with regard to the table name, attribute name and a metric from a BMP file.
   * @param tablename table name
   * @param complexattribname stillimage attribute name
   * @param metricname metric name
   * @param imagefilename BMP file name
   * @param oid object id
   * @returns an instance of tDynamicObject
   */
  tDynamicObject * BuildObjectFromBmpFile (string tablename, string complexattribname,
                                           string metricname, string imagefilename, long oid);

  /**
   * Builds a tDynamicObject with regard to the table name, attribute name and a metric from a BMP in memory.
   * @param tablename table name
   * @param complexattribname stillimage attribute name
   * @param metricname metric name
   * @param src a BMP in main memory
   * @param oid object id
   * @returns an instance of tDynamicObject
   */
  tDynamicObject * BuildObjectFromImage (string tablename, string complexattribname, string metricname,
                                         Image *src, long oid);

  tDynamicObject * buildObjectFromImage (string& tablename, string& complexattribname,
                                         string& metricname, string& filename, long& oid, string& extractorName, string& parameterName);


  /**
   * Sets the weights of the extractors parameters.
   * @param evaluator Pointer to the metric evaluator
   * @param tablename table name
   * @param attribname image attribute name
   * @param metricname metric name
   */
  void SetWeights (tDynamicDistanceEvaluator *evaluator, string tablename, string attribname,
                   string metricname);
  
  
  SirenResult * runSelect(const string& sql);

  /**
   * Gets client folder
   */
  string GetClientFolder ();

  /**
   * Time measures variables
   */
  double MetricTreeTotalTime;
  double TempJoinInsertsTotalTime;
  double GroupSimObjManipulation;

  //---------MUDEI AQUI---------------------------
  //AnsiString NearAndFarPredicates (Token **tokenlist, int activetokenlist);
  //AnsiString QueryOptimization (Token **tokenlist, int activetokenlist);
  Token* QueryOptimization (Token **tokenlist, int activetokenlist);


  /* Returns a pointer to a SirenQueries object. Used for sequences outside the DataDicionary.
   */
  SirenQueries *getSirenQueries ();

private:

  /**
   * User name
   */
  string user;

  string sql;

  /**
   * Pointer to the lex analiser
   */
  Lex *lexlist;

  /**
   * Pointer to the its owner
   */
  Parser *parser;

  bool deletetreemanager;

  //--------------------------------------------------------
  // connection properties
  //--------------------------------------------------------

  /**
   * Pointer to the database connection
   *
   */
  Conn *SqlConnection;
  Query *query;
  Query *qry;
  eConnections connectionsEnum;

  /**
   * Handle to the metric tree manager passed in the constructor
   */
  MetricTreeManager *treemanager;

  /**
   * Scope of the temp join table TMP$tmpjoin.
   */
  int tempjointablescope;


  SirenQueries *sirenQueries;

  /**
   * Adds a token in the middle of a token list.
   * @returns A pointer to the next token
   */
  Token *AddToken (Token *position, string Lexem, int TokenType, int LexemType);

  /**
   * Adds a token in the middle of a token list.
   * @returns A pointer to the next token
   */
  Token *AddToken (Token *position, Token *newtoken);

  /**
   * Adds a new multimedia column reference (IPV$tablename_attribname.image) or (APV$tablename_attribname.audio).
   * @param tk1 auxiliary pointer to the token list
   * @param tk1_p auxiliary pointer to the token previous to tk1
   * @param tk2 auxiliary pointer to the token "from"
   * @param mmtabletype type of the multimedia table (IPV$ for image or APV$ for audio)
   * @param tablename table name
   * @param mmattribtype type of the multimedia attribute (image or audio)
   * @param attribname attribute name
   * @param mmattribid id of the multimedia attribute (image_id or audio_id)
   * @param aliasname alias name
   * @param scope scope
   */
  void AddMMColRef (Token **tk1, Token **tk1_p, Token **tk2, string mmtabletype, string tablename,
                    string mmattribtype, string attribname, string mmattribid, string aliasname, int scope);

  /**
   * 
   * @return 
   */
  tDynamicObject* buildFromParticulate ();

  /**
   * 
   * @return 
   */
  string buildFromStillImage ();

  /**
   * 
   * @param tablename
   * @param attribname
   * @param metricname
   * @return 
   */
  tDynamicObject* buildObjectFromStillImage (const string& tablename, const string& attribname,
                                             const string& metricname);

  /**
   * 
   * @param tablename
   * @param parametercount
   * @param metricname
   * @param attribname
   * @param parameterlist
   * @param parametervaluelist
   * @return 
   */
  vector<tDynamicObject *> buildObjectFromParticulate (const string& tablename,
                                                       const int& parametercount, const string& metricname, const string& attribname,
                                                       const vector<string>& parameterlist, const vector<string>& parametervaluelist);


};

#endif
