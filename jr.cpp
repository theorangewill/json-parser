#include "jr.h"
#include<iostream>
#include<fstream>
using namespace std;

int main(int argc,char *argv[])
{
  fstream f(argv[1], fstream::in );
  string text;
  if(f.good()){
      //Pegar conteudo do arquivo
      getline( f, text, '\0');
      f.close();
      const char * textc = text.c_str();

      Parser parser;
      parser.parse(textc);
  }
  else
    printf("O arquivo não existe.\n");
  return 0;
}

