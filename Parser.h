#ifndef __Parser_h
#define __Parser_h
#include "Writer.h"
#include <ctype.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

class Object
{
public:
  // Destructor
  virtual ~Object(){
    // do nothing
  }

  // Make a deep-copy of this object
  virtual Object* clone() const{
    return 0;
  }

  // Get number of uses of this object
  int getNumberOfUses() const
  {
    return counter;
  }

  template <typename T> friend T* makeUse(T*);

  // Release this object
  void release()
  {
    if (--counter <= 0)
      delete this;
  }

protected:
  // Protected default constructor
  Object():
    counter(0)
  {
    // do nothing
  }

private:
  int counter; // reference counter

}; // Object

template <typename T>
inline T*
makeUse(T* object)
{
  if (object != 0)
    ++object->counter;
  return object;
}

template <typename T>
inline void
release(T* object)
{
  if (object != 0)
    object->release();
}

template <typename T>
class ObjectPtr
{
public:
  // Constructors
  ObjectPtr():
    object(0)
  {
    // do nothing
  }

  ObjectPtr(const ObjectPtr<T>& ptr)
  {
    this->object = makeUse(ptr.object);
  }

  ObjectPtr(T* object)
  {
    this->object = makeUse(object);
  }

  // Destructor
  ~ObjectPtr()
  {
    release(this->object);
  }

  ObjectPtr<T>& operator =(T* object)
  {
    release(this->object);
    this->object = makeUse(object);
    return *this;
  }

  ObjectPtr<T>& operator =(const ObjectPtr<T>& ptr)
  {
    release(this->object);
    this->object = makeUse(ptr.object);
    return *this;
  }

  bool operator ==(T* object) const
  {
    return this->object == object;
  }

  bool operator ==(const ObjectPtr<T>& ptr) const
  {
    return this->object == ptr.object;
  }

  bool operator !=(T* object) const
  {
    return !operator ==(object);
  }

  bool operator !=(const ObjectPtr<T>& ptr) const
  {
    return !operator ==(ptr);
  }

  operator T*() const
  {
    return this->object;
  }

  T* operator ->() const
  {
    return object;
  }

private:
  T* object; // this is the object

};
template <typename E> class Iterator;

template <typename E>
class Collection: public Object
{
public:
  virtual ~Collection()
  {
    // do nothing
  }

  virtual bool add(const E&) = 0;
  virtual bool addAll(const Collection<E>&) = 0;
  virtual bool remove(const E&) = 0;
  virtual void clear() = 0;

  virtual bool contains(const E&) const = 0;
  virtual int size() const = 0;
  virtual bool isEmpty() const = 0;
  virtual ObjectPtr<Iterator<E> > iterator() const = 0;

};

template <typename E>
class Iterator: public Object
{
public:
  virtual ~Iterator()
  {
    // do nothing
  }

  virtual bool hasNext() const = 0;

  virtual E& next() = 0;
  virtual void restart() = 0;

};

template <typename E> class LinkedListIterator;

template <typename E>
struct LinkedListElement
{
  E value;
  LinkedListElement<E>* next;
  LinkedListElement<E>* prev;

  // Constructor
  LinkedListElement(const E& aValue):
    value(aValue)
  {
    // do nothing
  }

};

template <typename E>
class LinkedList: public Collection<E>
{
public:
  // Constructor
  LinkedList()
  {
    head = tail = 0;
    numberOfElements = 0;
  }

  // Destructor
  ~LinkedList();

  bool add(const E&);
  bool addAll(const Collection<E>&);
  bool remove(const E&);
  void clear();

  bool contains(const E&) const;
  int size() const;
  bool isEmpty() const;
  ObjectPtr<Iterator<E> > iterator() const;

  const E& getFirst() const
  {
    return this->head->value;
  }

  Object* clone() const;
  bool operator ==(const LinkedList<E>&) const;

protected:
  LinkedListElement<E>* head;
  LinkedListElement<E>* tail;
  int numberOfElements;

  LinkedListElement<E>* find(const E&) const;

  LinkedList(const LinkedList<E>&);
  LinkedList<E>& operator =(const LinkedList<E>&);

  friend class LinkedListIterator<E>;

};

template <typename E>
class LinkedListIterator: public Iterator<E>
{
public:
  LinkedListIterator(const LinkedList<E>& list)
  {
    current = head = list.head;
  }

  bool hasNext() const;

  E& next();
  void restart();

private:
  LinkedListElement<E>* head;
  LinkedListElement<E>* current;

};

template <typename E>
LinkedList<E>::~LinkedList()
{
  clear();
}

template <typename E>
bool
LinkedList<E>::add(const E& value)
{
  LinkedListElement<E>* e = new LinkedListElement<E>(value);

  e->next = 0;
  e->prev = tail;
  if (tail == 0)
    head = e;
  else
    tail->next = e;
  tail = e;
  numberOfElements++;
  return true;
}

template <typename E>
bool
LinkedList<E>::addAll(const Collection<E>& c)
{
  bool changed = false;

  for (ObjectPtr<Iterator<E> > i = c.iterator(); i->hasNext();)
    changed |= add(i->next());
  return changed;
}

template <typename E>
bool
LinkedList<E>::remove(const E& value)
{
  LinkedListElement<E>* e = find(value);

  if (e == 0)
    return false;
  if (e == head)
    head = e->next;
  else
    e->prev->next = e->next;
  if (e == tail)
    tail = e->prev;
  else
    e->next->prev = e->prev;
  delete e;
  numberOfElements--;
  return true;
}

template <typename E>
void
LinkedList<E>::clear()
{
  while (head != 0)
  {
    LinkedListElement<E>* temp = head;

    head = head->next;
    delete temp;
  }
  tail = 0;
  numberOfElements = 0;
}

template <typename E>
bool
LinkedList<E>::contains(const E& value) const
{
  return find(value) != 0;
}

template <typename E>
int
LinkedList<E>::size() const
{
  return numberOfElements;
}

template <typename E>
bool
LinkedList<E>::isEmpty() const
{
  return numberOfElements == 0;
}

template <typename E>
ObjectPtr<Iterator<E> >
LinkedList<E>::iterator() const
{
  return new LinkedListIterator<E>(*this);
}

template <typename E>
LinkedListElement<E>*
LinkedList<E>::find(const E& value) const
{
  for (LinkedListElement<E>* e = head; e != 0; e = e->next)
    if (value == e->value)
      return e;
  return 0;
}

template <typename E>
Object*
LinkedList<E>::clone() const
{
  LinkedList<E>* c = new LinkedList<E>();

  for (LinkedListElement<E>* e = head; e != 0; e = e->next)
    c->add(e->value);
  return c;
}

template <typename E>
bool
LinkedList<E>::operator ==(const LinkedList<E>& list) const
{
  if (numberOfElements != list.numberOfElements)
    return false;

  LinkedListElement<E>* e1 = head;
  LinkedListElement<E>* e2 = list.head;

  for (; e1 != 0; e1 = e1->next, e2 = e2->next)
    if (e1->value != e2->value)
      return false;
  return true;
}

template <typename E>
bool
LinkedListIterator<E>::hasNext() const
{
  return current != 0;
}

template <typename E>
E&
LinkedListIterator<E>::next()
{
  E& value = current->value;

  current = current->next;
  return value;
}

template <typename E>
void
LinkedListIterator<E>::restart()
{
  current = head;
}

class Node: public Object
    {
        public:
          virtual void write(Writer&) const = 0;
    };

template <typename Node>
class List
{
public:
  typedef ObjectPtr<Iterator<ObjectPtr<Node> > > IteratorPtr;

  void add(Node* node)
  {
    this->nodes.add(node);
  }

  bool isEmpty() const
  {
    return this->nodes.isEmpty();
  }

  int size() const
  {
    return this->nodes.size();
  }

  IteratorPtr iterator() const
  {
    return this->nodes.iterator();
  }

  Node* getFirst() const
  {
    return nodes.getFirst();
  }

  void writeList(Writer& writer) const
  {
    if (this->nodes.isEmpty())
      return;
    for (IteratorPtr i = this->iterator(); i->hasNext();)
      i->next()->write(writer);
  }

  void writeList(Writer& writer, const wchar_t* sep) const
  {
    int count = this->nodes.size();

    if (count == 0)
      return;
    for (IteratorPtr i = this->iterator(); i->hasNext();)
    {
      i->next()->write(writer);
      if (--count > 0)
        writer.write(sep);
    }
  }

private:
  LinkedList<ObjectPtr<Node> > nodes;

};


class Value;
class Arr;
class Obj;
class Elements;
class Members;

using namespace std;
class Json: public Node
{
public:
    Value* v;
    Json()
    {
        v = NULL;
    }
    void write(Writer &writer) const;
};
    typedef ObjectPtr<Json> JsonPtr;

class Value: public Node
{
public:
    Obj* o;
    Arr* a;
    int q;
    string lexeme;
    string st;
    Value()
    {
        o = NULL;
        a = NULL;
        st = '\0';
    }
    void write(Writer &writer) const;
};
	typedef ObjectPtr<Value> ValuePtr;

class Obj : public Value
{
public:
    Members* m;
    Obj()
    {
        m=NULL;
    }
    Obj(Members* n)
    {
        m = n;
    }
    void write(Writer &writer) const;
};
	typedef ObjectPtr<Obj> ObjPtr;

class Members: public Obj, public List<Value>
{
public:
    Members()
    {
        // do nothing
    }
    void write(Writer &writer) const;
};
    typedef ObjectPtr<Members> MembersPtr;

class Arr: public Value
{
public:
    Elements* e;
    Arr()
    {
        e = NULL;
    }
    Arr(Elements* e)
    {
        this->e = e;
    }
    void write(Writer &writer) const;
};
    typedef ObjectPtr<Arr> ArrPtr;

class Elements : public Arr, public List<Value>
{
public:
    Elements()
    {
        // do nothing
    }
    void write(Writer &writer) const;
};
	typedef ObjectPtr<Elements> ElementsPtr;

/*Declaracao do Write*/

void Json::write(Writer &writer) const
{
    writer.beginLine();
    writer.write(L"Json");
    writer.endLine();
    writer.beginBlock();
    v->write(writer);
    writer.backspace();
    writer.endBlock();
}
void Value::write(Writer &writer) const
{
    writer.beginLine();
    if(q == 10)
        writer.write(L"%s:",st.c_str());
    if(a != NULL){
        writer.write(L"Value");
        writer.endLine();
        writer.beginBlock();
        a->write(writer);
        writer.endBlock();
    }
    else if(o != NULL){
        writer.write(L"Value");
        writer.endLine();
        writer.beginBlock();
        o->write(writer);
        writer.endBlock();
    }
    else if(q != 0){
        writer.write(L"%s",lexeme.c_str());
        writer.endLine();
    }
}
void Obj::write(Writer &writer) const
{
    writer.beginLine();
    writer.write(L"Object");
    writer.endLine();
    writer.beginBlock();
    if(m != NULL)
        m->write(writer);
    writer.endBlock();
}
void Members::write(Writer &writer) const
{
    List<Value>::writeList(writer);
}
void Arr::write(Writer &writer) const
{
    writer.beginLine();
    writer.write(L"Array");
    writer.endLine();
    writer.beginBlock();
    if(e != NULL)
        e->write(writer);
    writer.endBlock();
}
void Elements::write(Writer &writer) const
{
    List<Value>::writeList(writer);
}

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
  Json* parse(const char*);
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

  Json* json();
  Value* value();
  Obj* object();
  Members* members();
  Value* pairs();
  Arr* arrays();
  Elements* elements();

}; // Parser

//Construtor do Parser, inicializa com todos os valores em zero
Parser::Parser()
{
}

Json*
Parser::parse(const char* input)
{
  buffer = const_cast<char*>(input);
  lineNumber = 1;
  lookahead = nextToken();
  Json* J= json();
  if (lookahead.type != Token::EOF_)
    error();
  return J;
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
    if(*buffer == '-')
      ++buffer;

    if(*buffer != '0'){
      ++buffer;
      while(isdigit(*buffer))
        ++buffer;
    }
    else
        ++buffer;


    if(*buffer == '.'){
      ++buffer;
      if(isdigit(*buffer))
        ++buffer;
      else
	error();
      while (isdigit(*buffer))
        ++buffer;
    }

    if(*buffer == 'e' || *buffer == 'E'){
      ++buffer;
      if(*buffer == '+' || *buffer == '-')
      ++buffer;
      if(isdigit(*buffer))
        ++buffer;
      else
	error();
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
Json*
Parser::json()
{
	Json* j = new Json();
	Value* v = value();
	j->v = v;
	if (lookahead.type == Token::EOF_)
		advance();
	else
		error();
    return j;
}

Value*
Parser::value()
{
	Value* v = new Value();
	switch (lookahead.type)
	{
	case '{':
		v->o = object();
		break;
	case '[':
		v->a = arrays();
		break;
	case Token::STRING:
	case Token::NUMBER:
	case Token::TRUE:
	case Token::FALSE:
	case Token::NULL_:
		v->q = lookahead.type;
		v->lexeme = lookahead.lexeme;
		advance();
		break;
	default:
		error();
	}
	return v;
}

Obj*
Parser::object()
{
	Obj* o;
	if (lookahead.type == '{') {
		match('{');
		if (lookahead.type == '}'){
			o = new Obj();
			match('}');
		}
		else {
			Members* m = members();
			match('}');
			o = new Obj(m);
		}
	}
	else
		error();
    return o;
}

Members*
Parser::members()
{
	Members* m = new Members();
	for (;; advance()) {
		m->add(pairs());
		if (lookahead.type != ',')
			return m;
	}
}

Value*
Parser::pairs()
{
	if (lookahead.type == Token::STRING) {
		Value* v;
		string a = lookahead.lexeme;
		advance();
		match(':');
		v = value();
		v->q = 10;
		v->st = a;
		return v;
	}
	else
		error();
    return 0;
}

Arr*
Parser::arrays()
{
	Arr* a;
	if (lookahead.type == '[') {
		match('[');
		if (lookahead.type == ']') {
			match(']');
			a = new Arr();
		}
		else {
			Elements* e = elements();
			a = new Arr(e);
			match(']');
		}
	}
	else
		error();
    return a;
}

Elements*
Parser::elements()
{
	Elements* e = new Elements();
	for (;; advance()) {
		e->add(value());
		if (lookahead.type != ',')
			return e;
	}
}
#endif
