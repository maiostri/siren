//---------------------------------------------------------------------------
#include "DataDictionaryAccess.h"


//---------------------------------------------------------------------------

DataDictionaryAccess::DataDictionaryAccess(eConnections connectionsEnum, const string& username, const string& password,
        MetricTreeManager *TreeManager, Parser *p)
{

    // log4cpp
    string initFileName = "log4cpp.properties";
    PropertyConfigurator::configure(initFileName);    
    root = &Category::getRoot();
    
    this->connectionsEnum = connectionsEnum;
    this->user = username;

    this->parser = p;
    this->treemanager = TreeManager;
    deletetreemanager = false;

    //@TODO Remove this. Once the insert is stable, all the operations here will
    // performed using the SirenQueries class.
    if (this->connectionsEnum == POSTGRESQL) {
        SqlConnection = new ConnPostgres();
        char *connectionString;
        query = new QueryPostgres(connectionString);
    } else if (this->connectionsEnum == ORACLE) {
        SqlConnection = new ConnOracle(username, password);
        query = new QueryOracle(SqlConnection->getConnection());
        qry = new QueryOracle(SqlConnection->getConnection());
    }
    
    // Instantiate the connection with the database.
    this->sirenQueries = new SirenQueries(connectionsEnum, username, password);

    MetricTreeTotalTime = 0;
    TempJoinInsertsTotalTime = 0;
    GroupSimObjManipulation = 0;

    // initiate the scope with 0
    tempjointablescope = 0;
    
    
}

//---------------------------------------------------------------------------

DataDictionaryAccess::~DataDictionaryAccess()
{
    delete query;
    delete SqlConnection;
    if (deletetreemanager)
        delete treemanager;
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::RunSQL(string sql)
{

    string r = "OK!";
    query->Run(sql);
    return r;
}

//---------------------------------------------------------------------------

Token *
DataDictionaryAccess::AddToken(Token *position, string Lexem, int TokenType, int LexemType)
{

    Token *newtoken = new Token();
    newtoken->Lexem = Lexem;
    newtoken->TokenType = TokenType;
    newtoken->LexemType = LexemType;

    if (position == NULL) {
        position = newtoken;
    } else {
        Token *aux = position->Next;
        position->Next = newtoken;
        newtoken->Prior = position;
        newtoken->Next = aux;
    }

    return newtoken;
}
//---------------------------------------------------------------------------

Token *
DataDictionaryAccess::AddToken(Token *position, Token *newtoken)
{

    if (position == NULL) {
        position = newtoken;
    } else {
        Token *aux = position->Next;
        position->Next = newtoken;
        newtoken->Prior = position;
        newtoken->Next = aux;
    }

    return newtoken;
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::RunSQL(Token *tokenlist)
{

    // mounting the query from "tokenlist" to "command"
    string sql = "";
    Token *tmp = tokenlist;
    while (tmp != NULL) {
        sql = sql + tmp->Lexem;
        if (tmp->Next != NULL) {
            if (((tmp->Next->Lexem).compare(".") != 0) && (tmp->Lexem.compare(".") != 0))
                sql = sql + " ";
        }
        tmp = tmp->Next;
    }

    string r = "OK!";
    query->Run(sql);
    return r;
}

//---------------------------------------------------------------------------
// Runs a SQL statement and passes the filename as the first parameter of the sql

string
DataDictionaryAccess::RunInsertFile(string sql, string filename)
{

    string r = "OK!";
    if (query->FieldsCount() == 0) {
        r = "Error: the SQL statement must have one param.";
        return r;
    }

    query->Run(sql);
    return r;
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::DropSchema(vector<string> *commandlist)
{
    string sql;
    string insert;

    // client folder
    vector<string> *elementsToDDL = new vector<string > ();

    elementsToDDL->push_back("ParticulateIdSequence");
    elementsToDDL->push_back("ImageIdSequence");
    elementsToDDL->push_back("AudioIdSequence");
    elementsToDDL->push_back("ExtractorCodeSequence");
    elementsToDDL->push_back("MetricCodeSequence");
    elementsToDDL->push_back("IndexIdSequence");
    elementsToDDL->push_back("ParameterOrderSequence");

    for (unsigned int i = 0; i < elementsToDDL->size(); i++)
        query->dropSequence(elementsToDDL->at(i));

    elementsToDDL->clear();

    elementsToDDL->push_back("CDD$METRICINSTANCE");
    elementsToDDL->push_back("CDD$ComplexAttribMetric");
    elementsToDDL->push_back("CDD$ComplexAttribute");
    elementsToDDL->push_back("CDD$ParameterStruct");
    elementsToDDL->push_back("CDD$MetricStruct");
    elementsToDDL->push_back("EPD$Parameters");
    elementsToDDL->push_back("EPD$Extractors");
    elementsToDDL->push_back("TMP$Folder");

    for (unsigned int j = 0; j < 10; j++) {
        elementsToDDL->push_back("TMP$tmpjoin" + lexical_cast<string>(j));
    }

    for (unsigned int k = 0; k < elementsToDDL->size(); k++)
        query->dropTable(elementsToDDL->at(k));

    elementsToDDL->clear();

    // drop all arboretum index files

    sql = "select distinct indexfile from CDD$ComplexAttribMetric";
    query->Open(sql);

    while (!query->Eof()) {
        string f1 = user + "_slim_" + query->StringFieldByIndex(0);
        string f2 = user + + "_dummy_" + query->StringFieldByIndex(0);
        try {
            remove(f1.c_str());
            remove(f2.c_str());
        } catch (std::exception &E) {
            root->error("Error on removing the index files: "+string(E.what()));
        }
        insert = "Delete index files " + f1 + " and " +f2 +" OK!";
        commandlist->push_back(insert);

    }

    // drop all stillimage user tables
    vector<string> droplist;

    sql =
            "select distinct tablename, complexattribname from CDD$ComplexAttribMetric ca, cdd$metricstruct ms where ms.metriccode = ca.metriccode and ms.metrictype = 'stillimage'";
    query->Open(sql);
    while (!query->Eof()) {
        sql = "drop table ipv$";
        sql += query->StringFieldByIndex(0);
        sql += "_";
        sql += query->StringFieldByIndex(1);
        droplist.push_back(sql);
    }

    // drop all audio user tables
    sql =
            "select distinct tablename, complexattribname from CDD$ComplexAttribMetric ca, cdd$metricstruct ms where ms.metriccode = ca.metriccode and ms.metrictype = 'audio'";
    query->Open(sql);

    // For Oracle
    while (!query->Eof()) {
        sql = "drop table apv$";
        sql += query->StringFieldByIndex(0);
        sql += "_";
        sql += query->StringFieldByIndex(1);
        droplist.push_back(sql);
    }

    sql = "select distinct tablename from CDD$ComplexAttribMetric";
    query->Open(sql);

    while (!query->Eof()) {
        sql = "drop table ";
        sql += query->StringFieldByIndex(0);
        droplist.push_back(sql);
    }

    for (unsigned int i = 0; i < droplist.size(); i++) {
        insert = droplist.at(i);
        insert += " ";

        insert += RunSQL(droplist.at(i));
        insert += ". ";
        commandlist->push_back(insert);
    }

    delete elementsToDDL;
}

string
DataDictionaryAccess::AddSlashs(string t)
{
    string r;
    for (unsigned int i = 1; i <= t.length(); i++)
        if (t[i] == '\\')
            r += "\\\\";
        else
            r += t[i];
    return r;
}

string
DataDictionaryAccess::GetFilePath(string filename)
{
    string ret;
    size_t found = filename.find_last_of('/');
    if (found == string::npos)
        found = filename.find_last_of('\'');
    ret = filename.substr(0, int(found));
    return ret;
}

string
DataDictionaryAccess::GetFileName(string filename)
{
    string ret;
    size_t found = filename.find_last_of('/');
    if (found == string::npos)
        found = filename.find_last_of('\'');
    ret = filename.substr((int(found)) + 1, filename.size());
    return ret;
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::CreateSchema(vector<string> *commandlist)
{
    string sql;
    char filedir[200];
    getcwd(filedir, sizeof (filedir));
    string dir = filedir;

    // client folder
    sql = "CREATE TABLE TMP$Folder (Folder varchar(200) NOT NULL)";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // client folder
    sql = "INSERT INTO TMP$Folder (Folder) values ('";
    sql += dir.c_str();
    sql += "')";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create table EPD$Extractors
    sql =
            "CREATE TABLE EPD$Extractors (ExtractorName varchar(30) NOT NULL, ExtractorType varchar(20) NOT NULL, ExtractorCode integer NOT NULL, PRIMARY KEY (ExtractorCode))";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create table EPD$Parameters
    sql =
            "CREATE TABLE EPD$Parameters (ExtractorCode integer NOT NULL, ParameterName varchar(30) NOT NULL, ParameterType varchar(30), IsIndexable char(1) NOT NULL, FOREIGN KEY (ExtractorCode)REFERENCES EPD$Extractors, PRIMARY KEY (ExtractorCode, ParameterName))";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create table CDD$MetricStruct
    sql =
            "CREATE TABLE CDD$MetricStruct (MetricName varchar(30) NOT NULL, MetricType varchar(20) NOT NULL, LpP integer, MetricCode integer NOT NULL, PRIMARY KEY (MetricCode))";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create table CDD$ParameterStruct
    sql =
            "CREATE TABLE CDD$ParameterStruct(MetricCode integer NOT NULL, ExtractorCode integer NOT NULL, ParameterName varchar(30) NOT NULL, ParameterAlias varchar(30), ParameterType varchar(20), ParameterOrder decimal(10), Weight decimal(10,4), PRIMARY KEY (MetricCode, ExtractorCode, ParameterName), FOREIGN KEY (MetricCode) REFERENCES CDD$MetricStruct)";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create table CDD$ComplexAttribute
    sql =
            "CREATE TABLE CDD$ComplexAttribute (TableName varchar(30) NOT NULL, ComplexAttribName varchar(30) NOT NULL, ComplexAttribType varchar(15) NOT NULL, PRIMARY KEY (TableName, ComplexAttribName))";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create table CDD$ComplexAttribMetric
    sql =
            "CREATE TABLE CDD$ComplexAttribMetric (TableName varchar(30) NOT NULL, ComplexAttribName varchar(30) NOT NULL, MetricCode integer NOT NULL, IsDefault char(1), IndexName varchar(30) UNIQUE, IndexFile varchar(30), FOREIGN KEY (MetricCode) REFERENCES CDD$MetricStruct, FOREIGN KEY (TableName, ComplexAttribName) REFERENCES CDD$ComplexAttribute, PRIMARY KEY (TableName, ComplexAttribName, MetricCode))";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create table CDD$MetricInstance
    sql =
            "CREATE TABLE CDD$MetricInstance (TableName varchar(30) NOT NULL, ComplexAttribName varchar(30) NOT NULL, MetricCode integer NOT NULL, ParameterName varchar(30) NOT NULL, ExtractorCode integer NOT NULL, ParameterInstance varchar(30), PRIMARY KEY (TableName, ComplexAttribName, MetricCode, ParameterName, ExtractorCode), FOREIGN KEY (MetricCode, ParameterName, ExtractorCode) REFERENCES CDD$ParameterStruct (MetricCode, ParameterName, ExtractorCode))";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create table TMP$tmpjoin 0..9
    for (int i = 0; i < 10; i++) {
        stringstream out;
        out << i;
        sql = "CREATE TABLE TMP$tmpjoin" + out.str() + " (col1 decimal(10), col2 decimal(10))";
        commandlist->push_back(sql + " " + RunSQL(sql) + ". ");
    }

    // create sequence MetricCodeSequence
    sql = "CREATE SEQUENCE MetricCodeSequence START WITH 1";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create sequence ExtractorCodeSequence
    sql = "CREATE SEQUENCE ExtractorCodeSequence START WITH 1";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create sequence ImageIdSequence
    sql = "CREATE SEQUENCE ImageIdSequence START WITH 1";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create sequence AudioIdSequence
    sql = "CREATE SEQUENCE AudioIdSequence START WITH 1";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create sequence ParticulateIdSequence
    sql = "CREATE SEQUENCE ParticulateIdSequence START WITH 1";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create sequence IndexIdSequence
    sql = "CREATE SEQUENCE IndexIdSequence START WITH 1";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // create sequence ParameterOrderSequence
    sql = "CREATE SEQUENCE ParameterOrderSequence START WITH 1";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // image extractors
    sql =
            "insert into EPD$Extractors (ExtractorName, ExtractorType, ExtractorCode) values ('histogramext', 'stillimage' , 1)";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    sql =
            "insert into EPD$Extractors (ExtractorName, ExtractorType, ExtractorCode) values ('metrichistogramext', 'stillimage', 2)";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    sql =
            "insert into EPD$Extractors (ExtractorName, ExtractorType, ExtractorCode) values ('textureext', 'stillimage', 3)";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    sql =
            "insert into EPD$Extractors (ExtractorName, ExtractorType, ExtractorCode) values ('zernikeext', 'stillimage', 4)";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // audio extractor
    sql =
            "insert into EPD$Extractors (ExtractorName, ExtractorType, ExtractorCode) values ('soundtextureext', 'audio', 5)";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    // parameters
    sql =
            "insert into EPD$Parameters (ExtractorCode, ParameterName, ParameterType, IsIndexable) values (1,'histogram', 'double', 'Y')";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    sql =
            "insert into EPD$Parameters (ExtractorCode, ParameterName, ParameterType, IsIndexable) values (2,'metrichistogram', 'double', 'Y')";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    sql =
            "insert into EPD$Parameters (ExtractorCode, ParameterName, ParameterType, IsIndexable) values (3,'texture', 'double', 'Y')";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    sql =
            "insert into EPD$Parameters (ExtractorCode, ParameterName, ParameterType, IsIndexable) values (4,'zernike', 'double', 'Y')";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    sql =
            "insert into EPD$Parameters (ExtractorCode, ParameterName, ParameterType, IsIndexable) values (5,'stft', 'double', 'Y')";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

    sql =
            "insert into EPD$Parameters (ExtractorCode, ParameterName, ParameterType, IsIndexable) values (5,'mfcc', 'double', 'Y')";
    commandlist->push_back(sql + " " + RunSQL(sql) + ". ");

}

//---------------------------------------------------------------------------

int
DataDictionaryAccess::IsExtractor(string extractorname)
{
    sql = "select ExtractorCode from EPD$Extractors where ExtractorName = '" + extractorname + "'";
    query->Open(sql);
    if (!query->Eof()) {
        return query->IntFieldByIndex(0);
    } else
        return 0;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsValidExtractor(string extractorname, Token *tokenlist)
{

    Token *auxlist = tokenlist;
    string etype = "";
    bool res = false;

    while (auxlist->Next != NULL) {
        if (auxlist->Lexem.compare("for") == 0)
            etype = auxlist->Next->Lexem;
        auxlist = auxlist->Next;
    }

    sql = "select ExtractorType from EPD$Extractors where ExtractorName = '" + extractorname + "'";
    try {
        query->Open(sql);
    } catch (std::exception &E) {
        return -1;
    }
    if (!query->Eof())
        if (etype.compare(query->StringFieldByIndex(0)) == 0)
            res = true;
    return res;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsExtractorParameter(int extractorcode, string parametername)
{
    bool res = false;
    stringstream out;
    out << extractorcode;
    sql = "select count(ParameterName) as amount from EPD$Parameters where ExtractorCode = ";
    sql += out.str();
    sql += " and ParameterName = '";
    sql += parametername + "'";
    query->Open(sql);
    if (!query->Eof())
        if (query->IntFieldByIndex(0) > 0)
            res = true;
    return res;
}

//---------------------------------------------------------------------------

int
DataDictionaryAccess::IsMetric(string metricname)
{
    string temp;
    int ret = 0;

    sql = "select MetricCode from CDD$MetricStruct where MetricName = '" + metricname + "'";
    query->Open(sql);

    if (!query->Eof()) {
        ret = query->IntFieldByIndex(0);
    }
    return ret;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsParticulateMetric(string metricname)
{

    bool res = false;
    sql = "select MetricType from CDD$MetricStruct where MetricName = '" + metricname + "'";
    try {
        query->Open(sql);
    } catch (std::exception &E) {
        return false;
    }
    if (!query->Eof())
        if (query->StringFieldByIndex(0).compare("particulate") == 0)
            res = true;
    return res;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsStillimageAttribute(Token *tokenlist, string attribname)
{

    Token *auxlist = tokenlist;
    bool result = false;

    while (auxlist->Next != NULL) {
        if ((auxlist->Next->LexemType == LK_STILLIMAGE_DATATYPE) && (attribname == auxlist->Lexem))
            result = true;
        auxlist = auxlist->Next;
    }

    return result;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsParticulateAttribute(Token *tokenlist, string attribname)
{

    Token *auxlist = tokenlist;
    bool result = false;

    while (auxlist->Next != NULL) {
        if ((auxlist->Next->LexemType == LK_PARTICULATE_DATATYPE) && (attribname == auxlist->Lexem))
            result = true;
        auxlist = auxlist->Next;
    }

    return result;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsAudioAttribute(Token *tokenlist, string attribname)
{

    Token *auxlist = tokenlist;
    bool result = false;

    while (auxlist->Next != NULL) {
        if ((auxlist->Next->LexemType == LK_AUDIO_DATATYPE) && (attribname == auxlist->Lexem))
            result = true;
        auxlist = auxlist->Next;
    }

    return result;
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::IsComplexAttributeReference(string tablename, string attribname)
{

    string ctype = "";

    sql = "select ComplexAttribType as complextype from CDD$ComplexAttribute where TableName = '";
    sql += tablename;
    sql += "' and ComplexAttribName = '";
    sql += attribname;
    sql += "'";
    query->Open(sql);
    if (!query->Eof())
        ctype = query->StringFieldByIndex(0);
    return ctype;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsComplexAttributeReference(string attribname)
{
    bool res = false;
    string sql;
    sql = "select count(*) as amount from CDD$ComplexAttribute where ComplexAttribName = '";
    sql += attribname;
    sql += "'";
    query->Open(sql);
    if (!query->Eof())
        if (query->IntFieldByIndex(0) > 0)
            res = true;
    return res;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsStillimageAttributeReference(string tablename, string imgattribname)
{
    bool res = false;
    string sql;
    sql = "select Count(*) as amount from CDD$ComplexAttribute where TableName = '";
    sql += tablename + "' and ComplexAttribName = '" + imgattribname;
    sql += "' and ComplexAttribType = 'stillimage'";
    query->Run(sql);    

    if (!query->Eof())
        if (query->IntFieldByIndex(0) > 0)
            res = true;
    return res;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsStillimageAttributeReference(string attribname)
{
    bool res = false;
    string sql;
    sql = "select Count(*) as amount from CDD$ComplexAttribMetric where ComplexAttribName = '";
    sql += attribname;
    sql += "'";
    query->Run(sql);
    if (!query->Eof())
        if (query->IntFieldByIndex(0) > 0)
            res = true;
    return res;
}

int
DataDictionaryAccess::IsValidMetric(string metricname, string tablename, string attribname)
{

    int result = 0;

    string sql;
    sql = "select MetricCode from CDD$MetricStruct where MetricName = '";
    sql += metricname;
    sql += "'";
    query->Open(sql);    
    if (!query->Eof())
        result = query->IntFieldByIndex(0);

    if (result > 0) {

        int aux = 0;

        stringstream out;
        out << result;
        sql = "select Count(*) as amount from CDD$ComplexAttribMetric where TableName = '";
        sql += tablename;
        sql += "' and ComplexAttribName = '";
        sql += attribname;
        sql += "' and MetricCode = ";
        sql += out.str();
        query->Open(sql);        
        if (!query->Eof())
            aux = query->IntFieldByIndex(0);
        if (aux == 0)
            result = 0;
    }

    return result;
}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::IsComplexAttribIndex(string caindex)
{
    bool result = false;
    sql = "select count(*) as amount from CDD$ComplexAttribMetric where IndexName = '";
    sql += caindex;
    sql += "'";
    query->Open(sql);
    if (!query->Eof())
        if (query->IntFieldByIndex(0) > 0)
            result = true;
    return result;
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::GetDbmsDataType(string attribtype)
{

    string sql;

    Query *Qry = qry;

    if (this->connectionsEnum == POSTGRESQL) {
        sql = "select tablename from pg_tables where tableowner = user and tablename = 'tmp$attribname'";
    } else if (this->connectionsEnum == POSTGRESQL) {
        sql = "select table_name from all_tables where owner = user and table_name = 'TMP$ATTRIBNAME'";
    }
    Qry->Open(sql);    
    if (!Qry->Eof()) {
        if (Qry->StringFieldByIndex(0).compare("") != 0) {

            sql = "drop table TMP$ATTRIBNAME";
            Qry->Run(sql);            
        }
    }

    sql = "create table TMP$ATTRIBNAME (X ";
    sql += attribtype;
    sql += ")";
    Qry->Run(sql);
    

#ifdef POSTGRES
    sql =
            "select data_type from pg_tables join information_schema.columns on table_name = tablename where tableowner = 'postgres' and table_name = 'tmp$attribname' and column_name = 'x'";
#else
    sql =
            "select data_type from sys.all_tab_columns where owner = user and table_name = 'TMP$ATTRIBNAME' and column_name = 'X'";
#endif

    Qry->Open(sql);
    string result = "";

    if (!Qry->Eof())
        result = Qry->StringFieldByIndex(0);

    sql = "drop table TMP$ATTRIBNAME";
    Qry->Run(sql);
    
    delete Qry;
    transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::GetDefaultMetric(string tablename, string attribname)
{

    string result = "";

    sql =
            "select ms.MetricName from CDD$ComplexAttribMetric im, CDD$MetricStruct ms where im.metriccode = ms.metriccode and TableName = '"
            + tablename + "' and ComplexAttribName = '" + attribname + "' and IsDefault = 'y'";
       query->Open(sql);
    if (!query->Eof())
        result = query->StringFieldByIndex(0);

    return result;
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::GetMetricType(int metriccode)
{
    string result = "";

    stringstream out;
    out << metriccode;
    sql = "select MetricType from CDD$MetricStruct where metriccode = " + out.str();
    query->Open(sql);
    if (!query->Eof())
        result = query->StringFieldByIndex(0);

    return result;
}

//---------------------------------------------------------------------------

int
DataDictionaryAccess::GetMetricCode(string metricname)
{

    int result = 0;

    sql = "select MetricCode from CDD$MetricStruct where metricname = '" + metricname + "'";
    query->Open(sql);
        if (!query->Eof())
            result = query->IntFieldByIndex(0);
    return result;
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::GetMetrics(string tablename, string attribname,
        vector<string> *complexattriblist, vector<string> *metricreflist)
{

    sql = "select MetricCode from CDD$ComplexAttribMetric where TableName = '" + tablename
            + "' and ComplexAttribName = '" + attribname + "'";
    query->Open(sql);
    
        while (!query->Eof()) {
        metricreflist->push_back(query->StringFieldByIndex(0));
        complexattriblist->push_back(attribname);
    }
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::GetIndexFile(string tablename, string attribname, string metricname)
{

    string result = "";
    string sql;

    sql =
            "select im.IndexFile from CDD$ComplexAttribMetric im, CDD$MetricStruct ms where im.MetricCode = ms.MetricCode ";
    sql += "and im.TableName = '" + tablename + "' ";
    sql += "and im.ComplexAttribName = '" + attribname + "' ";
    sql += "and ms.MetricName = '" + metricname + "' ";

    query->Open(sql);
    if (!query->Eof())
        result = query->StringFieldByIndex(0);

    return result;
}

//---------------------------------------------------------------------------

vector<string> *
DataDictionaryAccess::GetStillimageAttribute(string tablename)
{

    vector<string> *stl = new vector<string > ();

    sql = "select distinct ComplexAttribName from CDD$ComplexAttribute where TableName = '" + tablename
            + "' and ComplexAttribType = 'stillimage'";
    query->Open(sql);
    

    while (!query->Eof()) {
        stl->push_back(query->StringFieldByIndex(0));
    }

    return stl;
}

//---------------------------------------------------------------------------

vector<string> *
DataDictionaryAccess::GetParticulateAttribute(string tablename)
{

    vector<string> *stl = new vector<string>;

    sql = "select distinct ComplexAttribName from CDD$ComplexAttribute where TableName = '" + tablename
            + "' and ComplexAttribType = 'particulate'";
    query->Open(sql);

    while (!query->Eof())
        stl->push_back(query->StringFieldByIndex(0));

    return stl;
}

//---------------------------------------------------------------------------

vector<string> *
DataDictionaryAccess::GetAttributes(string tablename)
{

    vector<string> *stl = new vector<string > ();

    sql = "select * from ";
    sql += tablename;
    query->Open(sql);

    if (!query->Eof()) {
        for (int i = 0; i < query->FieldsCount(); i++) {
            string name = query->FieldNameByIndex(i);
            transform(name.begin(), name.end(), name.begin(), ::tolower);
            stl->push_back(name);
        }
    }
    return stl;
}

//---------------------------------------------------------------------------

vector<string> *
DataDictionaryAccess::GetAttributes(string tablename,
        vector<string> *mmattriblist)
{

    vector<string> *attriblist = new vector<string > ();

    sql = "select * from " + tablename;
        query->Open(sql);

    // Inserts the other attribute references (except the image attribute references)
    if (!query->Eof()) {
        for (int i = 0; i < query->FieldsCount(); i++) {
            if (atoi(mmattriblist->at(atoi(query->FieldNameByIndex(i).c_str())).c_str()) == -1) {
                attriblist->push_back(query->FieldNameByIndex(i));
            }
        }
    }
    return attriblist;
}

//---------------------------------------------------------------------------

vector<string> *
DataDictionaryAccess::GetTableNameFromTokenList(Token *tokenlist, string attribname)
{

    vector<string> *tablenamelist = new vector<string > ();

    // auxiliary pointer to the token "from"
    Token *p_from = tokenlist;
    while (p_from->Lexem.compare("from") != 0) {
        p_from = p_from->Next;
    }

    Token *tk2 = p_from->Next;
    // Verify the tables listed in the from clause
    while ((tk2 != NULL) && (tk2->Lexem.compare("where") != 0)) {

        string tname = tk2->Lexem;
        // Verify if the column reference is an image column reference
        vector<string>::iterator ittable;
        ittable = find(tablenamelist->begin(), tablenamelist->end(), tname);
        if ((IsComplexAttributeReference(tname, attribname).compare("") != 0) && (ittable == tablenamelist->end())) {
            tablenamelist->push_back(tname);
        }

        if (tk2 != NULL)
            tk2 = tk2->Next;
    }

    return tablenamelist;
}

//---------------------------------------------------------------------------

int
DataDictionaryAccess::GetDistanceFunction(string tablename, string attribname, string metricname)
{

    int result = 0;

    sql =
            "select ms.LpP from CDD$ComplexAttribMetric im, CDD$MetricStruct ms where im.metriccode = ms.metriccode and TableName = '";
    sql += tablename;
    sql += "' and ComplexAttribName = '";
    sql += attribname;
    sql += "' and ms.MetricName = '";
    sql += metricname;
    sql += "'";
        query->Open(sql);
    if (!query->Eof())
        result = query->IntFieldByIndex(0);

    return result;
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::GetParameterType(string parametername, int extractorcode)
{

    string paramtype = "";

    sql = "select parametertype from EPD$Parameters where parametername = '";
    sql += parametername;
    sql += "' and extractorcode = ";
    sql += extractorcode;
        query->Open(sql);
    if (!query->Eof())
        paramtype = query->StringFieldByIndex(0);
    return paramtype;

}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::GetParameters(int metriccode, vector<string> *attriblist)
{

    sql = "select ParameterName from CDD$ParameterStruct where MetricCode = ";
    sql += lexical_cast<string>(metriccode);
    sql += " order by ParameterOrder";

    query->Open(sql);

    while (!query->Eof()) {
        attriblist->push_back(query->StringFieldByIndex(0));
        //attriblist->push_back(query->StringFieldByIndex(1));
    }

}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::GetParticleType(string tablename, string particlecolname)
{

    Query *Qry = qry;

    string result = "";

#ifdef POSTGRES
    sql =
            "select data_type from pg_tables join informat(i)on_schema.columns on table_name = tablename where tableowner = 'postgres' and table_name = '"
            + tablename + "' and column_name = '" + particlecolname
            + "'";
#else
    transform(tablename.begin(), tablename.end(), tablename.begin(), ::toupper);
    transform(particlecolname.begin(), particlecolname.end(), particlecolname.begin(), ::toupper);

    sql = "select data_type from sys.all_tab_columns where owner = user and table_name = '" + tablename
            + "' and column_name = '" + particlecolname + "'";
#endif
        Qry->Open(sql);

    if (!query->Eof())
        result = Qry->StringFieldByIndex(0);

    delete Qry;

    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;

}

//---------------------------------------------------------------------------
//@todo fix the database

void
DataDictionaryAccess::DropComplexIndex(string indexname)
{

    sql = "select TableName, ComplexAttribName, MetricCode, IndexFile from CDD$ComplexAttribMetric where IndexName = '"
            + indexname + "'";
    query->Open(sql);

    // deleting the index file
    remove(query->StringFieldByIndex(3).c_str());

    string tname = query->StringFieldByIndex(0);
    string caname = query->StringFieldByIndex(1);
    int mcode = query->IntFieldByIndex(2);
    stringstream out;
    out << mcode;

    sql = "select count(*) as amount from CDD$MetricInstance where TableName = '";
    sql += tname;
    sql += "' and ComplexAttribName = '";
    sql += caname;
    sql += "' and MetricCode = ";
    sql += out.str();
        query->Open(sql);

    // deleting the tuple of CDD$MetricInstance (for indexes built for particulate attributes)
    if (query->IntFieldByIndex(0) > 0) {

        sql = "delete from CDD$MetricInstance where TableName = '";
        sql += tname;
        sql += "' and ComplexAttribName = '";
        sql += caname;
        sql += "' and MetricCode = ";
        sql += out.str();
            query->Run(sql);
    }

    // deleting the tuple of CDD$ComplexAttribMetric

    sql = "delete from CDD$ComplexAttribMetric where IndexName = '" + indexname + "'";
        query->Run(sql);
    
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::UpdateTableComplexAttribMetric(string tablename, string imageattr)
{

    // deleting the tuple of CDD$ComplexAttribMetric

    sql = "update CDD$ComplexAttribMetric set IsDefault = 'n' where TableName = '" + tablename
            + "' and ComplexAttribName = '" + imageattr + "'";
    query->Run(sql);    
}

//---------------------------------------------------------------------------
/*void DataDictionaryAccess::StartTransaction() {

 } */

//---------------------------------------------------------------------------

void
DataDictionaryAccess::CommitTransaction()
{
    SqlConnection->Commit();
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::RollbackTransaction()
{
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::CreateTableStillimage(Token *tokenlist, vector<string> *createlist)
{
    //ORACLE
    // create table IPV$TableName_ColumnName '('
    //      image_id integer,
    //      image blob,
    //      foreign key (image_id) references TableName '(' ColumnName')' ')'

    //POSTGRES
    // create table IPV$TableName_ColumnName '('
    //      image_id integer,
    //      image OID,
    //      foreign key (image_id) references TableName '(' ColumnName')' ')'

    // table name
    string tablename = tokenlist->Next->Next->Lexem;

    // column name
    Token *tk = tokenlist;
    Token *tk_prev = tokenlist;
    Token *tk_prev_prev = tokenlist;
    while (tk->Next != NULL) {
        tk_prev_prev = tk_prev;
        tk_prev = tk;
        tk = tk->Next;
    }
    string columnname = tk_prev_prev->Lexem;

#ifdef POSTGRES
    string command =
            "create table IPV$" + tablename + "_" + columnname
            + " ( image_id integer primary key, image OID, foreign key (image_id) references "
            + tablename + " (" + columnname + "))";
#else
    string command = "create table IPV$" + tablename + "_" + columnname
            + " ( image_id integer primary key, image blob, foreign key (image_id) references " + tablename + " ("
            + columnname + "))";
#endif

    createlist->push_back(command);
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::CreateTableAudio(Token *tokenlist, vector<string> *createlist)
{
    //ORACLE
    // create table APV$TableName_ColumnName '('
    //      audio_id integer,
    //      audio blob,
    //      foreign key (audio_id) references TableName '(' ColumnName')' ')'

    //POSTGRES
    // create table APV$TableName_ColumnName '('
    //      audio_id integer,
    //      audio OID,
    //      foreign key (audio_id) references TableName '(' ColumnName')' ')'

    // table name
    string tablename = tokenlist->Next->Next->Lexem;

    // column name
    Token *tk = tokenlist;
    Token *tk_prev = tokenlist;
    Token *tk_prev_prev = tokenlist;
    while (tk->Next != NULL) {
        tk_prev_prev = tk_prev;
        tk_prev = tk;
        tk = tk->Next;
    }
    string columnname = tk_prev_prev->Lexem;
    string command;

#ifdef POSTGRES
    command =
            "create table APV$" + tablename + "_" + columnname
            + " ( audio_id integer primary key, audio OID, foreign key (audio_id) references "
            + tablename + " (" + columnname + "))";
#else
    command = "create table APV$" + tablename + "_" + columnname
            + " ( audio_id integer primary key, audio blob, foreign key (audio_id) references " + tablename + " ("
            + columnname + "))";
#endif

    createlist->push_back(command);
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::CreateTableInsertComplexAttribute(vector<string> *commandlist, Token *tokenlist,
        string complexattribname, string complexattribtype)
{

    string tablename = "";

    // gets the table name
    tablename = tokenlist->Next->Next->Lexem;

    string command = "insert into CDD$ComplexAttribute (TableName, ComplexAttribName, ComplexAttribType) values ('";
    command = command + tablename + "', '" + complexattribname + "', '" + complexattribtype + "')";
    commandlist->push_back(command);
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::CreateTableInsertComplexAttribMetric(Token *tokenlist, string tablename,
        vector<string> *commandlist, string complexattribname, int metriccode, bool isdefault)
{

    char IsDefault = 'n';
    if (isdefault)
        IsDefault = 'y';

    // index code
    int indexcode = this->query->runSequence("IndexIdSequence");
    string treename = "metrictree";
    stringstream out;
    out << indexcode;
    treename += out.str();
    treename += ".dat";

    string indexname = "";

    // gets the index name
    if (tokenlist->Next->Lexem.compare("table") == 0) {
        // when is a CREATE TABLE statement
        stringstream out;
        out << indexcode;
        indexname = "index";
        indexname += out.str();
    } else {
        // when is a CREATE INDEX statement
        indexname = tokenlist->Next->Next->Lexem;
    }

    string command =
            "insert into CDD$ComplexAttribMetric (TableName, ComplexAttribName, MetricCode, IsDefault, IndexName, IndexFile) values ('";
    out.str("");
    out << metriccode;
    command = command + tablename + "', '" + complexattribname + "', " + out.str() + ", '" + IsDefault + "', '"
            + indexname + "', '" + treename + "')";
    commandlist->push_back(command);
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::CreateTableInsertMetricInstance(vector<string> *commandlist, string tablename,
        string complexattribname, int metriccode, string parametername, int extractorcode,
        string parameterinstance)
{

    stringstream out;
    stringstream out2;
    out << metriccode;
    string command =
            "insert into CDD$MetricInstance (TableName, ComplexAttribName, MetricCode, ParameterName, ExtractorCode, ParameterInstance) values ('";
    command += tablename;
    command += "', '";
    command += complexattribname;
    command += "', ";
    command += out.str();
    command += ", '";
    command += parametername;
    command += "', ";
    out2 << extractorcode;
    command += out2.str();
    command += ", '";
    command += parameterinstance;
    command += "')";
    commandlist->push_back(command);

}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::CreateTableAlterPVTable(string attribtype, string tablename,
        vector<string> *commandlist, string complexattribname, int metriccode,
        vector<string> *complexattriblist, vector<string> *metricreflist)
{

    // list of metrics
    string metric = "";

    // getting all metrics
    for (unsigned int i = 0; i < complexattriblist->size(); i++) {
        if (complexattriblist->at(i) == complexattribname) {
            if (metric.compare("") != 0)
                metric = metric + ", ";
            metric = metric + metricreflist->at(i);
        }
    }

    string subselect = "";
    // getting all parameter names for all the metrics associated with a complex attribute

    //subselect = "select ParameterName from EPD$Parameters where ExtractorCode in";
    //subselect = subselect + "(select distinct ExtractorCode from CDD$ParameterStruct where MetricCode in (" + metric + "))";

    subselect = "select ParameterName from CDD$ParameterStruct where ";
    subselect += "ExtractorCode in (select distinct ExtractorCode from CDD$ParameterStruct where MetricCode in ("
            + metric + ")) and ";
    subselect += "ParameterName in (select distinct ParameterName from CDD$ParameterStruct where MetricCode in ("
            + metric + ")) ";

    // testing if it is the first metric
    string select = "";
    stringstream out;
    out << metriccode;
    if (metric.compare("") == 0)
        select = "select ParameterName from CDD$ParameterStruct where MetricCode = " + out.str();
    else
        select = "select ParameterName from CDD$ParameterStruct where MetricCode = " + out.str()
        + " and ParameterName not in (" + subselect + ")";

    // extractors name

    sql = select;
    query->Open(sql);
    

    string tabname = "";
    if (attribtype.compare("stillimage") == 0)
        tabname = "IPV$";
    else if (attribtype.compare("audio") == 0)
        tabname = "APV$";

    while (!query->Eof()) {

#ifdef POSTGRES
        string command = "alter table " + tabname + tablename + "_"
                + complexattribname + " add " + query->FieldByIndex(0) + " OID";
#else
        string command = "alter table " + tabname + tablename + "_" + complexattribname + " add "
                + query->StringFieldByIndex(0) + " blob";
#endif

        commandlist->push_back(command);

    }

}

//---------------------------------------------------------------------------

bool
DataDictionaryAccess::SelectAsteriskStatement(Token *tokenlist, int scope)
{

    if (tokenlist->Next->TokenType != TK_MUL)
        return false;

    // auxiliary variable used to check if it is needed to remove the "*"
    bool hasmultimediaattrib = false;

    // table name
    string tablename = "";

    // auxiliary pointer to the tokenlist
    Token *tk = tokenlist;

    // auxiliary list that stores the attributes references
    Token *auxselectlist = new Token();
    //@todo Verificar
    Token *selectlist = auxselectlist;

    // list of stillimage attributes
    vector<string> *tableimgattriblist;

    // auxiliary variable used to check if it is needed to add a comma in the select list
    // before the attributes of an other table
    bool firsttable = false;

    while (tk != NULL) {

        // for each table
        if (tk->LexemType == LK_TABLE_NAME) {

            // verifies if the table has image attributes
            tablename = tk->Lexem;
            tableimgattriblist = GetStillimageAttribute(tablename);
            

            if (tableimgattriblist->size() > 0) {

                hasmultimediaattrib = true;

                if (firsttable) {
                    auxselectlist = AddToken(auxselectlist, ",", TK_COMMA, LK_UNDEFINED);
                }

                // Updates the tokenlist with the join conditions and adds the image attributes references in an auxiliary tokenlist
                for (unsigned int i = 0; i < tableimgattriblist->size(); i++) {

                    // *** adds the table references and join conditions in the tokenlist appropriate place ***
                    // SELECT * FROM table1 JOIN table2 ON table1.attr1 = table2.Image_Id

                    Token *aux = tk;

                    // adding "JOIN"
                    aux = AddToken(aux, "join", TK_IDENTIFIER, LK_UNDEFINED);

                    // adding "table2"
                    string reftable = "IPV$" + tablename + "_" + tableimgattriblist->at(i);
                    aux = AddToken(aux, reftable, TK_IDENTIFIER, LK_TABLE_NAME);

                    // adding "ON"
                    aux = AddToken(aux, "on", TK_IDENTIFIER, LK_UNDEFINED);

                    // adding "table1"
                    aux = AddToken(aux, tablename, TK_IDENTIFIER, LK_TABLE_NAME_REF);

                    // adding "."
                    aux = AddToken(aux, ".", TK_PERIOD, LK_UNDEFINED);

                    // adding "attr1"
                    aux = AddToken(aux, tableimgattriblist->at(i), TK_IDENTIFIER, LK_UNDEFINED);

                    // adding "="
                    aux = AddToken(aux, "=", TK_EQUAL, LK_UNDEFINED);

                    // adding "table2"
                    aux = AddToken(aux, reftable, TK_IDENTIFIER, LK_TABLE_NAME_REF);

                    // adding "."
                    aux = AddToken(aux, ".", TK_PERIOD, LK_UNDEFINED);

                    // adding "Image_Id"
                    aux = AddToken(aux, "image_id", TK_EQUAL, LK_UNDEFINED);

                    //  *** adds the image attribute references in the selectlist "tablenameref.Image" ***
                    // adding "tablenameref"
                    auxselectlist = AddToken(auxselectlist, "IPV$" + tablename + "_" + tableimgattriblist->at(i),
                            TK_IDENTIFIER, LK_TABLE_NAME_REF);

                    // adding "."
                    auxselectlist = AddToken(auxselectlist, ".", TK_PERIOD, LK_UNDEFINED);

                    // adding "Image"
                    if (scope == 0)
                        auxselectlist = AddToken(auxselectlist, "Image", TK_IDENTIFIER, LK_UNDEFINED);
                    else
                        auxselectlist = AddToken(auxselectlist, "*", TK_IDENTIFIER, LK_UNDEFINED);

                    if (scope == 0) {
                        // adding "as"
                        auxselectlist = AddToken(auxselectlist, "as", TK_IDENTIFIER, LK_UNDEFINED);

                        // adding the attribute alias
                        auxselectlist = AddToken(auxselectlist, tableimgattriblist->at(i), TK_IDENTIFIER, LK_UNDEFINED);
                    }

                    // adding ","
                    if (i + 1 != tableimgattriblist->size())
                        auxselectlist = AddToken(auxselectlist, ",", TK_COMMA, LK_UNDEFINED);
                }

                if ((tableimgattriblist->size() > 0))
                    auxselectlist = AddToken(auxselectlist, ",", TK_COMMA, LK_UNDEFINED);

                // Updates the tokenlist with the join conditions and adds the audio attributes references in an auxiliary tokenlist

                // Gets the other attributes of tablename
                vector<string> * attriblist = new vector<string > ();
                vector<string> * tmplist = new vector<string > ();
                for (unsigned int i = 0; i < tableimgattriblist->size(); i++)
                    tmplist->push_back(tableimgattriblist->at(i));
                attriblist = GetAttributes(tablename, tmplist);
                delete tmplist;

                // Verifies if is needed to add an "," in the selectlist
                if (attriblist->size() > 0)
                    // adding ","
                    auxselectlist = AddToken(auxselectlist, ",", TK_COMMA, LK_UNDEFINED);

                // adds all the other attributes in the selectlist
                for (unsigned int i = 0; i < attriblist->size(); i++) {
                    // *** It is needed to add: "tablenameref.attribname, " ***
                    // adding "tablenameref"
                    auxselectlist = AddToken(auxselectlist, tablename, TK_IDENTIFIER, LK_TABLE_NAME_REF);

                    // adding "."
                    auxselectlist = AddToken(auxselectlist, ".", TK_PERIOD, LK_UNDEFINED);

                    // adding "attribname"
                    auxselectlist = AddToken(auxselectlist, attriblist->at(i), TK_IDENTIFIER, LK_UNDEFINED);

                    // adding ","
                    if (i + 1 != attriblist->size())
                        auxselectlist = AddToken(auxselectlist, ",", TK_COMMA, LK_UNDEFINED);

                }

                firsttable = true;
            }
        }

        tk = tk->Next;
    }

    if (hasmultimediaattrib) {
        // Removes the "*" from the select clause and includes the attributes of the selectlist
        tk = tokenlist;
        while (tk->Next != NULL) {

            Token *auxlist, *auxlist2;
            if (tk->Next->TokenType == TK_MUL) {
                auxlist = tk->Next;
                auxlist2 = selectlist;
                auxselectlist->Next = auxlist->Next;
                // Here, it is removed the extra token inserted in the inicializat(i)on of the selectlist
                tk->Next = selectlist->Next;
                delete auxlist;
                delete auxlist2;
            }
            tk = tk->Next;
        }
    }

    // cleaning the house!
    if (tableimgattriblist != NULL)
        delete tableimgattriblist;

    return true;
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::SelectColumnRefListStatement(Token *tokenlist, int scope)
{

    // error message
    string errmsg = "";

    // table name
    string tablename = "";

    // table name reference
    string tablenameref = "";

    // attribute name
    string attribname = "";

    // attribute complex type
    string attribtype = "";

    // alias name
    string aliasname = "";

    // auxiliary pointers to the tokenlist
    Token *tk1_p = tokenlist;
    Token *tk1 = tokenlist;
    Token *tk2 = tokenlist;

    // auxiliary pointer to the token "from"
    Token *p_from = tokenlist;
    while (p_from->Lexem.compare("from") != 0) {
        p_from = p_from->Next;
    }

    // For each token before the lexem "from" (examining the select list)
    while ((tk1->Next != NULL) && (tk1->Lexem.compare("from") != 0)) {

        // if the next token is not an period then the token must be an column reference
        if (tk1->Next->TokenType != TK_PERIOD) {

            tk2 = p_from->Next;
            // Verify the tables listed in the from clause
            while ((tk2 != NULL) && (tk2->Lexem.compare("where") != 0)) {

                tablename = tk2->Lexem;
                attribname = tk1->Lexem;
                aliasname = "";
                if ((tk2->Next != NULL) && (tk2->Next->TokenType == TK_PERIOD))
                    attribtype = "";
                else
                    attribtype = IsComplexAttributeReference(tablename, attribname);
                // Verify if the column reference is an image/audio column reference
                if ((attribtype.compare("stillimage") == 0) || (attribtype.compare("audio") == 0)) {

                    Token * auxlist1;
                    // Replace the initial image id/ audio id column reference for the image/audio column reference

                    // Verifies if there is an alias specified by the user
                    if (tk1->Next->Lexem.compare("as") == 0) {
                        Token *auxlist2, *auxlist3;
                        aliasname = tk1->Next->Next->Lexem;
                        auxlist1 = tk1; // "column reference"
                        auxlist2 = tk1->Next; // "as"
                        auxlist3 = tk1->Next->Next; // "identifier"
                        tk1_p->Next = tk1->Next->Next->Next;
                        delete auxlist1;
                        delete auxlist2;
                        delete auxlist3;
                    } else {
                        // Removes the initial column reference
                        auxlist1 = tk1;
                        tk1_p->Next = tk1->Next;
                        delete auxlist1;
                    }

                    // *** adds a new multimedia column reference (IPV$tablename_attribname.image) or (APV$tablename_attribname.audio) ***
                    if (scope == 0) {
                        if (attribtype.compare("stillimage") == 0) {
                            AddMMColRef(&tk1, &tk1_p, &tk2, "IPV$", tablename, "image", attribname, "image_id",
                                    aliasname, scope);
                        } else if (attribtype.compare("audio") == 0) {
                            AddMMColRef(&tk1, &tk1_p, &tk2, "APV$", tablename, "audio", attribname, "audio_id",
                                    aliasname, scope);
                        }
                    } else {
                        if (attribtype.compare("stillimage") == 0) {
                            AddMMColRef(&tk1, &tk1_p, &tk2, "IPV$", tablename, "*", attribname, "image_id", aliasname,
                                    scope);
                        } else if (attribtype.compare("audio") == 0) {
                            AddMMColRef(&tk1, &tk1_p, &tk2, "APV$", tablename, "*", attribname, "audio_id", aliasname,
                                    scope);
                        }
                    }
                }

                // next table
                tk2 = tk2->Next;
            }

        } else {

            // if the next token is an period then the token must be an table reference
            tk2 = p_from->Next;
            // Verify the tables listed in the from clause
            while ((tk2 != NULL) && (tk2->Lexem.compare("where") != 0)) {

                tablename = tk2->Lexem;
                tablenameref = tk1->Lexem;
                attribname = tk1->Next->Next->Lexem;
                aliasname = "";
                if ((tk2->Next != NULL) && (tk2->Next->TokenType == TK_PERIOD))
                    attribtype = "";
                else
                    attribtype = IsComplexAttributeReference(tablename, attribname);

                // Verify if the column reference is an image/audio column reference and
                // if the table name reference is a valid table name (i.e. if it is in the from clause)
                if ((attribtype.compare("stillimage") == 0) || (attribtype.compare("audio") == 0)) {

                    if (tablename.compare(tablenameref) == 0) {

                        // Substitute the initial image id/audio id column reference (tableref1.imageid) or (tableref1.audioid)
                        // for the image/audio column reference (tableref2.image) or (tableref2.audio)

                        Token * auxlist1, *auxlist2, *auxlist3;
                        // Verifies if there is an alias specified by the user
                        if (tk1->Next->Next->Next->Lexem.compare("as") == 0) {
                            Token *auxlist4, *auxlist5;
                            aliasname = tk1->Next->Next->Next->Next->Lexem;
                            auxlist1 = tk1; // "tableref"
                            auxlist2 = tk1->Next; // "."
                            auxlist3 = tk1->Next->Next; // "attribname"
                            auxlist4 = tk1->Next->Next->Next; // "as"
                            auxlist5 = tk1->Next->Next->Next->Next; // "identifier"
                            tk1_p->Next = tk1->Next->Next->Next->Next->Next;
                            delete auxlist1;
                            delete auxlist2;
                            delete auxlist3;
                            delete auxlist4;
                            delete auxlist5;
                        } else {
                            // Removes the initial column reference (tableref.attribname)
                            auxlist1 = tk1; // "tableref"
                            auxlist2 = tk1->Next; // "."
                            auxlist3 = tk1->Next->Next; // "attribname"
                            tk1_p->Next = tk1->Next->Next->Next;
                            delete auxlist1;
                            delete auxlist2;
                            delete auxlist3;
                        }

                        // *** adds a new multimedia column reference (IPV$tablename_attribname.image) or (APV$tablename_attribname.audio) ***
                        if (scope == 0) {
                            if (attribtype.compare("stillimage") == 0)
                                AddMMColRef(&tk1, &tk1_p, &tk2, "IPV$", tablename, "image", attribname, "image_id",
                                    aliasname, scope);
                            else if (attribtype.compare("audio") == 0)
                                AddMMColRef(&tk1, &tk1_p, &tk2, "APV$", tablename, "audio", attribname, "audio_id",
                                    aliasname, scope);
                        } else {
                            if (attribtype.compare("stillimage") == 0)
                                AddMMColRef(&tk1, &tk1_p, &tk2, "IPV$", tablename, "*", attribname, "image_id",
                                    aliasname, scope);
                            else if (attribtype.compare("audio") == 0)
                                AddMMColRef(&tk1, &tk1_p, &tk2, "APV$", tablename, "*", attribname, "audio_id",
                                    aliasname, scope);
                        }

                        errmsg = "";
                    } else {
                        errmsg = "Wrong table name reference: ";
                        errmsg += tablenameref;
                        errmsg += "!";
                        // updates the pointers for the next attribute reference
                        tk1_p = tk1;
                        tk1 = tk1->Next; // "."
                        tk1_p = tk1;
                        tk1 = tk1->Next; // "attribname"
                    }
                } else {
                    // updates the pointers for the next attribute reference
                    tk1_p = tk1;
                    tk1 = tk1->Next; // "."
                    tk1_p = tk1;
                    tk1 = tk1->Next; // "attribname"
                }
                tk2 = tk2->Next;
            }
        }
        tk1_p = tk1;
        tk1 = tk1->Next;
    }

    return errmsg;

}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::AddMMColRef(Token **tk1, Token **tk1_p, Token **tk2, string mmtabletype,
        string tablename, string mmattribtype, string attribname, string mmattribid,
        string aliasname, int scope)
{

    // adding the table name reference "IPV$tablename_attribname" or "APV$tablename_attribname"
    *tk1 = AddToken(*tk1_p, mmtabletype + tablename + "_" + attribname, TK_IDENTIFIER, LK_TABLE_NAME_REF);

    *tk1_p = *tk1;
    // adding the "."
    *tk1 = AddToken(*tk1, ".", TK_PERIOD, LK_UNDEFINED);

    *tk1_p = *tk1;
    // adding the attribute name "image" or "audio"
    *tk1 = AddToken(*tk1, mmattribtype, TK_IDENTIFIER, LK_UNDEFINED);

    if (scope == 0) {
        *tk1_p = *tk1;
        // adding "as"
        *tk1 = AddToken(*tk1, "as", TK_IDENTIFIER, LK_UNDEFINED);

        *tk1_p = *tk1;
        // adding the attribute alias
        if (aliasname.compare("") == 0)
            *tk1 = AddToken(*tk1, attribname, TK_IDENTIFIER, LK_UNDEFINED);
        else
            *tk1 = AddToken(*tk1, aliasname, TK_IDENTIFIER, LK_UNDEFINED);
    }

    // *** adds the table references and join conditions in the tokenlist appropriate place ***
    // SELECT * FROM table1 JOIN	 table2 ON table1.attr1 = table2.Image_Id or
    // SELECT * FROM table1 JOIN table2 ON table1.attr1 = table2.Audio_Id

    // adding "JOIN"
    *tk2 = AddToken(*tk2, "join", TK_IDENTIFIER, LK_UNDEFINED);

    // adding "table2"
    string reftable = mmtabletype + tablename + "_" + attribname;
    *tk2 = AddToken(*tk2, reftable, TK_IDENTIFIER, LK_TABLE_NAME);

    // adding "ON"
    *tk2 = AddToken(*tk2, "on", TK_IDENTIFIER, LK_UNDEFINED);

    // adding "table1"
    *tk2 = AddToken(*tk2, tablename, TK_IDENTIFIER, LK_TABLE_NAME_REF);

    // adding "."
    *tk2 = AddToken(*tk2, ".", TK_PERIOD, LK_UNDEFINED);

    // adding "attr1"
    *tk2 = AddToken(*tk2, attribname, TK_IDENTIFIER, LK_UNDEFINED);

    // adding "="
    *tk2 = AddToken(*tk2, "=", TK_EQUAL, LK_UNDEFINED);

    // adding "table2"
    *tk2 = AddToken(*tk2, reftable, TK_IDENTIFIER, LK_TABLE_NAME_REF);

    // adding "."
    *tk2 = AddToken(*tk2, ".", TK_PERIOD, LK_UNDEFINED);

    // adding "Image_Id" or "Audio_Id"
    *tk2 = AddToken(*tk2, mmattribid, TK_EQUAL, LK_UNDEFINED);

}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::NearAndFarPredicates(Token **tokenlist, int activetokenlist)
{

    // *** EXAMPLE ***
    // --> statement defined by the user:
    // SELECT ...
    // FROM ...
    // WHERE complex_obj_reference NEAR/FAR [similarity_grouping]
    //       complex_obj_definition [BY metric_name]
    //       [RANGE value]
    //       [STOP AFTER k]
    // --> statement that will be executed by SIREN:
    // SELECT ...
    // FROM ...
    // WHERE complex_obj_id_reference IN (complex_obj_id_reference_list)

    // error message
    string errmsg = "";

    // sql statement
    string sql;

    // stores the tokens 'near' or 'far' for further processing
    string predicate = "";

    // pointer to the token list
    Token *filequeryobject;

    // metric name
    string metricname = "";

    // auxiliary variable that stores a table name list
    vector<string> * tbnamelist = new vector<string > ();

    // table names
    string tablename = "";
    string tablename2 = "";

    // attribute names
    string attribname = "";
    string attribname2 = "";

    // attribute type (stillimage or particulate)
    string attribtype = "";

    // similarity grouping type
    string sgtype = "";

    // list of attribute references
    vector<string> * attrlist = new vector<string > ();

    // list of parameter values
    vector<string> * parametervaluelist = new vector<string > ();

    // list of parameters
    vector<string> * parameterlist = new vector<string > ();

    // list of parameters type
    vector<string> * parametertypelist = new vector<string > ();

    // count of parameters
    int parametercount = 0;

    // the image file name will be used on queries based on a file
    string imagefilename = "";

    // subquery is the select of the subquery
    string subquery = "";

    // the image blob will be used on queries based on a image stored in a table
    //JPGImage **jpgimage = NULL;
    //int jpgimagecount = 0;

    // knn value
    string knn = "";

    // tie list
    bool tielist = false;

    // range value
    string range = "";

    // pointer to follow the token list
    Token *list = NULL;
    list = tokenlist[activetokenlist];

    // activelist is the list that will be built
    Token *activelist = NULL;
    activelist = list->Clone();
    list = list->Next;
    Token *activelisttail = activelist;

    // query objects
    vector<tDynamicObject *> *queryobjects;
    int queryobjectscount = 0;

    // run the entire list building a new list
    while (list != NULL) {

        if ((list->Lexem.compare("near") != 0) && (list->Lexem.compare("far") != 0)) {
            // copies a token to the end of activelist
            activelisttail = AddToken(activelisttail, list->Clone());
        } else {
            tablename2 = "";
            attribname2 = "";

            metricname = "";

            // storing the tokens 'near' or 'far' for further processing
            predicate = list->Lexem;

            // storing attribute name
            attribname = list->Prior->Lexem;

            // getting table name
            if (list->Prior->Prior->Lexem.compare(".") == 0)
                tablename = list->Prior->Prior->Prior->Lexem;
            else {
                // in this case the tbnamelist will have only one item
                //to verify if the tbnamelist already has items
                tbnamelist = GetTableNameFromTokenList(tokenlist[activetokenlist], attribname);
                tablename = tbnamelist->at(tbnamelist->size() - 1);
            }

            // getting the attribute type
            attribtype = IsComplexAttributeReference(tablename, attribname);

            // next token
            list = list->Next;

            // treats similarity grouping clauses
            if (list->LexemType == LK_SUM_SG) {
                sgtype = "sum";

            } else if (list->LexemType == LK_AVG_SG) {
                sgtype = "avg";
            } else if (list->LexemType == LK_ALL_SG) {
                sgtype = "all";
            } else if (list->LexemType == LK_EVERY_SG) {
                sgtype = "every";
            } else if (list->LexemType == LK_ANY_SG) {
                sgtype = "any";
            } else if (list->LexemType == LK_MAX_SG) {
                sgtype = "max";
            }
            list = list->Next;
            // treats '('subselect')' or '('particulate_reference')'
            if (list->TokenType == TK_OPEN_BRACE) {

                // skipping token '('
                list = list->Next;

                // subselect
                if (list->Lexem.compare("subselect") == 0) {

                    // adding the token that will be substituted with the list of objects returned by similarity and pointing to it with filequeryobject
                    activelisttail = AddToken(activelisttail, list->Clone());
                    filequeryobject = activelisttail;

                    // getting scope
                    int scope = list->ScopePosition;

                    // recursive call
                    errmsg += errmsg + NearAndFarPredicates(tokenlist, list->ScopePosition);

                    // run the sql tokenlist[scope]. It must return an image

                    // mounting the subquery from "tokenlist[aux]" to "command"
                    Token *tmp = tokenlist[scope];
                    subquery = "";
                    while (tmp != NULL) {
                        subquery += tmp->Lexem;

                        if (tmp->Lexem.compare("as") == 0)
                            attrlist->push_back(tmp->Prior->Lexem);

                        if (tmp->Next != NULL) {
                            if ((tmp->Next->Lexem.compare(".") != 0) && (tmp->Lexem.compare(".") != 0))
                                subquery = subquery + " ";
                        }
                        tmp = tmp->Next;
                    }

                    // counting the records: if the query doesn't have a similarity grouping clause the subquery must return only one complex object

                    if (this->connectionsEnum == POSTGRESQL)
                        sql = "select count(*) as amount from (" + subquery + ") as subquery";
                    else if (this->connectionsEnum == ORACLE)
                        sql = "select count(*) as amount from (" + subquery + ")";
                    query->Open(sql);
                    
                    int auxcount;
                    if (!query->Eof()) {
                        auxcount = query->IntFieldByIndex(0);
                        if (auxcount == 0)
                            return "Error: query returned zero records.";
                    }

                    // running the subquery
                    sql = subquery;
                    query->Open(sql);
                    

                    if (attribtype.compare("particulate") == 0) {

                        string paramtype = "";
                        // if the query doesn't have a similarity grouping clause, the subquery must return only one particulate object
                        if (sgtype.compare("") == 0) {
                            // verifying if the query returns only one particulate object
                            if (auxcount == 1) {
                                parametercount = query->FieldsCount();
                                for (int i = 0; i < query->FieldsCount(); i++) {
                                    parameterlist->push_back(query->FieldNameByIndex(i));
                                    paramtype = GetParticleType(tablename, attrlist->at(i));
                                    if (paramtype.compare("") == 0)
                                        errmsg += "Invalid parameter reference" + parameterlist->at(i) + ".";
                                    else
                                        parametertypelist->push_back(paramtype);
                                    parametervaluelist->push_back(query->StringFieldByIndex(i));
                                }
                            } else {
                                errmsg += "After ";
                                errmsg += predicate;
                                errmsg += " it is expected only one valid particulate reference.";
                            }
                        } else // otherwise if the query has a similarity grouping clause, then the subquery can return more than one particulate object
                        {
                            query->First();
                            while (!query->Eof()) {
                                parametercount = query->FieldsCount();
                                for (int i = 0; i < query->FieldsCount(); i++) {
                                    parameterlist->push_back(query->FieldNameByIndex(i));
                                    paramtype = GetParticleType(tablename, attrlist->at(i));
                                    if (paramtype.compare("") == 0) {
                                        errmsg += "Invalid parameter reference";
                                        errmsg += parameterlist->at(i);
                                        errmsg += ".";
                                    } else
                                        parametertypelist->push_back(paramtype);
                                    parametervaluelist->push_back(query->StringFieldByIndex(i));
                                }

                            }
                        }
                    }
                    // skipping token 'subselect'
                    list = list->Next;
                } else // particulate_reference
                {
                    // adding the token that will be substituted with the list of objects returned by similarity and pointing to it with filequeryobject
                    activelisttail = AddToken(activelisttail, list->Clone());
                    filequeryobject = activelisttail;

                    while (list->TokenType != TK_CLOSE_BRACE) {

                        parametervaluelist->push_back(list->Lexem);

                        // storing the parameter type
                        if (list->TokenType == TK_INTEGER) {
                            parametertypelist->push_back(GetDbmsDataType("integer"));
                        } else if (list->TokenType == TK_FLOATING_POINT) {
                            parametertypelist->push_back(GetDbmsDataType("float"));
                        }

                        // skipping the parameter value
                        list = list->Next;
                        // skipping the lexem "as"
                        list = list->Next;

                        parameterlist->push_back(list->Lexem);

                        // skipping the parameter name
                        list = list->Next;
                        if (list->TokenType == TK_COMMA)
                            list = list->Next;

                        // computing parameter
                        parametercount++;
                    }
                }

                // skipping token ')'
                list = list->Next;
            }// treats file name
            else if (list->TokenType == TK_STRING) {

                // removing the apostrophes from the string
                list->Lexem = list->Lexem.substr(1, list->Lexem.length() - 2);

                // adding the file name and pointing to it with filequeryobject
                activelisttail = AddToken(activelisttail, list->Clone());
                filequeryobject = activelisttail;
                imagefilename = filequeryobject->Lexem;
                list = list->Next;
            }// treats complex attribute name reference (join)
            else if (list->TokenType == TK_IDENTIFIER) {
                // getting the second table name and the second attribute name
                if ((list->Next != NULL) && (list->Next->Lexem.compare(".") == 0)) {
                    tablename2 = list->Lexem;
                    list = list->Next->Next;
                    attribname2 = list->Lexem;
                } else {
                    // in this case the tbnamelist will have only one item
                    attribname2 = list->Lexem;
                    tbnamelist->clear();
                    tbnamelist = GetTableNameFromTokenList(tokenlist[activetokenlist], attribname2);
                    tablename2 = tbnamelist->at(tbnamelist->size() - 1);
                }
                list = list->Next;
            }

            if (list != NULL) {
                if (list->Lexem.compare("by") == 0) {
                    list = list->Next;
                    metricname = list->Lexem;
                    list = list->Next;

                    if ((attribtype.compare("stillimage") == 0) || (attribtype.compare("audio") == 0)) {
                        //  verify if the metricname is valid for this tablename and attribname
                        int metriccode = IsValidMetric(metricname, tablename, attribname);
                        if (metriccode <= 0)
                            errmsg += "Invalid metric '" + metricname + "'.";
                    } else // particulate
                    {
                        int metriccode = IsValidMetric(metricname, tablename, attribname);
                        if (metriccode > 0) {

                            // ParamList is a list that stores the pairs [parameter,type]
                            // of a metric created for a particulate attribute
                            vector<string> *paramauxlist = new vector<string > ();
                            GetParameters(metriccode, paramauxlist);

                            // ParamList->Count/2 because this list stores pairs [parameter,type]
                            if ((paramauxlist->size()) / 2 == parameterlist->size()) {
                                for (unsigned int j = 0; j < paramauxlist->size(); j += 2) {

                                    // get ParticulateParam position
                                    int ix = -1;
                                    unsigned int param = atoi(paramauxlist->at(j).c_str());
                                    if (parameterlist->size() != 0 && (param <= parameterlist->size()))
                                        ix = atoi(parameterlist->at(param).c_str());
                                    if (ix == -1) {
                                        errmsg += "Missing parameter: '" + paramauxlist->at(j) + "'";
                                    } else {
                                        if (((paramauxlist->at(j + 1)).compare(parametertypelist->at(ix).c_str())) != 0)
                                            errmsg += "Invalid parameter type. Expected '" + paramauxlist->at(j + 1)
                                            + "'. Found '" + parametertypelist->at(ix) + "'.";
                                    }
                                }
                            } else {
                                errmsg += "Missing or extra particulate metric parameter(s).";
                            }
                            delete paramauxlist;
                        } else {
                            errmsg += "Invalid metric '" + metricname + "'.";
                        }
                    }
                }

                if (list->Lexem.compare("stop") == 0) {
                    list = list->Next; // stop
                    list = list->Next; // after
                    knn = list->Lexem; // k
                    // optional tie list
                    if ((list->Next != NULL) && (list->Next->Lexem.compare("with") == 0)) {
                        list = list->Next; // with
                        list = list->Next; // tie
                        list = list->Next; // list
                        tielist = true;
                    }
                }

                if (list->Lexem.compare("range") == 0) {
                    list = list->Next; // range
                    range = list->Lexem; // range radius
                }

                // building the objects
                if (errmsg.compare("") == 0) {

                    if (attribname2.compare("") == 0) {
                        // build the objects
                        queryobjects = BuildSelectionObjects(tablename, attribname, metricname, imagefilename, subquery,
                                attribtype, parameterlist, parametervaluelist, parametercount, queryobjectscount);
                    }
                }

                // process each near/far predicate
                if ((errmsg.compare("") == 0) && (parser->errors == 0)) {

                    if (attribname2.compare("") == 0) {
                        /*errmsg += ProcessNearAndFarPredicates(&activelisttail, filequeryobject, predicate, tablename,
                         attribname, sgtype, metricname, imagefilename, knn, range, tielist, queryobjects,
                         queryobjectscount);*/
                    } else {
                        errmsg += ProcessJoinQueries(&activelisttail, activetokenlist, predicate, tablename, tablename2,
                                attribname, attribname2, sgtype, metricname, knn, range, tielist, attribtype,
                                parameterlist, parametervaluelist, parametertypelist);
                    }

                    //Clean parameterlist
                    int pcount = parameterlist->size();
                    while (pcount != 0) {
                        pcount--;
                        parameterlist->erase(parameterlist->begin() + pcount);
                    }
                } else
                    errmsg += "Missing 'stop after' and/or 'range' clause(s)";
            }

            // next token
            if (list != NULL)
                list = list->Next;

        }

        // delete query objects
        queryobjects->clear();
        delete queryobjects;

        // delete the tokenlist[activetokenlist]
        Token *del = tokenlist[activetokenlist];
        Token *tmp = del->Next;
        while (tmp != NULL) {
            delete del;
            del = tmp;
            tmp = tmp->Next;
        }
        delete del;

        // point to the new activetokenlist
        tokenlist[activetokenlist] = activelist;

        delete attrlist;
        delete parametervaluelist;
        delete parameterlist;
        delete parametertypelist;
        return errmsg;
    }
    return NULL;
}

vector<tDynamicObject*> *
DataDictionaryAccess::BuildSelectionObjects(string tablename, string attribname,
        string metricname, string filename, string subselect, string attribtype,
        vector<string>* parameterlist, vector<string> * parametervaluelist, int parametercount,
        int &returncount)
{

    returncount = 0;

    vector<string> *parameters = new vector<string> ();

    // if the user didn't specified the metric name, get the default
    if (metricname.compare("") == 0) {
        metricname = GetDefaultMetric(tablename, attribname);
        if (metricname.compare("") == 0) {
            parser->AddError("There isn't a default metric defined for " + tablename + "." + attribname + ".");
            return NULL;
        }
    }

    // Vector to store the objects to be used in the select.
    vector<tDynamicObject *> *obj = new vector<tDynamicObject *>();

    if (attribtype.compare("stillimage") == 0) {
           
        // from file name
        if (filename.compare("") != 0) {
            root->info("Build Selection Objects - Extract based on a file name.");
            tDynamicObject *tObject = new tDynamicObject();
            Image *img = imageUtils::createImageObjectByFilename(filename);
            root->info("Get the extractor name.");
            Query *query = sirenQueries->getExtractor(tablename, attribname, metricname);
            // For each extractor/parameter, we extract.
            while (!query->Eof()) {
                string extractor = query->StringFieldByIndex(0);
                string parameter = query->StringFieldByIndex(1);
                tObject->Extract(extractor, parameter, img);
            }
            //@TODO Isn't this the size of the vector? Why do we need it?
            returncount = 1;
            
            delete img;
            obj->push_back(tObject);
            
            // Bye! :}
            return obj;
            
        }
        // From sub select
        else if (subselect.compare("") != 0) {
            root->info("Build Selection Objects - Extract based on a sub select.");
        // And here we go.  
        // First, we execute the sub select.
        // We verify if there are parameters for that metric.
        // If there parameters, for each result of the sub query,
        // We load the parameters from the database, and populate the tDynamicObject with them.
        sql = subselect;
        qry->Open(sql);
        
        // @TODO Change this function to return the parameters list.
        GetParameters(GetMetricCode(metricname), parameters);
        
        // @TODO Make a function to return the count of the result set.
        // (The OCCI library doesnt have a method for this. The only way is loop again the result set).
            
        // There are parameters for that metric, we should extract them.
        if (parameters->size() != 0) {
            // @TODO Create a function for this.
            while (!qry->Eof()) {
                // Build the object here, and add it to the vector.
                tDynamicObject *tDynamicObjectIndex = new tDynamicObject();
                for (unsigned int j = 0; j < parameters->size(); j++) {
                    // Return the blob field with the parameter name in a char vector.
                    char *data = qry->blobFieldDumpToCharByName(parameters->at(j));
                    // Using the returned blob, dump it to the tDynamicObject.
                    tDynamicObjectIndex->setData(parameters->at(j), data);
                    delete[] data;
                }
                obj->push_back(tDynamicObjectIndex);                
            }
            return obj;
        }
        // In this case, there aren't parameters in the result query to build
        // the DynamicTypes.        
        // This raises me a question. Why does I extract all the parameters, if the
        // comparison is done with just a metric?
            else {
                // Build the still image object.  
                // @TODO Create a function to encapsulate this.
                while (!qry->Eof()) {
                    char *data = qry->blobFieldDumpToCharByName("image");
                    tDynamicObject *tDynamicObjectIndex = new tDynamicObject();
                    //@TODO Verify if the metric name is the same that parameter name.
                    tDynamicObjectIndex->setData(metricname, data);
                    delete[] data;
                    obj->push_back(tDynamicObjectIndex);                    
                }
                return obj;
            }
        }
    } else if (attribtype.compare("particulate") == 0) {
        // Here is the next point to attack.
        // What we need to do:
        // 1 - Get the number of parameters for that metric.
        // 2 - Calculate the number of objects: 
        int numberofobjects = parametervaluelist->size() / parametercount;
        vector<vector<string>*> *particulateData = sirenQueries->getParticulateData(tablename, attribname, metricname);
        vector<string> *metricParamNames = particulateData->at(1);
        vector<string> *metricParamTypes = particulateData->at(2);
        
        string neededattribute = sirenQueries->checkSelectParticles(tablename, metricname, *parameterlist);
        
        if (neededattribute.compare("") != 0) {
                parser->AddError("Particle '" + neededattribute + "' of the metric '" + metricname + "' must be informed.");
                return NULL;
        }
        
        // Here I get the sublist of parameters values for each
        // object to build.
        for (int i = 0; i < numberofobjects; i++) {
        tDynamicObject *tObject = new tDynamicObject();
        // std::vector constructor magic. :}
        vector<string> *subList = new vector<string>(&parametervaluelist->at(i*parametercount),&parametervaluelist->at((i+1)*parametercount));
        tObject = BuildParticulateObject(0, parameterlist, subList, metricParamNames, metricParamTypes);
        obj->push_back(tObject);
        delete subList;
        }

        delete metricParamNames;
        delete metricParamTypes;
        delete particulateData;
        return obj;        
}
    return NULL;
}

string
DataDictionaryAccess::ProcessNearAndFarPredicates(Token **activelisttail, Token *filequeryobject,
        string predicate, string tablename, string attribname, string sgtype, string metricname,
        string imagefilename, string knn, string range, bool tielist, tDynamicObject **obj,
        int numberofobjects)
{

    const string error = "The FAR predicate needs implementation.";

    // if the user didn't specified the metric name, get the default
    if (metricname.compare("") == 0) {
        metricname = GetDefaultMetric(tablename, attribname);
        if (metricname.compare("") == 0)
            return "There isn't a default metric defined for " + tablename + "." + attribname + ".";

    }

    // getting the metric
    int LpP = GetDistanceFunction(tablename, attribname, metricname);
    // opening the index
    DynamicSlimTree *slimtree = treemanager->OpenSlimTree(GetIndexFile(tablename, attribname, metricname), LpP);
    DynamicDummyTree *dummytree = treemanager->OpenDummyTree(GetIndexFile(tablename, attribname, metricname), LpP);
    // setting the weights
    SetWeights(slimtree->GetMetricEvaluator(), tablename, attribname, metricname);
    SetWeights(dummytree->GetMetricEvaluator(), tablename, attribname, metricname);

    // inserting a result and a query object
    DynamicResult *result;

    // knn select query or range select query or similarity grouping queries
    if (predicate.compare("near") != 0) return error;
    time_t tmp1 = time(NULL);
    if (sgtype.compare("") != 0) {
        if (knn.compare("") != 0) {
            if (sgtype.compare("sum") == 0) result = dummytree->SumNearestQuery(obj, numberofobjects, atoi(knn.c_str()), tielist);
            else if (sgtype.compare("all") == 0) result = dummytree->AllNearestQuery(obj, numberofobjects, atoi(knn.c_str()), tielist);
            else if (sgtype.compare("max") == 0) result = dummytree->MaxNearestQuery(obj, numberofobjects, atoi(knn.c_str()), tielist);
            else return error;
        } else {
            if (range.compare("") == 0) range = "0";
            double temp = boost::lexical_cast<double>(range.c_str());
            if (sgtype.compare("sum") == 0) result = dummytree->SumRangeQuery(obj, numberofobjects, temp);
            else if (sgtype.compare("all") == 0) result = dummytree->AllRangeQuery(obj, numberofobjects, temp);
            else if (sgtype.compare("max") == 0) result = dummytree->MaxRangeQuery(obj, numberofobjects, temp);
            else return error;
        }
    }// knn select query
    else if (knn.compare("") != 0) result = slimtree->NearestQuery(obj[0], boost::lexical_cast<int>(knn.c_str()), tielist);
        // range select query
    else if (range.compare("") != 0) {
        double temp = atof(range.c_str());
        result = slimtree->RangeQuery(obj[0], temp);
    }// if no query type informed, do a Range 0
    else result = slimtree->RangeQuery(obj[0], 0.0);
    time_t tmp2 = time(NULL);
    MetricTreeTotalTime += tmp2 - tmp1;

    // if the query returned no records
    if (result->GetNumOfEntries() == 0) {
        //errmsg += "Near/far returned no records."; // is this really an error???
        // storing prior and next pointers
        Token *priortoken = filequeryobject->Prior;
        Token *nexttoken = filequeryobject->Next;

        // deleting the query object
        delete filequeryobject;

        // inserting the "in" token
        Token *newtoken = AddToken(priortoken, "in", TK_IDENTIFIER, LK_UNDEFINED);
        // inserting the "("
        newtoken = AddToken(newtoken, "(", TK_OPEN_BRACE, LK_UNDEFINED);
        // inserting the objects OID in the token list
        newtoken = AddToken(newtoken, "0", TK_INTEGER, LK_UNDEFINED);
        // inserting the ")"
        newtoken = AddToken(newtoken, ")", TK_CLOSE_BRACE, LK_UNDEFINED);
        // pointing to the rest of the list
        newtoken->Next = nexttoken;
        *activelisttail = newtoken;
    }// else, include the query result in the token list
    else {
        // storing prior and next pointers
        Token *priortoken = filequeryobject->Prior;
        Token *nexttoken = filequeryobject->Next;

        // deleting the query object
        if (filequeryobject != NULL)
            delete filequeryobject;

        // inserting the "in" token
        Token *newtoken = AddToken(priortoken, "in", TK_IDENTIFIER, LK_UNDEFINED);

        // inserting the "("
        newtoken = AddToken(newtoken, "(", TK_OPEN_BRACE, LK_UNDEFINED);

        // inserting the objects OID in the token list
        for (unsigned int i = 0; i < result->GetNumOfEntries(); i++) {
            const tDynamicObject *tmp = result->GetPair(i)->GetObject();
            //tDynamicObject tmp = boost::lexical_cast<tDynamicObject>((result)[i]);
            string oid = boost::lexical_cast<string > (tmp->GetOID());
            newtoken = AddToken(newtoken, oid, TK_INTEGER, LK_UNDEFINED);
            if (i + 1 < result->GetNumOfEntries())
                newtoken = AddToken(newtoken, ",", TK_COMMA, LK_UNDEFINED);
        }
        // inserting the ")"
        newtoken = AddToken(newtoken, ")", TK_CLOSE_BRACE, LK_UNDEFINED);

        // pointing to the rest of the list
        newtoken->Next = nexttoken;

        *activelisttail = newtoken;
    }

    delete result;

    return "\0";
}

//---------------------------------------------------------------------------

string
DataDictionaryAccess::ProcessJoinQueries(Token **activelisttail, int activetokenlist, string predicate,
        string tablename, string tablename2, string attribname, string attribname2, string sgtype,
        string metricname, string knn, string range, bool tielist, string attribtype,
        vector<string> * parameterlist, vector<string> * parametervaluelist,
        vector<string> * parametertypelist)
{
    string errmsg = "";

    // if the user didn't specified the metric name, get the default
    if (metricname.compare("") == 0) {
        metricname = GetDefaultMetric(tablename, attribname);
        if (metricname.compare("") == 0) {
            errmsg += "There isn't a default metric defined for " + tablename + "." + attribname + ".";
        }
    }

    // if there are errors, stop
    if (errmsg.compare("") != 0)
        return errmsg;

    // getting the metric
    int LpP = GetDistanceFunction(tablename, attribname, metricname);
    // opening the index
    DynamicSlimTree *slimtree = treemanager->OpenSlimTree(GetIndexFile(tablename, attribname, metricname), LpP);
    DynamicDummyTree *dummytree = treemanager->OpenDummyTree(GetIndexFile(tablename, attribname, metricname), LpP);
    // setting the weights
    SetWeights(slimtree->GetMetricEvaluator(), tablename, attribname, metricname);
    SetWeights(dummytree->GetMetricEvaluator(), tablename, attribname, metricname);

    // inserting a result and a query object
    DynamicJoinedResult *result;

    // nearest join or closest join or range join
    if (knn.compare("") != 0) {
        // closest join
        if (sgtype.compare("any") == 0) {
            string idx = GetIndexFile(tablename2, attribname2, metricname);
            if (idx.compare("") == 0) {
                errmsg += "The attribute " + tablename2 + "." + attribname2 + " is not associated with the metric "
                        + metricname;
            } else {
                // opening the join tree
                MetricTreeManager *joinmanagerdummy = new MetricTreeManager(/*user*/);
                DynamicDummyTree *joindummytree = joinmanagerdummy->OpenDummyTree(idx, LpP);

                // setting the weights
                SetWeights(joindummytree->GetMetricEvaluator(), tablename2, attribname2, metricname);

                // computing join
                time_t tmp1 = time(NULL);
                if (predicate.compare("near") == 0)
                    result = dummytree->ClosestJoinQuery(joindummytree, atoi(knn.c_str()), tielist);
                else
                    errmsg += "The FAR predicate needs implementat(i)on!";
                time_t tmp2 = time(NULL);
                MetricTreeTotalTime += tmp2 - tmp1;

                // unloading the joined tree
                delete joinmanagerdummy;
            }
        }// nearest join
        else {
            string idx = GetIndexFile(tablename2, attribname2, metricname);
            if (idx.compare("") == 0) {
                errmsg += "The attribute ";
                errmsg += tablename2;
                errmsg += ".";
                errmsg += attribname2;
                errmsg += " is not associated with the metric ";
                errmsg += metricname;
            } else {
                // opening the join tree
                MetricTreeManager *joinmanagerdummy = new MetricTreeManager(/*user*/);
                DynamicDummyTree *joindummytree = joinmanagerdummy->OpenDummyTree(idx, LpP);

                // setting the weights
                SetWeights(joindummytree->GetMetricEvaluator(), tablename2, attribname2, metricname);

                // computing join
                time_t tmp1 = time(NULL);
                if (predicate.compare("near") == 0) {
                    //result = dummytree->NearestJoinQuery(joindummytree , atoi(knn.c_str()), tielist);
                } else {
                    errmsg += "The FAR predicate needs implementation!";
                }
                time_t tmp2 = time(NULL);
                MetricTreeTotalTime += tmp2 - tmp1;

                // unloading the joined tree
                delete joinmanagerdummy;
            }
        }
    }// range join
    else {
        if (range.compare("") == 0)
            range = "0.0";

        string idx = GetIndexFile(tablename2, attribname2, metricname);
        if (idx.compare("") == 0) {
            errmsg += "The attribute " + tablename2 + "." + attribname2 + " is not associated with the metric "
                    + metricname;
        } else {
            // opening the join tree
            MetricTreeManager *joinmanagerdummy = new MetricTreeManager(/*user*/);
            DynamicDummyTree *joindummytree = joinmanagerdummy->OpenDummyTree(idx, LpP);

            // setting the weights
            SetWeights(joindummytree->GetMetricEvaluator(), tablename2, attribname2, metricname);

            // computing join
            time_t tmp1 = time(NULL);
            if (predicate.compare("near") == 0) {
                double temp = atof(range.c_str());
                result = dummytree->RangeJoinQuery(joindummytree, temp);
            } else
                errmsg += "The FAR predicate needs implementat(i)on!";
            time_t tmp2 = time(NULL);
            MetricTreeTotalTime += tmp2 - tmp1;

            // unloading the joined tree
            delete joinmanagerdummy;
        }
    }

    if (errmsg.compare("") != 0)
        return errmsg;

    // inserting the tmpjoin table before the token "where"
    Token *newtoken = *activelisttail;
    while (newtoken->Lexem.compare("where") != 0) {
        newtoken = newtoken->Prior;
    }

    stringstream out;
    out << tempjointablescope;
    string tmptable = "TMP$tmpjoin" + out.str();
    tempjointablescope++;

    newtoken = newtoken->Prior;
    newtoken = AddToken(newtoken, ",", TK_COMMA, LK_UNDEFINED);
    newtoken = AddToken(newtoken, tmptable, TK_IDENTIFIER, LK_UNDEFINED);

    // insert the join conditions in the where clause
    newtoken = *activelisttail;
    newtoken = AddToken(newtoken, "=", TK_EQUAL, LK_UNDEFINED);
    newtoken = AddToken(newtoken, tmptable, TK_IDENTIFIER, LK_UNDEFINED);
    newtoken = AddToken(newtoken, ".", TK_PERIOD, LK_UNDEFINED);
    newtoken = AddToken(newtoken, "col1", TK_IDENTIFIER, LK_UNDEFINED);
    newtoken = AddToken(newtoken, "and", TK_IDENTIFIER, LK_UNDEFINED);
    newtoken = AddToken(newtoken, tablename2, TK_IDENTIFIER, LK_UNDEFINED);
    newtoken = AddToken(newtoken, ".", TK_PERIOD, LK_UNDEFINED);
    newtoken = AddToken(newtoken, attribname2, TK_IDENTIFIER, LK_UNDEFINED);
    newtoken = AddToken(newtoken, "=", TK_EQUAL, LK_UNDEFINED);
    newtoken = AddToken(newtoken, tmptable, TK_IDENTIFIER, LK_UNDEFINED);
    newtoken = AddToken(newtoken, ".", TK_PERIOD, LK_UNDEFINED);
    newtoken = AddToken(newtoken, "col2", TK_IDENTIFIER, LK_UNDEFINED);

    // updat(i)ng the new tail
    *activelisttail = newtoken;

    // deleting all records on TMP$tmpjoinX
    string sql = "delete from " + tmptable;
    RunSQL(sql);

    // insert the join result into table TMP$tmpjoin
    time_t tmp1 = time(NULL);
    sql = "insert into " + tmptable + " values(:value1, :value2)";
    //  query->Params->Items[0]->DataType = ftInteger;
    //    query->Params->Items[1]->DataType = ftInteger;
    for (unsigned int i = 0; i < result->GetNumOfEntries(); i++) {
        //        tDynamicObject *tmp1 = (tDynamicObject *)(*result)[i].GetObject();
        //        tDynamicObject *tmp2 = (tDynamicObject *)(*result)[i].GetJoinedObject();
        //        query->Params->Items[0]->Value = tmp1->GetOID();
        //        query->Params->Items[1]->Value = tmp2->GetOID();
        query->Run(sql);
    }
    time_t tmp2 = time(NULL);
    TempJoinInsertsTotalTime += tmp2 - tmp1;

    // clean up memory
    //    delete result;

    return errmsg;
}

tDynamicObject *
DataDictionaryAccess::BuildParticulateObject(int partid, vector<string> *UserParamList,
        vector<string> *UserParamValueList, vector<string> *MetricParamList,
        vector<string> *MetricParamTypes)
{

    tDynamicParticulate *po = new tDynamicParticulate();
    po->SetSize(MetricParamList->size());

    for (unsigned int i = 0; i < MetricParamList->size(); i++) {
        // the parameter being analysed
        string parametertype = MetricParamTypes->at(i);

        // Search the position of the particle in the insert
        vector<string>::iterator it;
        it = find(UserParamList->begin(), UserParamList->end(), MetricParamList->at(i));
        if (it == UserParamList->end()) {
            // if a particle was not found, the code should not have reached here, well, let's treat it anyway!
            root->error("Particle not found.");
            return NULL;
        }

        // adding the value of the parameter being passed in its respective position on the metric
        if (parametertype.compare("number") == 0 ||
                parametertype.compare("float") == 0 ||
                parametertype.compare("numeric") == 0 ||
                parametertype.compare("real") == 0) {
            
            int index = distance(it,UserParamList->begin());
            double value = lexical_cast<double>(UserParamValueList->at(index));
            po->SetParticle(i, value);
        }
        // else if // treat other particle types here, and good luck!   :-)
    }

    tDynamicObject *obj = new tDynamicObject();
    obj->SetParticulate(po);
    obj->SetOID(partid);
    delete po;
    return obj;
}

string
DataDictionaryAccess::CheckInsertParticles(string tablename, vector<string> *InsertColList)
{
    string result = "";

    // select the attributes needed in any metric associated to any particulate attribute in the table
    string sql;
    sql = "select distinct parameterinstance ";
    sql += "from CDD$metricstruct ms, CDD$metricinstance mi ";
    sql += "where ms.metriccode = mi.metriccode ";
    sql += "and mi.tablename = '" + tablename + "' ";

    query->Open(sql);
    
    string parameterinstance;
    while (!query->Eof()) {
        parameterinstance = query->StringFieldByIndex(0);
        if (atoi(InsertColList->at(atoi(parameterinstance.c_str())).c_str()) == -1)
            result = parameterinstance;

    }

    return result;
}

//---------------------------------------------------------------------------

void
DataDictionaryAccess::InsertParticulate(string tablename, int partid, vector<string> *InsertColList,
        vector<string> *InsertValueList, vector<string> *commandlist)
{

    // for each particulate attribute in the table tablename
    string sql;
    sql = "select distinct im.complexattribname ";
    sql += "from CDD$ComplexAttribMetric im, CDD$metricstruct ms ";
    sql += "where im.metriccode = ms.metriccode ";
    sql += "and im.tablename = '" + tablename + "' ";
    vector<string> *ComplexAttribNames = new vector<string > ();

    query->Open(sql);
    while (!query->Eof()) {
        ComplexAttribNames->push_back(query->StringFieldByIndex(0));
    }

    for (unsigned int i = 0; i < ComplexAttribNames->size(); i++)
        IndexParticulateAttribute(tablename, partid, ComplexAttribNames->at(i), InsertColList, InsertValueList,
            commandlist);

    delete ComplexAttribNames;
}

//---------------------------------------------------------------------------

vector<string> *
DataDictionaryAccess::ConvertAttributesToMetricParams(string tablename,
        string metricname, string particulatename, vector<string> *attributelist)
{
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

    query->Open(sql);
    
    string found;
    for (unsigned int i = 0; i < attributelist->size(); i++) {
        found = "";
        query->First();
        while (!query->Eof()) {
            if (query->StringFieldByIndex(1).compare(attributelist->at(i)) == 0)
                found = query->StringFieldByIndex(0);

        }
        list->push_back(found);
    }
    return list;
}


//---------------------------------------------------------------------------

void
DataDictionaryAccess::IndexParticulateAttribute(string tablename, int partid, string particulatename,
        vector<string> *InsertColList, vector<string> *InsertValueList,
        vector<string> *commandlist)
{

    string sql;
    sql = "select distinct ms.metricname, im.complexattribname, im.indexfile ";
    sql += "from CDD$ComplexAttribMetric im, CDD$metricstruct ms ";
    sql += "where im.metriccode = ms.metriccode ";
    sql += "and im.tablename = '" + tablename + "' ";
    sql += "and im.complexattribname = '" + particulatename + "' ";

    vector<string> *MetricName = new vector<string > ();
    vector<string> *IndexFile = new vector<string > ();
    vector<string> *UserParamList = new vector<string > ();

    query->Open(sql);
    
    while (!query->Eof()) {
        MetricName->push_back(query->StringFieldByIndex(0));
        IndexFile->push_back(query->StringFieldByIndex(2));

    }
    
    

    // for each metric associated with this table attribute
    for (unsigned int i = 0; i < MetricName->size(); i++) {

        // getting the metric params names, instances and types
        vector<vector<string> * > *particulateData = sirenQueries->getParticulateData(tablename,MetricName->at(i), particulatename);
        
        vector<string> *MetricParamNames = particulateData->at(0);
        vector<string> *MetricParamTypes = particulateData->at(2);
        UserParamList = ConvertAttributesToMetricParams(tablename, MetricName->at(i), particulatename, InsertColList);

        // getting the metric
        int LpP = GetDistanceFunction(tablename, particulatename, MetricName->at(i));
        // opening index
        DynamicSlimTree *slimtree = treemanager->OpenSlimTree(IndexFile->at(i), LpP);
        DynamicDummyTree *dummytree = treemanager->OpenDummyTree(IndexFile->at(i), LpP);

        // setting the weights
        SetWeights(slimtree->GetMetricEvaluator(), tablename, particulatename, MetricName->at(i));
        SetWeights(dummytree->GetMetricEvaluator(), tablename, particulatename, MetricName->at(i));

        // inserting in the metric tree
        tDynamicObject *obj = BuildParticulateObject(partid, UserParamList, InsertValueList, MetricParamNames,
                MetricParamTypes);

        slimtree->Add(obj);
        dummytree->Add(obj);
        //commandlist->push_back(
        //"Insert object " + out.str() + " using " + MetricName->at(i) + " to slim tree " + IndexFile->at(i));
        particulateData->clear();
        delete particulateData;
    }

    delete MetricName;
    delete IndexFile;
    
    
    delete UserParamList;
}

//---------------------------------------------------------------------------

//VALIDAR O QUE FOI FEITO PARA O TRATAMENTO DE ERRO QUANDO NAO EH POSSIVEL ABRIR UM ARQUIVO NO LO_IMPORT
//FAZER TAMBEM PARA InsertAudioAPV  <---------
//Colocar toda a funcao abaixo no relatorio e tambem as linhas:
//if (FileExists(f_name))
//  errors = datadictionary->InsertImageIPV(tab_name, attrib_name, f_name, id, CommandList);
//da funcao void Parser::insert_statement() { no arquivo parser.cpp
// There isn't differences on what kind of file we're dealing with.
// Even with a sound file, the only thing that will change it's the header of the table, since database storage methods will be same.
int
DataDictionaryAccess::InsertImageIPV(string tablename, string stillimagename, string imagefile,
        long imageid, vector<string> *commandlist)
{

    string sql;

    // This query return the metrics and the index file for the table where the image
    // will be inserted.
    sql = "select distinct ms.metricname, im.indexfile ";
    sql += "from CDD$ComplexAttribMetric im, CDD$metricstruct ms ";
    sql += "where im.metriccode = ms.metriccode ";
    sql += "and im.tablename = '" + tablename + "' ";
    sql += "and im.complexattribname = '" + stillimagename + "' ";

    query->Open(sql);
    

    // @TODO There is no sense in instantiate these vectors if there isn't any metrics.
    // To check this, create a function with recordCount and return the function.
    
    
    // Vector where the signatures of the extractor will stay before the insert statement.
    vector<char *> *tDynamicCharVector = new vector<char *>();
    
    // Vector to store the field names to use in update.
    vector<string> *fieldsVector = new vector<string>();
    
    // The first fields are image id and image.
    fieldsVector->push_back(string("IMAGE_ID"));
    fieldsVector->push_back(string("IMAGE"));
    
    
    // For each image, for each metric, 
    // After extract all the metrics, mount the insert into statement, within the image.
    // 3 - Insert the image into the database.
    // 4 - Insert the signatures from the extractors into the database.
    // That's it. I DO NOT FUCKING NEED TO UPDATE in another loop! Motherfucker performance!        
    
    while (!query->Eof()) {
        string metricName = query->StringFieldByIndex(0);
        string indexFile = query->StringFieldByIndex(1);
        string extractor;
        
        
        // Get the distance function
        root->info("Get the distance function.");
        // @TODO Hermes lib will be inserted here.
        const int LpP = GetDistanceFunction(tablename, stillimagename, metricName);
        
        // opening index
        DynamicSlimTree *slimtree = treemanager->OpenSlimTree(indexFile, LpP);
        DynamicDummyTree *dummytree = treemanager->OpenDummyTree(indexFile, LpP);
        // setting the weights
        // @TODO Modify the get extractor query to return weights. Soon, this will
        // be done in buildObjectFromImage() method.
        SetWeights(slimtree->GetMetricEvaluator(), tablename, stillimagename, metricName);
        SetWeights(dummytree->GetMetricEvaluator(), tablename, stillimagename, metricName);

        // inserting in the metric tree
        root->info("Starting extracting object.");
        tDynamicObject *obj = buildObjectFromImage(tablename, stillimagename, metricName, imagefile, imageid, extractor);
        slimtree->Add(obj);
        root->info("tDynamicObject added to the slim tree.");
        dummytree->Add(obj);
        root->info("tDynamicObject added to the dummy tree.");
        
        // Extract tDynamicType from tDynamicObject as a char vector.
        tDynamicCharVector->push_back(obj->GetExtraction(extractor)->convertDataToChar());
        
        fieldsVector->push_back(extractor);
        
        
    }
    
    // Here. Insert the image in the database here.
    // For oracle, I need to mount the insert statement, execute it, and populate the blobs.
    // Mounts the insert and populates the blobs.    
    query->mountInsert(tablename,*fieldsVector,*tDynamicCharVector);
            
    // Deallocate vectors.
    // @TODO Do clear() method deallocate the objects?
    tDynamicCharVector->clear();
    delete tDynamicCharVector;
    fieldsVector->clear();
    delete fieldsVector;
            
    return 0;
}

//---------------------------------------------------------------------------
tDynamicObject *
DataDictionaryAccess::buildObjectFromImage(string& tablename, string& complexattribname,
        string& metricname, string& filename, long& oid, string& extractorName)
{
    Image* img = imageUtils::createImageObjectByFilename(filename);    
    root->info("Create the Artemis object based on "+filename);
    
    
    // Get the extractor names and parameters
    // @TODO How about getting/setting the weights here?
    root->info("Get the extractor(s) on database");
    Query *query = sirenQueries->getExtractor(tablename, complexattribname, metricname);
    
    // Instantiate the new object
    tDynamicObject *obj = new tDynamicObject();
    obj->SetOID(oid);

    while (!query->Eof()) {

        string extractor = query->StringFieldByIndex(0);
        string parameter = query->StringFieldByIndex(1);
        extractorName = extractor;

        // extract the characteristics
        root->info("Extract the parameter "+parameter+" with the extractor: "+extractor);
        obj->Extract(extractor, parameter, img);        

    }

    delete img;
    // return the object
    return obj;
    
}

void
DataDictionaryAccess::SetWeights(tDynamicDistanceEvaluator *evaluator, string tablename,
        string attribname, string metricname)
{

    // get the metric type
    string sql = "select metrictype from cdd$metricstruct where metricname = '";
    sql += metricname;
    sql += "'";

    query->Open(sql);

    // check if it is stillimage or particulate
    if (!query->Eof()) {
        string metrictype = query->StringFieldByIndex(0);
        if (metrictype.compare("stillimage") == 0) {
            // get the extractor names and parameters
            sql = "";
            sql += "select distinct ex.extractorname, pa.parametername, ps.weight ";
            sql += "  from CDD$ComplexAttribMetric im, CDD$metricstruct ms, CDD$parameterstruct ps, ";
            sql += "       epd$extractors ex, epd$parameters pa ";
            sql += " where im.metriccode = ms.metriccode ";
            sql += "   and ms.metriccode = ps.metriccode ";
            sql += "   and ps.extractorcode = ex.extractorcode ";
            sql += "   and ex.extractorcode = pa.extractorcode and pa.isindexable = 'Y' ";
            sql += "   and im.tablename = '" + tablename + "' ";
            sql += "   and im.complexattribname = '" + attribname + "' ";
            sql += "   and ms.metricname = '" + metricname + "' ";

            qry->Open(sql);

            while (!qry->Eof()) {

                string extractor = qry->StringFieldByIndex(0);
                string parameter = qry->StringFieldByIndex(1);
                double weight = qry->DoubleFieldByIndex(2);

                // Sets the weights
                evaluator->SetWeight(extractor, parameter, weight);

            }
        }// particulate
        else if (query->StringFieldByIndex(0).compare("particulate") == 0) {
            // get the extractor names and parameters
            sql = "";
            sql += "select distinct ps.parameterorder, ps.parametername, ps.weight ";
            sql += "from cdd$parameterstruct ps, cdd$metricstruct ms, cdd$complexattribmetric ca ";
            sql += "where ps.metriccode = ms.metriccode ";
            sql += "  and ca.metriccode = ms.metriccode ";
            sql += "  and ca.tablename = '" + tablename + "' ";
            sql += "  and ca.complexattribname = '" + attribname + "' ";
            sql += "  and ms.metricname = '" + metricname + "' ";
            sql += "order by ps.parameterorder ";

            qry->Open(sql);

            int position = 0;
            while (!qry->Eof()) {

                double weight = qry->FloatFieldByIndex(2);

                // Sets the weights
                evaluator->SetParticulateWeight(position, weight);

                // move to the next record
                position++;
            }
        }
    }
}

string
DataDictionaryAccess::GetClientFolder()
{

    string sreturn = "";
    string sql;
    sql = "select Folder from TMP$Folder";
    query->Open(sql);
    if (!query->Eof())
        sreturn = query->StringFieldByIndex(0);
    return sreturn;
}

SirenQueries * DataDictionaryAccess::getSirenQueries() {
    return this->sirenQueries;
}
