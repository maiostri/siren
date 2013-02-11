#include "SirenQueries.h"

SirenQueries::SirenQueries(eConnections connectionsEnum, const string& username, const string& password) {
    if (connectionsEnum == POSTGRESQL) {
        conn = new ConnPostgres();
        char *connectionString;
        query = new QueryPostgres(connectionString);
    } else if (connectionsEnum == ORACLE) {
        this->conn = new ConnOracle(username, password);
        this->query = new QueryOracle(conn->getConnection());
    }
}

SirenQueries::~SirenQueries() {
    delete query;
    delete conn;
}

string
SirenQueries::checkSelectParticles(const string& tablename, const string& metricname,
    vector<string>& InsertColList) {
    string result = "";

    // select the attributes needed in any metric associated to any particulate attribute in the table
    string sql;
    sql = "select distinct mi.parametername ";
    sql += "from CDD$metricstruct ms, CDD$metricinstance mi ";
    sql += "where ms.metriccode = mi.metriccode ";
    sql += "and mi.tablename = '" + tablename + "' ";
    sql += "and ms.metricname = '" + metricname + "' ";

    query->Open(sql);

    string parameterinstance;
    while (!query->Eof()) {
        parameterinstance = query->StringFieldByIndex(0);
        // Check if the parameter instance is in 
        if (find(InsertColList.begin(), InsertColList.end(), parameterinstance) != InsertColList.end()) {
            result = parameterinstance;
        }



    }

    return result;
}

int
SirenQueries::createMetricInsertMetric(const string& metricname, const int& lpp, const string& metrictype) {
    int seqvalue = this->query->runSequence("MetricCodeSequence");
    string sql;

    sql = "insert into CDD$MetricStruct (MetricName, LpP, MetricCode, MetricType) values ('";
    sql += metricname + "', ";
    sql += lexical_cast<string>(lpp)+",";
    sql += lexical_cast<string>(seqvalue)+",";
    sql += "'"+metrictype+"')";
    this->query->Run(sql);
    return seqvalue;
}

void
SirenQueries::createMetricInsertParameter(const int& metriccode, const int& extractorcode, const string& parametername,
    const string& parameteralias, const string& parametertype, const float& weight, vector<string> *createlist) {

    string sql;

    // Getting the sequence next value.
    const int parameterOrderSequence = query->runSequence("ParameterOrderSequence");
    sql = "insert into CDD$ParameterStruct (ParameterName, ExtractorCode, MetricCode, ParameterAlias, ParameterType, ParameterOrder, Weight) values ";
    sql += "('" + parametername + "',";
    sql += lexical_cast<string > (extractorcode) + ",";
    sql += lexical_cast<string > (metriccode) + ",";
    sql += "'" + parameteralias + "',";
    sql += "'" + parametertype + "',";
    sql += lexical_cast<string > (parameterOrderSequence) + ",";
    sql += lexical_cast<string > (weight);

    this->query->Open(sql);

}

Query* SirenQueries::getExtractor(const string& tableName, const string& complexAttribName, const string& metricName) {
    string sql;
    sql += "select distinct ex.extractorname, ps.parametername ";
    sql += "  from CDD$ComplexAttribMetric im, CDD$metricstruct ms, CDD$parameterstruct ps, ";
    sql += "       epd$extractors ex ";
    sql += " where im.metriccode = ms.metriccode ";
    sql += "   and ms.metriccode = ps.metriccode ";
    sql += "   and ps.extractorcode = ex.extractorcode ";
    sql += "   and im.tablename = '" + tableName + "' ";
    sql += "   and im.complexattribname = '" + complexAttribName + "' ";
    sql += "   and ms.metricname = '" + metricName + "' ";
    this->query->Open(sql);
    return query;
}

Query* SirenQueries::getQuery() {
    return this->query;
}

int
SirenQueries::getNumberOfParticulateParams(string& tablename, string& attribname,
    string& metricname) {

    string sql =
        "select count(*) amount from CDD$MetricStruct ms, CDD$MetricInstance mi where ms.metriccode = mi.metriccode and mi.tablename = '"
        + tablename + "' and mi.complexattribname = '" + attribname + "' and ms.metricname = '" + metricname
        + "'";
    query->Open(sql);
    return query->IntFieldByIndex(0);
}

vector<vector<string> *> *
SirenQueries::getParticulateData(string& tablename,
    string& metricname, string& particulatename) {
    vector< vector<string> *> *list = new vector< vector<string>*> ();
    // getting the parameters of this metric
    string sql;
    sql = "select distinct mi.parametername, mi.parameterinstance, ps.parametertype, ps.parameterorder ";
    sql += "from cdd$metricstruct ms, cdd$metricinstance mi, cdd$parameterstruct ps ";
    sql += "where ms.metriccode = mi.metriccode ";
    sql += "and mi.metriccode = ps.metriccode ";
    sql += "and mi.parametername = ps.parametername ";
    sql += "and mi.tablename = '" + tablename + "' ";
    sql += "and ms.metricname = '" + metricname + "' ";
    sql += "and mi.complexattribname = '" + particulatename + "' ";
    sql += "order by ps.parameterorder ";

    query->Open(sql);

    vector<string> *instancesVector = new vector<string > ();
    vector<string> *namesVector = new vector<string > ();
    vector<string> *typesVector = new vector<string > ();
    vector<string> *ordersVector = new vector<string > ();

    list->push_back(namesVector);
    list->push_back(instancesVector);
    list->push_back(typesVector);
    list->push_back(ordersVector);

    while (!query->Eof()) {
        for (unsigned int i = 0; i < list->size(); i++) {
            list->at(i)->push_back(query->StringFieldByIndex(i));
        }
    }
    return list;
}




