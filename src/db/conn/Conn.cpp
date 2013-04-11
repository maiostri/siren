// todo ifndef para o include de oracle ou postgres
#include "Conn.h"

Conn::Conn()
{

}

Conn::Conn(const std::string& username, const std::string& password)
{
    setUsername(username);
    setPassword(password);
}

Conn::~Conn()
{

}

void Conn::setUsername(const std::string& username)
{
    this->username = username;
}

void Conn::setPassword(const std::string& password)
{
    this->password = password;
}

std::string Conn::getUsername()
{
    return this->username;
}

std::string Conn::getPassword()
{
    return this->password;
}

oracle::occi::Connection *Conn::getConnection()
{
    return NULL;
}

void Conn::createConnection(const std::string& username, const std::string& password)
{
}

void Conn::Commit()
{

}

void Conn::closeConnection()
{

}

