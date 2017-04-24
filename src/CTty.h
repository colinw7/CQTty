#ifndef CTTY_H
#define CTTY_H

#include <string>
#include <sys/types.h>

class CTty {
 public:
  CTty();
 ~CTty();

  void init();

  void setReadWait(uint wait) { read_wait_ = wait; }

  void waitWrite();

  void write(const std::string &str);

  bool read(std::string &str1, std::string &str2);

  bool read(std::string &str);

  //void readLoop();

  bool setPageSize(int rows, int cols);

  void setIsCTerm(bool flag) { is_cterm_ = flag; }

  void setShell(const std::string &shell) { shell_ = shell; }

 private:
  bool setRaw(int fd, struct termios *t);

 private:
  int         master_fd_ { -1 };
  int         fds_  [2];
  bool        flags_[2];
  uint        read_wait_ { 5000 }; // read timeout (microseconds)
  bool        is_cterm_ { true };
  std::string shell_;
};

#endif
