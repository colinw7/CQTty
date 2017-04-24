#ifndef CLOG_FILE
#define CLOG_FILE

#include <CFile.h>
#include <CAutoPtr.h>

class CLogFile {
 public:
  CLogFile(const char *fileName);
 ~CLogFile();

  void init();

  void write(const std::string &str);
  void write(char c);

 private:
  CAutoPtr<CFile> file_;
  std::string     fileName_;
};

#endif
