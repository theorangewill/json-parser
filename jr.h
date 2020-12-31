/*
Grammar
=================
Json:
  Value EOF
;

Value:
  Object
| Array
| STRING
| NUMBER
| TRUE
| FALSE
| NULL
;

Object:
  '{' MembersOpt '}'
;

MembersOpt:
  Members
| // empty
;

Members:
  Pair M_
;

M_:
  ',' Pair M_
| // empty
;

Pair:
  STRING ':' Value
;

Array:
  '[' ElementsOpt ']'
;

ElementsOpt:
  Elements
| // empty
;

Elements:
  Value E_
;

E_:
  ',' Value E_
| // empty
;

LL(1) grammar
=============
Json: Value EOF;
Value: Object | Array | STRING | NUMBER | TRUE | FALSE | NULL;
Object: '{' MembersOpt '}';
MembersOpt: Members | ;
Members: Pair M_;
M_: ',' Pair M_ | ;
Pair: STRING ':' Value;
Array: '[' ElementsOpt ']';
ElementsOpt: Elements | ;
Elements: Value E_;
E_: ',' Value E_ | ;

Syntax diagrams
===============
Json: Value EOF;
Value: Object | Array | STRING | NUMBER | TRUE | FALSE | NULL;
Object: "{" Members? "}";
Members: Pair ("," Pair)*;
Pair: STRING ":" Value;
Array: "[" Elements? "]";
Elements: Value ("," Value)*;

First
=====
FIRST(Json)={ '{', '[', STRING, NUMBER, TRUE, FALSE, NULL }
FIRST(Value)={ '{', '[', STRING, NUMBER, TRUE, FALSE, NULL }
FIRST(Object)={ '{' }
FIRST(MembersOpt)={ STRING, empty}
FIRST(Members)={ STRING }
FIRST(M_)={ ',', empty }
FIRST(Pair)={ STRING }
FIRST(Array)={ '[' }
FIRST(ElementsOpt)={ '{', '[', STRING, NUMBER, TRUE, FALSE, NULL, empty}
FIRST(Elements)={ '{', '[', STRING, NUMBER, TRUE, FALSE, NULL }
FIRST(E_)={ ',', empty }
*/

#ifndef __lex_h
#define __lex_h
#include <ctype.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class Token
{
public:
  enum TokenType
  {
    ERROR = -1,
    EOF_,
    STRING,
    NUMBER,
    TRUE,
    FALSE,
    NULL_
  }; // TokenType

  int type;
  string lexeme; // lexeme and value of ID
  char c; // value of CHAR

}; // Token

class Parser
{
public:
  void parse(const char*);
  int objetos;
  int membros;
  int arrays;
  int elementos;
  Parser();
private:
  char* buffer; // input buffer
  int lineNumber; // current line number
  Token lookahead; // current token

  void error();
  Token nextToken(); // lexical analyser

  void advance()
  {
    lookahead = nextToken();
  }

  void match(int t)
  {
    if (lookahead.type == t)
      advance();
    else
      error();
  }

  void Json();
  void Value();
  void Object();
  void MembersOpt();
  void Members();
  void M_();
  void Pair();
  void Array();
  void ElementsOpt();
  void Elements();
  void E_();

}; // Parser

//Construtor do Parser, inicializa com todos os valores em zero
Parser::Parser()
{
  objetos = 0;
  membros = 0;
  arrays = 0;
  elementos = 0;
}

void
Parser::parse(const char* input)
{
  buffer = const_cast<char*>(input);
  lineNumber = 1;
  lookahead = nextToken();
  Json();
  if (lookahead.type != Token::EOF_)
    error();
  printf("Numero de Objetos: %d\nNumero de Membros: %d\nNumero de Arrays: %d\nNumero de Elementos: %d\n", objetos, membros, arrays, elementos);
  puts("Concluido");
}

void
Parser::error()
{
  printf("**Error (%d): %s\n", lineNumber, "desconhecido");
  exit(1);
}

Token
Parser::nextToken()
{
  while (isspace(*buffer))
    if (*buffer++ == '\n')
      ++lineNumber;

  Token t;
  if (*buffer == '\0')
  {
        t.type = Token::EOF_;
        return t;
  }

  char* beginLexeme = buffer;

  if (*buffer == '-' || isdigit(*buffer))
  {
    if(*buffer == '-'){
      ++buffer;
      if(isdigit(*buffer))
        ++buffer;
    }
    else
        ++buffer;

    while(isdigit(*buffer))
      ++buffer;

    if(*buffer == '.'){
      if(isdigit(*buffer))
        ++buffer;
      while (isdigit(*buffer))
        ++buffer;
    }

    if(*buffer == 'e' || *buffer == 'E'){
      if(*buffer == '+' || *buffer == '-')
      ++buffer;
      if(isdigit(*buffer))
        ++buffer;
      while (isdigit(*buffer))
        ++buffer;
    }

    t.type = Token::NUMBER;
    t.lexeme = string(beginLexeme, buffer);
    return t;
  }

  if (*buffer == 34)
  {
    ++buffer;
    while (*buffer != 34){
      if (*buffer == 9 || *buffer == 10){
        error();
      }
      else if(*buffer == 92){
        ++buffer;
        if(*buffer == 34 || *buffer == 10 || *buffer == 9 || *buffer == 92){
         ++buffer;
        }
        else
         error();
      }
      else
        ++buffer;
    }
    t.c = *buffer++;
    t.type = Token::STRING;
    t.lexeme = string(beginLexeme, buffer);
    return t;
  }

  if (*buffer == 't')
  {
   ++buffer;
    if(*buffer == 'r'){
      ++buffer;
      if(*buffer == 'u'){
        ++buffer;
          if(*buffer == 'e'){
            t.c = *buffer++;
            t.type = Token::TRUE;
            t.lexeme = string(beginLexeme, buffer);
            return t;
          }
       }
    }
  }

  if (*buffer == 'f')
  {
    ++buffer;
    if(*buffer == 'a'){
      ++buffer;
      if(*buffer == 'l'){
        ++buffer;
          if(*buffer == 's'){
            ++buffer;
            if(*buffer == 'e'){
              t.c = *buffer++;
              t.type = Token::FALSE;
              t.lexeme = string(beginLexeme, buffer);
              return t;
            }
          }
      }
    }
  }


  if (*buffer == 'n')
  {
    ++buffer;
    if(*buffer == 'u'){
      ++buffer;
      if(*buffer == 'l'){
        ++buffer;
          if(*buffer == 'l'){
            t.c = *buffer++;
            t.type = Token::NULL_;
            t.lexeme = string(beginLexeme, buffer);
            return t;
          }
       }
     }
   }
  char c = *buffer++;

  switch (c)
  {
    case '{':
    case '}':
    case '[':
    case ']':
    case '+':
    case ',':
    case ':':
      t.type = c;
      t.lexeme = string(beginLexeme, buffer);
      return t;
  }


  error();
  t.type = Token::ERROR;
  return t;
}


/***********************
Implementacao das Producoes
***********************/
void
Parser::Json()
{
  Value();
  if (lookahead.type == Token::EOF_)
    advance();
  else
    error();
}

void
Parser::Value()
{
  switch (lookahead.type)
  {
    case '{':
      Object();
      break;
    case '[':
      Array();
      break;
    case Token::STRING:
    case Token::NUMBER:
    case Token::TRUE:
    case Token::FALSE:
    case Token::NULL_:
      advance();
      break;
    default:
      error();
  }
}

void
Parser::Object()
{
  if (lookahead.type == '{'){
    objetos++;
    match('{');
    MembersOpt();
    match('}');
  }
  else
    error();
}

void
Parser::MembersOpt()
{
  if (lookahead.type == Token::STRING){
    Members();
  }
  else
    ;
}

void
Parser::Members()
{
  membros++;
  Pair();
  M_();
}

void
Parser::M_()
{
  if (lookahead.type == ','){
    membros++;
    match(',');
    Pair();
    M_();
  }
  else
    ;
}

void
Parser::Pair()
{
  if (lookahead.type == Token::STRING){
    advance();
    match(':');
    Value();
  }
  else
    error();
}

void
Parser::Array()
{
  if (lookahead.type == '['){
    arrays++;
    match('[');
    ElementsOpt();
    match(']');
  }
  else
    error();
}

void
Parser::ElementsOpt()
{
  switch (lookahead.type)
  {
    case '{':
    case '[':
    case Token::STRING:
    case Token::NUMBER:
    case Token::TRUE:
    case Token::FALSE:
    case Token::NULL_:
      Elements();
      break;
    default:
      ;
  }
}

void
Parser::Elements()
{
  elementos++;
  switch (lookahead.type)
  {
    case '{':
    case '[':
    case Token::STRING:
    case Token::NUMBER:
    case Token::TRUE:
    case Token::FALSE:
    case Token::NULL_:
      Value();
      E_();
      break;
    default:
      error();
  }
}

void
Parser::E_()
{
  if (lookahead.type == ','){
    elementos++;
    match(',');
    Value();
    E_();
  }
  else
    ;
}

#endif // __lex_h
