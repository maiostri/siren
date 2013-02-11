/**
 * @file
 *
 * This file defines a connection interface for DBMSs.
 *
 * @version 1.0
 * @author Ricardo Pereira Maiostri (maiostri@gmail.com)
 * @date 11-03-2011
 */

#ifndef __CONN_H
#define __CONN_H

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <occi.h>
#include <occiControl.h>
#include <pqxx/pqxx>

namespace Connections {
enum eConnections {
	ORACLE = 1, POSTGRESQL = 2
};
}

class Conn {
private:
	std::string username;
	std::string password;

public:

	Conn();

	Conn(const std::string& username, const std::string& password);

	virtual ~Conn();

	virtual void setUsername(const std::string& username);

	virtual void setPassword(const std::string& password);

	virtual std::string getUsername();

	virtual std::string getPassword();

	virtual oracle::occi::Connection *getConnection();

	virtual void createConnection(const std::string& username,
			const std::string& password);

	virtual void Commit();

	virtual void closeConnection();

};

#endif /*__CONN_H */
