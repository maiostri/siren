/**
 * @file
 *
 * This file implements the queries operations for PostgreSQL.
 *
 * @version 1.0
 * @author Ricardo Pereira Maiostri (maiostri@gmail.com)
 * @date 11-03-2011
 */
#include "QueryPostgres.h"


QueryPostgres::QueryPostgres(const char *connection)
{
    this->connection = connection;
}

void QueryPostgres::Open(const string& sql)
{
    try {
        pqxx::connection conn(connection);
        pqxx::work Xaction(conn);
        ret = Xaction.exec(sql);
        row = ret.begin();
    } catch (exception &e) {
        cerr << e.what() << endl;
    }
}

void QueryPostgres::Run(const string& sql)
{
    try {
        pqxx::connection conn(connection);
        pqxx::work Xaction(conn);
        Xaction.exec(sql);
        Xaction.commit();
    } catch (exception &e) {
        cerr << e.what() << endl;
    }
}

void QueryPostgres::First()
{
    row = ret.begin();
}

int QueryPostgres::Next()
{
    if (row != ret.end()) {
        ++row;
        return 1;
    } else
        return 0;
}

int QueryPostgres::FieldsCount()
{
    // const pqxx::result::tuple & myTuple = ret[0];
    return 0;
}

int QueryPostgres::Eof()
{
    if (row != ret.end())
        return 1;
    else
        return 0;
}

string QueryPostgres::StringFieldByIndex(const int& index)
{
    return row[index].as<string > ();
}

int QueryPostgres::IntFieldByIndex(const int& index)
{
    return row[index].as<int>();
}

float QueryPostgres::FloatFieldByIndex(const int& index)
{
    return row[index].as<float>();
}

double QueryPostgres::DoubleFieldByIndex(const int& index)
{
    return row[index].as<double>();
}

string QueryPostgres::FieldNameByIndex(const int& index)
{
    return row[index].name();
}

int QueryPostgres::FieldIsNullByIndex(const int& index)
{
    return 0;
}

int QueryPostgres::countRows(const string& table)
{
    const string sql = "SELECT COUNT(*) AS AMOUNT FROM (" + table + ") as SUBQUERY";
    QueryPostgres::Open(sql);
    return row[0].as<int>();
}

void QueryPostgres::mountInsert(const string& tableName, const vector<string>& fieldNames, vector<char *>&
        tDynamicObjectCharVector)
{
}

void QueryPostgres::dropTable(const string& table) {
    
}

void QueryPostgres::createSequence(const string& sequenceName, const int& startWith) {
    
}
void QueryPostgres::dropSequence(const string& sequence) {
    
}

void QueryPostgres::RunPLSQL (const string& sql) {
    
}

char * QueryPostgres::blobFieldDumpToCharByName(const string& field) {
    return NULL;
}

int QueryPostgres::runSequence(const string& sequenceName) {
    
}