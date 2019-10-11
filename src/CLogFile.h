#ifndef CLOG_FILE
#define CLOG_FILE

#include <CFile.h>
#include <memory>

class CLogFile {
 public:
  CLogFile(const char *fileName);
 ~CLogFile();

  void init();

  void write(const std::string &str);
  void write(char c);

 private:
  using FileP = std::unique_ptr<CFile>;

  FileP       file_;
  std::string fileName_;
};

#endif
