/**
* @file
*
* This file defines a connection interface for PostgreSQL.
*
* @version 1.0
* @author Ricardo Pereira Maiostri (maiostri@gmail.com)
* @date 11-03-2011
*/

#ifndef __CONN_POSTGRES_H
#define __CONN_POSTGRES_H

#include "Conn.h"

class ConnPostgres: public Conn
{
private:
    pqxx::work *trans;
    pqxx::result *ret;
public:
    ConnPostgres();

    ConnPostgres(char *username, char *password);

    ~ConnPostgres();

    void createConnection(char *username, char *password);

    pqxx::work *getWork();

    void closeConnection();

};
#endif
