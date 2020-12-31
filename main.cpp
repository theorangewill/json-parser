#include "Parser.h"
#include "Writer.h"
#include<iostream>
#include<fstream>

using namespace std;

int main(int argc, char** argv)
{
    if(argc==1){
	printf("Está faltando argumentos!\n");
	return 0;
    }
    fstream f(argv[1], fstream::in);
    string text;
    if(f.good()){
        //Pegar conteudo do arquivo
        getline(f, text, '\0');
        f.close();
        const char *textc = text.c_str();
        Parser parser;
        Json* J = parser.parse(textc);
        Writer writer;
        J->write(writer);
    }
    else
        printf("O arquivo não existe.\n");
    return 0;
}
