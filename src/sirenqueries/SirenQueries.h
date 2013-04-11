/* 
 * File:   SirenQueries.h
 * Author: Ricardo Maiostri
 * Email:  rmaiostri@grad.icmc.usp.br
 *
 * This class contains common sql queries used in the DataDicionary.
 * Since the mapping of the results for some queries is too complex, the Query object will be returned,
 * and manipulated in DataDicionaryAccess.
 * @TODO Consider using a ResultSet class. In this way, the DataDicionary won't know the Query classes, and
 * won't be affected by Query code changes.
 * We are doing this to encapsulate all the SQL statements in one class, and let the DataDicionary
 * as an abstraction layer to manipulate the results.
 * 
 */

#ifndef SIRENQUERIES_H
#define	SIRENQUERIES_H

#include <string>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "db/conn/Conn.h"
#include "db/conn/ConnPostgres.h"
#include "db/conn/ConnOracle.h"
#include "db/query/Query.h"
#include "db/query/QueryPostgres.h"
#include "db/query/QueryOracle.h"

using namespace boost;
using namespace std;
using namespace Connections;

class SirenQueries
{
private:
  Conn *conn;
  Query *query;

  // This vector register all the sql statements executed for the SirenQueries instance.
  vector<string> *sqlStatements;
  
  eConnections connectionsEnum;
  
public:

  /**
   * Constructor.
   * @param connectionsEnum
   *          Which SGBD to use.
   * @param username
   *          The username.
   * @param password
   *          The password.
   */
  SirenQueries (eConnections connectionsEnum, const string& username, const string& password);


  /**
   * Destructor.
   */
  virtual ~SirenQueries ();

  /**
   * 
   * @param tablename
   * @param metricname
   * @param InsertColList
   * @return 
   */
  string checkSelectParticles (const string& tablename, const string& metricname,
                               vector<string>& InsertColList);


  /**
   * This functions will insert in the database a metric.
   * @param metricname
   *        Metric name.
   * @param lpp
   *        The distance function.
   * @param metrictype
   *        The metric type.
   * @return 
   *        The code of the metric.   
   */
  int createMetricInsertMetric (const string& metricname, const int& lpp, const string& metrictype);


  /**
   * This function will insert in the database a parameter metric, with the functions parameters as values.
   * @param metriccode
   *            Code of the metric.
   * @param extractorcode
   *            Code of the extractor.
   * @param parametername
   *            Name of the parameter.
   * @param parameteralias
   *            Alias of the parameter.
   * @param parametertype
   *            Type of the parameter.
   * @param weight
   *            Weight of the parameter.
   * @param createlist
   */
  void createMetricInsertParameter (const int& metriccode, const int& extractorcode, const string& parametername,
                                    const string& parameteralias, const string& parametertype, const float& weight, vector<string> *createlist);

  
  /**
   * Create the insert statement for the CDD$MetricInstance.
   * @param tablename table name
   * @param complexattribname complex attribute name
   * @param metriccode code that identifies a metric
   * @param parametername parameter name
   * @param extractorcode extractor code
   * @param parameterinstance attribute associated with a parameter in the metric column constraint
   */
  void createTableInsertMetricInstance(const string& tablename,
                                                      const string& complexattribname, const int& metriccode, const string& parametername, const int& extractorcode,
                                                      const string& parameterinstance);
  
  /**
   * Converts the attributes names to the respective metric parameter name.
   * @param tablename name of the table
   * @param metricname name of the metric
   * @param particulatename name of the particulate attribute
   * @param attributelist the list of attributes
   * @return the list of metric parameters correspondent to the list of attributes
   */
  vector<string> *convertAttributesToMetricParams (const string& tablename, const string& metricname,
                                                   const string& particulatename, const vector<string>& attributelist, vector<string>& parameterInstanceVector);
  
    
  /**
   * Get the non-complex attributes from the table.
   * @param tablename
   *    name of the table.
   * @param mmattriblist
   *    list with the complex attributes for that table.
   * @return 
   *    list of the non-complex attributes.
   */
  vector<string>* getAttributes(const string& tablename, const vector<string> *mmattriblist);
  
  
  
  /**
   * Gets the data type used by Oracle.
   * @param attribtype attribute type
   * @return the data type used by Oracle
   */
   string getDbmsDataType(const string& attribtype);
  
  /**
   * This function returns the table name, the complex attribute name and the metric name for the parameters
   * that were passed.
   * @param tableName
   * @param complexAttribName
   * @param metricName
   * @return 
   */
  Query* getExtractor (const string& tableName, const string& complexAttribName, const string& metricName);


  /**
   * Returns the index file.
   * @param tablename
   *        The table name.
   * @param attribname
   *        The attribute name.
   * @param metricname
   *        The metric name.
   * @return 
   *        The name of the file.
   */
  string getIndexFile (const string& tablename, const string& attribname, const string& metricname);

  /**
   * Gets the query object.
   * @return 
   *    Query object.
   */
  Query* getQuery ();


  /**
   * 
   * @param tablename
   * @param attribname
   * @param metricname
   * @return 
   */
  int getNumberOfParticulateParams (string& tablename, string& attribname,
                                    string& metricname);

  /**
   * 
   * @param metriccode
   * @param attriblist
   */
  void getParameters(const int& metriccode, vector<string> *attriblist);
  
  
  /**
   * Gets the particle type of a given particle column name
   * @param tablename table name
   * @param particlecolname particle column name
   * @return if the particle column name exists it returns the particle type else it returns ""
   */
  string getParticleType (const string& tablename, const string& particlecolname);
  
    
  /**
   * 
   * @param tablename
   * @param metricname
   * @param particulatename
   * @return 
   */
  vector<vector<string> *> * getParticulateData (const string& tablename,
                                                 const string& metricname, const string& particulatename);


  /**
   * Returns the type of a parameter.
   * @param parameterName
   *        The parameter name.
   * @param extractorCode
   *        The extractor code.
   * @return 
   *        The parameter type.
   */
  string getParameterType (const string& parameterName, const int& extractorCode);
  
  /**
   * Returns the type of the connection for the query object inside the sirenqueries instance.
   * @return 
   *    type.
   */
  eConnections getTypeConnection();


  /**
   * Checks if the attribute is complex.
   * @param attribname
   *        The name of the attribute.
   * @return 
   *        Complex or not.
   */
  bool isComplexAttributeReference (const string& attribName);
  
  
  Query * runQuery(const string& sql);

};



#endif	/* SIRENQUERIES_H */

