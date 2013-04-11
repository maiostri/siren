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

QueryOracle::QueryOracle(Connection* conn) {
	this->conn = conn;
	stmt = this->conn->createStatement();
	// map
	mapFieldsIndex = new map<string, int>();
}

QueryOracle::~QueryOracle() {
	if (mapFieldsIndex != NULL) {
		mapFieldsIndex->clear();
	}
	delete stmt;
}

void QueryOracle::Open(const string& sql) {
	try {
		stmt->setSQL(sql);
		rs = stmt->executeQuery();
		logUtils::getInstance().logSQL("SQL Statement executed: " + sql);
		conn->commit();
		mountMap();
	} catch (SQLException &oraex) //Oracle/OCCI errors
	{
		string errmsg = oraex.getMessage();
		logUtils::getInstance().logError("SQL ERROR: " + sql);
		logUtils::getInstance().logError("Message error: " + errmsg);
	}
}

void QueryOracle::mountMap() {

	this->mapFieldsIndex->clear();
	vector<MetaData> metaDataVector = rs->getColumnListMetaData();
	int it = 0;

	for_each(metaDataVector.begin(), metaDataVector.end(), [this, &it] (MetaData metaData)
	{
		string field = metaData.getString(MetaData::ATTR_NAME);
		++it;
		if (field.compare("") != 0) {
			mapFieldsIndex->insert(pair<string,int>(field,it));
		}
	});
}

Blob QueryOracle::blobFieldByName(const string& field) {

	map<string, int>::iterator it;
	// Finding the element in the map

	it = mapFieldsIndex->find(field);
	// Calculate the index.
	int index = distance(mapFieldsIndex->begin(), it);
	return rs->getBlob(index);
}

void QueryOracle::blobFieldDumpToFileByIndex(const int& index, const string& filename) {
	Blob blob = rs->getBlob(index + 1);
	dumpBlob(blob, filename);
}

char * QueryOracle::blobFieldDumpToCharByName(const string& field) {
	int size = 0;
	Blob blob = blobFieldByName(field);
	return dumpBlobToChar(blob, size);
}

void QueryOracle::Run(const string& sql) {
	try {
		stmt->setSQL(sql);
		stmt->executeUpdate();
		logUtils::getInstance().logSQL("SQL Statement executed: " + sql);
		conn->commit();
	} catch (SQLException &oraex) //Oracle/OCCI errors
	{
		string errmsg = oraex.getMessage();
		logUtils::getInstance().logError("SQL ERROR: " + sql);
		logUtils::getInstance().logError("Message error: " + errmsg);
	} catch (std::exception &ex) //any other C++/STL error
	{
		logUtils::getInstance().logError("Error: " + string(ex.what()));
	}
}

int QueryOracle::Next() {
	return rs->next();
}

int QueryOracle::Eof() {
	if (rs->next() == ResultSet::END_OF_FETCH)
		return 1;
	else
		return 0;
}

// Columns are numbered beggining at 1

string QueryOracle::StringFieldByIndex(const int& index) {
	return rs->getString(index + 1);
}

int QueryOracle::IntFieldByIndex(const int& index) {
	return rs->getInt(index + 1);
}

float QueryOracle::FloatFieldByIndex(const int& index) {
	return rs->getFloat(index + 1);
}

double QueryOracle::DoubleFieldByIndex(const int& index) {
	return rs->getDouble(index + 1);
}

void QueryOracle::RunPLSQL(const string& sql) {
	try {
		stmt->setSQL(sql);
		rs = stmt->executeQuery();
		logUtils::getInstance().logSQL("SQL Statement executed: " + sql);
		conn->commit();
	} catch (SQLException &oraex) //Oracle/OCCI errors
	{
		string errmsg = oraex.getMessage();
		logUtils::getInstance().logError("SQL ERROR: " + sql);
		logUtils::getInstance().logError("Message error: " + errmsg);
	} catch (std::exception &ex) //any other C++/STL error
	{
		logUtils::getInstance().logError("Error: " + string(ex.what()));
	}
}

int QueryOracle::FieldsCount() {
	vector<MetaData> MetaData = rs->getColumnListMetaData();
	return MetaData.size();
}

string QueryOracle::FieldNameByIndex(const int& index) {
	vector<MetaData> MetaData = rs->getColumnListMetaData();
	return MetaData[index].getString(MetaData::ATTR_NAME);
}

int QueryOracle::FieldIsNullByIndex(const int& index) {
	return rs->isNull(index + 1);
}

int QueryOracle::countRows(const string& table) {
	stmt->setSQL(string("SELECT COUNT(*) AS AMOUNT FROM (" + table + ")"));
	rs = stmt->executeQuery();
	return rs->getInt(1);
}

int QueryOracle::countRows() {
	return 0;
}

void QueryOracle::dropTable(const string& table) {
	stmt->setSQL(string("DROP TABLE " + table));
	stmt->executeUpdate();
	conn->commit();
	logUtils::getInstance().logInfo("Table dropped: " + table);
}

void QueryOracle::createSequence(const string& sequenceName, const int& startWith) {
	string sql = "CREATE SEQUENCE " + sequenceName + boost::lexical_cast<string>(startWith);
	stmt->setSQL(sql);
	stmt->executeUpdate();
	logUtils::getInstance().logSQL("SQL Statement executed: " + sql);
	conn->commit();
	logUtils::getInstance().logInfo("Sequence created: " + sequenceName);
}

void QueryOracle::dropSequence(const string& sequence) {
	stmt->setSQL(string("DROP SEQUENCE " + sequence));
	stmt->executeUpdate();
	conn->commit();
	logUtils::getInstance().logInfo("Sequence dropped: " + sequence);
}

void QueryOracle::dumpBlob(Blob& blob, const string& file) {
	// Dump blob to a char array.
	int size = 0;
	char * buffer = dumpBlobToChar(blob, size);

	// Creating the output stream
	ofstream outfile(file.c_str());

	for (int i = 0; i < size; i++) {
		outfile << (char) buffer[i];
	}
	outfile.close();

	// Deallocate the char *buffer, close the stream and close the blob.
	delete (buffer);
	blob.close();
}

char * QueryOracle::dumpBlobToChar(Blob& blob, int& size) {
	char *buffer = nullptr;
	try {

		// Opening the blob in the read mode.
		blob.open(OCCI_LOB_READONLY);

		// Read the blob stream to the char buffer
		Stream *instream = blob.getStream(1, 0);
		buffer = new char[blob.length()];
		size = blob.length();

		instream->readBuffer(buffer, blob.length());
	} catch (std::exception &ex) {
		// There were problems in the previous write of the blob in the database. Throw an exception.
		throw ErrorReadingBlobException(ex.what());
	}
	return buffer;
}

// Populate a blob based on a pointer to char. Used to populate the blob with the signature of the extractors.

void QueryOracle::populateBlob(Blob& blob, char* data) {
	// Open the blob in the read write mode
	logUtils::getInstance().logInfo("Opening the blob in the read/write mode.");
	blob.open(OCCI_LOB_READWRITE);

	// Open the blob stream.
	Stream *outstream = blob.getStream(1, 0);
	outstream->writeBuffer(data, sizeof(data));
	logUtils::getInstance().logInfo("Write the buffer in the blob stream.");
	char *c = (char *) "";
	outstream->writeLastBuffer(c, 0);
	blob.closeStream(outstream);
	blob.close();
}

// Populate a blob based on a file. Used to populate the blob with the image.

void QueryOracle::populateBlob(Blob& blob, const string& file) {
	// Buffer to populate the blob
	char *buffer;
	// Open the file and read the stream
	logUtils::getInstance().logInfo("Open the file to read the stream");
	ifstream inputfile(file.c_str());

	// Get length of file.
	inputfile.seekg(0, ios::end);
	int length = inputfile.tellg();
	inputfile.seekg(0, ios::beg);
	logUtils::getInstance().logInfo("Get the length of the file: " + lexical_cast<string>(length));

	// Allocating memory in the buffer and reading into it.
	buffer = new char[length];
	inputfile.read(buffer, length);

	// Open the blob's stream and write the data into it.
	logUtils::getInstance().logInfo("Open the blob to write into it.");
	blob.open(OCCI_LOB_READWRITE);
	Stream *outstream = blob.getStream(1, 0);
	outstream->writeBuffer(buffer, length);

	// @TODO Check.
	char *c = (char *) "";
	outstream->writeLastBuffer(c, 0);

	// Close the file, delete the char buffer, and close the blob.
	// @TODO Do I need to close the stream buffer?
	inputfile.close();
	delete[] buffer;
	blob.closeStream(outstream);
	blob.close();
}

int QueryOracle::runSequence(const string& sequenceName) {
	string sql = "select " + sequenceName + ".nextval as seqvalue from dual";
	Open(sql);
	logUtils::getInstance().logSQL("SQL Statement executed: " + sql);
	Next();
	int id = IntFieldByIndex(0);
	return id;
}

// This function mounts the insert statement, runs it and populate the blobs.
// Since manipulate SQL directly, should stay in a intermediate class between the QueryOracle and the DataDicionaryAccess.

void QueryOracle::mountInsert(const string& tableName, const vector<string>& fieldNames,
		vector<char *>& tDynamicObjectCharVector, const int& imageid, const string& fileName) {

	logUtils::getInstance().logInfo("Starting mounting the insert statement.");
	unsigned int i;

	// @TODO Things to be re-written in this function
	string sql = "INSERT INTO " + tableName + "( ";

	// Declaring the fields for the table.
	for (i = 0; i < fieldNames.size() - 1; i++) {
		sql += fieldNames.at(i) + ", ";
	}
	sql += fieldNames.at(i) + ") ";

	sql += " VALUES (";

	// Get the value of the sequence.
	sql += lexical_cast<string>(imageid) + ", ";

	// Declaring the value of the fields.
	for (i = 1; i < fieldNames.size() - 1; i++) {
		sql += "EMPTY_BLOB(), ";
	}
	sql += "EMPTY_BLOB())";

	logUtils::getInstance().logInfo("Insert statement mounted.");
	logUtils::getInstance().logInfo("SQL: " + sql);

	Statement* stmtInsert = this->conn->createStatement(sql);

	logUtils::getInstance().logInfo("Insert statement mounted.");
	logUtils::getInstance().logInfo("SQL: " + sql);

	// The insert statement was created! Execute!
	//@TODO Change for Run() method here.
	stmtInsert->executeUpdate();
	conn->commit();
	conn->terminateStatement(stmtInsert);

	mountSelectBlobs(fieldNames, tableName, imageid);

	while (rs->next()) {
		populateBlobs(tDynamicObjectCharVector, fieldNames.size(), fileName);
	}
}

void QueryOracle::mountSelectBlobs(const vector<string>& fieldNames, const string& tableName, const int& id) {

	string sql = "SELECT ";
	unsigned int i;
	for (i = 1; i < fieldNames.size() - 1; i++) {
		sql += fieldNames.at(i) + ", ";
	}
	sql += fieldNames.at(i) + " FROM " + tableName + " WHERE " + fieldNames.at(0);
	sql += " = " + lexical_cast<string>(id) + " FOR UPDATE";
	logUtils::getInstance().logInfo("SQL statement for return the blob created: " + sql);
	//@TODO Change for Run() method here.
	Statement *stmtSelect = this->conn->createStatement(sql);
	this->rs = stmtSelect->executeQuery();
	logUtils::getInstance().logSQL("SQL Statement executed: " + sql);

}

void QueryOracle::populateBlobs(const vector<char *>& tDynamicObjectCharVector, const int& count,
		const string& fileName) {
	// The first blob must be populate with the file.
	Blob blob = rs->getBlob(1);
	populateBlob(blob, fileName);
	logUtils::getInstance().logInfo("Populate the first blob with the file" + fileName);

	for (int i = 1; i < count - 1; i++) {
		Blob blob = rs->getBlob(i + 1);
		populateBlob(blob, tDynamicObjectCharVector.at(i - 1));
		logUtils::getInstance().logInfo("Populate blob number " + i + 1);
	}
	logUtils::getInstance().logInfo("The blobs were populated.");
}

eDataTypes QueryOracle::getFieldType(const int& index) {
	vector<MetaData> metaDataVector = rs->getColumnListMetaData();
	int type = metaDataVector[index].getInt(MetaData::MetaData::ATTR_DATA_TYPE);

	switch (type) {
	case OCCI_SQLT_BLOB:
		return BLOB;
	case OCCI_SQLT_NUM:
		return NUMBER;
	case OCCI_SQLT_AFC:
		return STRING;
	default:
		return INTEGER;
	}
}
