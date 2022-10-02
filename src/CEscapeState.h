#ifndef CESCAPE_STATE_H
#define CESCAPE_STATE_H

// terminal state
class CEscapeState {
 public:
  struct Point {
    int x { 0 };
    int y { 0 };

    Point() { }

    Point(int x1, int y1) :
     x(x1), y(y1) {
    }
  };

  typedef std::vector<Point> Points;

 public:
  CEscapeState() {
    reset();
  }

  void reset() {
    scrollBottomKey_    = true;
    scrollBottomTty_    = true;
    bracketedPasteMode_ = false;
    lineWrap_           = true;
    cursorVisible_      = true;
    cursorBlink_        = true;
    allow80132_         = true;
  }

  bool getInverseVideo() const { return inverseVideo_; }
  void setInverseVideo(bool flag) { inverseVideo_ = flag; }

  bool getSendMousePress() const { return sendMousePress_; }
  void setSendMousePress(bool flag) { sendMousePress_ = flag; }

  bool getSendMouseRelease() const { return sendMouseRelease_; }
  void setSendMouseRelease(bool flag) { sendMouseRelease_ = flag; }

  bool getSendMouseMotion() const { return sendMouseMotion_; }
  void setSendMouseMotion(bool flag) { sendMouseMotion_ = flag; }

  bool getSendFocusInOut() const { return sendFocus_; }
  void setSendFocusInOut(bool flag) { sendFocus_ = flag; }

  bool getScrollBottomOnKey() const { return scrollBottomKey_; }
  void setScrollBottomOnKey(bool flag) { scrollBottomKey_ = flag; }

  bool getScrollBottomOnTty() const { return scrollBottomTty_; }
  void setScrollBottomOnTty(bool flag) { scrollBottomTty_ = flag; }

  bool getBracketedPasteMode() const { return bracketedPasteMode_; }
  void setBracketedPasteMode(bool flag) { bracketedPasteMode_ = flag; }

  bool getApplicationCursorKeys() const { return appCursorKeys_; }
  void setApplicationCursorKeys(bool flag) { appCursorKeys_ = flag; }

  bool getInsertMode() const { return insertMode_; }
  void setInsertMode(bool flag) { insertMode_ = flag; }

  bool getLineWrap() const { return lineWrap_; }
  void setLineWrap(bool flag) { lineWrap_ = flag; }

  bool getAnsiVT52Mode() const { return ansiVt52Mode_; }
  void setAnsiVT52Mode(bool flag) { ansiVt52Mode_ = flag; }

  bool getTek4014() const { return tek4014_; }
  void setTek4014(bool flag) { tek4014_ = flag; }

  bool getKeyPadMode() const { return keyPadMode_; }
  void setKeyPadMode(bool flag) { keyPadMode_ = flag; }

  bool getLfNlMode() const { return lfNlMode_; }
  void setLfNlMode(bool flag) { lfNlMode_ = flag; }

  bool getFfNpMode() const { return ffNpMode_; }
  void setFfNpMode(bool flag) { ffNpMode_ = flag; }

  bool getSmoothScroll() const { return smoothScroll_; }
  void setSmoothScroll(bool flag) { smoothScroll_ = flag; }

  bool getOriginMode() const { return originMode_; }
  void setOriginMode(bool flag) { originMode_ = flag; }

  bool getAutoRepeat() const { return autoRepeat_; }
  void setAutoRepeat(bool flag) { autoRepeat_ = flag; }

  bool getCursorVisible() const { return cursorVisible_; }
  void setCursorVisible(bool flag) { cursorVisible_ = flag; }

  bool getCursorBlink() const { return cursorBlink_; }
  void setCursorBlink(bool blink) { cursorBlink_ = blink; }

  bool getReverseWrap() const { return reverseWrap_; }
  void setReverseWrap(bool wrap) { reverseWrap_ = wrap; }

  bool getAllow80To132() const { return allow80132_; }
  void setAllow80To132(bool flag) { allow80132_ = flag; }

  bool getControl8Bit() const { return control8_; }
  void setControl8Bit(bool flag) { control8_ = flag; }

  //---

  int get4014GIN() const { return data4014_.gin; }
  void set4014GIN(bool b) { data4014_.gin = b; }

  int get4014CharSet() const { return data4014_.charSet; }
  void set4014CharSet(int i) { data4014_.charSet = i; }

  void clear4014Points() { data4014_.points.clear(); }
  void add4014Point(int x, int y) { data4014_.points.push_back(Point(x, y)); }
  const Points &get4014Points() const { return data4014_.points; }

  CEscapeLineStyle get4014LineStyle() const { return data4014_.lineStyle; }
  void set4014LineStyle(CEscapeLineStyle style) { data4014_.lineStyle = style; }

 private:
  struct Data4014 {
    bool             gin       { false };
    int              charSet   { 1 };
    CEscapeLineStyle lineStyle { CEscapeLineStyle::SOLID };
    Points           points;
  };

  bool     inverseVideo_       { false };
  bool     sendMousePress_     { false };
  bool     sendMouseRelease_   { false };
  bool     sendMouseMotion_    { false };
  bool     sendFocus_          { false };
  bool     scrollBottomKey_    { false };
  bool     scrollBottomTty_    { false };
  bool     bracketedPasteMode_ { false };
  bool     appCursorKeys_      { false };
  bool     insertMode_         { false };
  bool     lineWrap_           { false };
  bool     ansiVt52Mode_       { false };
  bool     tek4014_            { false };
  bool     keyPadMode_         { false };
  bool     lfNlMode_           { false };
  bool     ffNpMode_           { false };
  bool     smoothScroll_       { false };
  bool     originMode_         { false }; // origin is top of scroll area (true)
                                        // or (0,0) false
  bool     autoRepeat_         { false };
  bool     cursorVisible_      { true  };
  bool     cursorBlink_        { false };
  bool     reverseWrap_        { false };
  bool     allow80132_         { false };
  bool     control8_           { false };
  Data4014 data4014_;
};

#endif
