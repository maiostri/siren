//---------------------------------------------------------------------------
#include "Parser.h"

//---------------------------------------------------------------------------

//===============================================================
// parser methods!
//=============i==================================================

Parser::Parser(std::string input, std::string username, std::string password, MetricTreeManager *treemanager)
{

    // creating lex
    lex = new Lex();
    lex->Input(input);

    // creating the object that accesses the data dictionary
    datadictionary = new DataDictionaryAccess(username, password, treemanager, this);

    // creating the list of sql statements for update the CDD$ and create the IPV$
    CommandList = new std::vector<std::string > ();

    //creating the current list of image attributes
    CIAList = new std::vector<std::string > ();


    // creating the current list of particulate attributes
    CPAList = new std::vector<std::string > ();


    // creating the current list of audio attributes
    CAAList = new std::vector<std::string > ();


    // creating the list of complex attributes
    ComplexAttributeList = new std::vector<std::string > ();



    // creating the list of metrics associated with a complex attribute
    MetricReferenceList = new std::vector<std::string > ();



    // creating the list of metrics for each image attribute
    MetricList = new std::vector<std::string > ();



    // creating the list of Image Attributes that already has a default metric
    DefaultMetricList = new std::vector<std::string > ();



    // creating the list of particle column names
    ParticleColName = new std::vector<std::string > ();



    // creating the list of particle column types
    ParticleColType = new std::vector<std::string > ();



    // creating the list of particulate parameters
    ParticulateParam = new std::vector<std::string > ();



    // creating the image attribute list
    ImgAttribList = new std::vector<std::string > ();



    // creating the audio attribute list
    AudAttribList = new std::vector<std::string > ();



    // creating the insert column list
    InsertColList = new std::vector<std::string > ();


    // creating the insert value list
    InsertValueList = new std::vector<std::string > ();


    // creating the stillimage insert column list
    ImgAttribPosList = new std::vector<std::string > ();


    // creating the audio insert column list
    AudAttribPosList = new std::vector<std::string > ();



    // creating a list of parameters that are used in the insert commands, used to alter IPV
    ImgInsertAuxList = new std::vector<std::string > ();



    // creating a list of parameters that are used in the insert commands, used to alter APV
    AudInsertAuxList = new std::vector<std::string > ();

    // creating the list of errors
    error = new std::vector<std::string > ();

    // initializing variables
    errors = 0;
    ActiveTokenList = 0;
    TokenListCount = 0;

    TokenList = new Token*[MAX_SCOPE];
    TokenListTail = new Token*[MAX_SCOPE];
    for (int i = 0; i < MAX_SCOPE; i++)
    {
        TokenList[i] = NULL;
        TokenListTail[i] = NULL;
    }
    IsSelect = false;
    IsInsert = false;
}

Parser::~Parser()
{
    delete lex;
    delete CommandList;
    delete CIAList;
    delete CPAList;
    delete CAAList;
    delete ComplexAttributeList;
    delete MetricReferenceList;
    delete MetricList;
    delete DefaultMetricList;
    delete ParticleColName;
    delete ParticleColType;
    delete ParticulateParam;
    delete ImgAttribList;
    delete AudAttribList;
    delete InsertColList;
    delete InsertValueList;
    delete ImgAttribPosList;
    delete AudAttribPosList;
    delete ImgInsertAuxList;
    delete AudInsertAuxList;

    delete datadictionary;

    // to do: clean TokenList. Remember that it is a list of lists!  :-)
}

std::vector<std::string> *Parser::ReturnTokenList()
{
    std::vector<std::string> *list = new std::vector<std::string > ();
    std::string temp = "********";

    for (int i = 0; i <= TokenListCount; i++)
    {
        Token *aux = TokenList[i];
        while (aux != NULL)
        {
            list->push_back(aux->Lexem);
            //list->push_back(aux->Lexem);
            aux = aux->Next;
        }
        list->push_back(temp);
    }

    return list;
}

int Parser::Parse()
{

    Lookahead = lex->GetToken();


    sql_statement();

    // if there is another token
    if (Lookahead->Lexem.length() > 1)
        AddError("Incorrect end of statement.");

    return errors;
}

void Parser::AddError(std::string e)
{
    std::stringstream out;
    out << lex->GetPosition();
    error->push_back("[" + out.str() + "] " + e);
    std::cout << "Error: " + e << std::endl;
    errors++;
}

void Parser::Match(std::string Identifier, int TokenType)
{
    // everything is ok

    if ((Lookahead->Lexem.compare(Identifier) == 0) && (Lookahead->TokenType == TokenType))
    {
        AddToken(Lookahead);
        Lookahead = lex->GetToken();

        // to do: check for lex errors here!
    }
    else
        AddError("Expected \"" + Identifier + "\" found \"" + Lookahead->Lexem + "\".");

}

void Parser::AddToken(Token *token)
{
    // first token
    if (TokenList[ActiveTokenList] == NULL)
    {
        TokenList[ActiveTokenList] = token;
        TokenListTail[ActiveTokenList] = token;
    }
        // second token and so on
    else
    {
        Token *prior = TokenListTail[ActiveTokenList];
        TokenListTail[ActiveTokenList]->Next = token;
        TokenListTail[ActiveTokenList] = token;
        TokenListTail[ActiveTokenList]->Prior = prior;
    }
}

//===============================================================
// rules section!
//===============================================================

void Parser::sql_statement()
{
    // sql definition statement: create, alter, drop
    if ((Lookahead->Lexem.compare("create") == 0) ||
            (Lookahead->Lexem.compare("alter") == 0) ||
            (Lookahead->Lexem.compare("drop") == 0))
    {
        sql_definition_statement();
    }
        // sql manipulation statement: select, insert, update, delete
    else if ((Lookahead->Lexem.compare("select") == 0) ||
            (Lookahead->Lexem.compare("insert") == 0) ||
            (Lookahead->Lexem.compare("update") == 0) ||
            (Lookahead->Lexem.compare("delete") == 0))
    {

        // computing times!
        time_t SelectStart = time(NULL);

        sql_manipulation_statement();

        time_t SelectEnd = time(NULL);
        time_t SelectTotalTime = SelectEnd - SelectStart -
                datadictionary->MetricTreeTotalTime -
                datadictionary->TempJoinInsertsTotalTime -
                datadictionary->GroupSimObjManipulation;

        StrTime = "Parsing: " + TimeInMilliseconds(SelectTotalTime) + " ms.   "
                "MAM Query: " + TimeInMilliseconds(datadictionary->MetricTreeTotalTime) + " ms.   ";
        TotalTime = "Total Time: " + TimeInMilliseconds(SelectTotalTime + datadictionary->MetricTreeTotalTime) + " ms.";
        if ((double) datadictionary->GroupSimObjManipulation > 0.0)
            StrTime += "GroupSimObjManipulation: " + TimeInMilliseconds(datadictionary->GroupSimObjManipulation) + " ms. ";
        if ((double) datadictionary->TempJoinInsertsTotalTime > 0.0)
            StrTime += "Temp join inserts: " + TimeInMilliseconds(datadictionary->TempJoinInsertsTotalTime) + " ms.";

    }
        // describe
    else if ((Lookahead->Lexem.compare("describe") == 0) || (Lookahead->Lexem.compare("desc") == 0))
        describe_specification();
        //show
    else if (Lookahead->Lexem.compare("show") == 0)
        show_specification();
        // error
    else
        AddError("Invalid command: \"" + Lookahead->Lexem + "\"");
}

void Parser::sql_definition_statement()
{
    CommandList = new std::vector<std::string > ();
    // create
    if (Lookahead->Lexem.compare("create") == 0)
    {

        Match("create", TK_IDENTIFIER);


        // table definition
        if (Lookahead->Lexem.compare("table") == 0)
            table_definition();
            // metric definition
        else if (Lookahead->Lexem.compare("metric") == 0)
            metric_definition();
            // index definition
        else if (Lookahead->Lexem.compare("index") == 0)
            index_definition();

            // schema definition (for administrations purposes)
        else if (Lookahead->Lexem.compare("sirenschema") == 0)
        {
            Match("sirenschema", TK_IDENTIFIER);
            datadictionary->CreateSchema(CommandList);
        }
        else
        {
            AddError("Invalid command.");
        }
    }
        //drop
    else if (Lookahead->Lexem.compare("drop") == 0)
    {

        Match("drop", TK_IDENTIFIER);

        // schema definition (for administrations purposes)
        if (Lookahead->Lexem.compare("sirenschema") == 0)
        {
            Match("sirenschema", TK_IDENTIFIER);
            datadictionary->DropSchema(CommandList);
        }
        else if (Lookahead->Lexem.compare("index") == 0)
        {
            drop_index_statement();
        }
        else
            AddError("drop needs implementation!");
    }
}

//===============================================================
// CREATE TABLE STATEMENT
//===============================================================

void Parser::table_definition()
{
    CommandList->push_back("create table");

    // table_definition: create table table_name '(' table_element_list ')'
    Match("table", TK_IDENTIFIER);
    table_name();
    Match("(", TK_OPEN_BRACE);
    table_element_list();
    Match(")", TK_CLOSE_BRACE);


    // Execute create tables, alter tables and inserts available in CommandList string list
    if (errors == 0)
    {

        // Updates the first create table in the CommandList string list
        std::string sql = "";
        Token *prior = TokenList[ActiveTokenList];
        Token *next = TokenList[ActiveTokenList];

        while (next != NULL)
        {

            // Removing the metric constraints
            while ((next->Lexem.compare("constraint") == 0) || (next->Lexem.compare("metric") == 0))
            {

                // Verifies if it is needed to remove the (optional) constraint name
                if (next->Lexem.compare("constraint") == 0)
                {

                    // Table constraint
                    if (prior->Lexem.compare(",") == 0)
                    {
                        // dropping the comma
                        sql = sql.substr(0, sql.length() - 2);

                        // dropping the attribute list
                        while (next->Lexem.compare("using") != 0)
                        {
                            prior = next;
                            next = next->Next;
                        }
                        prior = next;
                        next = next->Next;

                        // dropping the metric list
                        while (next->Lexem.compare(")") != 0)
                        {
                            prior = next;
                            next = next->Next;
                        }
                        prior = next;
                        next = next->Next;
                    }
                        // Column constraint
                    else
                    {
                        // dropping the optional parameter reference list
                        while (next->Lexem.compare("using") != 0)
                        {
                            prior = next;
                            next = next->Next;
                        }
                        prior = next;
                        next = next->Next;

                        // dropping the metric list
                        while (next->Lexem.compare(")") != 0)
                        {
                            prior = next;
                            next = next->Next;
                        }
                        prior = next;
                        next = next->Next;
                    }
                }
                    // Table constraint
                else if (prior->Lexem .compare(",") == 0)
                {
                    // dropping the comma
                    sql = sql.substr(0, sql.length() - 2);

                    // dropping the attribute list
                    while (next->Lexem != "using")
                    {
                        prior = next;
                        next = next->Next;
                    }
                    prior = next;
                    next = next->Next;

                    // dropping the metric list
                    while (next->Lexem.compare(")") != 0)
                    {
                        prior = next;
                        next = next->Next;
                    }
                    prior = next;
                    next = next->Next;
                }
                    // Column constraint
                else
                {

                    // dropping the optional parameter reference list
                    while (next->Lexem.compare("using") != 0)
                    {
                        prior = next;
                        next = next->Next;
                    }
                    prior = next;
                    next = next->Next;

                    // dropping the metric list
                    while (next->Lexem.compare(")") != 0)
                    {
                        prior = next;
                        next = next->Next;
                    }
                    prior = next;
                    next = next->Next;
                }
            }

            sql = sql + next->Lexem + " ";

            prior = next;
            next = next->Next;
        }


        CommandList->at(0) = sql;
        // Execute the sql statements
        for (unsigned int i = 0; i < CommandList->size(); i++)
        {
            sql = CommandList->at(i);
            CommandList->at(i) = sql + " " + datadictionary->RunSQL(sql);
        }
    }
    else
    {
        CommandList->clear();
    }
}

void Parser::table_name()
{

    if (Lookahead->TokenType == TK_IDENTIFIER)
        Match(Lookahead->Lexem, Lookahead->TokenType);
    else
        AddError("Identifier expected.");
}

void Parser::table_element_list()
{

    table_element();
    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        table_element_list();
    }
}

void Parser::table_element()
{

    // a table element is a column definition or a table constraint
    if ((Lookahead->Lexem.compare("constraint") != 0) && (Lookahead->Lexem.compare("primary") != 0) &&
            (Lookahead->Lexem.compare("unique") != 0) && (Lookahead->Lexem.compare("foreign") != 0) &&
            (Lookahead->Lexem.compare("metric") != 0))
    {
        column_definition();
    }
    else
        table_constraint();

}

void Parser::column_definition()
{

    // column definition: column_name data_type [column_constraint_list]
    // column_name
    CIAList->clear(); // clear the current image attribute list
    CPAList->clear(); // clear the current particulate attribute list
    CAAList->clear(); // clear the current audio attribute list

    column_name();

    // data_type

    data_type();
    // optional column_constraint_list
    if (Lookahead->TokenType == TK_IDENTIFIER)
        column_constraint_list();
}

void Parser::column_name()
{

    // column name
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        Match(Lookahead->Lexem, Lookahead->TokenType);
    }
    else
        AddError("Identifier expected.");
}

void Parser::data_type()
{

    // STILLIMAGE treatment
    if (Lookahead->Lexem.compare("stillimage") == 0)
    {
        // auxiliary variable that stores the current column data type
        CurrentColDataType = "stillimage";

        // auxiliary variable that stores the current image attributes for the datadictionary->CreateTableInsertComplexAttribMetric method
        CIAList->push_back(TokenListTail[ActiveTokenList]->Lexem);

        // stillimage becomes integer here!
        Lookahead->Lexem = "integer";
        Lookahead->LexemType = LK_STILLIMAGE_DATATYPE;
        Match("integer", TK_IDENTIFIER);

        // inserting the unique token
        Token *tk = new Token();
        tk->Lexem = "unique";
        tk->TokenType = TK_IDENTIFIER;
        AddToken(tk);

        // creates the IPV$ table for the image attribute
        datadictionary->CreateTableStillimage(TokenList[ActiveTokenList], CommandList);
        // creates the sql statement for the insert in the CDD$ComplexAttribute table
        datadictionary->CreateTableInsertComplexAttribute(CommandList, TokenList[ActiveTokenList], TokenListTail[ActiveTokenList]->Prior->Prior->Lexem, "stillimage");
    }
        // AUDIO treatment
    else if (Lookahead->Lexem.compare("audio") == 0)
    {
        // auxiliary variable that stores the current column data type
        CurrentColDataType = "audio";

        // auxiliary variable that stores the current image attributes for the datadictionary->CreateTableInsertComplexAttribMetric method
        CAAList->push_back(TokenListTail[ActiveTokenList]->Lexem);

        // audio becomes integer here!
        Lookahead->Lexem = "integer";
        Lookahead->LexemType = LK_AUDIO_DATATYPE;
        Match("integer", TK_IDENTIFIER);

        // inserting the unique token
        Token *tk = new Token();
        tk->Lexem = "unique";
        tk->TokenType = TK_IDENTIFIER;
        AddToken(tk);

        // creates the APV$ table for the audio attribute
        datadictionary->CreateTableAudio(TokenList[ActiveTokenList], CommandList);
        // creates the sql statement for the insert in the CDD$ComplexAttribute table
        datadictionary->CreateTableInsertComplexAttribute(CommandList, TokenList[ActiveTokenList], TokenListTail[ActiveTokenList]->Prior->Prior->Lexem, "audio");
    }
        // identifier
    else if (Lookahead->Lexem.compare("particulate") == 0)
    {
        // auxiliary variable that stores the current column data type
        CurrentColDataType = "particulate";

        // auxiliary variable that stores the curret particulate attribute
        CPAList->push_back(TokenListTail[ActiveTokenList]->Lexem);

        // particulate becomes integer here!
        Lookahead->Lexem = "integer";
        Lookahead->LexemType = LK_PARTICULATE_DATATYPE;

        Match("integer", TK_IDENTIFIER);

        // creates the sql statement for the insert in the CDD$ComplexAttribute table
        datadictionary->CreateTableInsertComplexAttribute(CommandList, TokenList[ActiveTokenList], TokenListTail[ActiveTokenList]->Prior->Lexem, "particulate");

    }
    else if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        Match(Lookahead->Lexem, Lookahead->TokenType);
    }
    else
        AddError("Data type expected.");

    // if next token is "(", an integer and an ")" are expected. Eg. varchar(20)
    if (Lookahead->TokenType == TK_OPEN_BRACE)
    {
        Match("(", TK_OPEN_BRACE);
        if (Lookahead->TokenType == TK_INTEGER)
            Match(Lookahead->Lexem, TK_INTEGER);
        else
            AddError("Integer expected.");
        Match(")", TK_CLOSE_BRACE);
    }
}

void Parser::column_constraint_list()
{
    column_constraint();
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        column_constraint_list();
    }

}

void Parser::column_constraint()
{

    // optional constraint name
    constraint_name();
    // constraint: not null
    if (Lookahead->Lexem.compare("not") == 0)
    {
        Match("not", TK_IDENTIFIER);
        Match("null", TK_IDENTIFIER);
    }
        // constraint: null
    else if (Lookahead->Lexem.compare("null") == 0)
    {
        Match("null", TK_IDENTIFIER);
    }
        // constraint: unique
    else if (Lookahead->Lexem.compare("unique") == 0)
    {
        Match("unique", TK_IDENTIFIER);
    }
        // constraint: primary key
    else if (Lookahead->Lexem.compare("primary") == 0)
    {
        Match("primary", TK_IDENTIFIER);
        Match("key", TK_IDENTIFIER);
    }
        // constraint: references table_name ['(' column_name_list ')']
    else if (Lookahead->Lexem.compare("references") == 0)
    {
        Match("references", TK_IDENTIFIER);

        table_name();
        // optional column name list
        if (Lookahead->Lexem.compare("(") == 0)
        {
            Match("(", TK_OPEN_BRACE);
            column_name_list();
            Match(")", TK_CLOSE_BRACE);
        }
    }
        // constraint: metric [references '(' parameter_assoc_list ')'] using '(' metric_name [default], metric_name ... ')'
    else if (Lookahead->Lexem.compare("metric") == 0)
    {
        Match("metric", TK_IDENTIFIER);

        // optional tokens for the particulate data type
        if (CurrentColDataType.compare("particulate") == 0)
        {
            Match("references", TK_IDENTIFIER);
            Match("(", TK_OPEN_BRACE);

            ParticleColName->clear();
            ParticleColType->clear();
            ParticulateParam->clear();


            parameter_assoc_list();
            Match(")", TK_CLOSE_BRACE);
        }
        Match("using", TK_IDENTIFIER);
        Match("(", TK_OPEN_BRACE);
        metric_name_list();
        Match(")", TK_CLOSE_BRACE);
    }
    else
    {
        AddError("Constraint expected.");
        // if it is an identifier then is wrong!
        if (Lookahead->TokenType == TK_IDENTIFIER)
            Match(Lookahead->Lexem, Lookahead->TokenType);
    }
}

void Parser::constraint_name()
{

    if (Lookahead->Lexem.compare("constraint") == 0)
    {
        Match("constraint", TK_IDENTIFIER);
        if (Lookahead->TokenType == TK_IDENTIFIER)
            Match(Lookahead->Lexem, TK_IDENTIFIER);
        else
            AddError("Identifier expected.");
    }
}

void Parser::column_name_list()
{

    column_name();

    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        column_name_list();
    }
}

void Parser::parameter_assoc_list()
{
    // parameter_assoc_list: parameter_assoc | parameter_assoc ',' parameter_assoc_list
    parameter_assoc();
    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        parameter_assoc_list();
    }
}

void Parser::parameter_assoc()
{
    // parameter_assoc: particle_column_ref as parameter_name

    // particle_colum_ref
    Match(Lookahead->Lexem, TK_IDENTIFIER);

    // verifying if the particle column was already defined
    std::string particle_colum_ref = TokenListTail[ActiveTokenList]->Lexem;
    std::string oradatatype = "";
    Token *auxp = TokenListTail[ActiveTokenList]->Prior;
    while (((auxp->Lexem != particle_colum_ref) || (auxp->Next->Lexem.compare("as") == 0)) && (auxp->Prior != NULL))
    {
        auxp = auxp->Prior;

    }
    if ((auxp->Prior != NULL) && (auxp->Next->TokenType == TK_IDENTIFIER))
    {
        ParticleColName->push_back(auxp->Lexem);
        oradatatype = datadictionary->GetDbmsDataType(auxp->Next->Lexem);
        ParticleColType->push_back(oradatatype);

    }
    else
        AddError("The attribute: '" + particle_colum_ref + "' must be defined before being used as a reference.");

    // token as
    Match("as", TK_IDENTIFIER);
    // parameter_name
    Match(Lookahead->Lexem, TK_IDENTIFIER);

    ParticulateParam->push_back(TokenListTail[ActiveTokenList]->Lexem);
}

void Parser::metric_name_list()
{

    metric_name();

    std::string tablename = "";
    // gets the table name
    tablename = TokenList[ActiveTokenList]->Next->Next->Lexem;

    if (errors == 0)
    {

        // Verifies if the metric exists
        MetricCode = datadictionary->IsMetric(TokenListTail[ActiveTokenList]->Lexem);

        if (MetricCode == 0)
            AddError("Invalid metric: " + TokenListTail[ActiveTokenList]->Lexem + ".");
        else
        {
            // auxilary variable that stores the name of the current metric
            std::string metricname = TokenListTail[ActiveTokenList]->Lexem;
            // auxilary variable that stores the type of the current metric
            std::string metrictype = datadictionary->GetMetricType(MetricCode);

            if (CurrentColDataType.compare("stillimage") == 0)
            {
                if (metrictype.compare("stillimage") != 0)
                {
                    AddError("Invalid metric type. The metric '" + metricname + "' isn't a valid metric for stillimage attributes.");
                }
                else
                {
                    // verify/insert the pair Attribute Metric in the ComplexAttributeList and in the MetricReferenceList respectively
                    for (unsigned int i = 0; i < CIAList->size(); i++)
                    {
                        int attrindex;

                        unsigned int cianumber = atoi(CIAList->at(i).c_str());
                        if ((cianumber > ComplexAttributeList->size()) ||
                                (ComplexAttributeList->size() == 0))
                            attrindex = -1;
                        else
                        {
                            std::string complexnumber = ComplexAttributeList->at(cianumber).c_str();
                            attrindex = atoi(complexnumber.c_str());

                        }
                        bool imgmetric = false;
                        // if ComplexAttributeList != NULL
                        if (attrindex != -1)
                        {
                            for (unsigned int j = 0; j < ComplexAttributeList->size(); j++)
                            {
                                if (ComplexAttributeList->at(j) == CIAList->at(i))
                                {
                                    if (atoi(MetricReferenceList->at(j).c_str()) == MetricCode)
                                        imgmetric = true;
                                }
                            }
                        }
                        // Verify if the association between the ImageAttribute and the Metric already exists
                        if ((!imgmetric) &&
                                (datadictionary->IsValidMetric(metricname, tablename, CIAList->at(i).c_str()) == 0))
                        {
                            // Auxilary variable that stores 'true|false' for the isdefault parameter of the datadictionary->CreateTableInsertComplexAttribMetric method
                            bool isdefault = false;

                            // optional: default
                            if (Lookahead->Lexem.compare("default") == 0)
                            {
                                //int cialistnumber = atoi(CIAList->at(i).c_str());

                                std::string cia = CIAList->at(i).c_str();
                                std::vector<std::string>::iterator it;

                                it = std::find(DefaultMetricList->begin(), DefaultMetricList->end(), cia);
                                if (it == DefaultMetricList->end())
                                {

                                    //if ((cialistnumber > DefaultMetricList->size()) ||
                                    //   (DefaultMetricList->size() == 0)) {
                                    DefaultMetricList->push_back(CIAList->at(i));
                                    isdefault = true;
                                }

                                    // if the image attribute is not in the default metric list
                                    //if (atoi((DefaultMetricList->at(atoi((CIAList->at(i)).c_str()))).c_str()) == -1) {

                                else
                                    AddError("There are more than one default metric specified for the attribute: '" + CIAList->at(i) + "'.");
                            }

                            if (errors == 0)
                            {
                                // Creates the sql statement for the insert (in the CDD$ComplexAttribMetric table) that join an image attribute with a metric
                                datadictionary->CreateTableInsertComplexAttribMetric(TokenList[ActiveTokenList], tablename, CommandList, CIAList->at(i), MetricCode, isdefault);

                                // Alters the IPV$ table. Adds one attribute for each extractor of the specified metric.
                                datadictionary->CreateTableAlterPVTable(CurrentColDataType, tablename, CommandList, CIAList->at(i), MetricCode, ComplexAttributeList, MetricReferenceList);
                                std::stringstream out;
                                out << MetricCode;
                                MetricList->push_back(out.str());
                            }

                            ComplexAttributeList->push_back(CIAList->at(i));
                            std::stringstream out;
                            out << MetricCode;
                            MetricReferenceList->push_back(out.str());
                        }
                        else
                            AddError("It is not allowed to duplicate the metric '" + TokenListTail[ActiveTokenList]->Lexem + "' for the attribute '" + CIAList->at(i) + "'.");
                    }
                }
            }

            else if (CurrentColDataType.compare("audio") == 0)
            {
                if (metrictype.compare("audio") != 0)
                {
                    AddError("Invalid metric type. The metric '" + metricname + "' isn't a valid metric for audio attributes.");
                }
                else
                {

                    // verify/insert the pair Attribute Metric in the ComplexAttributeList and in the MetricReferenceList respectively
                    for (unsigned int i = 0; i < CAAList->size(); i++)
                    {

                        int attrindex = atoi((ComplexAttributeList->at(atoi((CAAList->at(i)).c_str()))).c_str());
                        bool audiometric = false;
                        // if ComplexAttributeList != NULL
                        if (attrindex != -1)
                        {
                            for (unsigned int j = 0; j < ComplexAttributeList->size(); j++)
                            {
                                if (ComplexAttributeList->at(j).compare(CAAList->at(i).c_str()) == 0)
                                {
                                    if (atoi(MetricReferenceList->at(j).c_str()) == MetricCode)
                                        audiometric = true;
                                }
                            }
                        }

                        // Verify if the association between the AudioAttribute and the Metric already exists
                        if ((!audiometric) &&
                                (datadictionary->IsValidMetric(metricname, tablename, CAAList->at(i).c_str()) == 0))
                        {

                            // Auxilary variable that stores 'true|false' for the isdefault parameter of the datadictionary->CreateTableInsertComplexAttribMetric method
                            bool isdefault = false;

                            // optional: default
                            if (Lookahead->Lexem.compare("default") == 0)
                            {

                                std::string caa = CAAList->at(i).c_str();
                                std::vector<std::string>::iterator it;

                                it = std::find(DefaultMetricList->begin(), DefaultMetricList->end(), caa);
                                if (it == DefaultMetricList->end())
                                {
                                    DefaultMetricList->push_back(CAAList->at(i));
                                    isdefault = true;
                                }
                                else
                                    AddError("There are more than one default metric specified for the attribute: '" + CAAList->at(i) + "'.");
                            }

                            if (errors == 0)
                            {
                                // Creates the sql statement for the insert (in the CDD$ComplexAttribMetric table) that join an audio attribute with a metric
                                datadictionary->CreateTableInsertComplexAttribMetric(TokenList[ActiveTokenList], tablename, CommandList, CAAList->at(i), MetricCode, isdefault);

                                // Alters the APV$ table. Adds one attribute for each extractor of the specified metric.
                                datadictionary->CreateTableAlterPVTable(CurrentColDataType, tablename, CommandList, CAAList->at(i), MetricCode, ComplexAttributeList, MetricReferenceList);
                                std::stringstream out;
                                out << MetricCode;
                                MetricList->push_back(out.str());
                            }

                            ComplexAttributeList->push_back(CAAList->at(i));
                            std::stringstream out;
                            out << MetricCode;
                            MetricReferenceList->push_back(out.str());
                        }
                        else
                            AddError("It is not allowed to duplicate the metric '" + TokenListTail[ActiveTokenList]->Lexem + "' for the attribute '" + CAAList->at(i) + "'.");
                    }
                }
            }
            else if (CurrentColDataType.compare("particulate") == 0)
            {
                if (metrictype.compare("particulate") != 0)
                {
                    AddError("Invalid metric type. The metric '" + metricname + "' isn't a valid metric for particulate attributes.");
                }
                else
                {

                    // verify/insert the pair Attribute/Metric in the list AttributeMetricList
                    for (unsigned int i = 0; i < CPAList->size(); i++)
                    {
                        unsigned int cpalistnumber = atoi((CPAList->at(i)).c_str());
                        int attrindex = -1;
                        if ((ComplexAttributeList->size() != 0) && (cpalistnumber < CPAList->size()))
                            attrindex = atoi((ComplexAttributeList->at(cpalistnumber)).c_str());
                        bool particulatemetric = false;
                        // if ComplexAttributeList != NULL
                        if (attrindex != -1)
                        {
                            for (unsigned int j = 0; j < ComplexAttributeList->size(); j++)
                            {
                                if (ComplexAttributeList->at(j).compare(CPAList->at(i)) == 0)
                                {
                                    if (atoi(MetricReferenceList->at(j).c_str()) == MetricCode)
                                        particulatemetric = true;
                                }
                            }
                        }

                        // Verify if the association between the ParticulateAttribute and the Metric already exists
                        if ((!particulatemetric) &&
                                (datadictionary->IsValidMetric(metricname, tablename, CPAList->at(i)) == 0))
                        {
                            // ParamList is a list that stores the pairs [parameter,type]
                            // of a metric created for a particulate attribute
                            std::vector<std::string> *ParamList = new std::vector<std::string > ();
                            datadictionary->GetParameters(MetricCode, ParamList);
                            // ParamList->size()/2 because this list stores pairs [parameter,type]
                            if (ParamList->size() / 2 == ParticleColName->size())
                            {
                                for (unsigned int j = 0; j < ParamList->size(); j += 2)
                                {
                                    int ix;
                                    // get ParticulateParam position
                                    unsigned int paramlistnumber = atoi((ParamList->at(j)).c_str());
                                    if ((paramlistnumber > ParticulateParam->size()) ||
                                            (ParticulateParam->size() == 0))
                                    {
                                        ix = -1;
                                    }
                                    else
                                        ix = atoi((ParticulateParam->at(paramlistnumber)).c_str());
                                    if (ix == -1)
                                    {
                                        AddError("Missing parameter: '" + ParamList->at(j) + "'");
                                    }
                                    else
                                    {
                                        if (ParamList->at(j + 1) == ParticleColType->at(ix))
                                        {
                                            // insert into the data dictionary (CDD$MetricInstance). Parameters needed: tablename, complexattribname, metriccode, parametername, extractorcode, parameterinstance
                                            // The extractor code for the particulate data is 0
                                            datadictionary->CreateTableInsertMetricInstance(CommandList, tablename, CPAList->at(i), MetricCode, ParamList->at(j), 0, ParticleColName->at(ix));
                                        }
                                        else
                                        {
                                            AddError("Invalid parameter type. Expected '" + ParamList->at(j + 1) + "'.");
                                        }
                                    }
                                }
                                delete ParamList;
                            }
                            else
                            {
                                AddError("Missing or extra particulate metric parameter(s).");
                            }

                            // Auxilary variable that stores 'true|false' for the isdefault parameter of the datadictionary->CreateTableInsertComplexAttribMetric method
                            bool isdefault = false;

                            // optional: default
                            if (Lookahead->Lexem.compare("default") == 0)
                            {

                                std::string cpa = CPAList->at(i).c_str();
                                std::vector<std::string>::iterator it;

                                it = std::find(DefaultMetricList->begin(), DefaultMetricList->end(), cpa);
                                if (it == DefaultMetricList->end())
                                {

                                    DefaultMetricList->push_back(CPAList->at(i));
                                    isdefault = true;
                                }
                                else
                                    AddError("There are more than one default metric specified for the attribute: '" + CPAList->at(i) + "'.");
                            }

                            if (errors == 0)
                            {
                                // Creates the sql statement for the insert (in the CDD$ComplexAttribMetric table) that join a particulate attribute with a metric
                                datadictionary->CreateTableInsertComplexAttribMetric(TokenList[ActiveTokenList], tablename, CommandList, CPAList->at(i), MetricCode, isdefault);
                            }

                            ComplexAttributeList->push_back(CPAList->at(i));
                            std::stringstream out;
                            out << MetricCode;
                            MetricReferenceList->push_back(out.str());
                        }
                        else
                            AddError("It is not allowed to duplicate the metric '" + TokenListTail[ActiveTokenList]->Lexem + "' for the attribute '" + CPAList->at(i) + "'.");
                    }
                }
            }
        }
    }

    // optional: default
    if (Lookahead->TokenType == TK_IDENTIFIER)
        Match("default", TK_IDENTIFIER);

    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        metric_name_list();
    }
}

void Parser::metric_name()
{

    // metric_name
    if (Lookahead->TokenType == TK_IDENTIFIER)
        Match(Lookahead->Lexem, Lookahead->TokenType);
    else
        AddError("Metric name expected.");
}

void Parser::table_constraint()
{

    // optional constraint name
    constraint_name();

    // unique constraint definition: {primary key | unique} '(' column_name_list ')'
    if (Lookahead->Lexem.compare("primary") == 0)
    {
        Match("primary", TK_IDENTIFIER);
        Match("key", TK_IDENTIFIER);
        Match("(", TK_OPEN_BRACE);
        column_name_list();
        Match(")", TK_CLOSE_BRACE);
    }
        // unique constraint definition: {primary key | unique} '(' column_name_list ')'
    else if (Lookahead->Lexem.compare("unique") == 0)
    {
        Match("unique", TK_IDENTIFIER);
        Match("(", TK_OPEN_BRACE);
        column_name_list();
        Match(")", TK_CLOSE_BRACE);
    }
        // referential constraint definition: foreign key '(' column_name_list ')' references table_name ['(' column_name_list ')']
    else if (Lookahead->Lexem.compare("foreign") == 0)
    {
        Match("foreign", TK_IDENTIFIER);
        Match("key", TK_IDENTIFIER);
        Match("(", TK_OPEN_BRACE);
        column_name_list();
        Match(")", TK_CLOSE_BRACE);
        Match("references", TK_IDENTIFIER);
        table_name();
        // optional column name list
        if (Lookahead->Lexem.compare("(") == 0)
        {
            Match("(", TK_OPEN_BRACE);
            column_name_list();
            Match(")", TK_CLOSE_BRACE);
        }
    }
        // metric constraint definition: metric {'(' stillimage_column_name_list ')'
        //                                     | '(' audio_column_name_list ')'
        //                                     | '(' particulate_column_name_list ')' references '(' parameter_assoc_list ')'}
        //                                     |  using '(' metric_name_list ')'
    else if (Lookahead->Lexem.compare("metric") == 0)
    {
        Match("metric", TK_IDENTIFIER);
        Match("(", TK_OPEN_BRACE);

        // clear the current image attribute list
        CIAList->clear();
        // clear the current particulate attribute list
        CPAList->clear();
        // clear the current audio attribute list
        CAAList->clear();

        // verifying the data type of the attributes specified in the list
        if (datadictionary->IsStillimageAttribute(TokenList[ActiveTokenList], Lookahead->Lexem))
        {
            stillimage_column_name_list();
            CurrentColDataType = "stillimage";
        }
        else if (datadictionary->IsParticulateAttribute(TokenList[ActiveTokenList], Lookahead->Lexem))
        {
            particulate_column_name_list();
            CurrentColDataType = "particulate";
        }
        else if (datadictionary->IsAudioAttribute(TokenList[ActiveTokenList], Lookahead->Lexem))
        {
            audio_column_name_list();
            CurrentColDataType = "audio";
        }
        else
            AddError("Invalid attribute(s). A stillimage attribute list, an audio attribute list or a particulate attribute list is expected.");

        Match(")", TK_CLOSE_BRACE);

        // optional tokens for the particulate data type
        if (CurrentColDataType.compare("particulate") == 0)
        {
            Match("references", TK_IDENTIFIER);
            Match("(", TK_OPEN_BRACE);

            ParticleColName->clear();
            ParticleColType->clear();
            ParticulateParam->clear();

            parameter_assoc_list();
            Match(")", TK_CLOSE_BRACE);
        }

        Match("using", TK_IDENTIFIER);
        Match("(", TK_OPEN_BRACE);
        metric_name_list();
        Match(")", TK_CLOSE_BRACE);
    }

}

void Parser::stillimage_column_name_list()
{

    column_name();

    if (errors == 0)
    {
        // Verifies if the attribute type is stillimage
        if (datadictionary->IsStillimageAttribute(TokenList[ActiveTokenList], TokenListTail[ActiveTokenList]->Lexem))
        {

            CIAList->push_back(TokenListTail[ActiveTokenList]->Lexem);

            if (Lookahead->TokenType == TK_COMMA)
            {
                Match(",", TK_COMMA);
                stillimage_column_name_list();
            }
        }
        else
            AddError("Invalid attribute name: '" + TokenListTail[ActiveTokenList]->Lexem + "'. Stillimage attribute expected.");
    }
}

void Parser::particulate_column_name_list()
{

    column_name();

    if (errors == 0)
    {
        // Verifies if the attribute type is particulate
        if (datadictionary->IsParticulateAttribute(TokenList[ActiveTokenList], TokenListTail[ActiveTokenList]->Lexem))
        {

            CPAList->push_back(TokenListTail[ActiveTokenList]->Lexem);

            if (Lookahead->TokenType == TK_COMMA)
            {
                Match(",", TK_COMMA);
                particulate_column_name_list();
            }
        }
        else
            AddError("Invalid attribute name: '" + TokenListTail[ActiveTokenList]->Lexem + "'. Particulate attribute expected.");
    }
}

void Parser::audio_column_name_list()
{

    column_name();

    if (errors == 0)
    {
        // Verifies if the attribute type is audio
        if (datadictionary->IsAudioAttribute(TokenList[ActiveTokenList], TokenListTail[ActiveTokenList]->Lexem))
        {

            CAAList->push_back(TokenListTail[ActiveTokenList]->Lexem);

            if (Lookahead->TokenType == TK_COMMA)
            {
                Match(",", TK_COMMA);
                audio_column_name_list();
            }
        }
        else
            AddError("Invalid attribute name: '" + TokenListTail[ActiveTokenList]->Lexem + "'. Audio attribute expected.");
    }
}

void Parser::sql_manipulation_statement()
{

    std::string errmsg = "";
    //Token *errmsgw = new Token();
    bool onOptimizer = false;


    //onOptimizer indicates if Query Optimization is on/off
    onOptimizer = GetCkOptimization();

    if (Lookahead->Lexem.compare("select") == 0)
    {
        IsSelect = true;
        // parse "select"
        query_specification();

        // Treat the NEAR and FAR predicates
        if (errors == 0)
        {

            //Treat the Query Optimization ON
            if (onOptimizer)
            {
                //Query Optimization
                /*errmsgw = datadictionary->QueryOptimization(TokenList,ActiveTokenList); */
            }

            //TokenList = errmsgw;

            //errmsg = datadictionary->NearAndFarPredicates (,ActiveTokenList);
            errmsg = datadictionary->NearAndFarPredicates(TokenList, ActiveTokenList);

            if (errmsg.compare("") != 0)
                AddError(errmsg);
        }

        // insert the tokenlist in the command list
        Token *tl = TokenList[ActiveTokenList], *tlaux;
        std::string selectstm = "";
        while (tl != NULL)
        {
            selectstm = selectstm + tl->Lexem;
            tlaux = tl->Next;
            if (tlaux == NULL) tlaux = tl;
            if ((tl->Lexem != ".") && (tlaux->Lexem != "."))
                selectstm = selectstm + " ";
            tl = tl->Next;
        }
        CommandList->push_back(selectstm);
    }
    else if (Lookahead->Lexem.compare("insert") == 0)
    {
        insert_statement();
    }
    else if (Lookahead->Lexem.compare("update") == 0)
    {
        AddError("Update needs implementation!");
    }
    else if (Lookahead->Lexem.compare("delete") == 0)
    {
        AddError("Delete needs implementation!");
    }
}

//===============================================================
// CREATE METRIC STATEMENT
//===============================================================

void Parser::metric_definition()
{

    // metric_definition: create metric metric_name [using LP0 | LP1 | LP2]
    //                         for {particulate '(' particulate_param_ref_list ')'
    //                              | stillimage '(' extractor_reference_list ')'
    //                              | audio '(' extractor_reference_list ')'}
    Match("metric", TK_IDENTIFIER);
    metric_name();


    // Auxilary variable that stores the metric name for the datadictionary->CreateMetricInsertMetric method
    std::string MetricName = TokenListTail[ActiveTokenList]->Lexem;

    // Auxilary variable that stores the index of the distance function for the datadictionary->CreateMetricInsertMetric method
    int LpP = 2;

    // Auxilary variable that stores for what data type the metric is created for
    std::string DataType = "";

    // optional distance function specification
    if ((Lookahead->TokenType == TK_IDENTIFIER) && (Lookahead->Lexem.compare("using") == 0))
    {
        Match("using", TK_IDENTIFIER);
        if (Lookahead->TokenType == TK_IDENTIFIER)
        {
            if (Lookahead->Lexem.compare("lp0") == 0)
            {
                Match("lp0", TK_IDENTIFIER);
                LpP = 0;
            }
            else if (Lookahead->Lexem.compare("lp1") == 0)
            {
                Match("lp1", TK_IDENTIFIER);
                LpP = 1;
            }
            else if (Lookahead->Lexem.compare("lp2") == 0)
            {
                Match("lp2", TK_IDENTIFIER);
                LpP = 2;
            }
            else AddError("Invalid distance function.");
        }
        else AddError("Missing distance function.");
    }

    // data type specification: particulate, stillimage or audio
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        Match("for", TK_IDENTIFIER);
        if (Lookahead->TokenType == TK_IDENTIFIER)
        {
            if (Lookahead->Lexem.compare("stillimage") == 0)
            {
                Match("stillimage", TK_IDENTIFIER);
                DataType = "stillimage";
            }
            else if (Lookahead->Lexem.compare("particulate") == 0)
            {
                Match("particulate", TK_IDENTIFIER);
                DataType = "particulate";
            }
            else if (Lookahead->Lexem.compare("audio") == 0)
            {
                Match("audio", TK_IDENTIFIER);
                DataType = "audio";
            }
        }
        else AddError("Missing reserved word: 'STILLIMAGE', 'PARTICULATE' or 'AUDIO'.");
    }
    else AddError("Missing clause: 'FOR STILLIMAGE', 'FOR PARTICULATE', or 'FOR AUDIO'.");


    if (errors == 0)
    {
        // Verifies if the metric name already exists
        if (datadictionary->IsMetric(MetricName) > 0)
        {
            AddError("Metric name " + MetricName + " already exists.");
        }
        else
        {
            if ((DataType.compare("stillimage") == 0) || (DataType.compare("audio") == 0))
            {
                //datadictionary->StartTransaction();

                // Inserts the metric name and the index of the distance function in the CDD$MetricStruct
                MetricCode = datadictionary->CreateMetricInsertMetric(MetricName, LpP, CommandList, DataType);
                Match("(", TK_OPEN_BRACE);
                extractor_reference_list();
                Match(")", TK_CLOSE_BRACE);

                if (errors == 0)
                {
                    datadictionary->CommitTransaction();
                }
                else
                {
                    datadictionary->RollbackTransaction();
                    CommandList->clear();
                }
            }
            else
            {
                // particulate
                //datadictionary->StartTransaction();

                // Inserts the metric name and the index of the distance function in the CDD$MetricStruct
                MetricCode = datadictionary->CreateMetricInsertMetric(MetricName, LpP, CommandList, DataType);

                Match("(", TK_OPEN_BRACE);

                // The extractor code for the particulate data is 0
                ExtractorCode = 0;

                particulate_param_ref_list();

                Match(")", TK_CLOSE_BRACE);

                if (errors == 0)
                {
                    datadictionary->CommitTransaction();
                }
                else
                {
                    datadictionary->RollbackTransaction();
                    CommandList->clear();
                }
            }
        }
    }
}

void Parser::extractor_reference_list()
{

    // extractor_reference_list : extractor_reference | extractor_reference ',' extractor_reference_list
    extractor_reference();
    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        extractor_reference_list();
    }
}

void Parser::extractor_reference()
{

    // extractor_reference: extractor_name '(' parameter_reference_list ')'
    extractor_name();
    Match("(", TK_OPEN_BRACE);
    parameter_reference_list();
    Match(")", TK_CLOSE_BRACE);
}

void Parser::extractor_name()
{

    if (Lookahead->TokenType == TK_IDENTIFIER)
    {

        // Verifies if the extractor exists
        ExtractorCode = datadictionary->IsExtractor(Lookahead->Lexem);

        if (ExtractorCode > 0)
        {
            // Verifies if the extractor is valid for the metric type

            if (datadictionary->IsValidExtractor(Lookahead->Lexem, TokenList[ActiveTokenList]))
                Match(Lookahead->Lexem, Lookahead->TokenType);
            else
                AddError("Invalid extractor.");
        }
        else
        {
            AddError("Invalid extractor name.");
        }
    }
    else
        AddError("Missing or invalid extractor name.");
}

void Parser::parameter_reference_list()
{

    // parameter_reference_list: parameter_reference | parameter_reference ',' parameter_reference_list
    parameter_reference();
    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        parameter_reference_list();
    }
}

void Parser::parameter_reference()
{

    // parameter_reference: parameter_name AS parameter_alias [weight_value]
    parameter_name();

    // Auxilary variable that stores the parameter name for the datadictionary->CreateMetricInsertParameter method
    std::string ParameterName = TokenListTail[ActiveTokenList]->Lexem;

    // Auxilary variable that stores the parameter type
    std::string ParameterType = datadictionary->GetParameterType(ParameterName, ExtractorCode);

    // Auxilary variable that stores the parameter alias for the datadictionary->CreateMetricInsertParameter method
    std::string ParameterAlias;

    // AS
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        Match("as", TK_IDENTIFIER);
    }

    // parameter_alias
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        Match(Lookahead->Lexem, Lookahead->TokenType);
        ParameterAlias = TokenListTail[ActiveTokenList]->Lexem;
    }
    else
        AddError("Missing or invalid parameter alias.");

    // Auxilary variable that stores the parameter weight for the datadictionary->CreateMetricInsertParameter method
    float ParameterWeight = 1;

    // Optional weight_value
    if ((Lookahead->TokenType == TK_FLOATING_POINT) || (Lookahead->TokenType == TK_INTEGER))
    {
        Match(Lookahead->Lexem, Lookahead->TokenType);
        ParameterWeight = atof(TokenListTail[ActiveTokenList]->Lexem.c_str());
    }

    if (errors == 0)
        datadictionary->CreateMetricInsertParameter(MetricCode, ExtractorCode, ParameterName, ParameterAlias, ParameterType, ParameterWeight, CommandList);
}

void Parser::parameter_name()
{
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {

        // Verifies if the parameter exists
        if (datadictionary->IsExtractorParameter(ExtractorCode, Lookahead->Lexem))
        {
            Match(Lookahead->Lexem, Lookahead->TokenType);
        }
        else
        {
            AddError("Invalid parameter name.");
        }
    }
    else
        AddError("Missing or invalid parameter name.");
}

void Parser::particulate_param_ref_list()
{

    // particulate_param_ref_list: particulate_param_ref | particulate_param_ref ',' particulate_param_ref_list
    particulate_param_ref();
    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        particulate_param_ref_list();
    }
}

void Parser::particulate_param_ref()
{

    // Auxiliary variable that stores the parameter name for the datadictionary->CreateMetricInsertParameter method
    std::string ParameterName = "";

    // Auxiliary variable that stores the parameter type
    std::string ParameterType = "";

    // Auxiliary variable that stores the parameter alias for the datadictionary->CreateMetricInsertParameter method
    // the parameter alias is "none" for the particulate data type
    std::string ParameterAlias = "none";

    // particulate_param_ref: particulate_parameter_name parameter_type [weight_value]

    // particulate_parameter_name
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        Match(Lookahead->Lexem, Lookahead->TokenType);

        ParameterName = TokenListTail[ActiveTokenList]->Lexem;
    }
    else
        AddError("Missing or invalid parameter name.");

    // parameter_type
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        // verifying if the parameter type is valid
#ifdef POSTGRES
        if ((Lookahead->Lexem.compare("real") == 0) || (Lookahead->Lexem.compare("integer") == 0) || (Lookahead->Lexem.compare("double precision") == 0) || (Lookahead->Lexem("numeric") == 0) || (Lookahead->Lexem.compare("smallint") == 0))
        {
#else
        if ((Lookahead->Lexem.compare("float") == 0) || (Lookahead->Lexem.compare("integer") == 0) || (Lookahead->Lexem.compare("long") == 0) || (Lookahead->Lexem.compare("number") == 0) || (Lookahead->Lexem.compare("decimal") == 0))
        {
#endif

            ParameterType = datadictionary->GetDbmsDataType(Lookahead->Lexem);
            Lookahead->Lexem = ParameterType;
            Match(Lookahead->Lexem, Lookahead->TokenType);
        }
        else
            AddError("Invalid parameter type: '" + Lookahead->Lexem + "'. Only numeric types are allowed");

    }
    else
        AddError("Missing or invalid parameter type.");

    // Auxilary variable that stores the parameter weight for the datadictionary->CreateMetricInsertParameter method
    float ParameterWeight = 1;

    // optional weight value
    if ((Lookahead->TokenType == TK_FLOATING_POINT) || (Lookahead->TokenType == TK_INTEGER))
    {
        Match(Lookahead->Lexem, Lookahead->TokenType);
        ParameterWeight = atof((TokenListTail[ActiveTokenList]->Lexem).c_str());
    }

    if (errors == 0)
        datadictionary->CreateMetricInsertParameter(MetricCode, ExtractorCode, ParameterName, ParameterAlias, ParameterType, ParameterWeight, CommandList);
}

//===============================================================
// CREATE INDEX STATEMENT
//===============================================================

void Parser::index_definition()
{

    // index_definition: create index index_name on table_name '(' complex_column_reference ')' [references '(' param_assoc_ref_list()')'] using metric_name_reference
    Match("index", TK_IDENTIFIER);
    index_name();
    Match("on", TK_IDENTIFIER);
    table_name();
    std::string tablename = TokenListTail[ActiveTokenList]->Lexem;
    Match("(", TK_OPEN_BRACE);

    // clear the current image attribute list and the current particulate attribute list
    CIAList->clear();
    // clear the current particulate attribute list and the current particulate attribute list
    CPAList->clear();
    // clear the current audio attribute list and the current particulate attribute list
    CAAList->clear();
    CurrentColDataType = "";

    complex_column_reference();
    Match(")", TK_CLOSE_BRACE);
    // optional tokens references '('param_assoc_list')'
    if (CurrentColDataType.compare("particulate") == 0)
    {
        Match("references", TK_IDENTIFIER);
        Match("(", TK_OPEN_BRACE);

        ParticleColName->clear();
        ParticleColType->clear();
        ParticulateParam->clear();

        param_assoc_ref_list();
        Match(")", TK_CLOSE_BRACE);
    }
    Match("using", TK_IDENTIFIER);
    metric_name_reference();

    if (errors == 0)
    {

        // the problem of starting a transaction is that in the following RunSQL
        // an alter table is executed, thus commits the transaction (a DDL command
        // in oracle commits the current transaction)

        //datadictionary->StartTransaction();

        // process the inserts in the data dictionary
        for (unsigned int i = 0; i < CommandList->size(); i++)
        {
            datadictionary->RunSQL(CommandList->at(i));
        }

        if (CurrentColDataType.compare("stillimage") == 0)
        {
            // build the index for an image attribute
            //datadictionary->BuildStillImageIndexFile(CIAList, CurrentMetric, tablename, CommandList);
        }
        else if (CurrentColDataType.compare("audio") == 0)
        {
            // build the index for an image attribute
            //datadictionary->BuildAudioIndexFile(CAAList, CurrentMetric, tablename, CommandList);
        }
        else if (CurrentColDataType.compare("particulate") == 0)
        {
            // todo  :-)
        }

        /*
        if (errors == 0) {
            datadictionary->CommitTransaction();
        }
        else {
            std::string indexfile;

            // delete the image index file created
            for (int i = 0; i < CIAList->size(); i++) {
                // gets the index file
                indexfile = datadictionary->GetIndexFile(tablename, CAAList->at(i), CurrentMetric);
                DeleteFile(indexfile);
            }

            // delete the audio index file created
            for (int i = 0; i < CAAList->size(); i++) {
                // gets the index file
                indexfile = datadictionary->GetIndexFile(tablename, CAAList->at(i), CurrentMetric);
                DeleteFile(indexfile);
            }

            // delete the particulate index file created
            for (int i = 0; i < CPAList->size(); i++) {
                // gets the index file
                indexfile = datadictionary->GetIndexFile(tablename, CPAList->at(i), CurrentMetric);
                DeleteFile(indexfile);
            }

            // rollback transaction
            datadictionary->RollbackTransaction();
        }
         */
    }
}

void Parser::index_name()
{

    if (Lookahead->TokenType == TK_IDENTIFIER)
        Match(Lookahead->Lexem, Lookahead->TokenType);
    else
        AddError("Identifier expected.");
}

void Parser::complex_column_reference()
{

    column_name();

    // auxiliary variables for the datadictionary->IsComplexAttributeReference()
    std::string tablename = TokenListTail[ActiveTokenList]->Prior->Prior->Lexem;
    std::string ctype = datadictionary->IsComplexAttributeReference(tablename, TokenListTail[ActiveTokenList]->Lexem);

    if (errors == 0)
    {
        // Verifies if the attribute is a complex attribute (stillimage, particulate or audio)
        if (ctype.compare("") != 0)
        {
            if (ctype.compare("stillimage") == 0)
                CIAList->push_back(TokenListTail[ActiveTokenList]->Lexem);
            else if (ctype.compare("particulate") == 0)
                CPAList->push_back(TokenListTail[ActiveTokenList]->Lexem);
            else if (ctype.compare("audio") == 0)
                CAAList->push_back(TokenListTail[ActiveTokenList]->Lexem);
            CurrentColDataType = ctype;
        }
        else
        {
            AddError("Invalid attribute name: '" + TokenListTail[ActiveTokenList]->Lexem + "'. Complex attribute expected.");
        }

    }
}

void Parser::param_assoc_ref_list()
{

    // param_assoc_ref_list: param_assoc_ref | param_assoc_ref ',' param_assoc_ref_list
    param_assoc_ref();
    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        param_assoc_ref_list();
    }
}

void Parser::param_assoc_ref()
{

    // param_assoc_ref: particle_column_ref as parameter_name

    // particle_colum_ref
    Match(Lookahead->Lexem, TK_IDENTIFIER);

    ParticleColName->push_back(TokenListTail[ActiveTokenList]->Lexem);
    std::string pcoltype = datadictionary->GetParticleType(TokenList[ActiveTokenList]->Next->Next->Next->Next->Lexem, TokenListTail[ActiveTokenList]->Lexem);

    if (pcoltype.compare("") == 0)
    {
        AddError("Invalid attribute reference: '" + TokenListTail[ActiveTokenList]->Lexem + "'");
    }
    else
        ParticleColType->push_back(pcoltype);

    // token as
    Match("as", TK_IDENTIFIER);
    // parameter_name
    Match(Lookahead->Lexem, TK_IDENTIFIER);

    ParticulateParam->push_back(TokenListTail[ActiveTokenList]->Lexem);
}

void Parser::metric_name_reference()
{

    metric_name();

    std::string tablename = "";
    // gets the table name
    tablename = TokenList[ActiveTokenList]->Next->Next->Next->Next->Lexem;

    if (errors == 0)
    {

        // Verifies if the metric exists
        MetricCode = datadictionary->IsMetric(TokenListTail[ActiveTokenList]->Lexem);
        if (MetricCode == 0)
            AddError("Invalid metric: " + TokenListTail[ActiveTokenList]->Lexem + ".");
        else
        {
            // auxilary variable that stores the name of the current metric
            std::string metricname = TokenListTail[ActiveTokenList]->Lexem;
            // auxilary variable that stores the type of the current metric
            std::string metrictype = datadictionary->GetMetricType(MetricCode);

            if (CurrentColDataType.compare("stillimage") == 0)
            {

                if (metrictype.compare("stillimage") != 0)
                {
                    AddError("Invalid metric type. The metric '" + metricname + "' isn't a valid metric for stillimage attributes.");
                }
                else
                {
                    // Verify if the association between the ImageAttribute and the Metric already exists
                    if (datadictionary->IsValidMetric(metricname, tablename, CIAList->at(0).c_str()) == 0)
                    {

                        CurrentMetric = metricname;
                        // getting all the metrics already associated with the image attribute
                        datadictionary->GetMetrics(tablename, CIAList->at(0), ComplexAttributeList, MetricReferenceList);

                        // Auxilary variable that stores 'true|false' for the isdefault parameter of the datadictionary->CreateTableInsertComplexAttribMetric method
                        bool isdefault = false;

                        // optional: default
                        if (Lookahead->Lexem.compare("default") == 0)
                        {
                            isdefault = true;
                        }

                        if (errors == 0)
                        {
                            // Sets "CDD$ComplexAttribMetric.IsDefault = false" for all the metrics of a specified image attribute
                            if (isdefault)
                                datadictionary->UpdateTableComplexAttribMetric(tablename, CIAList->at(0));

                            // Creates the sql statement for the insert (in the CDD$ComplexAttribMetric table) that join an image attribute with a metric
                            datadictionary->CreateTableInsertComplexAttribMetric(TokenList[ActiveTokenList], tablename, CommandList, CIAList->at(0), MetricCode, isdefault);

                            // Alters the IPV$ table. Adds one attribute for each extractor of the specified metric.
                            datadictionary->CreateTableAlterPVTable(CurrentColDataType, tablename, CommandList, CIAList->at(0), MetricCode, ComplexAttributeList, MetricReferenceList);
                        }
                    }
                    else
                        AddError("It is not allowed to duplicate the metric '" + TokenListTail[ActiveTokenList]->Lexem + "' for the attribute '" + CIAList->at(0) + "'.");
                }
            }
            else if (CurrentColDataType.compare("audio") == 0)
            {

                if (metrictype.compare("audio") != 0)
                {
                    AddError("Invalid metric type. The metric '" + metricname + "' isn't a valid metric for audio attributes.");
                }
                else
                {
                    // Verify if the association between the AudioAttribute and the Metric already exists
                    if (datadictionary->IsValidMetric(metricname, tablename, CAAList->at(0).c_str()) == 0)
                    {

                        CurrentMetric = metricname;
                        // getting all the metrics already associated with the audio attribute
                        datadictionary->GetMetrics(tablename, CAAList->at(0), ComplexAttributeList, MetricReferenceList);

                        // Auxilary variable that stores 'true|false' for the isdefault parameter of the datadictionary->CreateTableInsertComplexAttribMetric method
                        bool isdefault = false;

                        // optional: default
                        if (Lookahead->Lexem.compare("default") == 0)
                        {
                            isdefault = true;
                        }

                        if (errors == 0)
                        {
                            // Sets "CDD$ComplexAttribMetric.IsDefault = false" for all the metrics of a specified audio attribute
                            if (isdefault)
                                datadictionary->UpdateTableComplexAttribMetric(tablename, CAAList->at(0));

                            // Creates the sql statement for the insert (in the CDD$ComplexAttribMetric table) that join an audio attribute with a metric
                            datadictionary->CreateTableInsertComplexAttribMetric(TokenList[ActiveTokenList], tablename, CommandList, CAAList->at(0), MetricCode, isdefault);

                            // Alters the APV$ table. Adds one attribute for each extractor of the specified metric.
                            datadictionary->CreateTableAlterPVTable(CurrentColDataType, tablename, CommandList, CAAList->at(0), MetricCode, ComplexAttributeList, MetricReferenceList);
                        }
                    }
                    else
                        AddError("It is not allowed to duplicate the metric '" + TokenListTail[ActiveTokenList]->Lexem + "' for the attribute '" + CAAList->at(0) + "'.");
                }
            }
            else if (CurrentColDataType.compare("particulate") == 0)
            {

                if (metrictype.compare("particulate") != 0)
                {
                    AddError("Invalid metric type. The metric '" + metricname + "' isn't a valid metric for particulate attributes.");
                }
                else
                {
                    // Verify if the association between the ParticulateAttribute and the Metric already exists
                    if (datadictionary->IsValidMetric(metricname, tablename, CPAList->at(0).c_str()) == 0)
                    {

                        // ParamList is a list that stores the pairs [parameter,type]
                        // of a metric created for a particulate attribute
                        std::vector<std::string> *ParamList = new std::vector<std::string > ();
                        datadictionary->GetParameters(MetricCode, ParamList);

                        // ParamList->size()/2 because this list stores pairs [parameter,type]
                        if (ParamList->size() / 2 == ParticleColName->size())
                        {
                            for (unsigned int j = 0; j < ParamList->size(); j += 2)
                            {

                                int ix = -1;
                                unsigned int pnumber = atoi(ParamList->at(j).c_str());
                                if ((ParticulateParam->size() != 0) && (pnumber <= ParticulateParam->size()))
                                    // get ParticulateParam position
                                    ix = atoi(ParticulateParam->at(pnumber).c_str());
                                if (ix == -1)
                                {
                                    AddError("Missing parameter: '" + ParamList->at(j) + "'");
                                }
                                else
                                {
                                    if (ParamList->at(j + 1) == ParticleColType->at(ix))
                                    {
                                        // insert into the data dictionary (CDD$MetricInstance). Parameters needed: tablename, complexattribname, metriccode, parametername, extractorcode, parameterinstance
                                        // The extractor code for the particulate data is 0
                                        datadictionary->CreateTableInsertMetricInstance(CommandList, tablename, CPAList->at(0), MetricCode, ParamList->at(j), 0, ParticleColName->at(ix));
                                    }
                                    else
                                    {
                                        AddError("Invalid parameter type. Expected '" + ParamList->at(j + 1) + "'. Found '" + ParticleColType->at(ix) + "'.");
                                    }
                                }
                            }
                            delete ParamList;
                        }
                        else
                        {
                            AddError("Missing or extra particulate metric parameter(s).");
                        }

                        // Auxilary variable that stores 'true|false' for the isdefault parameter of the datadictionary->CreateTableInsertComplexAttribMetric method
                        bool isdefault = false;

                        // optional: default
                        if (Lookahead->Lexem.compare("default") == 0)
                        {


                            std::string cpa = CPAList->at(0).c_str();
                            std::vector<std::string>::iterator it;

                            it = std::find(DefaultMetricList->begin(), DefaultMetricList->end(), cpa);
                            if (it == DefaultMetricList->end())
                            {

                                // if the particulate attribute is not in the default metric list
                                DefaultMetricList->push_back(CPAList->at(0));
                                isdefault = true;
                            }
                            else
                                AddError("There are more than one default metric specified for the attribute: '" + CPAList->at(0) + "'.");
                        }

                        if (errors == 0)
                        {
                            // Creates the sql statement for the insert (in the CDD$ComplexAttribMetric table) that join a particulate attribute with a metric
                            datadictionary->CreateTableInsertComplexAttribMetric(TokenList[ActiveTokenList], tablename, CommandList, CPAList->at(0), MetricCode, isdefault);
                        }

                    }
                    else
                        AddError("It is not allowed to duplicate the metric '" + TokenListTail[ActiveTokenList]->Lexem + "' for the attribute '" + CPAList->at(0) + "'.");
                }
            }
        }
    }

    // optional: default
    if (Lookahead->TokenType == TK_IDENTIFIER)
        Match("default", TK_IDENTIFIER);

}

//===============================================================
// DROP INDEX STATEMENT
//===============================================================

void Parser::drop_index_statement()
{

    // drop_index_statement: drop index complex_index_name
    Match("index", TK_IDENTIFIER);
    complex_index_name();

    if (errors == 0)
    {
        // drops the image index
        datadictionary->DropComplexIndex(TokenListTail[ActiveTokenList]->Lexem);
    }
}

void Parser::complex_index_name()
{

    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        Match(Lookahead->Lexem, Lookahead->TokenType);
        if (!datadictionary->IsComplexAttribIndex(TokenListTail[ActiveTokenList]->Lexem))
            AddError("The index '" + TokenListTail[ActiveTokenList]->Lexem + "' doesn't exist or it is not an index built for a complex attribute.");
    }
    else
        AddError("Identifier expected.");
}

//===============================================================
// SELECT STATEMENT
//===============================================================

void Parser::query_specification()
{

    // Auxiliary variable that stores an error message
    std::string errmsg = "";

    // SELECT [DISTINCT | ALL] select_list table_expression
    Match("select", TK_IDENTIFIER);
    // optional quantifier
    if ((Lookahead->Lexem.compare("distinct") == 0) || (Lookahead->Lexem.compare("all") == 0))
        Match(Lookahead->Lexem, TK_IDENTIFIER);
    select_list();
    table_expression();

    if (errors == 0)
    {
        // replaces the "*" if there are stillimage attributes and includes respective join conditions
        bool checkasterisk = datadictionary->SelectAsteriskStatement(TokenList[ActiveTokenList], ActiveTokenList);

        // replaces the image id column references of the select list for the appropriate image column references
        if (!checkasterisk)
        {
            errmsg = datadictionary->SelectColumnRefListStatement(TokenList[ActiveTokenList], ActiveTokenList);
            if (errmsg.compare("") != 0)
                AddError(errmsg);
        }
    }
}

void Parser::select_list()
{

    // select_list ::= '*' | column_reference_list
    if (Lookahead->Lexem.compare("*") == 0)
        Match("*", TK_MUL);
    else if (Lookahead->TokenType == TK_IDENTIFIER)
        column_reference_list();
    else
        AddError("Invalid column reference.");
}

void Parser::column_reference_list()
{

    // column_reference_list ::= column_reference [ as_clause ] | column_reference [ as_clause ] ',' column_reference_list
    column_reference();

    // optional as_clause
    if (Lookahead->Lexem.compare("as") == 0)
    {
        as_clause();
    }

    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        if (Lookahead->TokenType == TK_IDENTIFIER)
            column_reference_list();
        else
            AddError("Identifier expected.");
    }
}

void Parser::column_reference()
{

    // supporting grouping functions
    if ((Lookahead->Lexem.compare("size()") == 0) ||
            (Lookahead->Lexem.compare("min") == 0) ||
            (Lookahead->Lexem.compare("max") == 0) ||
            (Lookahead->Lexem.compare("sum") == 0) ||
            (Lookahead->Lexem.compare("avg") == 0))
    {
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        Match(Lookahead->Lexem, TK_OPEN_BRACE);
        Match(Lookahead->Lexem, Lookahead->TokenType);
        Match(Lookahead->Lexem, TK_CLOSE_BRACE);
    }
    else
    {
        // column_reference ::= column_name['.'extractor_name'.'extractor_parameter]
        //                            | table_name'.'column_name['.'extractor_name'.'extractor_parameter]
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        // if the token is a TK_PERIOD then the next token must be a identifier
        if (Lookahead->TokenType == TK_PERIOD)
        {
            Match(".", TK_PERIOD);
            if (Lookahead->TokenType == TK_IDENTIFIER)
                column_reference();
            else
                AddError("Identifier expected.");
        }
    }
}

void Parser::as_clause()
{

    // as_clause ::= AS identifier
    Match("as", TK_IDENTIFIER);
    if (Lookahead->TokenType == TK_IDENTIFIER)
        Match(Lookahead->Lexem, TK_IDENTIFIER);
    else
        AddError("Identifier expected.");
}

void Parser::table_expression()
{

    // table_expression ::= from_clause | where_clause | group_by_clause | having_clause
    if (Lookahead->Lexem.compare("from") == 0)
    {
        from_clause();

        // optional where clause
        if ((Lookahead->TokenType == TK_IDENTIFIER) && (Lookahead->Lexem.compare("where") == 0))
            where_clause();

        // optional group by clause
        if ((Lookahead->TokenType == TK_IDENTIFIER) && (Lookahead->Lexem.compare("group") == 0))
            groupby_clause();

        // optional having clause
        if ((Lookahead->TokenType == TK_IDENTIFIER) && (Lookahead->Lexem.compare("having") == 0))
            having_clause();

        // if its not any of above, then its an error
        if ((Lookahead->TokenType == TK_IDENTIFIER) && (Lookahead->Lexem.compare("where") != 0)
                && (Lookahead->Lexem.compare("group") != 0)
                && (Lookahead->Lexem.compare("having") != 0))
            AddError("Invalid clause: \"" + Lookahead->Lexem + "\".");
    }
    else
        AddError("From clause expected.");
}

void Parser::from_clause()
{

    //from_clause ::= FROM table_reference_list
    Match("from", TK_IDENTIFIER);
    if (Lookahead->TokenType == TK_IDENTIFIER)
        table_reference_list();
    else
        AddError("Identifier expected.");
}

void Parser::table_reference_list()
{

    // table_reference_list ::= table_name | table_name ',' table_name_list
    Lookahead->LexemType = LK_TABLE_NAME;
    Match(Lookahead->Lexem, TK_IDENTIFIER);
    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        if (Lookahead->TokenType == TK_IDENTIFIER)
            table_reference_list();
        else
            AddError("Identifier expected.");
    }
}

void Parser::where_clause()
{

    // where_clause ::= WHERE search_condition
    Match("where", TK_IDENTIFIER);
    if (Lookahead->TokenType == TK_IDENTIFIER)
        search_condition();
    else
        AddError("Identifier expected.");

}

void Parser::groupby_clause()
{

    // groupby_clause ::= GROUP BY column_reference_list()
    Match("group", TK_IDENTIFIER);
    Match("by", TK_IDENTIFIER);
    if (Lookahead->TokenType == TK_IDENTIFIER)
        column_reference_list();
    else
        AddError("Identifier expected.");

}

void Parser::having_clause()
{

    // having_clause ::= HAVING search_condition
    Match("having", TK_IDENTIFIER);
    if (Lookahead->TokenType == TK_IDENTIFIER)
        search_condition();
    else
        AddError("Identifier expected.");

}

void Parser::search_condition()
{

    // search_condition ::= NOT search_condition | predicate | search_condition AND search_condition
    //                      | search_condition OR search_condition

    if (Lookahead->Lexem.compare("not") == 0)
    {
        Match("not", TK_IDENTIFIER);
        if (Lookahead->TokenType == TK_IDENTIFIER)
            search_condition();
        else
            AddError("Identifier expected.");
    }

    predicate();

    if ((Lookahead->Lexem.compare("and") == 0) || (Lookahead->Lexem.compare("or") == 0))
    {
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        if (Lookahead->TokenType == TK_IDENTIFIER)
            search_condition();
        else
            AddError("Predicate expected.");
    }
}

void Parser::predicate()
{

    // predicate ::= comparison_predicate | between_predicate | like_predicate
    //                  | test_for_null | in_predicate | all_or_any_predicate
    //                  | existence_test | near_predicate | far_predicate
    // notes: 1) the operators comparison_predicate, between_predicate, like_predicate,
    //       test_for_null, in_predicate, all_or_any_predicate and existence_test
    //       only can be used with numeric and string attributes
    //       2) the operators near_predicate and far_predicate only can be used with
    //       complex attributes (stillimage or particulate)

    column_reference();

    if (errors == 0)
    {

        // verify if the column reference is a valid complex column reference
        bool check_attr_ref = datadictionary->IsComplexAttributeReference(TokenListTail[ActiveTokenList]->Lexem);
        // table name reference
        std::string tname = "";
        bool tname_check = false;
        if (check_attr_ref)
        {

            std::vector<std::string> * tbnamelist = new std::vector<std::string > ();
            tbnamelist = datadictionary->GetTableNameFromTokenList(TokenList[ActiveTokenList], TokenListTail[ActiveTokenList]->Lexem);
            if (tbnamelist->size() == 0)
                AddError("Invalid complex attribute: " + TokenListTail[ActiveTokenList]->Lexem + ".");
            else
            {
                for (unsigned int i = 0; i < tbnamelist->size(); i++)
                {
                    tname = tbnamelist->at(i);
                    if (TokenListTail[ActiveTokenList]->Prior->TokenType == TK_PERIOD)
                    {
                        if (TokenListTail[ActiveTokenList]->Prior->Prior->Lexem == tname)
                            tname_check = true;
                    }
                    else
                        tname_check = true;
                }
                if (!tname_check)
                    AddError("Invalid complex attribute: " + tname + "." + TokenListTail[ActiveTokenList]->Lexem + ".");
            }
        }

        //.............................
        // comparison_predicate ::= column_reference comp_op column_reference
        //                          | column_reference comp_op value_expression
        //.............................

        // equals operator
        if (Lookahead->Lexem.compare("=") == 0)
        {
            if (!check_attr_ref)
            {
                Match("=", TK_EQUAL);
                if (Lookahead->TokenType == TK_IDENTIFIER)
                    column_reference();
                else
                    value_expression();
            }
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" can not be used with a complex attribute.");

        }
            // not equals operator
        else if (Lookahead->Lexem.compare("!=") == 0)
        {
            if (!check_attr_ref)
            {
                Match("!=", TK_NE);
                if (Lookahead->TokenType == TK_IDENTIFIER)
                    column_reference();
                else
                    value_expression();
            }
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" can not be used with a complex attribute.");
        }
            // like operator
        else if (Lookahead->Lexem.compare("like") == 0)
        {
            if (!check_attr_ref)
            {
                Match("like", TK_IDENTIFIER);
                value_expression();
            }
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" can not be used with a complex attribute.");
        }
            // not equals operator
        else if (Lookahead->Lexem.compare("<>") == 0)
        {
            if (!check_attr_ref)
            {
                Match("<>", TK_N2);
                if (Lookahead->TokenType == TK_IDENTIFIER)
                    column_reference();
                else
                    value_expression();
            }
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" can not be used with a complex attribute.");
        }
            // less than operator
        else if (Lookahead->Lexem.compare("<") == 0)
        {
            if (!check_attr_ref)
            {
                Match("<", TK_LQ);
                if (Lookahead->TokenType == TK_IDENTIFIER)
                    column_reference();
                else
                    value_expression();
            }
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" can not be used with a complex attribute.");
        }
            // less than or equals operator
        else if (Lookahead->Lexem.compare("<=") == 0)
        {
            if (!check_attr_ref)
            {
                Match("<=", TK_LE);
                if (Lookahead->TokenType == TK_IDENTIFIER)
                    column_reference();
                else
                    value_expression();
            }
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" can not be used with a complex attribute.");
        }
            // greater than operator
        else if (Lookahead->Lexem.compare(">") == 0)
        {
            if (!check_attr_ref)
            {
                Match(">", TK_GQ);
                if (Lookahead->TokenType == TK_IDENTIFIER)
                    column_reference();
                else
                    value_expression();
            }
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" can not be used with a complex attribute.");
        }
            // greater than or equals operator
        else if (Lookahead->Lexem.compare(">=") == 0)
        {
            if (!check_attr_ref)
            {
                Match(">=", TK_GE);
                if (Lookahead->TokenType == TK_IDENTIFIER)
                    column_reference();
                else
                    value_expression();
            }
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" can not be used with a complex attribute.");
        }

            //.............................
            // near_predicate
            //.............................

        else if (Lookahead->Lexem.compare("near") == 0)
            if (check_attr_ref)
                near_predicate();
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" can only be used with a complex attribute.");

            //.............................
            // far_predicate
            //.............................

        else if (Lookahead->Lexem.compare("far") == 0)
            if (check_attr_ref)
                far_predicate();
            else
                AddError("The operator \"" + Lookahead->Lexem + "\" only can be used with a complex attribute.");

            //.............................
            // predicate not implemented!
            //.............................

        else if (Lookahead->Lexem.compare("") == 0)
            AddError("Operator expected.");
        else
            AddError("Invalid operator \"" + Lookahead->Lexem + "\".");
    }
}

void Parser::value_expression()
{

    // a valid value_expression can be: a string, an integer or a floating point
    if (Lookahead->TokenType == TK_STRING)
        Match(Lookahead->Lexem, TK_STRING);
    else if (Lookahead->TokenType == TK_INTEGER)
        Match(Lookahead->Lexem, TK_INTEGER);
    else if (Lookahead->TokenType == TK_FLOATING_POINT)
        Match(Lookahead->Lexem, TK_FLOATING_POINT);
    else
        AddError("Invalid value expression.");
}

void Parser::near_predicate()
{

    //near_predicate ::= img_column_reference NEAR [similarity_grouping] image_definition
    //                      [BY metric_name]
    //                      [RANGE val_range]
    //                      [STOP AFTER k_nearest [WITH TIE LIST]]

    Match("near", TK_IDENTIFIER);

    // optional similarity grouping
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        if (Lookahead->Lexem.compare("sum") == 0)
        {
            Lookahead->LexemType = LK_SUM_SG;
            Match("sum", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("avg") == 0)
        {
            Lookahead->LexemType = LK_AVG_SG;
            Match("avg", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("all") == 0)
        {
            Lookahead->LexemType = LK_ALL_SG;
            Match("all", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("every") == 0)
        {
            Lookahead->LexemType = LK_EVERY_SG;
            Match("every", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("any") == 0)
        {
            Lookahead->LexemType = LK_ANY_SG;
            Match("any", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("max") == 0)
        {
            Lookahead->LexemType = LK_MAX_SG;
            Match("max", TK_IDENTIFIER);
        }
    }

    query_obj_definition();

    // optional BY clause
    if (Lookahead->Lexem.compare("by") == 0)
    {
        Match("by", TK_IDENTIFIER);
        metric_name();
    }
    // optional RANGE clause
    if (Lookahead->Lexem.compare("range") == 0)
    {
        Match("range", TK_IDENTIFIER);
        val_range();
    }
    // optional STOP AFTER clause
    if (Lookahead->Lexem.compare("stop") == 0)
    {
        Match("stop", TK_IDENTIFIER);
        Match("after", TK_IDENTIFIER);
        k_nearest();
        // optional WITH clause
        if (Lookahead->Lexem.compare("with") == 0)
        {
            Match("with", TK_IDENTIFIER);
            tie_list();
        }
    }
}

void Parser::query_obj_definition()
{

    // query_obj_definition ::= complex_attribute_reference | subquery | particulate_reference | image_location | audio_location

    // complex_attribute_reference
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {

        column_reference();

        bool check_attr_ref = datadictionary->IsComplexAttributeReference(TokenListTail[ActiveTokenList]->Lexem);
        // table name reference
        std::string tname = "";
        bool tname_check = false;
        if (check_attr_ref)
        {
            std::vector<std::string> *tbnamelist = new std::vector<std::string > ();
            tbnamelist = datadictionary->GetTableNameFromTokenList(TokenList[ActiveTokenList], TokenListTail[ActiveTokenList]->Lexem);
            if (tbnamelist->size() == 0)
                AddError("Invalid complex attribute: " + TokenListTail[ActiveTokenList]->Lexem + ".");
            else
            {
                for (unsigned int i = 0; i < tbnamelist->size(); i++)
                {
                    tname = tbnamelist->at(i);
                    if (TokenListTail[ActiveTokenList]->Prior->TokenType == TK_PERIOD)
                    {
                        if (TokenListTail[ActiveTokenList]->Prior->Prior->Lexem == tname)
                            tname_check = true;
                    }
                    else
                        tname_check = true;
                }
                if (!tname_check)
                    AddError("Invalid complex attribute: " + tname + "." + TokenListTail[ActiveTokenList]->Lexem + ".");
            }
        }
    }
        // subquery ::= '(' query_specification ')' or particulate_reference ::=  '(' parameter_val_assoc_list ')'
    else if (Lookahead->TokenType == TK_OPEN_BRACE)
    {

        Match("(", TK_OPEN_BRACE);

        if (Lookahead->Lexem.compare("select") == 0)
        {

            int LocalActiveTokenList = ActiveTokenList;

            TokenListCount++;

            Token *aux = new Token();
            aux->Lexem = "subselect";
            aux->ScopePosition = TokenListCount;
            AddToken(aux);

            ActiveTokenList = TokenListCount;

            query_specification();

            ActiveTokenList = LocalActiveTokenList;
        }
        else // particulate_reference ::=  '(' parameter_val_assoc_list ')'
        {
            parameter_val_assoc_list();
        }
        Match(")", TK_CLOSE_BRACE);
    }
        // image_location ::= 'TK_STRING' or audio_location ::= 'TK_STRING'
    else if (Lookahead->TokenType == TK_STRING)
    {
        std::string filename = Lookahead->Lexem.substr(1, Lookahead->Lexem.length() - 1);
        ofstream ifile(filename.c_str());
        if (ifile)
        {
            Match(Lookahead->Lexem, TK_STRING);
        }
        else
        {
            Match(Lookahead->Lexem, TK_STRING);
            AddError("File " + filename + " not found.");
        }
    }
    else
        AddError("Invalid complex object definition.");
}

void Parser::parameter_val_assoc_list()
{

    // parameter_val_assoc_list: parameter_val_assoc | parameter_val_assoc ',' parameter_val_assoc_list
    parameter_val_assoc();
    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        parameter_val_assoc_list();
    }
}

void Parser::parameter_val_assoc()
{

    // parameter_val_assoc: particle_column_val_ref as parameter_name

    // particle_colum_val_ref (Only integer and floating point values are allowed)
    if (Lookahead->TokenType == TK_INTEGER)
        Match(Lookahead->Lexem, TK_INTEGER);
    else if (Lookahead->TokenType == TK_FLOATING_POINT)
        Match(Lookahead->Lexem, TK_FLOATING_POINT);
    else
        AddError("Invalid parameter type.");

    // token as
    Match("as", TK_IDENTIFIER);

    // parameter_name
    Match(Lookahead->Lexem, TK_IDENTIFIER);

}

void Parser::val_range()
{

    // val_range ::= TK_FLOATING_POINT | TK_INTEGER
    if ((Lookahead->TokenType == TK_INTEGER) || (Lookahead->TokenType == TK_FLOATING_POINT))
        Match(Lookahead->Lexem, Lookahead->TokenType);
    else
        AddError("Value expected.");
}

void Parser::k_nearest()
{

    // k_nearest::= TK_INTEGER
    if (Lookahead->TokenType == TK_INTEGER)
        Match(Lookahead->Lexem, TK_INTEGER);
    else
        AddError("Integer expected.");
}

void Parser::tie_list()
{

    // tie_list ::= TIE LIST
    Match("tie", TK_IDENTIFIER);
    Match("list", TK_IDENTIFIER);
}

void Parser::far_predicate()
{

    // far_predicate ::= img_column_reference FAR [similarity_grouping] image_definition
    //                      [BY metric_name]
    //                      [RANGE val_range]
    //                      [STOP AFTER k_nearest [WITH tie_list]]

    Match("far", TK_IDENTIFIER);

    // optional similarity grouping
    if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        if (Lookahead->Lexem.compare("sum") == 0)
        {
            Lookahead->LexemType = LK_SUM_SG;
            Match("sum", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("avg") == 0)
        {
            Lookahead->LexemType = LK_AVG_SG;
            Match("avg", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("all") == 0)
        {
            Lookahead->LexemType = LK_ALL_SG;
            Match("all", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("every") == 0)
        {
            Lookahead->LexemType = LK_EVERY_SG;
            Match("every", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("any") == 0)
        {
            Lookahead->LexemType = LK_ANY_SG;
            Match("any", TK_IDENTIFIER);
        }
        else if (Lookahead->Lexem.compare("max") == 0)
        {
            Lookahead->LexemType = LK_MAX_SG;
            Match("max", TK_IDENTIFIER);
        }
    }

    query_obj_definition();

    // optional BY clause
    if (Lookahead->Lexem.compare("by") == 0)
    {
        Match("by", TK_IDENTIFIER);
        metric_name();
    }
    // optional RANGE clause
    if (Lookahead->Lexem.compare("range") == 0)
    {
        Match("range", TK_IDENTIFIER);
        val_range();
    }
    // optional STOP AFTER clause
    if (Lookahead->Lexem.compare("stop") == 0)
    {
        Match("stop", TK_IDENTIFIER);
        Match("after", TK_IDENTIFIER);
        k_nearest();
        // optional WITH clause
        if (Lookahead->Lexem.compare("with") == 0)
        {
            Match("with", TK_IDENTIFIER);
            tie_list();
        }
    }
}

//===============================================================
// INSERT STATEMENT
//===============================================================

void Parser::insert_statement()
{

    // Auxiliary list
    std::vector<std::string> * PAttribList = new std::vector<std::string > ();

    // clears the auxiliary lists
    ImgAttribList->clear();
    AudAttribList->clear();
    InsertColList->clear();
    InsertValueList->clear();
    ImgAttribPosList->clear();
    AudAttribPosList->clear();
    ImgInsertAuxList->clear();
    AudInsertAuxList->clear();
    AttribPosition = 0;

    // insert_statement: insert into table_name ['(' insert_column_list ')'] values '(' insert_value_list ')'

    Match("insert", TK_IDENTIFIER);

    IsInsert = true;

    Match("into", TK_IDENTIFIER);

    // table name
    std::string tablename = Lookahead->Lexem;
    Match(Lookahead->Lexem, TK_IDENTIFIER);

    //getting the stillimage attributes.
    ImgAttribList = datadictionary->GetStillimageAttribute(TokenListTail[ActiveTokenList]->Lexem);

    //getting the audio attributes.
    AudAttribList = datadictionary->GetAudioAttribute(TokenListTail[ActiveTokenList]->Lexem);

    //getting the particulate attributes.
    PAttribList = datadictionary->GetParticulateAttribute(TokenListTail[ActiveTokenList]->Lexem);

    // optional insert column list
    if (Lookahead->TokenType == TK_OPEN_BRACE)
    {
        Match("(", TK_OPEN_BRACE);
        insert_column_list();

        // inserting the particulate attributes
        for (unsigned int i = 0; i < PAttribList->size(); i++)
        {
            // adding the ','
            Token *tk = new Token();
            tk->Lexem = ",";
            tk->TokenType = TK_COMMA;
            AddToken(tk);

            // adding the particulate attribute
            tk = new Token();
            tk->Lexem = PAttribList->at(i);
            tk->TokenType = TK_IDENTIFIER;
            AddToken(tk);
        }

        Match(")", TK_CLOSE_BRACE);
    }
    else //gets all the attributes of table name
    {
        if (PAttribList->size() > 0)
            AddError("Particulate attributes are system controlled. It is not allowed to assign values to them.");
        else
            InsertColList = datadictionary->GetAttributes(TokenListTail[ActiveTokenList]->Lexem);
    }

    if (errors == 0)
    {


        // verifies the position of the complex attributes in the insert column list
        for (unsigned int i = 0; i < InsertColList->size(); i++)
        {
            std::stringstream out;
            // stillimage

            std::string icol = InsertColList->at(i).c_str();
            std::vector<std::string>::iterator itimg;
            std::vector<std::string>::iterator itaudio;
            std::vector<std::string>::iterator itparticulate;
            itimg = std::find(ImgAttribList->begin(), ImgAttribList->end(), icol);
            itaudio = std::find(AudAttribList->begin(), AudAttribList->end(), icol);
            itparticulate = std::find(PAttribList->begin(), PAttribList->end(), icol);

            if (itimg != ImgAttribList->end())
            {
                out << i;
                ImgAttribPosList->push_back(out.str());
            }
            else if (itaudio != AudAttribList->end())
            {
                out << i;
                AudAttribPosList->push_back(out.str());
            }
            else if (itparticulate != PAttribList->end())
            {
                AddError("The particulate attribute '" + InsertColList->at(i) + "' is system controlled.");
            }

        }

        if (errors == 0)
        {

            // inserting the particulate attributes
            for (unsigned int i = 0; i < PAttribList->size(); i++)
                InsertColList->push_back(PAttribList->at(i));

            Match("values", TK_IDENTIFIER);

            Match("(", TK_OPEN_BRACE);
            insert_value_list();

            // assigning values to the particulate attributes
            int partid = -1;
            if (PAttribList->size() > 0)
                partid = datadictionary->RunSequence("ParticulateIdSequence");
            for (unsigned int i = 0; i < PAttribList->size(); i++)
            {
                std::stringstream out;
                // adding the ','
                Token *tk = new Token();
                tk->Lexem = ",";
                tk->TokenType = TK_COMMA;
                AddToken(tk);

                // adding the particulate attribute
                tk = new Token();
                out << partid;
                tk->Lexem = out.str();
                tk->TokenType = TK_INTEGER;
                AddToken(tk);
            }

            Match(")", TK_CLOSE_BRACE);

            // check if the insert can be run:
            // - verify if all attributes needed to build the object were informed
            std::string neededattribute = datadictionary->CheckInsertParticles(tablename, InsertColList);
            if (neededattribute.compare("") != 0)
            {
                AddError("The attribute '" + neededattribute + "' needs to be present in the insert statement since there is a particulate referencing it.");
            }

            if (errors == 0)
            {
                //datadictionary->StartTransaction();



                // Run the insert statement
                CommandList->push_back(datadictionary->RunSQL(TokenList[ActiveTokenList]));
                if (errors == 0)
                {

                    if (PAttribList->size() > 0)
                    {
                        // insert each object in its related metric trees
                        datadictionary->InsertParticulate(tablename, partid, InsertColList, InsertValueList, CommandList);
                    }

                    // Auxiliary variables used int the datadictionary->InsertImageIPV() procedure
                    std::string tab_name = "";
                    std::string attrib_name = "";
                    std::string f_name = "";
                    long id = 0;

                    // Execute the other insert statements for stillimage
                    unsigned int i = 0;
                    while (i < ImgInsertAuxList->size())
                    {

                        tab_name = ImgInsertAuxList->at(i);
                        i += 1;
                        attrib_name = ImgInsertAuxList->at(i);
                        i += 1;
                        f_name = ImgInsertAuxList->at(i);
                        i += 1;
                        id = atoi((ImgInsertAuxList->at(i)).c_str());


                        struct stat BUF;


                        ofstream ifile(f_name.c_str());

                        if (stat(f_name.c_str(), &BUF) == 0)
                            datadictionary->InsertImageIPV(tab_name, attrib_name, f_name, id, CommandList);
                        else
                            AddError("File not found: " + f_name + ".");

                        i += 1;
                    }

                    // Execute the other insert statements for audio
                    i = 0;
                    while (i < AudInsertAuxList->size())
                    {

                        tab_name = AudInsertAuxList->at(i);
                        i += 1;
                        attrib_name = AudInsertAuxList->at(i);
                        i += 1;
                        f_name = AudInsertAuxList->at(i);
                        i += 1;
                        id = atoi((AudInsertAuxList->at(i)).c_str());

                        ofstream ifile2(f_name.c_str());
                        if (ifile2)
                            //datadictionary->InsertAudioAPV(tab_name, attrib_name, f_name, id, CommandList);
                            AddError("not done yet: " + f_name + ".");
                        else
                            AddError("File not found: " + f_name + ".");

                        i += 1;
                    }
                }

                if (errors == 0)
                    datadictionary->CommitTransaction();
                else
                    datadictionary->RollbackTransaction();
            }
        }
    }

    delete PAttribList;
}

void Parser::insert_column_list()
{

    insert_column();
    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        insert_column_list();
    }
}

void Parser::insert_column()
{

    Match(Lookahead->Lexem, TK_IDENTIFIER);
    InsertColList->push_back(TokenListTail[ActiveTokenList]->Lexem);
}

void Parser::insert_value_list()
{

    insert_value();

    // auxiliary variable that stores the position of the current value in the in
    AttribPosition += 1;

    if (Lookahead->TokenType == TK_COMMA)
    {
        Match(",", TK_COMMA);
        insert_value_list();
    }
}

void Parser::insert_value()
{

    std::stringstream out, out2;
    long id = 0;
    std::string file = "";

    // verifies if the insert value is an image
    out2 << AttribPosition;
    std::vector<std::string>::iterator itimg;
    std::vector<std::string>::iterator itaudio;
    itimg = std::find(ImgAttribPosList->begin(), ImgAttribPosList->end(), out2.str());
    itaudio = std::find(AudAttribPosList->begin(), AudAttribPosList->end(), out2.str());
    if (itimg != ImgAttribPosList->end())
    {

        // replaces the image for an image id and inserts the image in the appropriate IPV$ table
        file = Lookahead->Lexem;
        file = file.substr(1, file.length() - 2);
        id = datadictionary->RunSequence("ImageIdSequence");

        // parameters stored in the InsertAuxList: table name, stillimage attribute name, image file, image id
        ImgInsertAuxList->push_back(TokenList[ActiveTokenList]->Next->Next->Lexem); // table name
        ImgInsertAuxList->push_back(InsertColList->at(AttribPosition)); // stillimage attribute name
        ImgInsertAuxList->push_back(file); // image file
        out << id;
        ImgInsertAuxList->push_back(out.str()); // image id

        std::stringstream out;
        out << id;
        Lookahead->Lexem = out.str();
        Lookahead->TokenType = TK_INTEGER;
        InsertValueList->push_back(Lookahead->Lexem);
        Match(Lookahead->Lexem, Lookahead->TokenType);
    }
        // verifies if the insert value is an audio
    else if (itaudio != AudAttribList->end())
    {

        // replaces the audio for an audio id and inserts the audio in the appropriate APV$ table
        file = Lookahead->Lexem;
        file = file.substr(2, file.length() - 2);
        id = datadictionary->RunSequence("AudioIdSequence");

        // parameters stored in the InsertAuxList: table name, audio attribute name, audio file, audio id
        AudInsertAuxList->push_back(TokenList[ActiveTokenList]->Next->Next->Lexem); // table name
        AudInsertAuxList->push_back(InsertColList->at(AttribPosition)); // audio attribute name
        AudInsertAuxList->push_back(file); // audio file
        std::stringstream out;
        out << id;
        AudInsertAuxList->push_back(out.str()); // audio id

        Lookahead->Lexem = out.str();
        Lookahead->TokenType = TK_INTEGER;
        InsertValueList->push_back(Lookahead->Lexem);
        Match(Lookahead->Lexem, Lookahead->TokenType);
    }
    else
    {
        InsertValueList->push_back(Lookahead->Lexem);
        Match(Lookahead->Lexem, Lookahead->TokenType);
    }

}

//===============================================================
// OTHER STATEMENTS
//===============================================================

void Parser::describe_specification()
{

    // describe_specification ::= DESCRIBE METRIC <metric_name> | COMPLEX INDEX <image_index_name> | <table_name>

    // describe | desc
    Match(Lookahead->Lexem, TK_IDENTIFIER);

    // describe a metric
    if (Lookahead->Lexem.compare("metric") == 0)
    {
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        std::string metricname = Lookahead->Lexem;
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        IsSelect = true;
        std::string sql;
        if (!datadictionary->IsParticulateMetric(metricname))
        {
            sql = "select ms.MetricName, ms.MetricType, ms.LpP, e.ExtractorName, ps.ParameterName, ps.ParameterAlias, ps.Weight ";
            sql = sql + "from CDD$MetricStruct ms, CDD$ParameterStruct ps, EPD$Extractors e ";
            sql = sql + "where ms.MetricCode = ps.MetricCode ";
            sql = sql + "and ps.ExtractorCode = e.ExtractorCode ";
            sql = sql + "and ms.metricname = '" + metricname + "'";
        }
        else
        {
            sql = "select ms.MetricName, ms.MetricType, ms.LpP, '' as ExtractorName, ps.ParameterName, ps.Weight ";
            sql = sql + "from CDD$MetricStruct ms, CDD$ParameterStruct ps ";
            sql = sql + "where ms.MetricCode = ps.MetricCode ";
            sql = sql + "and ms.metricname = '" + metricname + "'";
        }
        CommandList->push_back(sql);
    }
        // describe a image index
    else if (Lookahead->Lexem.compare("complex") == 0)
    {
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        if (Lookahead->Lexem.compare("index") == 0)
        {
            Match(Lookahead->Lexem, TK_IDENTIFIER);
            std::string indexname = Lookahead->Lexem;
            Match(Lookahead->Lexem, TK_IDENTIFIER);
            IsSelect = true;
            CommandList->push_back("select IndexName, MetricName from CDD$ComplexAttribMetric im, CDD$MetricStruct ms where im.MetricCode = ms.MetricCode and im.IndexName = '" + indexname + "'");
        }
    }
        // describe 'tablename': describe table
    else if (Lookahead->TokenType == TK_IDENTIFIER)
    {
        std::string tablename = Lookahead->Lexem;
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        IsSelect = true;

        //std::vector *tmp = datadictionary->GetStillimageAttribute(tablename);
        //if (tmp->size() == 0) {
        //    AddError("Object " + tablename + " is not a table having stillimage attributes.");
        //}
        //else {
        std::string sql = "";
        // the first part of this select selects the non-stillimage attributes
        sql += "select ac.column_name, ac.data_type ";
        sql += "from all_tab_columns ac, CDD$ComplexAttribMetric im ";
        sql += "where lower(ac.table_name) = im.tablename ";
        sql += "and lower(ac.column_name) != im.complexattribname ";
        sql += "and lower(table_name) = '" + tablename + "' ";
        // the second part of this select selects the stillimage attributes
        sql += "union ";
        sql += "select upper(im.complexattribname), upper(ms.metrictype) as DataType ";
        sql += "from all_tab_columns ac, CDD$ComplexAttribMetric im, cdd$metricstruct ms ";
        sql += "where lower(ac.table_name) = im.tablename ";
        sql += "and lower(ac.column_name) = im.complexattribname ";
        sql += "and ms.metriccode = im.metriccode ";
        sql += "and lower(table_name) = '" + tablename + "' ";

        CommandList->push_back(sql);
        //}
    }
        // error
    else
        AddError("Invalid DESCRIBE statement! Hint: DESCRIBE { METRIC metric_name | COMPLEX INDEX image_index_name | table_name }");

}

void Parser::show_specification()
{

    // show_specification ::= SHOW EXTRACTORS | METRICS | COMPLEX INDEXES

    // show
    Match(Lookahead->Lexem, TK_IDENTIFIER);

    // available extractors
    if (Lookahead->Lexem.compare("extractors") == 0)
    {
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        IsSelect = true;
        CommandList->push_back("select e.ExtractorName, p.ParameterName, p.IsIndexable from EPD$Extractors e, EPD$Parameters p where e.extractorcode = p.extractorcode");
    }
        // available metrics
    else if (Lookahead->Lexem.compare("metrics") == 0)
    {
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        IsSelect = true;
        std::string sql = "select ms.MetricName, ms.LpP, e.ExtractorName, 'stillimage' metrictype ";
        sql = sql + "from CDD$MetricStruct ms, CDD$ParameterStruct ps, EPD$Extractors e ";
        sql = sql + "where ms.MetricCode = ps.MetricCode ";
        sql = sql + "and ps.ExtractorCode = e.ExtractorCode ";
        sql = sql + "union ";
        sql = sql + "select ms.MetricName, ms.LpP, ' ' as temp, 'particulate' metrictype ";
        sql = sql + "from CDD$MetricStruct ms ";
        sql = sql + "where ms.metrictype = 'particulate' ";
        sql = sql + "order by metrictype";
        CommandList->push_back(sql);
    }
        // available image indexes
    else if (Lookahead->Lexem.compare("complex") == 0)
    {
        Match(Lookahead->Lexem, TK_IDENTIFIER);
        if (Lookahead->Lexem.compare("indexes") == 0)
        {
            Match(Lookahead->Lexem, TK_IDENTIFIER);
            IsSelect = true;
            CommandList->push_back("select IndexName from CDD$ComplexAttribMetric");
        }
    }
        // error
    else
        AddError("Invalid SHOW statement! Hint: SHOW { EXTRACTORS | METRICS | COMPLEX INDEXES }");
}

std::string Parser::ClientFolder()
{
    return datadictionary->GetClientFolder();
}

bool Parser::RightClientFolder()
{
    char currentpath[200];
    getcwd(currentpath, sizeof (currentpath));
    std::string dir = currentpath;
    if ((dir.compare(datadictionary->GetClientFolder()) == 0) || ((datadictionary->GetClientFolder()).compare("") == 0))
        return true;
    else
        return false;
}

/*
void Parser::upload_images() {
    // upload directory tablename keyname stillimagename

    // upload
    Match("upload", TK_IDENTIFIER);

    // directory
    std::string directory = "";
    if (Lookahead->TokenType == TK_STRING) {
        directory = Lookahead->Lexem;
        directory = directory.substr(2,directory.length()-2);
        Match(Lookahead->Lexem,TK_STRING);
    }
    else {
        AddError("Directory string expected.");
    }

    // tablename
    std::string tablename = "";
    if (Lookahead->TokenType == TK_IDENTIFIER) {
        tablename = Lookahead->Lexem;
        Match(Lookahead->Lexem,TK_IDENTIFIER);
    }
    else {
        AddError("Table name expected.");
    }

    // keyname
    std::string keyname = "";
    if (Lookahead->TokenType == TK_IDENTIFIER) {
        keyname = Lookahead->Lexem;
        Match(Lookahead->Lexem,TK_IDENTIFIER);
    }
    else {
        AddError("Key name expected.");
    }

    // stillimagename
    std::string stillimagename = "";
    if (Lookahead->TokenType == TK_IDENTIFIER) {
        stillimagename = Lookahead->Lexem;
        Match(Lookahead->Lexem,TK_IDENTIFIER);
    }
    else {
        AddError("Stillimage attribute name expected.");
    }

    if (errors > 0) {
        AddError("Sintax hint: upload directory tablename keyname stillimagename");
        return;
    }

    // check if stillimagename is really an stillimage attribute
    bool isstillimage = datadictionary->IsStillimageAttributeReference(tablename,stillimagename);
    if (!isstillimage) {
        AddError("Attribute " + stillimagename + " from table "+ tablename + " is not a stillimage attribute.");
        return;
    }

    // execute the inserts
    datadictionary->InsertFiles(directory, tablename, keyname, stillimagename, CommandList);
}
 */

//---------------------------------------------------------------------------

