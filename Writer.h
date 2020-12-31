#ifndef __Writer_h
#define __Writer_h
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <wchar.h>
#endif
#define MB_LENGTH 1024

class Writer
{
public:
  // Constructors
  Writer()
  {
   // out = fopen("oie.txt", "w");
    out = stdout;
    level = 0;
  }

  Writer(const char* fileName)
  {
    out = fopen(fileName, "w");
    level = 0;
  }

  // Destructor
  ~Writer()
  {
    fclose(out);
  }

  void write(int c)
  {
    fputc(c, out);
  }

  void write(const wchar_t*, ...);

  void beginBlock()
  {
    writeTabs();
    fprintf(out, "{\n");
    level++;
  }

  void endBlock()
  {
    level--;
    writeTabs();
    fprintf(out, "}\n");
  }

  void backspace()
  {
    level--;
  }

  void tab()
  {
    level++;
  }

  void beginLine()
  {
    writeTabs();
  }

  void endLine()
  {
    fputc('\n', out);
  }

private:
  void writeTabs()
  {
    if (level == 0)
      return;
    for (int i = 0; i < level; i++)
      fprintf(out, "  ");
  }

  FILE* out;
  int level;

}; // Writer

void
Writer::write(const wchar_t* fmt, ...)
{
  va_list args;
  char mbfmt[MB_LENGTH];

  wcstombs(mbfmt, fmt, MB_LENGTH - 1);
  va_start(args, fmt);
  vfprintf(out, mbfmt, args);
}

#endif // __Writer_h

