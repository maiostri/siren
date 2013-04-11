/**
 * @file
 *
 * This file defines a connection interface for Oracle.
 *
 * @version 1.0
 * @author Ricardo Pereira Maiostri (maiostri@gmail.com)
 * @date 11-03-2011
 */

#ifndef __CONN_ORACLE_H
#define __CONN_ORACLE_H

#include "Conn.h"

class ConnOracle : public Conn
{
private:
  oracle::occi::Environment* env;
  oracle::occi::Connection* conn;
  oracle::occi::ResultSet *rs;

public:
  ConnOracle ();

  ConnOracle (const std::string& username, const std::string& password);

  ~ConnOracle ();

  void createConnection (const std::string& username,
                         const std::string& password);

  void Commit ();

  void closeConnection ();

  oracle::occi::Connection *getConnection ();

  oracle::occi::Environment *getEnvironment ();
};
#endif
