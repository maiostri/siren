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

#include "db/conn/Conn.h"
#include "db/conn/ConnPostgres.h"
#include "db/conn/ConnOracle.h"
#include "db/query/Query.h"
#include "db/query/QueryPostgres.h"
#include "db/query/QueryOracle.h"


using namespace std;
using namespace Connections;

class SirenQueries
{
private:
  Conn *conn;
  Query *query;

  // This vector register all the sql statements executed for the SirenQueries instance.
  vector<string> *sqlStatements;
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
   * This function returns the table name, the complex attribute name and the metric name for the parameters
   * that were passed.
   * @param tableName
   * @param complexAttribName
   * @param metricName
   * @return 
   */
  Query* getExtractor (const string& tableName, const string& complexAttribName, const string& metricName);
  
  
  /**
   * Gets the query object.
   * @return 
   *    Query object.
   */
  Query* getQuery();


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
   * @param tablename
   * @param metricname
   * @param particulatename
   * @return 
   */
  vector<vector<string> *> * getParticulateData (string& tablename,
                                                 string& metricname, string& particulatename);


};



#endif	/* SIRENQUERIES_H */

