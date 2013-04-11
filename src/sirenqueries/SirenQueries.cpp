#include "SirenQueries.h"
#include "utils/sirenUtils.h"

SirenQueries::SirenQueries(eConnections connectionsEnum, const string& username, const string& password) {
	this->connectionsEnum = connectionsEnum;
	if (connectionsEnum == POSTGRESQL) {
		this->conn = new ConnPostgres();
		char *connectionString;
		this->query = new QueryPostgres(connectionString);
	} else if (connectionsEnum == ORACLE) {
		this->conn = new ConnOracle(username, password);
		this->query = new QueryOracle(conn->getConnection());
	}
}

SirenQueries::~SirenQueries() {
	delete query;
	delete conn;
}

string SirenQueries::checkSelectParticles(const string& tablename, const string& metricname,
		vector<string>& InsertColList) {

	// select the attributes needed in any metric associated to any particulate attribute in the table
	string sql;
	sql = "select distinct mi.parametername ";
	sql += "from CDD$metricstruct ms, CDD$metricinstance mi ";
	sql += "where ms.metriccode = mi.metriccode ";
	sql += "and mi.tablename = '" + tablename + "' ";
	sql += "and ms.metricname = '" + metricname + "' ";

	query->Open(sql);

	string result = "";
	while (!query->Eof()) {
		string parameterinstance = query->StringFieldByIndex(0);
		// Check if the parameter instance is in
		int idx = sirenUtils::getInstance().getPositionCopy(InsertColList, parameterinstance);
		if (idx == -1) {
			return parameterinstance;
		}
	}

	return result;
}

int SirenQueries::createMetricInsertMetric(const string& metricname, const int& lpp, const string& metrictype) {
	int seqvalue = this->query->runSequence("MetricCodeSequence");
	string sql;

	sql = "insert into CDD$MetricStruct (MetricName, LpP, MetricCode, MetricType) values ('";
	sql += metricname + "', ";
	sql += lexical_cast<string>(lpp) + ",";
	sql += lexical_cast<string>(seqvalue) + ",";
	sql += "'" + metrictype + "')";
	this->query->Run(sql);
	return seqvalue;
}

void SirenQueries::createMetricInsertParameter(const int& metriccode, const int& extractorcode,
		const string& parametername, const string& parameteralias, const string& parametertype, const float& weight,
		vector<string> *createlist) {

	string sql;

	// Getting the sequence next value.
	const int parameterOrderSequence = query->runSequence("ParameterOrderSequence");
	sql =
			"insert into CDD$ParameterStruct (ParameterName, ExtractorCode, MetricCode, ParameterAlias, ParameterType, ParameterOrder, Weight) values ";
	sql += "('" + parametername + "',";
	sql += lexical_cast<string>(extractorcode) + ",";
	sql += lexical_cast<string>(metriccode) + ",";
	sql += "'" + parameteralias + "',";
	sql += "'" + parametertype + "',";
	sql += lexical_cast<string>(parameterOrderSequence) + ",";
	sql += lexical_cast<string>(weight) + ')';
	this->query->Run(sql);

}

void SirenQueries::createTableInsertMetricInstance(const string& tablename, const string& complexattribname,
		const int& metriccode, const string& parametername, const int& extractorcode, const string& parameterinstance) {

	string command =
			"insert into CDD$MetricInstance (TableName, ComplexAttribName, MetricCode, ParameterName, ExtractorCode, ParameterInstance) values ('";
	command += tablename + "', '" + complexattribname + "', " + lexical_cast<string>(metriccode) + ", '"
			+ parametername;
	command += "', " + lexical_cast<string>(extractorcode) + ", '" + parameterinstance + "')";
	this->query->Run(command);

}

vector<string> *
SirenQueries::convertAttributesToMetricParams(const string& tablename, const string& metricname,
		const string& particulatename, const vector<string>& attributelist, vector<string>& parameterInstanceVector) {
	vector<string> *list = new vector<string>();
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

	this->query->Open(sql);

	while (!query->Eof()) {
		string found = "";
		string parameterInstance = query->StringFieldByIndex(1);
		const int idx = sirenUtils::getInstance().getPositionCopy(attributelist, parameterInstance);
		if (idx != -1) {
			found = query->StringFieldByIndex(0);
			list->push_back(found);
			parameterInstanceVector.push_back(parameterInstance);
		}

	}
	return list;
}

vector<string> *
SirenQueries::getAttributes(const string& tablename, const vector<string> *mmattriblist) {

	vector<string> *attriblist = new vector<string>();

	string sql = "select * from " + tablename;
	this->query->Open(sql);

	// Inserts the other attribute references (except the image attribute references)
	this->query->Next();
	for (int i = 0; i < this->query->FieldsCount(); i++) {
		string fieldName = this->query->FieldNameByIndex(i);
		if (find(mmattriblist->begin(), mmattriblist->end(), fieldName) == mmattriblist->end()) {
			attriblist->push_back(fieldName);
		}
	}
	return attriblist;
}

string SirenQueries::getDbmsDataType(const string& attribtype) {

	// Instantiate a new query object.
	Query *query = NULL;

	string sql, auxsql;

	if (this->connectionsEnum == POSTGRESQL) {
		query = new QueryPostgres(NULL);
		sql = "select tablename from pg_tables where tableowner = user and tablename = 'tmp$attribname'";
		auxsql =
				"select data_type from pg_tables join information_schema.columns on table_name = tablename where tableowner = 'postgres' and table_name = 'tmp$attribname' and column_name = 'x'";
	} else if (this->connectionsEnum == ORACLE) {
		query = new QueryOracle(conn->getConnection());
		sql = "select table_name from all_tables where owner = user and table_name = 'TMP$ATTRIBNAME'";
		auxsql =
				"select data_type from sys.all_tab_columns where owner = user and table_name = 'TMP$ATTRIBNAME' and column_name = 'X'";
	}
	query->Open(sql);
	if (!query->Eof()) {
		if (query->StringFieldByIndex(0).compare("") != 0) {
			sql = "drop table TMP$ATTRIBNAME";
			query->Run(sql);
		}
	}

	sql = "create table TMP$ATTRIBNAME (X ";
	sql += attribtype;
	sql += ")";
	query->Run(sql);

	query->Open(auxsql);
	string result = "";

	if (!query->Eof()) {
		result = query->StringFieldByIndex(0);
	}

	sql = "drop table TMP$ATTRIBNAME";
	query->Run(sql);

	delete query;
	transform(result.begin(), result.end(), result.begin(), ::tolower);

	return result;
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

string SirenQueries::getIndexFile(const string& tablename, const string& attribname, const string& metricname) {

	string result = "";
	string sql;

	sql =
			"select im.IndexFile from CDD$ComplexAttribMetric im, CDD$MetricStruct ms where im.MetricCode = ms.MetricCode ";
	sql += "and im.TableName = '" + tablename + "' ";
	sql += "and im.ComplexAttribName = '" + attribname + "' ";
	sql += "and ms.MetricName = '" + metricname + "' ";

	query->Open(sql);
	if (!query->Eof()) {
		result = query->StringFieldByIndex(0);
	}
	return result;
}

Query* SirenQueries::getQuery() {
	return this->query;
}

int SirenQueries::getNumberOfParticulateParams(string& tablename, string& attribname, string& metricname) {

	string sql =
			"select count(*) amount from CDD$MetricStruct ms, CDD$MetricInstance mi where ms.metriccode = mi.metriccode and mi.tablename = '"
					+ tablename + "' and mi.complexattribname = '" + attribname + "' and ms.metricname = '" + metricname
					+ "'";
	query->Open(sql);
	return query->IntFieldByIndex(0);
}

void SirenQueries::getParameters(const int& metriccode, vector<string> *attriblist) {
	string sql = "select ParameterName, ParameterType from CDD$ParameterStruct where MetricCode = ";
	sql += lexical_cast<string>(metriccode);
	sql += " order by ParameterOrder";

	query->Open(sql);

	while (!query->Eof()) {
		attriblist->push_back(query->StringFieldByIndex(0));
		attriblist->push_back(query->StringFieldByIndex(1));
	}
}

string SirenQueries::getParameterType(const string& parameterName, const int& extractorCode) {
	string sql;
	string paramType = "";

	sql = "select parametertype from EPD$Parameters where parametername = '";
	sql += parameterName;
	sql += "' and extractorcode = ";
	sql += lexical_cast<string>(extractorCode);
	this->query->Open(sql);
	this->query->Next();
	paramType = query->StringFieldByIndex(0);
	return paramType;
}

string SirenQueries::getParticleType(const string& tablename, const string& particlecolname) {
	string sql, partsql, particle;
	Query *query = nullptr;

	string tableNameUpper = sirenUtils::getInstance().toUpper(tablename);
	string particleColNameUpper = sirenUtils::getInstance().toUpper(particlecolname);

	if (this->connectionsEnum == POSTGRESQL) {
		query = new QueryPostgres(nullptr);
		sql =
				"select data_type from pg_tables join information_schema.columns on table_name = tablename where tableowner = 'postgres' and table_name = '"
						+ tableNameUpper + "' and column_name = '";
	} else if (this->connectionsEnum == ORACLE) {
		query = new QueryOracle(conn->getConnection());
		sql = "select data_type from sys.all_tab_columns where owner = user and table_name = '" + tableNameUpper
				+ "' and column_name = '";

	}

	// Check if the particle is the parameter name or parameter instance.
	partsql = "select parameterinstance from cdd$metricinstance where parametername = '" + particlecolname + "'";
	query->Open(partsql);
	// The particle col name was an parameter name. So we need to search for the parameterinstance.
	if (!query->Eof()) {
		particle = query->StringFieldByIndex(1);
	}
	// The particle col name was indeed the name of the parameter instance.
	else {
		particle = particlecolname;
	}
	sql += particleColNameUpper + "'";
	query->Open(sql);
	string result;
	if (!query->Eof()) {
		result = query->StringFieldByIndex(0);
	}
	delete query;
	return result;
}

vector<vector<string> *> *
SirenQueries::getParticulateData(const string& tablename, const string& metricname, const string& particulatename) {
	vector<vector<string> *> *list = new vector<vector<string>*>();
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

	vector<string> *instancesVector = new vector<string>();
	vector<string> *namesVector = new vector<string>();
	vector<string> *typesVector = new vector<string>();
	vector<string> *ordersVector = new vector<string>();

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

eConnections SirenQueries::getTypeConnection() {
	return this->connectionsEnum;
}

bool SirenQueries::isComplexAttributeReference(const string& attribname) {
	bool res = false;
	string sql = "select count(*) as amount from CDD$ComplexAttribute where ComplexAttribName = '" + attribname + "'";
	this->query->Open(sql);
	while (query->Next()) {
		if (query->IntFieldByIndex(0) > 0) {
			res = true;
		}
	}
	return res;
}

Query * SirenQueries::runQuery(const string& sql) {
	this->query->Open(sql);
	return this->query;
}

