#include <CTty.h>
#include <COSPty.h>
#include <COSRead.h>
#include <CEnv.h>
//#include <CReadLine.h>

#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

extern "C" {
# include <termios.h>
# include <sys/ioctl.h>
}

void
exitSignalHandler(int)
{
  exit(0);
}

void
childSignalHandler(int)
{
  exit(0);
}

// TODO: Add SIGWINCH handler

//--------

#define TTYNAME "/dev/tty"

CTty::
CTty()
{
  const char *shell = getenv("SHELL");

  if (shell && shell[0] != '\0')
    shell_ = shell;
  else
    shell_ = "/bin/sh";
}

CTty::
~CTty()
{
  close(master_fd_);
}

void
CTty::
init()
{
  // save terminal state
  struct termios *save_termios = new struct termios;

  if (tcgetattr(0, save_termios) < 0)
    std::cerr << "tcgetattr failed" << std::endl;

  //struct termios buf = *save_termios;

  //------

  // close unneeded files
  int fd = open("/dev/null", O_RDONLY);

  if (fd < 0)
    exit(1);

  if (fd > STDIN_FILENO) {
    dup2(fd, STDIN_FILENO);
    close(fd);
  }

  dup2(STDERR_FILENO, STDOUT_FILENO);

#if 0
  uint max_fds = (int) sysconf(_SC_OPEN_MAX);

  for (uint fd1 = STDERR_FILENO + 1; fd1 < max_fds; ++fd1)
    close(fd1);
#endif

  //------

  // create master pseudo terminal
  master_fd_ = getpt();

  if (master_fd_ <= 0)
    exit(1);

  if (grantpt(master_fd_) < 0 || unlockpt(master_fd_) < 0)
    exit(1);

  char *dev_name = ptsname(master_fd_);

  fcntl(master_fd_, F_SETFL, O_NDELAY);

  //------

  // create slave pseudo terminal
  int slave_fd = open(dev_name, O_RDWR | O_NOCTTY, 0);

  //-------

  signal(SIGHUP , exitSignalHandler);
  signal(SIGINT , exitSignalHandler);
  signal(SIGQUIT, exitSignalHandler);
  signal(SIGTERM, exitSignalHandler);
  signal(SIGCHLD, childSignalHandler);

  //------

  CEnvInst.set("TERM", "xterm");

  if (is_cterm_)
    CEnvInst.set("CTERM_VERSION", "0.1");

  //------

  // create child shell

  pid_t pid = fork();

  if (pid < 0) // failed
    exit(1);

  if (pid == 0) { /* child (slave) */
    close(master_fd_);

    // control tty (make slave_fd the child processes terminal)

    setsid(); // start new session (child loses controlling terminal)

    setpgid(0, 0); // set process group to curret pid

    //------

    // close terminal (needed ?)
    int fd1 = open(TTYNAME, O_RDWR | O_NOCTTY);

    if (fd1 >= 0) {
      ioctl(fd1, TIOCNOTTY, NULL);
      close(fd1);
    }

    fd1 = open(TTYNAME, O_RDWR | O_NOCTTY);

    if (fd1 >= 0)
      close(fd1);

    //------

    // make pty slave the controlling terminal

#ifdef TIOCSCTTY
    fd1 = ioctl(slave_fd, TIOCSCTTY, NULL);
#endif

    //------

    // ensure we got a terminal (needed ?)

    fd1 = open(TTYNAME, O_WRONLY);

    if (fd1 < 0)
      exit(1);

    close(fd1);

    //------

    // Reopen stdin, stdout and stderr over the tty file descriptor
    dup2(slave_fd, STDIN_FILENO );
    dup2(slave_fd, STDOUT_FILENO);
    dup2(slave_fd, STDERR_FILENO);

    close(slave_fd);

    // run child

    // (void) cfsetospeed(save_termios, B38400);
    // (void) cfsetispeed(save_termios, B38400);

    // TODO: should be a tcsetattr here ?

    signal(SIGINT , SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    // exec shell

    execlp(shell_.c_str(), shell_.c_str(), NULL);

    _exit(255);
  }

  // parent (master)

  close(slave_fd);

  // wait for write
  waitWrite();

  // set raw
  setRaw(STDIN_FILENO, save_termios);

  // setup select values
  fds_[0] = master_fd_;
  fds_[1] = STDIN_FILENO;
}

#if 0
void
CTty::
readLoop()
{
  // main loop

  CReadLine readline;

  while (true) {
    std::string line = readline.readLine();

    if (readline.eof()) break;

    write(line);

    //------

    read(line);

    std::cout << line << std::endl;
  }
}
#endif

void
CTty::
waitWrite()
{
  COSRead::wait_write(master_fd_);
}

void
CTty::
write(const std::string &str)
{
  COSRead::write(master_fd_, str);
}

bool
CTty::
read(std::string &str)
{
  std::string str1, str2;

  if (! read(str1, str2))
    return false;

  str = str1;

  return true;
}

bool
CTty::
read(std::string &str1, std::string &str2)
{
  str1 = ""; str2 = "";

  flags_[0] = false;
  flags_[1] = false;

  if (! COSRead::wait_read(fds_, 1, flags_, 0, read_wait_))
    return false;

  while (flags_[0] || flags_[1]) {
    if (flags_[0]) {
      std::string str;

      if (! COSRead::read(master_fd_, str))
        goto next;

      str1 += str;
    }

    if (flags_[1]) {
      std::string str;

      if (! COSRead::read(0, str))
        goto next;

      str2 += str;
    }

  next:
    if (! COSRead::wait_read(fds_, 1, flags_, 0, read_wait_))
      break;
  }

  return true;
}

#ifdef _POSIX_VDISABLE
# define VDISABLE _POSIX_VDISABLE
#else
# define VDISABLE 255
#endif

bool
CTty::
setRaw(int fd, struct termios *t)
{
  t->c_cc[VEOF] = CEOF;
  t->c_cc[VEOL] = VDISABLE;

#ifdef VEOL2
  t->c_cc[VEOL2] = VDISABLE;
#endif
#ifdef VSWTC
  t->c_cc[VSWTC] = VDISABLE;
#endif
#ifdef VSWTCH
  t->c_cc[VSWTCH] = VDISABLE;
#endif

  /* echo off, canonical mode off, extended input processing off, signal chars off */
  t->c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  /* no SIGINT on BREAK, CR-to-NL off, input parity check off, don't strip 8th bit on input,
     output flow control off */
  t->c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  /* clear size bits, parity checking off */
  t->c_cflag &= ~(CSIZE | PARENB);

  /* set 8 bits/char */
  t->c_cflag |= CS8;

  /* output processing off */
  t->c_oflag &= ~OPOST;

  t->c_cc[VMIN ] = 1; /* Case B: 1 byte at a time, no timer */
  t->c_cc[VTIME] = 0;

  if (tcsetattr(fd, TCSANOW, t) < 0)
    return false;

  return true;
}

bool
CTty::
setPageSize(int rows, int cols)
{
  struct winsize ts;

  if (ioctl(master_fd_, TIOCGWINSZ, &ts) == -1)
    return false;

  if (ts.ws_col != 0) ts.ws_xpixel = short(cols*(ts.ws_xpixel/ts.ws_col));
  if (ts.ws_row != 0) ts.ws_ypixel = short(rows*(ts.ws_ypixel/ts.ws_row));

  ts.ws_row = ushort(rows);
  ts.ws_col = ushort(cols);

  ioctl(master_fd_, TIOCSWINSZ, reinterpret_cast<char *>(&ts));

  return true;
}
