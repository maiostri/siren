//---------------------------------------------------------------------------

#include "Lex.h"

//---------------------------------------------------------------------------

void Lex::Input(std::string s)
{
    input = s;
    Length = s.length();
}

Token *Lex::GetToken ()
{
    Token *token = new Token();

    char ch;
    state = ST_BEGIN;
    std::locale loc;
    do
        {
            ch = GetChar();
            // end of string
            if (Position > Length)
                {
                    state = ST_END;
                }
            // drop spaces
            else if (ch == ' ')
                {
                    state = ST_BEGIN;
                }
            // strings 'xyz'
            else if ((ch == '\''))
                {
                    token->Lexem = ch;
                    //string tmpchar;
                    do
                        {
                            ch = GetChar();
                            token->Lexem += ch;

                            /*
                            // to allow two apostrofes in the middle of a string
                            tmpchar = GetChar();
                            if ((ch == '\'') && (tmpchar == '\''))
                                ch = 'a'; // to avoid exiting the do...while
                            else
                                UnGetChar();
                            */
                        }
                    while ((ch != '\'') && (state != ST_ERROR));

                    if (state == ST_ERROR)
                        {
                            token->TokenType = TK_UNTERMINATED_STRING;
                        }
                    else
                        {
                            state = ST_END;
                            token->TokenType = TK_STRING;
                        }
                }
            // strings 'xyz'
            else if (ch == '\'')
                {
                    token->Lexem = ch;

                    do
                        {
                            ch = GetChar();
                            token->Lexem += ch;
                        }
                    while ((ch != '\'') && (state != ST_ERROR));

                    if (state == ST_ERROR)
                        {
                            token->TokenType = TK_UNTERMINATED_STRING;
                        }
                    else
                        {
                            state = ST_END;
                            token->TokenType = TK_STRING;
                        }
                }
            // identifier
            // [a-zA-Z][a-zA-z0-9$]*
            else if (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')) || (ch == '$'))
                {
                    UnGetChar(token);
                    do
                        {
                            ch = GetChar();
                            ch = std::tolower(ch,loc);
                            token->Lexem += ch;
                        }
                    while (
                        ((ch >= 'A') && (ch <= 'Z'))
                        || ((ch >= 'a') && (ch <= 'z'))
                        || ((ch >= '0') && (ch <= '9'))
                        || (ch == '_')
                        || (ch == '0')
                        || (ch == '$')
                    );
                    UnGetChar(token);
                    token->TokenType = TK_IDENTIFIER;
                    state = ST_END;
                }
            // numbers: integers and floating points
            // '+' and '-' operators
            // [-+]?[0-9]+('.'[0-9]*)?([eE]'-'?[0-9]*)?
            else if (((ch >= '0') && (ch <= '9')) || (ch == '+') || (ch == '-'))
                {

                    if ((ch == '+') || (ch == '-'))
                        {
                            if (ch == '+')
                                token->TokenType = TK_SUM; // +
                            if (ch == '-')
                                token->TokenType = TK_SUB; // -
                            token->Lexem += ch;
                            ch = GetChar();
                            token->Lexem += ch;
                            if (!((ch >= '0') && (ch <= '9')))
                                {
                                    UnGetChar(token);
                                    state = ST_END;
                                }
                        }

                    if (state != ST_END)
                        {
                            UnGetChar(token);

                            // integer part of the number
                            integer_number_part(token);

                            // put back the character and get it again
                            UnGetChar(token);
                            ch = GetChar();
                            token->Lexem += ch;

                            // check for an 'e' or a '.' after an integer number
                            if (ch == 'e')
                                {
                                    exponential(token);
                                }
                            // floating point
                            else if (ch == '.')
                                {
                                    // there must be at least a character between '0' and '9'
                                    ch = GetChar();
                                    token->Lexem += ch;
                                    if ((ch >= '0') && (ch <= '9'))
                                        {
                                            // after a '.' an integer is expected
                                            UnGetChar(token);
                                            integer_number_part(token);
                                            token->TokenType = TK_FLOATING_POINT;
                                        }
                                    else
                                        {
                                            token->TokenType = TK_FLOATINGPOINT_ERROR;
                                        }

                                    // check for an 'e' after a floating point number
                                    UnGetChar(token);
                                    ch = GetChar();
                                    token->Lexem += ch;
                                    if (ch == 'e')
                                        exponential(token);
                                }
                            // integer
                            else
                                {
                                    token->TokenType = TK_INTEGER;
                                }

                            state = ST_END;
                            UnGetChar(token);
                        }
                }
            // symbols > and >=
            else if (ch == '>')
                {
                    token->Lexem = ch;

                    ch = GetChar();
                    token->Lexem += ch;

                    if (ch == '=')
                        {
                            token->TokenType = TK_GE; // >=
                        }
                    else
                        {
                            UnGetChar(token);
                            token->TokenType = TK_GQ; // >
                        }

                    state = ST_END;
                }
            // symbols < and <= and <>
            else if ((ch == '<'))
                {
                    token->Lexem = ch;

                    ch = GetChar();
                    token->Lexem += ch;

                    if (ch == '=')
                        {
                            token->TokenType = TK_LE; // <=
                        }
                    else if (ch == '>')
                        {
                            token->TokenType = TK_N2; // <>
                        }
                    else
                        {
                            UnGetChar(token);
                            token->TokenType = TK_LQ; // <
                        }

                    state = ST_END;
                }
            // symbol !=
            else if (ch == '!')
                {
                    token->Lexem = ch;

                    ch = GetChar();
                    if (ch == '=')
                        {
                            token->Lexem += ch;
                            token->TokenType = TK_NE; // !=
                        }
                    else
                        {
                            UnGetChar(token);
                            token->TokenType = TK_LEX_ERROR; // !
                        }

                    state = ST_END;
                }
            // symbol (
            else if ((ch == '('))
                {
                    token->Lexem = ch;
                    token->TokenType = TK_OPEN_BRACE; // (
                    state = ST_END;
                }
            // symbol )
            else if ((ch == ')'))
                {
                    token->Lexem = ch;
                    token->TokenType = TK_CLOSE_BRACE; // )
                    state = ST_END;
                }
            // symbol {
            else if ((ch == '{'))
                {
                    token->Lexem = ch;
                    token->TokenType = TK_OPEN_BLOCK; // {
                    state = ST_END;
                }
            // symbol }
            else if (ch == '}')
                {
                    token->Lexem = ch;
                    token->TokenType = TK_CLOSE_BLOCK; // }
                    state = ST_END;
                }
            // symbol ,
            else if ((ch == ','))
                {
                    token->Lexem = ch;
                    token->TokenType = TK_COMMA; // ,
                    state = ST_END;
                }
            // symbol ;
            else if ((ch == ';'))
                {
                    token->Lexem = ch;
                    token->TokenType = TK_SEMICOLON; // ;
                    state = ST_END;
                }
            // symbol .
            else if ((ch == '.'))
                {
                    token->Lexem = ch;
                    token->TokenType = TK_PERIOD; // .
                    state = ST_END;
                }
            // symbol =
            else if ((ch == '='))
                {
                    token->Lexem = ch;
                    token->TokenType = TK_EQUAL; // =
                    state = ST_END;
                }
            // symbol *
            else if ((ch == '*'))
                {
                    token->Lexem = ch;
                    token->TokenType = TK_MUL; // *
                    state = ST_END;
                }
            // symbol /
            else if ((ch == '/'))
                {
                    token->Lexem = ch;
                    token->TokenType = TK_DIV; // /
                    state = ST_END;
                }
            // symbol %
            else if (ch == '%')
                {
                    token->Lexem = ch;
                    token->TokenType = TK_PER; // %
                    state = ST_END;
                }
        }
    while (state != ST_END && state != ST_ERROR);

    return token;
}

char Lex::GetChar()
{
    if (Position < Length)
    {
        char c = input.at(Position);        
        Position++;
        return c;        
    }
    else  {
        state = ST_END;       
        return '\0';
      }
    
}

void Lex::UnGetChar(Token *token)
{
    std::string tmp;
    tmp = token->Lexem.substr(0,token->Lexem.length() - 1 + NullCount);
    token->Lexem = tmp;
    if (NullCount > 0)
        NullCount--;
    Position--;
}

void Lex::UnGetChar()
{
    if (NullCount > 0)
        NullCount--;
    Position--;
}

void Lex::integer_number_part(Token *token)
{
    std::string ch;
    // integer part of the number
    do
        {
            ch = GetChar();
            token->Lexem = token->Lexem + ch;
        }
    while ((ch >= "0") && (ch <= "9"));
}


void Lex::exponential(Token *token)
{
    std::string ch;
    UnGetChar(token);

    // add the 'e' to the lexem
    ch = GetChar();
    token->Lexem = token->Lexem + ch;

    // check for '+' and '-' signals
    ch = GetChar();
    if ((ch == "+") || (ch == "-"))
        token->Lexem = token->Lexem + ch;
    else
        {
            token->Lexem = token->Lexem + ch;
            UnGetChar(token);
        }

    // there must be at least a character between '0' and '9'
    ch = GetChar();
    if ((ch >= "0") && (ch <= "9"))
        {
            // after an 'e' an integer is expected
            token->Lexem = token->Lexem + ch;
            UnGetChar(token);
            integer_number_part(token);
            token->TokenType = TK_FLOATING_POINT;
        }
    else
        {
            token->TokenType = TK_FLOATINGPOINT_ERROR;
        }
}

//---------------------------------------------------------------------------

