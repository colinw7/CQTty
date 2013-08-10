#ifndef CCELL_STYLE_H
#define CCELL_STYLE_H

#include <CEscapeType.h>
#include <CRGB.h>
#include <CFontStyle.h>

// Individual Char Cell Style
class CCellStyle {
 private:
  // Color
  uint bg_;
  uint fg_;
  bool bg_is_rgb_;
  bool fg_is_rgb_;
  bool dim_;
  bool invert_;
  bool hidden_;

  // Font
  bool bold_;
  bool underscore_;

  bool blink_;

 public:
  CCellStyle() {
    reset();
  }

  void reset() {
    bg_ = CESCAPE_COLOR_BG;
    fg_ = CESCAPE_COLOR_FG;

    bg_is_rgb_ = false;
    fg_is_rgb_ = false;

    dim_    = false;
    invert_ = false;
    hidden_ = false;

    bold_       = false;
    underscore_ = false;

    blink_  = false;
  }

  void setFg(CEscapeColor color) {
    fg_        = color;
    fg_is_rgb_ = false;
  }

  void setBg(CEscapeColor color) {
    bg_        = color;
    bg_is_rgb_ = false;
  }

  void setFg(const CRGB &rgb) {
    fg_        = (CEscapeColor) rgb.encodeRGB();
    fg_is_rgb_ = true;
  }

  void setBg(const CRGB &rgb) {
    bg_        = (CEscapeColor) rgb.encodeRGB();
    bg_is_rgb_ = true;
  }

  bool isBgRGB() const { return bg_is_rgb_; }
  bool isFgRGB() const { return fg_is_rgb_; }

  void setDim   (bool flag) { dim_    = flag; }
  void setInvert(bool flag) { invert_ = flag; }
  void setHidden(bool flag) { hidden_ = flag; }

  void setBold      (bool flag) { bold_       = flag; }
  void setUnderscore(bool flag) { underscore_ = flag; }

  void setBlink(bool flag) { blink_ = flag; }

  CEscapeColor getBg() const {
    if (invert_)
      return getFgColor();
    else
      return getBgColor();
  }

  CEscapeColor getFg() const {
    CEscapeColor color;

    if (hidden_)
      color = getBgColor();
    else {
      if (invert_)
        color = getBgColor();
      else
        color = getFgColor();

      if (dim_)
        color = CEscapeColor(int(color) | CESCAPE_COLOR_DIM);
    }

    return color;
  }

  void getBg(CRGB &rgb) const {
    if (invert_)
      rgb = getFgRGB();
    else
      rgb = getBgRGB();
  }

  void getFg(CRGB &rgb) const {
    if (hidden_)
      rgb = getBgRGB();
    else {
      if (invert_)
        rgb = getBgRGB();
      else
        rgb = getFgRGB();

      if (dim_)
        rgb = rgb.getDarkRGB();
    }
  }

  CFontStyle getFontStyle() const {
    if (bold_) {
      if (underscore_)
        return CFONT_STYLE_BOLD_ITALIC;
      else
        return CFONT_STYLE_BOLD;
    }
    else {
      if (underscore_)
        return CFONT_STYLE_ITALIC;
      else
        return CFONT_STYLE_NORMAL;
    }
  }

  bool getBlink() const { return blink_; }

 private:
  CEscapeColor getFgColor() const {
    if (! fg_is_rgb_)
      return (CEscapeColor) fg_;
    else
      return CESCAPE_COLOR_FG;
  }

  CEscapeColor getBgColor() const {
    if (! bg_is_rgb_)
      return (CEscapeColor) bg_;
    else
      return CESCAPE_COLOR_BG;
  }

  CRGB getFgRGB() const {
    if (fg_is_rgb_)
      return CRGB::decodeRGB((uint) fg_);
    else
      return CRGB(1,1,1);
  }

  CRGB getBgRGB() const {
    if (bg_is_rgb_)
      return CRGB::decodeRGB((uint) bg_);
    else
      return CRGB(0,0,0);
  }
};

#endif
