/**
 * @file
 *
 * This file implements a connection interface for Oracle.
 *
 * @version 1.0
 * @author Ricardo Pereira Maiostri (maiostri@gmail.com)
 * @date 11-03-2011
 */


#include "ConnOracle.h"

ConnOracle::ConnOracle() : Conn()
{

}

ConnOracle::ConnOracle(const std::string& username, const std::string& password) : Conn(username, password)
{
    createConnection(username, password);
}

ConnOracle::~ConnOracle()
{
    env->terminateConnection(conn);
    oracle::occi::Environment::terminateEnvironment(env);
}

void ConnOracle::createConnection(const std::string& username, const std::string& password)
{
    env = oracle::occi::Environment::createEnvironment();
    conn = env->createConnection(username, password);
}

void ConnOracle::Commit()
{
    conn->commit();
}

void ConnOracle::closeConnection()
{
    env->terminateConnection(conn);
    oracle::occi::Environment::terminateEnvironment(env);
}

oracle::occi::Connection * ConnOracle::getConnection()
{
    return conn;
}

oracle::occi::Environment * ConnOracle::getEnvironment()
{
    return env;
}


