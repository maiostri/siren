/**
 * @file
 *
 * This file defines the queries operations for Oracle.
 *
 * @version 1.0
 * @author Ricardo Pereira Maiostri (maiostri@gmail.com)
 * @date 11-03-2011
 */

#include "QueryOracle.h"

QueryOracle::QueryOracle(Connection* conn)
{
    this->conn = conn;
    stmt = this->conn->createStatement();

    // log4cpp
    string initFileName = "log4cpp.properties";
    PropertyConfigurator::configure(initFileName);
    root = &Category::getRoot();

    // map
    mapFieldsIndex = new map<string, int>();
}

QueryOracle::~QueryOracle()
{
    mapFieldsIndex->clear();
    delete mapFieldsIndex;
    delete root;
    delete stmt;
}

void QueryOracle::Open(const string& sql)
{
    try {
        stmt->setSQL(sql);
        rs = stmt->executeQuery();
        conn->commit();
        //mountMap();
    } catch (SQLException &oraex) //Oracle/OCCI errors
    {
        string errmsg = oraex.getMessage();
        root->error("SQL ERROR: " + sql);
        root->error("Message error: " + errmsg);
    } catch (std::exception &ex) //any other C++/STL error
    {
        root->error("SQL ERROR:" + sql);
        root->error("Error: " + string(ex.what()));
    }
}

void QueryOracle::mountMap()
{
    this->mapFieldsIndex->clear();
    vector<MetaData> metaDataVector = rs->getColumnListMetaData();
    for (unsigned int i = 1; i < metaDataVector.size(); i++) {
        // @TODO Does the index starts with 1 in the MetaData? I think i will only
        // know this in when debugging...
        
        // Get the field`s name.
        string field = metaDataVector.at(i+1).getString(MetaData::ATTR_NAME);        
        // Setting the map.
        mapFieldsIndex->at(field) = i + 1;
    }
}

Blob QueryOracle::blobFieldByName(const string& field)
{
    map<string, int>::iterator it;
    // Finding the element in the map
    it = mapFieldsIndex->find(field);
    // Calculate the index.
    int index = distance(mapFieldsIndex->begin(), it);
    return rs->getBlob(index);
}

char * QueryOracle::blobFieldDumpToCharByName(const string& field)
{
    Blob blob = blobFieldByName(field);
    return dumpBlobToChar(blob);
}

void QueryOracle::Run(const string& sql)
{
    try {
        stmt->setSQL(sql);
        stmt->executeUpdate();
        conn->commit();
    } catch (SQLException &oraex) //Oracle/OCCI errors
    {
        string errmsg = oraex.getMessage();
        root->error("SQL ERROR: " + sql);
        root->error("Message error: " + errmsg);
        //more application error handling
    } catch (std::exception &ex) //any other C++/STL error
    {
        root->error("SQL ERROR:" + sql);
        root->error("Error: " + string(ex.what()));
    }
}

void QueryOracle::First()
{

}

int QueryOracle::Next()
{
    return rs->next();
}

int QueryOracle::Eof()
{
    if (rs->next() == ResultSet::END_OF_FETCH)
        return 1;
    else
        return 0;
}


// Columns are numbered beggining at 1

string QueryOracle::StringFieldByIndex(const int& index)
{
    return rs->getString(index + 1);
}

int QueryOracle::IntFieldByIndex(const int& index)
{
    return rs->getInt(index + 1);
}

float QueryOracle::FloatFieldByIndex(const int& index)
{
    return rs->getFloat(index + 1);
}

double QueryOracle::DoubleFieldByIndex(const int& index)
{
    return rs->getDouble(index + 1);
}

void QueryOracle::RunPLSQL(const string& sql)
{
    try {
        stmt->setSQL(sql);
        rs = stmt->executeQuery();
        conn->commit();
    } catch (SQLException &oraex) //Oracle/OCCI errors
    {

        string errmsg = oraex.getMessage();
        root->error("SQL ERROR: " + sql);
        root->error("Message error: " + errmsg);
    } catch (std::exception &ex) //any other C++/STL error
    {
        root->error("SQL ERROR: " + sql);
        root->error("Message error: " + string(ex.what()));

    }
}

int QueryOracle::FieldsCount()
{
    return 0;
}

string QueryOracle::FieldNameByIndex(const int& index)
{
    vector<MetaData> MetaData = rs->getColumnListMetaData();
    return MetaData[index + 1].getString(MetaData::ATTR_NAME);
}

int QueryOracle::FieldIsNullByIndex(const int& index)
{
    return rs->isNull(index + 1);
}

int QueryOracle::countRows(const string& table)
{
    stmt->setSQL(string("SELECT COUNT(*) AS AMOUNT FROM (" + table + ")"));
    rs = stmt->executeQuery();
    return rs->getInt(1);
}

int QueryOracle::countRows()
{
    return 0;
}

void QueryOracle::dropTable(const string& table)
{
    stmt->setSQL(string("DROP TABLE " + table));
    stmt->executeUpdate();
    conn->commit();
    root->info("Table dropped: "+table);
}

void QueryOracle::createSequence(const string& sequenceName, const int& startWith)
{
    string sql = "CREATE SEQUENCE " + sequenceName + boost::lexical_cast<string > (startWith);
    stmt->setSQL(sql);
    stmt->executeUpdate();
    conn->commit();
    root->info("Sequence created: " + sequenceName);
}

void QueryOracle::dropSequence(const string& sequence)
{
    stmt->setSQL(string("DROP SEQUENCE " + sequence));
    stmt->executeUpdate();
    conn->commit();
    root->info("Sequence dropped: " + sequence);
}


// TODO Once the read/write of the blobs with the buffer methods work, this function should be removed. Do not forget!

void QueryOracle::createOrReplaceDirectory(const string& directoryName, const string& location)
{
    stmt->setSQL(string("CREATE OR REPLACE DIRECTORY " + location + " AS '" + location + "'"));
    stmt->executeUpdate();
    conn->commit();
}

void QueryOracle::dumpBlob(Blob& blob, const string& file)
{
    // Dump blob to a char array.
    char * buffer = dumpBlobToChar(blob);

    // Creating the output stream
    ofstream outfile(file.c_str());


    outfile << buffer;
    outfile.close();

    // Deallocate the char *buffer, close the stream and close the blob.
    delete (buffer);
    blob.close();
}

char * QueryOracle::dumpBlobToChar(Blob& blob)
{
    // Opening the blob in the read mode.
    blob.open(OCCI_LOB_READONLY);

    // Read the blob stream to the char buffer
    Stream *instream = blob.getStream(1, 0);
    char *buffer = new char[blob.length()];

    instream->readBuffer(buffer, blob.length());

    return buffer;
}

// Populate a blob based on a pointer to char. Used to populate the blob with the signature of the extractors.

void QueryOracle::populateBlob(Blob& blob, char* data)
{
    // Open the blob in the read write mode
    root->info("Opening the blob in the read/write mode.");
    blob.open(OCCI_LOB_READWRITE);

    // Open the blob stream.
    Stream *outstream = blob.getStream(1, 0);
    outstream->writeBuffer(data, sizeof (data));
    root->info("Write the buffer in the blob stream.");
    // @TODO Search if this is really necessary. Last Char stuff.
    char *c = (char *) "";
    outstream->writeLastBuffer(c, 0);
    blob.closeStream(outstream);
    blob.close();
}

// Populate a blob based on a file. Used to populate the blob with the image.

void QueryOracle::populateBlob(Blob& blob, const string& file)
{
    // Buffer to populate the blob
    char *buffer;
    // Open the file and read the stream
    ifstream inputfile(file.c_str());

    // Get length of file.
    inputfile.seekg(0, ios::end);
    int length = inputfile.tellg();
    inputfile.seekg(0, ios::beg);

    // Allocating memory in the buffer and reading into it.
    buffer = new char[length];
    inputfile.read(buffer, length);

    // Open the blob's stream and write the data into it.
    Stream *outstream = blob.getStream(1, 0);
    outstream->writeBuffer(buffer, length);

    // @TODO Check.
    char *c = (char *) "";
    outstream->writeLastBuffer(c, 0);

    // Close the file, delete the char buffer, and close the blob.
    // @TODO Do I need to close the stream buffer?
    inputfile.close();
    delete[] buffer;
    blob.close();

}

int QueryOracle::runSequence(const string& sequenceName)
{
    string sql = "select " + sequenceName + ".nextval as seqvalue from dual";
    Open(sql);
    int id = IntFieldByIndex(0);
    return id;
}


// This function mounts the insert statement, runs it and populate the blobs.
// THIS FUNCTION IS IN THE WRONG PLACE!
// Since manipulate SQL directly, should stay in a intermediate class between the QueryOracle and the DataDicionaryAccess.

void QueryOracle::mountInsert(const string& tableName, const vector<string>& fieldNames, vector<char *>&
        tDynamicObjectCharVector)
{
    root->warn("Starting mounting the insert statement.");
    unsigned int i;

    // @TODO Things to be re-written in this function
    // 1 - Check the index. I will have problems with them.
    string sql = "INSERT INTO " + tableName + "( ";

    // Declaring the fields for the table.
    for (i = 0; i <= fieldNames.size(); i++) {
        sql += fieldNames.at(i) + ", ";
    }
    sql += fieldNames.at(i + 1) + ") ";

    sql += " VALUES (";


    // Get the value of the sequence.
    int sequence = runSequence("IMAGEIDSEQUENCE");
    sql += sequence + ", ";

    // Declaring the value of the fields.
    for (i = 1; i < fieldNames.size(); i++) {
        sql += "EMPTY_BLOB(), ";
    }
    sql += "EMPTY_BLOB()) RETURNING ";

    // Declaring the fields to be returned.
    for (i = 1; i < fieldNames.size(); i++) {
        sql += fieldNames.at(i) + ", ";
    }
    sql += fieldNames.at(i + 1) + " INTO ";

    // Creating the output parameters
    for (i = 0; i < fieldNames.size(); i++) {
        sql += ":v" + lexical_cast<string > (i) + ", ";
    }
    sql += ":v" + lexical_cast<string > (i + 1) + ")";

    // Register output parameters!
    for (i = 0; i < fieldNames.size(); i++) {
        stmt->registerOutParam(i, OCCIBLOB);
    }
    root->info("Insert statement mounted.");
    root->info("SQL: " + sql);

    // The insert statement was created! Execute!
    stmt->executeUpdate(sql);


    // Populate the blobs.
    // @TODO Check index.
    // For the first field, things are different. 
    int outparamOffsetBegin = fieldNames.size() + 1;
    root->info("First output parameter offset calculated: " + outparamOffsetBegin);
    int outparamOffsetEnd = outparamOffsetBegin + fieldNames.size();
    root->info("Last output parameter offset calculated: " + outparamOffsetEnd);
    for (int j = outparamOffsetBegin; j < outparamOffsetEnd; j++) {
        Blob blob = stmt->getBlob(j);
        populateBlob(blob, tDynamicObjectCharVector.at(j));
        root->info("Populate blob number " + j);
    }

    root->info("The blobs are populated.");

}









