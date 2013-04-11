/* 
 * This singleton class encapsulate all the log operations.
 *
 * Created on February 11, 2013, 11:21 PM
 */

#ifndef LOGUTILS_H
#define	LOGUTILS_H

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

using namespace std;
using namespace log4cpp;

class logUtils
{
private:
  Category *root;
  Category *sqlLog;
  Category *errorLog;
  
  logUtils (logUtils const& copy);
  logUtils& operator= (logUtils const& copy);

  /**
   * Private constructor.
   */
  logUtils ();
public:

  static logUtils& getInstance ();

  void logInfo (const string& message);

  void logError (const string& message);

  void logSQL (const string& sql);

};

#endif	/* LOGUTILS_H */

