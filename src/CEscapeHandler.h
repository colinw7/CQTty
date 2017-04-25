#ifndef CESCAPE_HANDLER_H
#define CESCAPE_HANDLER_H

#include <CEscapeParse.h>
#include <CEscapeState.h>
#include <CCellStyle.h>
#include <CTextTabs.h>

class CWindow;

//------

// class to handle and interpret escape text.
// The escape characters are interpreted as operations on a character based screen
class CEscapeHandler : public CEscapeParse {
 protected:
  // scoll area
  class ScrollArea {
   public:
    ScrollArea() { }

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

   private:
    bool is_set_    { false };
    uint start_row_ { 0 };
    uint end_row_   { 0 };
  };

  //---

  // char set
  struct CharSet {
    int  num { 0 }; // 0-3
    char id[4];     // char set ids

    CharSet() {
      memset(id, 'B', sizeof(id));
    }
  };

  //---

  // save cursor
  struct SaveCursor {
    int          row { 0 };
    int          col { 0 };
    CEscapeState state;
    CCellStyle   style;
    CharSet      charset;

    SaveCursor() { }
  };

  //---

 public:
  CEscapeHandler();

  virtual ~CEscapeHandler() { }

  const CEscapeState &getState() const { return state_; }
  void setState(const CEscapeState &state);

  void resetState();

  // parser interface
  void handleChar   (char c) override;
  void handleGraphic(char c) override;
  void handleEscape (const CEscapeData *esc) override;

  void log(const std::string &str) const override;

  bool isVT52() const override { return getAnsiVT52Mode(); }
  void setVT52(bool b) override { setAnsiVT52Mode(b); }

  bool is4014() const override { return getTek4014(); }
  void set4014(bool b) override { setTek4014(b); }

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
  void escapeDECDHL(CEscapeDataDECDHL::Pos pos);
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
  void escapeSSA();
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
  void escape4014(const CEscapeDataTek4014 *esc4014);

  void incOutputRow();
  void decOutputRow();

  void incOutputCol();
  void decOutputCol();

  //------

 public:
  // state
  virtual bool getInverseVideo() const { return state_.getInverseVideo(); }
  virtual void setInverseVideo(bool flag);

  virtual bool getSendMousePress() const { return state_.getSendMousePress(); }
  virtual void setSendMousePress(bool flag);

  virtual bool getSendMouseRelease() const { return state_.getSendMouseRelease(); }
  virtual void setSendMouseRelease(bool flag);

  virtual bool getSendMouseMotion() const { return state_.getSendMouseMotion(); }
  virtual void setSendMouseMotion(bool flag);

  virtual bool getSendFocusInOut() const { return state_.getSendFocusInOut(); }
  virtual void setSendFocusInOut(bool flag);

  virtual bool getScrollBottomOnKey() const { return state_.getScrollBottomOnKey(); }
  virtual void setScrollBottomOnKey(bool flag);

  virtual bool getScrollBottomOnTty() const { return state_.getScrollBottomOnTty(); }
  virtual void setScrollBottomOnTty(bool flag);

  virtual bool getApplicationCursorKeys() const { return state_.getApplicationCursorKeys(); }
  virtual void setApplicationCursorKeys(bool flag);

  virtual bool getInsertMode() const { return state_.getInsertMode(); }
  virtual void setInsertMode(bool flag);

  virtual bool getLineWrap() const { return state_.getLineWrap(); }
  virtual void setLineWrap(bool flag);

  virtual bool getAnsiVT52Mode() const { return state_.getAnsiVT52Mode(); }
  virtual void setAnsiVT52Mode(bool flag);

  virtual bool getTek4014() const { return state_.getTek4014(); }
  virtual void setTek4014(bool flag);

  virtual bool getKeyPadMode() const { return state_.getKeyPadMode(); }
  virtual void setKeyPadMode(bool flag);

  virtual bool getLfNlMode() const { return state_.getLfNlMode(); }
  virtual void setLfNlMode(bool flag);

  virtual bool getFfNpMode() const { return state_.getFfNpMode(); }
  virtual void setFfNpMode(bool flag);

  virtual bool getSmoothScroll() const { return state_.getSmoothScroll(); }
  virtual void setSmoothScroll(bool flag);

  virtual bool getOriginMode() const { return state_.getOriginMode(); }
  virtual void setOriginMode(bool flag);

  virtual bool getAutoRepeat() const { return state_.getAutoRepeat(); }
  virtual void setAutoRepeat(bool flag);

  virtual bool getCursorVisible() const { return state_.getCursorVisible(); }
  virtual void setCursorVisible(bool flag);

  virtual bool getCursorBlink() const { return state_.getCursorBlink(); }
  virtual void setCursorBlink(bool blink);

  virtual bool getReverseWrap() const { return state_.getReverseWrap(); }
  virtual void setReverseWrap(bool wrap);

  virtual bool getAllow80To132() const { return state_.getAllow80To132(); }
  virtual void setAllow80To132(bool flag);

  virtual bool getControl8Bit() const { return state_.getControl8Bit(); }
  virtual void setControl8Bit(bool flag);

  virtual CEscapeLineStyle getLineStyle() const { return state_.getLineStyle(); }
  virtual void setLineStyle(CEscapeLineStyle style);

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

  virtual void setLineWidthStyle(uint row, const CCellLineWidthStyle &lineStyle) = 0;
  virtual void setLineHeightStyle(uint row, const CCellLineHeightStyle &lineStyle) = 0;

  virtual void setCell(uint row, uint col, char c, const CCellStyle &style) = 0;

  virtual void addSizedImageChar(const std::string &path, CImageFile *file,
                                 int x1, int y1, int x2, int y2) = 0;
  virtual void addImageChar(const std::string &path, CImageFile *file,
                            int x1, int y1, int x2, int y2) = 0;
  virtual void addImage(const std::string &fileName, CImageFile *file) = 0;
  virtual void addImage(const CImagePtr &image) = 0;

  virtual void addPixel(int, int, const CEscapeColor &) { }

  virtual void addLine(int, int, int, int, const CEscapeColor &, const CEscapeLineStyle &) { }

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
  virtual void notifyEnter(char c) = 0;

  virtual void notifyStateChange() = 0;

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
  virtual CWindow *getWindow() const { return nullptr; }

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
  CEscapeState state_;
  CTextTabs    tabs_;
  ScrollArea   scroll_area_;
  CharSet      charset_;
  SaveCursor   save_cursor_;
  CRGBA        cursor_color_;
  bool         debug_ { false };
  bool         trace_ { false };
};

#endif
