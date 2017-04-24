#ifndef CESCAPE_STATE_H
#define CESCAPE_STATE_H

// terminal state
class CEscapeState {
 public:
  struct Point {
    int x { 0 };
    int y { 0 };

    Point(int x=0, int y=0) :
     x(x), y(y) {
    }
  };

  typedef std::vector<Point> Points;

 public:
  CEscapeState() {
    reset();
  }

  void reset() {
    scroll_bottom_key_ = true;
    scroll_bottom_tty_ = true;
    line_wrap_         = true;
    cursor_visible_    = true;
    cursor_blink_      = true;
    allow_80_132_      = true;
  }

  bool getInverseVideo() const { return inverse_video_; }
  void setInverseVideo(bool flag) { inverse_video_ = flag; }

  bool getSendMousePress() const { return send_mouse_press_; }
  void setSendMousePress(bool flag) { send_mouse_press_ = flag; }

  bool getSendMouseRelease() const { return send_mouse_release_; }
  void setSendMouseRelease(bool flag) { send_mouse_release_ = flag; }

  bool getSendMouseMotion() const { return send_mouse_motion_; }
  void setSendMouseMotion(bool flag) { send_mouse_motion_ = flag; }

  bool getSendFocusInOut() const { return send_focus_; }
  void setSendFocusInOut(bool flag) { send_focus_ = flag; }

  bool getScrollBottomOnKey() const { return scroll_bottom_key_; }
  void setScrollBottomOnKey(bool flag) { scroll_bottom_key_ = flag; }

  bool getScrollBottomOnTty() const { return scroll_bottom_tty_; }
  void setScrollBottomOnTty(bool flag) { scroll_bottom_tty_ = flag; }

  bool getApplicationCursorKeys() const { return app_cursor_keys_; }
  void setApplicationCursorKeys(bool flag) { app_cursor_keys_ = flag; }

  bool getInsertMode() const { return insert_mode_; }
  void setInsertMode(bool flag) { insert_mode_ = flag; }

  bool getLineWrap() const { return line_wrap_; }
  void setLineWrap(bool flag) { line_wrap_ = flag; }

  bool getAnsiVT52Mode() const { return ansi_vt52_mode_; }
  void setAnsiVT52Mode(bool flag) { ansi_vt52_mode_ = flag; }

  bool getTek4014() const { return tek_4014_; }
  void setTek4014(bool flag) { tek_4014_ = flag; }

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

  bool getControl8Bit() const { return control8_; }
  void setControl8Bit(bool flag) { control8_ = flag; }

  void clearPoints() { points_.clear(); }
  void addPoint(int x, int y) { points_.push_back(Point(x, y)); }
  const Points &points() const { return points_; }

  CEscapeLineStyle getLineStyle() const { return lineStyle_; }
  void setLineStyle(CEscapeLineStyle style) { lineStyle_ = style; }

 private:
  bool             inverse_video_      { false };
  bool             send_mouse_press_   { false };
  bool             send_mouse_release_ { false };
  bool             send_mouse_motion_  { false };
  bool             send_focus_         { false };
  bool             scroll_bottom_key_  { false };
  bool             scroll_bottom_tty_  { false };
  bool             app_cursor_keys_    { false };
  bool             insert_mode_        { false };
  bool             line_wrap_          { false };
  bool             ansi_vt52_mode_     { false };
  bool             tek_4014_           { false };
  bool             key_pad_mode_       { false };
  bool             lf_nl_mode_         { false };
  bool             ff_np_mode_         { false };
  bool             smooth_scroll_      { false };
  bool             origin_mode_        { false }; // origin is top of scroll area (true)
                                                  // or (0,0) false
  bool             auto_repeat_        { false };
  bool             cursor_visible_     { false };
  bool             cursor_blink_       { false };
  bool             reverse_wrap_       { false };
  bool             allow_80_132_       { false };
  bool             control8_           { false };
  CEscapeLineStyle lineStyle_          { CEscapeLineStyle::SOLID };
  Points           points_;
};

#endif
