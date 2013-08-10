#ifndef CESCAPE_HANDLER_H
#define CESCAPE_HANDLER_H

#include <CEscapeParse.h>
#include <CCellStyle.h>
#include <CTextTabs.h>
#include <CEscapeColors.h>

class CWindow;

// terminal state
class CEscapeState {
 private:
  bool inverse_video_;
  bool send_mouse_press_;
  bool send_mouse_release_;
  bool scroll_bottom_key_;
  bool scroll_bottom_tty_;
  bool application_cursor_keys_;
  bool insert_mode_;
  bool line_wrap_;
  bool ansi_vt52_mode_;
  bool key_pad_mode_;
  bool lf_nl_mode_;
  bool ff_np_mode_;
  bool smooth_scroll_;
  bool origin_mode_; // whether origin is top of scroll area (true) or (0,0) false
  bool auto_repeat_;
  bool cursor_visible_;
  bool cursor_blink_;
  bool reverse_wrap_;
  bool allow_80_132_;

 public:
  CEscapeState() {
    reset();
  }

  void reset() {
    inverse_video_           = false;
    send_mouse_press_        = false;
    send_mouse_release_      = false;
    scroll_bottom_key_       = true ;
    scroll_bottom_tty_       = true ;
    application_cursor_keys_ = false;
    insert_mode_             = false;
    line_wrap_               = true ;
    ansi_vt52_mode_          = false;
    key_pad_mode_            = false;
    lf_nl_mode_              = false;
    ff_np_mode_              = false;
    smooth_scroll_           = false;
    origin_mode_             = false;
    auto_repeat_             = false;
    cursor_visible_          = true;
    cursor_blink_            = true ;
    reverse_wrap_            = false;
    allow_80_132_            = true;
  }

  bool getInverseVideo() const { return inverse_video_; }
  void setInverseVideo(bool flag) { inverse_video_ = flag; }

  bool getSendMousePress() const { return send_mouse_press_; }
  void setSendMousePress(bool flag) { send_mouse_press_ = flag; }

  bool getSendMouseRelease() const { return send_mouse_release_; }
  void setSendMouseRelease(bool flag) { send_mouse_release_ = flag; }

  bool getScrollBottomOnKey() const { return scroll_bottom_key_; }
  void setScrollBottomOnKey(bool flag) { scroll_bottom_key_ = flag; }

  bool getScrollBottomOnTty() const { return scroll_bottom_tty_; }
  void setScrollBottomOnTty(bool flag) { scroll_bottom_tty_ = flag; }

  bool getApplicationCursorKeys() const { return application_cursor_keys_; }
  void setApplicationCursorKeys(bool flag) { application_cursor_keys_ = flag; }

  bool getInsertMode() const { return insert_mode_; }
  void setInsertMode(bool flag) { insert_mode_ = flag; }

  bool getLineWrap() const { return line_wrap_; }
  void setLineWrap(bool flag) { line_wrap_ = flag; }

  bool getAnsiVT52Mode() const { return ansi_vt52_mode_; }
  void setAnsiVT52Mode(bool flag) { ansi_vt52_mode_ = flag; }

  bool getKeyPadMode() const { return key_pad_mode_; }
  void setKeyPadMode(bool flag) { key_pad_mode_ = flag; }

  bool getLfNlMode() const { return lf_nl_mode_; }
  void setLfNlMode(bool flag) { lf_nl_mode_ = flag; }

  bool getFfNpMode() const { return ff_np_mode_; }
  void setFfNpMode(bool flag) { ff_np_mode_ = flag; }

  bool getSmoothScroll() const { return smooth_scroll_; }
  void setSmoothScroll(bool flag) { smooth_scroll_ = flag; }

  bool getOriginMode() const { return origin_mode_; }
  void setOriginMode(bool flag) { origin_mode_ = flag; }

  bool getAutoRepeat() const { return auto_repeat_; }
  void setAutoRepeat(bool flag) { auto_repeat_ = flag; }

  bool getCursorVisible() const { return cursor_visible_; }
  void setCursorVisible(bool flag) { cursor_visible_ = flag; }

  bool getCursorBlink() const { return cursor_blink_; }
  void setCursorBlink(bool blink) { cursor_blink_ = blink; }

  bool getReverseWrap() const { return reverse_wrap_; }
  void setReverseWrap(bool wrap) { reverse_wrap_ = wrap; }

  bool getAllow80To132() const { return allow_80_132_; }
  void setAllow80To132(bool flag) { allow_80_132_ = flag; }
};

// class to handle and interpret escape text.
// The escape characters are interpreted as operations on a character based screen
class CEscapeHandler : public CEscapeParse {
 protected:
  // scoll area
  struct ScrollArea {
   private:
    bool is_set_;
    uint start_row_;
    uint end_row_;

   public:
    ScrollArea() :
     is_set_(false), start_row_(0), end_row_(0) {
    }

    bool isSet() const { return is_set_; }

    int getTop   () const { return start_row_; }
    int getBottom() const { return end_row_  ; }

    void set(int start_row, int end_row) {
      assert(start_row < end_row);

      is_set_    = true;
      start_row_ = start_row;
      end_row_   = end_row;
    }

    void clear() {
      is_set_ = false;
    }
  };

  // char set
  struct CharSet {
    int  num;   // 0-3
    char id[4]; // char set ids

    CharSet() :
     num(0) {
      memset(id, 'B', sizeof(id));
    }
  };

  // save cursor
  struct SaveCursor {
    int          row;
    int          col;
    CEscapeState state;
    CCellStyle   style;
    CharSet      charset;

    SaveCursor() :
     row(0), col(0) {
    }
  };

 public:
  CEscapeHandler();

  virtual ~CEscapeHandler() { }

  const CEscapeState &getState() const { return state_; }

  void setState(const CEscapeState &state) { state_ = state; }

  void resetState() { state_.reset(); }

  // parser interface
  void handleChar   (char c);
  void handleGraphic(char c);
  void handleEscape (const CEscapeData *esc);

  void log(const std::string &str) const;

 protected:
  void escapeICH(int num);

  void escapeCUU(int num);
  void escapeCUD(int num);
  void escapeCUF(int num);
  void escapeCUB(int num);
  void escapeCNL(int num);
  void escapeCPL(int num);
  void escapeCHA(int num);
  void escapeCUP(int row, int col);
  void escapeCHT(int num);
  void escapeDECSED(int num);
  void escapeDECSACE(int num1, int num2, int num3);
  void escapeED(int num);
  void escapeDECSEL(int num);
  void escapeEL(int num);
  void escapeIL(int num);
  void escapeDL(int num);
  void escapeDCH(int num);
  void escapeSU(int num);
  void escapeSD(int num);
  void escapeECH(int num);
  void escapeCBT(int num);
  void escapeHPA(int num);
  void escapeVPA(int num);
  void escapeHVP(int row, int col);
  void escapeDispPos(int row, int col);
  void escapeREP(int num);
  void escapeTBC(int num);
  void escapeSM(int num);
  void escapeMC(int num);
  void escapeDECMC(int num);
  void escapeRM(int num);
  void escapeDECSET(int num);
  void escapeDECRST(int num);
  void escapeSGR(const CEscapeDataSGR *sgr);
  void escapeDECID();
  void escapeDA1(int *num, int nn);
  void escapeDA2(int *num, int nn);
  void escapeDSR(int num);
  void escapeDECSTBM(int top, int bottom);
  void escapeDECREQTPARM(int num);
  void escapeDECTST(int num);
  void escapeWindowManip(int *num);
  void escapeDECCARA(int t, int l, int b, int r, int m);
  void escapeDECRARA(int t, int l, int b, int r, int m);
  void escapeDECCRA(int st, int sl, int sb, int sr, int dt, int dl, int db, int dr);
  void escapeDECFRA(int c, int t, int l, int b, int r);
  void escapeDECERA(int t, int l, int b, int r);
  void escapeDECSERA(int t, int l, int b, int r);
  void escapeS7C1T();
  void escapeS8C1T();
  void escapeANSIConformance(int mode);
  void escapeDECDHLTop();
  void escapeDECDHLBottom();
  void escapeDECSWL();
  void escapeDECDWL();
  void escapeDECALN();
  void escapeDECSC();
  void escapeSC();
  void escapeDECRC();
  void escapeRC();
  void escapeDECPAM();
  void escapeDECPNM();
  void escapeRIS();
  void escapeMemoryLock();
  void escapeMemoryUnlock();
  void escapeLS2();
  void escapeLS3();
  void escapeISO8859_1();
  void escapeUTF_8();
  void escapeDesignateG0(char c);
  void escapeDesignateG1(char c);
  void escapeDesignateG2(char c);
  void escapeDesignateG3(char c);
  void escapeLS1R();
  void escapeLS2R();
  void escapeLS3R();
  void escapeIND();
  void escapeNEL();
  void escapeESA();
  void escapeHTS();
  void escapeRI();
  void escapeSS2();
  void escapeSS3();
  void escapeCCH();
  void escapeSPA();
  void escapeEPA();
  void escapeSOS();
  void escapeDECDSR(int mode);
  void escapeDECSTR();
  void escapeDECSCL(int num1, int num2);
  void escapeDECSCA(int mode);
  void escapeDECLL(int mode);
  void escapeDECEFR(int t, int b, int l, int r);
  void escapeDECLRP(int t, int b, int l, int r);
  void escapeDECELR(int num1, int num2);
  void escapeDECSLE(int mode);
  void escapeDECRQLP(int mode);
  void escapeDECCKM(bool set);
  void escapeDECANM(bool set);
  void escapeDECCOLM(bool set);
  void escapeDECSLPP(int num_rows);
  void escapeDECSCLM(bool set);
  void escapeDECSCNM(bool set);
  void escapeDECOM(bool set);
  void escapeDECAWM(bool set);
  void escapeDECARM(bool set);
  void escapeDECPFF(bool set);
  void escapeDECPEX(bool set);
  void escapeDECTCEM(bool set);
  void escapeDECTEK(bool set);
  void escapeDECNRCM(bool set);
  void escapeDECNKM(bool set);
  void escapeDECBKM(bool set);
  void escapeAM(bool set);
  void escapeIRM(bool set);
  void escapeSRM(bool set);
  void escapeLNM(bool set);
  void escapeDECSavePriv(int *modes, int num_modes);
  void escapeDECRestorePriv(int *modes, int num_modes);
  void escapeStartMouseTrack(int func, int x, int y, int first, int last);
  void escapeOSC(int num, const std::string &str1);

  void incOutputRow();
  void decOutputRow();

  void incOutputCol();
  void decOutputCol();

  //------

 public:
  // state
  virtual bool getInverseVideo() const { return state_.getInverseVideo(); }
  virtual void setInverseVideo(bool flag) { state_.setInverseVideo(flag); }

  virtual bool getSendMousePress() const { return state_.getSendMousePress(); }
  virtual void setSendMousePress(bool flag) { state_.setSendMousePress(flag); }

  virtual bool getSendMouseRelease() const { return state_.getSendMouseRelease(); }
  virtual void setSendMouseRelease(bool flag) { state_.setSendMouseRelease(flag); }

  virtual bool getScrollBottomOnKey() const { return state_.getScrollBottomOnKey(); }
  virtual void setScrollBottomOnKey(bool flag) { state_.setScrollBottomOnKey(flag); }

  virtual bool getScrollBottomOnTty() const { return state_.getScrollBottomOnTty(); }
  virtual void setScrollBottomOnTty(bool flag) { state_.setScrollBottomOnTty(flag); }

  virtual bool getApplicationCursorKeys() const { return state_.getApplicationCursorKeys(); }
  virtual void setApplicationCursorKeys(bool flag) { state_.setApplicationCursorKeys(flag); }

  virtual bool getInsertMode() const { return state_.getInsertMode(); }
  virtual void setInsertMode(bool flag) { state_.setInsertMode(flag); }

  virtual bool getLineWrap() const { return state_.getLineWrap(); }
  virtual void setLineWrap(bool flag) { state_.setLineWrap(flag); }

  virtual bool getAnsiVT52Mode() const { return state_.getAnsiVT52Mode(); }
  virtual void setAnsiVT52Mode(bool flag) { state_.setAnsiVT52Mode(flag); }

  virtual bool getKeyPadMode() const { return state_.getKeyPadMode(); }
  virtual void setKeyPadMode(bool flag) { state_.setKeyPadMode(flag); }

  virtual bool getLfNlMode() const { return state_.getLfNlMode(); }
  virtual void setLfNlMode(bool flag) { state_.setLfNlMode(flag); }

  virtual bool getFfNpMode() const { return state_.getFfNpMode(); }
  virtual void setFfNpMode(bool flag) { state_.setFfNpMode(flag); }

  virtual bool getSmoothScroll() const { return state_.getSmoothScroll(); }
  virtual void setSmoothScroll(bool flag) { state_.setSmoothScroll(flag); }

  virtual bool getOriginMode() const { return state_.getOriginMode(); }
  virtual void setOriginMode(bool flag) { state_.setOriginMode(flag); }

  virtual bool getAutoRepeat() const { return state_.getAutoRepeat(); }
  virtual void setAutoRepeat(bool flag) { state_.setAutoRepeat(flag); }

  virtual bool getCursorVisible() const { return state_.getCursorVisible(); }
  virtual void setCursorVisible(bool flag) { state_.setCursorVisible(flag); }

  virtual bool getCursorBlink() const { return state_.getCursorBlink(); }
  virtual void setCursorBlink(bool blink) { state_.setCursorBlink(blink); }

  virtual bool getReverseWrap() const { return state_.getReverseWrap(); }
  virtual void setReverseWrap(bool wrap) { state_.setReverseWrap(wrap); }

  virtual bool getAllow80To132() const { return state_.getAllow80To132(); }
  virtual void setAllow80To132(bool flag) { state_.setAllow80To132(flag); }

  //------

  // tabs
  virtual void nextTab();
  virtual void prevTab();
  virtual void setTab(uint col);
  virtual void clearTab(uint col);
  virtual void clearTabAll();

  //------

  // scroll area
  virtual bool hasScrollArea() const { return scroll_area_.isSet(); }

  virtual void setScrollArea(int start, int end) { scroll_area_.set(start, end); }
  virtual void resetScrollArea() { scroll_area_.clear(); }

  virtual uint getScrollBottom() const;
  virtual uint getScrollTop() const;

  //------

  // save/restore

  virtual void saveCursor();
  virtual void restoreCursor();

  //------

  // char set

  virtual char getCharSet(int id) const;

  //------

  // colors

  virtual const CRGBA &getColor(CEscapeColor color) const;
  virtual void         setColor(CEscapeColor color, const CRGBA &rgba);

  virtual const CRGBA &getCursorColor() const;
  virtual void         setCursorColor(const CRGBA &rgba);

  //------

  // mouse

  virtual void sendMousePress  (int button, int x, int y);
  virtual void sendMouseRelease(int button, int x, int y);

  //------

  // interface to be implemented
 public:
  // Data Position Get/Set
  virtual const CIPoint2D &getDataPos() const = 0;
  virtual void setDataPos(const CIPoint2D &pos) = 0;

  // Display Position Get/Set
  virtual void getDispPos(int *row, int *col) const = 0;
  virtual void setDispPos(int row, int col) = 0;

  virtual void setDispRow(int row) = 0;
  virtual void setDispCol(int col) = 0;

  virtual void setDispSize(uint num_rows, uint num_cols) = 0;
  virtual void getDispSize(uint *num_rows, uint *num_cols) const = 0;

  virtual void resetAll() = 0;

  virtual void setCell(uint row, uint col, char c, const CCellStyle &style) = 0;

  virtual void addSizedImageChar(const std::string &path, CImageFile *file,
                                 int x1, int y1, int x2, int y2) = 0;
  virtual void addImageChar(const std::string &path, CImageFile *file,
                            int x1, int y1, int x2, int y2) = 0;
  virtual void addImage(const std::string &fileName, CImageFile *file) = 0;

  virtual void addPixel(int, int, const std::string &) { }

  virtual void addLine(int, int, int, int, const std::string &) { }

  virtual void setLinkCell(uint row, uint col, char c, const CCellStyle &style,
                           const std::string &linkName, const std::string &linkDest) = 0;
  virtual void addLink(const std::string &name, const std::string &path,
                       const std::string &type) = 0;

  virtual void resetCell() = 0;

  virtual void pixelsToChars(int x, int y, int *col, int *row) = 0;
  virtual void charsToPixels(int col, int row, int *x, int *y) = 0;

  virtual uint getNumDispRows() const = 0;
  virtual uint getNumDispCols() const = 0;

  virtual uint getNumDataRows() const = 0;
  virtual uint getNumDataCols() const = 0;

  virtual void shiftLeft () = 0;
  virtual void shiftRight() = 0;

  virtual const CCellStyle &getStyle() const = 0;

  virtual void setStyle(const CCellStyle &style) = 0;

  virtual void notifyChar(uint x, uint y, char c) = 0;
  virtual void notifyEnter() = 0;

  virtual void paste(const std::string &str) = 0;

  virtual void setTrace(bool trace) { trace_ = trace; }
  virtual bool getTrace() const { return trace_; }

  virtual void setDebug(bool debug) { debug_ = debug; }
  virtual bool getDebug() const { return true; }

  virtual void setAlternative(bool flag) = 0;

  //! insert N lines at the current position
  virtual void insertLine(uint num) = 0;
  //! delete N lines at the current position
  virtual void deleteLine(uint num) = 0;

  virtual void replicatePrevChar() = 0;

  virtual void eraseLineLeft() = 0;
  virtual void eraseLineRight() = 0;
  virtual void eraseLineAll() = 0;

  virtual void scrollUp() = 0;
  virtual void scrollDown() = 0;

  virtual void clearScreenBelow() = 0;
  virtual void clearScreenAbove() = 0;
  virtual void clearScreen() = 0;
  virtual void clearScreenSaved() = 0;

  virtual void fill(char c) = 0;

  virtual void startPrintLog() = 0;
  virtual void stopPrintLog() = 0;
  virtual void printLine() = 0;
  virtual void printScreen() = 0;
  virtual void printComposedScreen() = 0;
  virtual void printAllPages() = 0;

  // window manipulation
  virtual CWindow *getWindow() const { return NULL; }

  virtual void setWindowTitle(const std::string &) { }
  virtual void getWindowTitle(std::string &title) { title = ""; }
  virtual void setIconTitle(const std::string &) { }
  virtual void getIconTitle(std::string &title) { title = ""; }
  virtual void setWindowProperty(const std::string &, const std::string &) { }
  virtual void moveWindow(int, int) { }
  virtual void resizeWindow(uint, uint) { }
  virtual void lowerWindow() { }
  virtual void raiseWindow() { }
  virtual void exposeWindow() { }
  virtual void maximizeWindow() { }
  virtual void demaximizeWindow() { }
  virtual void iconizeWindow() { }
  virtual void deiconizeWindow() { }
  virtual bool isWindowMapped() { return true; }
  virtual void getWindowPosition(int *x, int *y) { *x = 0; *y = 0; }
  virtual void getWindowSize(uint *w, uint *h) { *w = 100; *h = 100; }
  virtual void getScreenSize(uint *w, uint *h) { *w = 1000; *h = 1000; }

  virtual void beep() = 0;

  virtual void setShowToolBar  (bool show) = 0;
  virtual void setShowScrollBar(bool show) = 0;
  virtual void setShowStatusBar(bool show) = 0;

  virtual void previewFile(const std::string &fileName) = 0;
  virtual void setDirName(const std::string &dirName) = 0;

  virtual void processString(const char *str) = 0;

  virtual void logDebug(const std::string &str) const = 0;
  virtual void logTrace(char c) const;
  virtual void logTrace(const std::string &str) const = 0;

 private:
  // private data
  CEscapeState  state_;
  CTextTabs     tabs_;
  ScrollArea    scroll_area_;
  CharSet       charset_;
  SaveCursor    save_cursor_;
  CEscapeColors colors_;
  CRGBA         cursor_color_;
  bool          debug_;
  bool          trace_;
};

#endif
