#ifndef COS_PTY_H
#define COS_PTY_H

#include <sys/param.h>
#include <termios.h>

#include <set>
#include <string>

enum CPtyState {
  CPTY_CANON,
  CPTY_RAW,
  CPTY_CBREAK
};

class COSPty {
 public:
  COSPty();
 ~COSPty();

  void close_files(const std::set<int> &skip);

  int get_master_fd() const { return master_fd_; }
  int get_slave_fd () const { return slave_fd_ ; }

  bool open_master();
  bool open_slave();

  bool sync_master();

  bool read_master(std::string &str);

  bool wait_read_master(std::string &str1, std::string &str2, int read_wait=10);

  bool write_master(char c);
  bool write_master(const std::string &str);

  bool control_slave();

  bool close_tty();
  bool check_tty();

  bool redirect_slave();

  bool set_raw();
  bool reset_raw();

  bool set_cbreak();
  bool reset_cbreak();

  bool close_master();
  bool close_slave();

  struct termios getSaveTermIOS() const { return *save_termios_; }

  static bool set_raw  (int fd, struct termios *t);
  static bool reset_raw(int fd, struct termios *t);

  static bool set_cbreak  (int fd, struct termios *t);
  static bool reset_cbreak(int fd, struct termios *t);

  //------

  static bool initRawSignals();
  static bool initCBreakSignals();

  static bool initSignals(CPtyState state);
  static void termSignals();

 private:
  std::string     masterName_;
  int             master_fd_;
  std::string     slaveName_;
  int             slave_fd_;
  struct termios *save_termios_;
};

#endif
