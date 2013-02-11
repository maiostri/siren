/**
* @file
*
* This file implements lex analiser.
*
* @version 1.0
*
* @author Humberto Luiz Razente (hlr@icmc.usp.br)
* @author Maria Camila Nardini Barioni (mcamila@icmc.usp.br)
* @todo Documentation review and tests.
*/

//---------------------------------------------------------------------------

#ifndef __LEX_H
#define __LEX_H

#include <string>
#include <locale>
#include <iostream>
//---------------------------------------------------------------------------

// engine state
#define ST_BEGIN                 0
#define ST_END                   1
#define ST_ERROR                 2

// errors
#define TK_LEX_ERROR           -10
#define TK_UNTERMINATED_STRING -11
#define TK_FLOATINGPOINT_ERROR -12

// tokens
#define TK_UNDEFINED            -1
#define TK_IDENTIFIER          100
#define TK_INTEGER             101
#define TK_FLOATING_POINT      102
#define TK_STRING              103
#define TK_GQ                  110 // >
#define TK_GE                  111 // >=
#define TK_LQ                  112 // <
#define TK_NE                  113 // !=
#define TK_N2                  114 // <>
#define TK_LE                  115 // <=
#define TK_OPEN_BRACE          120 // (
#define TK_CLOSE_BRACE         121 // )
#define TK_OPEN_BLOCK          122 // {
#define TK_CLOSE_BLOCK         123 // }
#define TK_COMMA               124 // ,
#define TK_SEMICOLON           125 // ;
#define TK_PERIOD              126 // .
#define TK_EQUAL               127 // =
#define TK_SUM                 128 // +
#define TK_SUB                 129 // -
#define TK_MUL                 130 // *
#define TK_DIV                 131 // /
#define TK_PER                 132 // %

// lexems types
#define LK_UNDEFINED            -2
#define LK_COLUMN_NAME          200
#define LK_TABLE_NAME           201
#define LK_TABLE_NAME_REF       202
#define LK_STILLIMAGE_DATATYPE  203
#define LK_PARTICULATE_DATATYPE 204
#define LK_AUDIO_DATATYPE       205
#define LK_DELETE_CLAUSE        206
#define LK_SUM_SG               207
#define LK_AVG_SG               208
#define LK_ALL_SG               209
#define LK_EVERY_SG             210
#define LK_ANY_SG               211
#define LK_MAX_SG               212

/**
* This class implements the token!
*
* @author Humberto Luiz Razente (hlr@icmc.usp.br)
* @author Maria Camila Nardini Barioni (mcamila@icmc.usp.br)
*/
class Token
{
public:
    /**
    * Token constructor!!!
    */
    Token ()
    {
        Lexem = "";
        TokenType = TK_UNDEFINED;
        LexemType = LK_UNDEFINED;
        ScopePosition = -1;
        Prior = NULL;
        Next = NULL;
    }

    /**
    * Token destructor!!!
    */
    ~Token () {}

    /**
    * Clones the token.
    */
    Token *Clone()
    {
        Token *t = new Token();
        t->Lexem = this->Lexem;
        t->TokenType = this->TokenType;
        t->LexemType = this->LexemType;
        t->ScopePosition = this->ScopePosition;
        t->Prior = NULL;
        t->Next = NULL;
        return t;
    }

    /**
    * Lexem: string of the token from input string
    */

    std::string Lexem;

    /**
    * TokenType: defines the type of the token: TK_IDENTIFIER, TK_INTEGER, TK_STRING, etc
    */
    int TokenType;

    /**
    * LexemType: defines the type of the lexem: LK_COLUMN_NAME, LK_TABLE_NAME, etc
    */
    int LexemType;

    /**
    * ScopePosition: defines the position of the scope in the list of token lists
    */
    int ScopePosition;

    /**
    * Prior: pointer to the prior token
    */
    Token *Prior;

    /**
    * Next: pointer to the next token, to form a token list
    */
    Token *Next;
private:
};

/**
* This class implements the lex analiser!
*
* @author Humberto Luiz Razente (hlr@icmc.usp.br)
* @author Maria Camila Nardini Barioni (mcamila@icmc.usp.br)
*/
class Lex
{
public:
    /**
    * Lex constructor!!!
    */
    Lex ()
    {
        Position = 0;
        NullCount = 0;
    }

    /**
    * Lex destructor!!!
    */
    ~Lex () {}

    /**
    * GetToken returns the next token, including the lexem and the token type
    * @todo Implementar o ponto flutuante e 'strings'.
    */
    Token *GetToken ();

    /**
    * Returns the position of the state engine
    */
    int GetPosition()
    {
        return Position;
    }

    /**
    * Method used for the data input
    */
    void Input(std::string s);

private:
    /**
    * Position of the state engine
    */
    int Position;

    /**
    * Counts how many GetChar() calls returned 0 characters, indicating end of string
    */
    int NullCount;

    /**
    * Input text length
    */
    int Length;

    /**
    * State of the state engine: ST_BEGIN, ST_END or ST_ERROR
    */
    int state;

    /**
    * Input string
    */
    std::string input;

    /**
    * Method used on method GetToken() to pick next character
    */
    char GetChar();

    /**
    * Method used on method GetToken() to return to the input a character
    */
    void UnGetChar(Token *token);

    /**
    * Method used on method GetToken() to return to the input a character
    */
    void UnGetChar();

    /**
    * Method used on method GetToken() to process the integer number part
    */
    void integer_number_part(Token *token);

    /**
    * Method used on method GetToken() to process exponentials
    */
    void exponential(Token *token);
};

#endif
