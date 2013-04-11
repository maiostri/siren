#include "logUtils.h"

logUtils::logUtils()
{
    string initFileName = "log4cpp.properties";
    PropertyConfigurator::configure(initFileName);
    root = &Category::getRoot();
    sqlLog = &Category::getInstance(string("sql"));
    errorLog = &Category::getInstance(string("error"));
}

logUtils& logUtils::getInstance()
{
    static logUtils instance;
    return instance;
}

void logUtils::logInfo(const string& message)
{
    root->info(message);
}

void logUtils::logError(const string& message)
{
    errorLog->error(message);
}

void logUtils::logSQL(const string& sql)
{
    sqlLog->info(sql);
}