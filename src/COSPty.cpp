#include <COSPty.h>
#include <COSTerm.h>
#include <COSSignal.h>
#include <COSRead.h>
#include <COSErr.h>
#include <CSignalOpt.h>

#include <cstdio>
#include <cerrno>
#include <cassert>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C" {
# include <termios.h>
# include <sys/ioctl.h>
};

#define HAS_POSIX_OPENPT 1

COSPty::
COSPty() :
 master_fd_   (-1),
 slave_fd_    (-1),
 save_termios_(NULL)
{
}

COSPty::
~COSPty()
{
  close_master();

  delete save_termios_;
}

void
COSPty::
close_files(const std::set<int> &skip_fds)
{
  // replace stdin with /dev/null
  int nfd = open("/dev/null", O_RDONLY);

  if (nfd > STDIN_FILENO) {
    dup2(nfd, STDIN_FILENO);
    close(nfd);
  }

  // replace stdout with stderr
  dup2(STDERR_FILENO, STDOUT_FILENO);

  int num_fds = (int) sysconf(_SC_OPEN_MAX);

  for (int i = STDERR_FILENO + 1; i < num_fds; ++i) {
    if (skip_fds.find(i) == skip_fds.end())
      close(i);
  }
}

bool
COSPty::
open_master()
{
  masterName_ = "/dev/ptmx";

  if (! COSTerm::openMasterPty(&master_fd_, slaveName_))
    return false;

  fcntl(master_fd_, F_SETFL, O_NDELAY);

  return true;
}

bool
COSPty::
open_slave()
{
  bool rc = false;

  if (slave_fd_ != -1)
    close(slave_fd_);

  slave_fd_ = open(slaveName_.c_str(), O_RDWR);

  if (slave_fd_ < 0)
    goto done;

  if (fchmod(slave_fd_, 0666) == -1 && errno != EPERM)
    goto done;

  rc = control_slave();

 done:
  if (! rc)
    close_slave();

  return rc;
}

bool
COSPty::
control_slave()
{
  setsid();

  setpgid(0, 0);

  close_tty();

#ifdef TIOCSCTTY
  // set controlling tty to slave tty
  if (ioctl(slave_fd_, TIOCSCTTY, 0) < 0)
    return false;
#endif

  // make sure we have a tty now
  if (! check_tty())
    return false;

  return true;
}

bool
COSPty::
close_tty()
{
  // void /dev/tty associations
  int fd = open("/dev/tty", O_RDWR | O_NOCTTY);

  if (fd >= 0) {
    ioctl(fd, TIOCNOTTY, NULL);
    close(fd);
  }

  // do we still have tty ? shouldn't have
  fd = open("/dev/tty", O_RDWR | O_NOCTTY);

  if (fd >= 0)
    close(fd);

  return true;
}

bool
COSPty::
check_tty()
{
  int fd = open("/dev/tty", O_WRONLY);

  if (fd < 0)
    return false;

  close(fd);

  return true;
}

bool
COSPty::
sync_master()
{
  return COSRead::wait_write(master_fd_);
}

bool
COSPty::
wait_read_master(std::string &str1, std::string &str2, int read_wait)
{
  std::string str;

  int  fds[2];
  bool flags[2];

  fds[0] = get_master_fd();
  fds[1] = 0;

  flags[0] = false;
  flags[1] = false;

  if (! COSRead::wait_read(fds, 1, flags, 0, read_wait))
    return true;

  while (flags[0] || flags[1]) {
    if (flags[0]) {
      if (! read_master(str))
        break;

      str1 += str;
    }

    if (flags[1]) {
      if (! COSRead::read(0, str))
        break;

      str2 += str;
    }

    if (! COSRead::wait_read(fds, 1, flags, 0, read_wait))
      break;
  }

  //str = str1;

  return true;
}

bool
COSPty::
read_master(std::string &str)
{
  return COSRead::read(master_fd_, str);
}

bool
COSPty::
write_master(char c)
{
  return COSRead::write(master_fd_, c);
}

bool
COSPty::
write_master(const std::string &str)
{
  return COSRead::write(master_fd_, str);
}

bool
COSPty::
redirect_slave()
{
  dup2(slave_fd_, STDIN_FILENO );
  dup2(slave_fd_, STDOUT_FILENO);
  dup2(slave_fd_, STDERR_FILENO);

  if (slave_fd_ > STDERR_FILENO)
    close(slave_fd_);

  return true;
}

bool
COSPty::
set_raw()
{
  assert(save_termios_ == NULL);

  save_termios_ = new struct termios;

  bool rc = set_raw(0, save_termios_);

  return rc;
}

bool
COSPty::
set_cbreak()
{
  assert(save_termios_ == NULL);

  save_termios_ = new struct termios;

  bool rc = set_cbreak(0, save_termios_);

  return rc;
}

bool
COSPty::
reset_raw()
{
  assert(save_termios_ != NULL);

  bool rc = reset_raw(0, save_termios_);

  delete save_termios_;

  save_termios_ = NULL;

  return rc;
}

bool
COSPty::
reset_cbreak()
{
  assert(save_termios_ != NULL);

  bool rc = reset_cbreak(0, save_termios_);

  delete save_termios_;

  save_termios_ = NULL;

  return rc;
}

bool
COSPty::
set_raw(int fd, struct termios *t)
{
  if (tcgetattr(fd, t) < 0)
    return false;

  struct termios t1 = *t;

  /* echo off, canonical mode off, extended input processing off, signal chars off */
  t1.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  /* no SIGINT on BREAK, CR-to-NL off, input parity check off, don't strip 8th bit on input,
     output flow control off */
//t1.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  t1.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON | IGNBRK | IGNCR | INLCR | PARMRK);

  /* clear size bits, parity checking off */
  t1.c_cflag &= ~(CSIZE | PARENB);

  /* set 8 bits/char */
  t1.c_cflag |= CS8;

  /* output processing off */
  t1.c_oflag &= ~OPOST;

  t1.c_cc[VMIN ] = 1; /* Case B: 1 byte at a time, no timer */
  t1.c_cc[VTIME] = 0;

  if (tcsetattr(fd, TCSAFLUSH, &t1) < 0)
    return false;

#if 0
  ttystate  = CPTY_RAW;
  ttysavefd = fd;
#endif

  return true;
}

bool
COSPty::
set_cbreak(int fd, struct termios *t)
{
  if (tcgetattr(fd, t) < 0)
    return false;

  struct termios t1 = *t;

  /* echo off, canonical mode off */
  t1.c_lflag &= ~(ECHO | ICANON);

  /* signals on */
  t1.c_lflag |= ISIG;

  /* CR-to-NL off */
  t1.c_iflag &= ~ICRNL;

  t1.c_cc[VMIN ] = 1; /* Case B: 1 byte at a time, no timer */
  t1.c_cc[VTIME] = 0;

  if (tcsetattr(fd, TCSAFLUSH, &t1) < 0)
    return false;

#if 0
  ttystate  = CPTY_CBREAK;
  ttysavefd = fd;
#endif

  return true;
}

bool
COSPty::
reset_raw(int fd, struct termios *save_termios)
{
  assert(save_termios != NULL);

#if 0
  if (ttystate != CPTY_CBREAK && ttystate != CPTY_RAW)
    return 0;
#endif

  if (tcsetattr(fd, TCSAFLUSH, save_termios) < 0)
    return false;

#if 0
  ttystate = RESET;
#endif

  return true;
}

bool
COSPty::
reset_cbreak(int fd, struct termios *save_termios)
{
  assert(save_termios != NULL);

#if 0
  if (ttystate != CPTY_CBREAK && ttystate != CPTY_RAW)
    return 0;
#endif

  if (tcsetattr(fd, TCSAFLUSH, save_termios) < 0)
    return false;

#if 0
  ttystate = RESET;
#endif

  return true;
}

bool
COSPty::
close_master()
{
  if (master_fd_ >= 0) {
    close(master_fd_);

    master_fd_ = -1;
  }

  return true;
}

bool
COSPty::
close_slave()
{
  if (slave_fd_ >= 0) {
    close(slave_fd_);

    slave_fd_ = -1;
  }

  return true;
}

//----------------

/* Saved user terminal settings */
static struct termios userTermios;
static bool           userTermiosSet;

/* General handler: restore tty settings and exit */
static void
termGenHandler(int /*sig*/)
{
  if (userTermiosSet) {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &userTermios) == -1)
      COSErr::exit("tcsetattr");
  }

  _exit(EXIT_SUCCESS);
}

/* Handler for SIGTSTP */
static void
termTSTPHandler(int /*sig*/)
{
  int savedErrno = errno; /* We might change 'errno' here */

  /* Save current terminal settings, restore terminal to state at time of program startup */
  struct termios ourTermios;

  if (tcgetattr(STDIN_FILENO, &ourTermios) == -1)
    COSErr::exit("tcgetattr");

  if (userTermiosSet) {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &userTermios) == -1)
      COSErr::exit("tcsetattr");
  }

  /* Set the disposition of SIGTSTP to the default, raise the signal
     once more, and then unblock it so that we actually stop */
  COSSignal::defaultSignal(SIGTSTP);

  raise(SIGTSTP);

  COSSignal::unblockSignal(SIGTSTP);

  /* Execution resumes here after SIGCONT */
  COSSignal::setOldSigProcMask();

  COSSignal::addSignalHandler(SIGTSTP, termTSTPHandler, CSIGNAL_OPT_RESTART);

  /* The user may have changed the terminal settings while we were stopped;
     save the settings so we can restore them later */

  if (tcgetattr(STDIN_FILENO, &userTermios) == -1)
    COSErr::exit("tcgetattr");

  userTermiosSet = false;

  /* Restore our terminal settings */

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ourTermios) == -1)
    COSErr::exit("tcsetattr");

  errno = savedErrno;
}

//----------------

bool
COSPty::
initRawSignals()
{
  return initSignals(CPTY_RAW);
}

bool
COSPty::
initCBreakSignals()
{
  return initSignals(CPTY_CBREAK);
}

bool
COSPty::
initSignals(CPtyState state)
{
  /* Use cbreak mode */
  if      (state == CPTY_CBREAK) {
    if (! COSPty::set_cbreak(STDIN_FILENO, &userTermios))
      COSErr::exit("ttySetCbreak");

    userTermiosSet = true;

    /* Terminal special characters can generate signals in cbreak
       mode. Catch them so that we can adjust the terminal mode.
       We establish handlers only if the signals are not being ignored. */

    if (! COSSignal::isSignalIgnored(SIGQUIT))
      COSSignal::addSignalHandler(SIGQUIT, termGenHandler, CSIGNAL_OPT_RESTART);

    if (! COSSignal::isSignalIgnored(SIGINT))
      COSSignal::addSignalHandler(SIGINT, termGenHandler, CSIGNAL_OPT_RESTART);

    if (! COSSignal::isSignalIgnored(SIGTSTP))
      COSSignal::addSignalHandler(SIGTSTP, termTSTPHandler, CSIGNAL_OPT_RESTART);
  }
  /* Use raw mode */
  else if (state == CPTY_RAW) {
    if (! COSPty::set_raw(STDIN_FILENO, &userTermios))
      COSErr::exit("ttySetRaw");

    userTermiosSet = true;
  }

  COSSignal::addSignalHandler(SIGTERM, termGenHandler, CSIGNAL_OPT_RESTART);

  return true;
}

void
COSPty::
termSignals()
{
  // TODO: more reseting to do here if not exiting

  if (userTermiosSet) {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &userTermios) == -1)
      COSErr::exit("tcsetattr");
  }
}
