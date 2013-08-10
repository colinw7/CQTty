#ifndef CESCAPE_H
#define CESCAPE_H

#include <string>
#include <vector>
#include <iostream>

// utility functions for getting escape strings
namespace CEscape {
  enum WindowOp {
    WINDOW_OP_DEICONIFY          = 1,
    WINDOW_OP_ICONIFY            = 2,
    WINDOW_OP_MOVE               = 3,
    WINDOW_OP_PIXEL_SIZE         = 4,
    WINDOW_OP_RAISE              = 5,
    WINDOW_OP_LOWER              = 6,
    WINDOW_OP_REFRESH            = 7,
    WINDOW_OP_CHAR_SIZE          = 8,
    WINDOW_OP_MAXIMIZE           = 9,
    WINDOW_OP_REPORT_STATE       = 11,
    WINDOW_OP_REPORT_POS         = WINDOW_OP_MOVE       + 10,
    WINDOW_OP_REPORT_PIXEL_SIZE  = WINDOW_OP_PIXEL_SIZE + 10,
    WINDOW_OP_REPORT_CHAR_SIZE   = WINDOW_OP_CHAR_SIZE  + 10,
    WINDOW_OP_REPORT_SCREEN_SIZE = WINDOW_OP_MAXIMIZE   + 10,
    WINDOW_OP_REPORT_ICON_STR    = 20,
    WINDOW_OP_REPORT_TITLE_STR   = 21,
    WINDOW_OP_LINE_SIZE_24       = 24,
    // ...
  };

  std::string NUL();
  std::string SOH();
  std::string STX();
  std::string ETX();
  std::string EOT();
  std::string ENQ();
  std::string ACK();
  std::string BEL();
  std::string BS();
  std::string HT();
  std::string LF();
  std::string VT();
  std::string FF();
  std::string CR();
  std::string SO();
  std::string SI();
  std::string DLE();
  std::string DC1();
  std::string DC2();
  std::string DC3();
  std::string DC4();
  std::string NAK();
  std::string SYN();
  std::string ETB();
  std::string CAN();
  std::string EM();
  std::string SUB();
  std::string DEL();

  std::string SP();

  std::string IND();
  std::string NEL();
  std::string HTS();
  std::string RI();
  std::string SS2();
  std::string SS3();
  std::string DCS();
  std::string SPA();
  std::string EPA();
  std::string SOS();
  std::string DECID();

  std::string windowOpDeiconify();
  std::string windowOpIconify();
  std::string windowOpMove(int x, int y);
  std::string windowOpPixelSize(int w, int h);
  std::string windowOpRaise();
  std::string windowOpLower();
  std::string windowOpRefresh();
  std::string windowOpCharSize(int w, int h);
  std::string windowOpRestoreMaximized();
  std::string windowOpMaximize();
  std::string windowOpReportState();
  std::string windowOpReportPos();
  std::string windowOpReportPixelSize();
  std::string windowOpReportCharSize();
  std::string windowOpReportScreenSize();
  std::string windowOpReportIconLabel();
  std::string windowOpReportWindowTitle();
  std::string windowOpResizeNLines(int n);

  std::string windowOp(WindowOp op, const std::string &arg1="", const std::string &arg2="");

  bool decodeWindowOp(const std::string &str, WindowOp &op, std::string &arg1, std::string &arg2);

  std::string oscIconWindowTitle(const std::string &str);
  std::string oscIconTitle(const std::string &str);
  std::string oscWindowTitle(const std::string &str);
  std::string oscWindowProp(const std::string &str);
  std::string oscColor(int num, const std::string &str);
  std::string oscFg(const std::string &str);
  std::string oscBg(const std::string &str);
  std::string oscCursorColor(const std::string &str);
  std::string oscFont(const std::string &str);

  std::string DECSC();
  std::string DECRC();
  std::string DECPAM();
  std::string DECPNM();

  std::string RIS();

  std::string DECALN();

  std::string ICH(int n=-1);
  std::string CUU(int n=-1);
  std::string CUD(int n=-1);
  std::string CUF(int n=-1);
  std::string CUB(int n=-1);
  std::string CNL(int n=-1);
  std::string CPL(int n=-1);
  std::string CHA(int n=-1);
  std::string CUP(int row=-1, int col=-1);
  std::string CHT(int n=-1);
  std::string ED(int n=-1);
  std::string DECSED(int n=-1);
  std::string EL(int n=-1);
  std::string DECSEL(int n=-1);
  std::string IL(int n=-1);
  std::string DL(int n=-1);
  std::string DCH(int n=-1);
  std::string SU(int n=-1);
  std::string SD(int n=-1);
  std::string ECH(int n=-1);
  std::string CBT(int n=-1);
  std::string HPA(int n=-1);
  std::string REP(int n=-1);
  std::string DA1(int n=-1);
  std::string DA2(int n=-1);
  std::string VPA(int n=-1);
  std::string HVP(int row=-1, int col=-1);
  std::string TBC(int n=-1);
  std::string SM(int n=-1);
  std::string DECSET(int n=-1);
  std::string DECSET(int n1, int n2);
  std::string MC(int n=-1);
  std::string DECMC(int n=-1);
  std::string RM(int n=-1);
  std::string DECRST(int n=-1);
  std::string DECRST(int n1, int n2);
  std::string SGR(int n=-1);
  std::string SGR(int n, int r, int g, int b);
  std::string DSR(int n=-1);
  std::string DECDSR(int n=-1);
  std::string DECSTR();
  std::string DECSCL(int n=-1, int m=1);
  std::string DECSTBM(int top=-1, int bottom=-1);
  std::string DECCARA(int top=-1, int left=-1, int bottom=-1, int right=-1, int attr=-1);
  std::string SC();
  std::string DECRARA(int top=-1, int left=-1, int bottom=-1, int right=-1, int attr=-1);
  std::string SC1();
  std::string DECCRA(int src_top=-1, int src_left=-1, int src_bottom=-1, int src_right=-1,
                     int src=-1, int dst_top=-1, int dst_left=-1, int dst=-1);
  std::string DECEFR(int top=-1, int left=-1, int bottom=-1, int right=-1);
  std::string DECREQTPARM(int n=-1);
  std::string DECSACE(int n=-1);
  std::string DECFRA(int c=-1, int top=-1, int left=-1, int bottom=-1, int right=-1);
  std::string DECELR(int n1=-1, int n2=-1);
  std::string DECERA(int top=-1, int left=-1, int bottom=-1, int right=-1);
  std::string DECSLE(int n=-1);
  std::string DECSERA(int top=-1, int left=-1, int bottom=-1, int right=-1);
  std::string DECRQLP(int n=-1);

  void        APC(std::ostream &os, const std::string &str);
  std::string APC(const std::string &str);

  void        CSI(std::ostream &os, const std::string &str);
  std::string CSI(const std::string &str);

  std::string stringToEscape(const std::string &str);

  std::string stringWinOpToEscape(const std::vector<std::string> &words);
  std::string stringOSCOpToEscape(const std::vector<std::string> &words);
  std::string stringCSIOpToEscape(const std::vector<std::string> &words);
  std::string stringSCSOpToEscape(const std::vector<std::string> &words);

  std::string imageToEscape(const std::string &filename, int size, int x1, int y1, int x2, int y2);
  std::string imageToEscape(const std::string &filename, int size);

  std::string pixelPointToEscape(int x, int y, const std::string &color);
  std::string pixelLineToEscape(int x1, int y1, int x2, int y2, const std::string &color);

  std::string linkToEscape(const std::string &name, const std::string &path,
                           const std::string &type);

  std::string commandToEscape(const std::string &name, const std::string &path, bool start);

  bool getWindowCharSize(int *rows, int *cols);
  bool getWindowPixelSize(int *width, int *height);

  std::string readResult();

  bool parseEscape(const std::string &str, std::vector<std::string> &args);

  bool parseMouse(const std::string &str, int *button, int *x, int *y, bool *release);
};

#endif
