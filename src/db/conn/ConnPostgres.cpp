/**
 * @file
 *
 * This file implements a connection interface for PostgreSQL.
 *
 * @version 1.0
 * @author Ricardo Pereira Maiostri (maiostri@gmail.com)
 * @date 11-03-2011
 */

#include "ConnPostgres.h"

ConnPostgres::ConnPostgres() : Conn()
{

}

ConnPostgres::ConnPostgres(char *username, char *password) : Conn(username, password)
{
    createConnection(username, password);
}

ConnPostgres::~ConnPostgres()
{
    closeConnection();
}

void ConnPostgres::createConnection(char *username, char *password)
{
    try
    {

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

pqxx::work *ConnPostgres::getWork()
{
    return this->trans;
}

void ConnPostgres::closeConnection()
{
    delete trans;
}


