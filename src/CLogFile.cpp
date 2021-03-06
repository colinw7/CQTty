#include <CLogFile.h>

CLogFile::
CLogFile(const char *fileName) :
 fileName_(fileName)
{
}

CLogFile::
~CLogFile()
{
}

void
CLogFile::
init()
{
  if (CFile::exists(fileName_))
    CFile::remove(fileName_);

  file_ = std::make_unique<CFile>(fileName_);

  file_->open(CFileBase::Mode::APPEND);
}

void
CLogFile::
write(const std::string &str)
{
  if (file_) {
    file_->write(str);

    file_->flush();
  }
}

void
CLogFile::
write(char c)
{
  std::string str(&c, 1);

  write(str);
}
