#ifndef CCELL_STYLE_H
#define CCELL_STYLE_H

#include <CEscapeType.h>
#include <CRGB.h>
#include <CFontStyle.h>

enum class CCellLineWidthStyle {
  SINGLE,
  DOUBLE
};

enum class CCellLineHeightStyle {
  SINGLE,
  DOUBLE_TOP,
  DOUBLE_BOTTOM
};

// Individual Char Cell Style
class CCellStyle {
 public:
  CCellStyle() {
    reset();
  }

  void reset() {
    bg_ = uint(CEscapeColor::BG);
    fg_ = uint(CEscapeColor::FG);

    bg_is_rgb_ = false;
    fg_is_rgb_ = false;

    dim_    = false;
    invert_ = false;
    hidden_ = false;

    bold_       = false;
    italic_     = false;
    underscore_ = false;
    strikeout_  = false;

    blink_  = false;
  }

  void setFg(CEscapeColor color) {
    fg_        = uint(color);
    fg_is_rgb_ = false;
  }

  void setBg(CEscapeColor color) {
    bg_        = uint(color);
    bg_is_rgb_ = false;
  }

  void setFg(const CRGB &rgb) {
    fg_        = rgb.encodeRGB();
    fg_is_rgb_ = true;
  }

  void setBg(const CRGB &rgb) {
    bg_        = rgb.encodeRGB();
    bg_is_rgb_ = true;
  }

  bool isBgRGB() const { return bg_is_rgb_; }
  bool isFgRGB() const { return fg_is_rgb_; }

  bool isDim() const { return dim_; }
  void setDim(bool flag) { dim_ = flag; }

  bool isInvert() const { return invert_; }
  void setInvert(bool flag) { invert_ = flag; }

  bool isHidden() const { return hidden_; }
  void setHidden(bool flag) { hidden_ = flag; }

  bool isBold() const { return bold_; }
  void setBold(bool flag) { bold_ = flag; }

  bool isItalic() const { return italic_; }
  void setItalic(bool flag) { italic_ = flag; }

  bool isUnderscore() const { return underscore_; }
  void setUnderscore(bool flag) { underscore_ = flag; }

  bool isStrikeout() const { return strikeout_; }
  void setStrikeout(bool flag) { strikeout_ = flag; }

  bool isBlink() const { return blink_; }
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
        color = CEscapeColor(uint(color) | uint(CEscapeColor::DIM));
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
    if (hidden_) {
      rgb = getBgRGB();
    }
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
    CFontStyle style = CFONT_STYLE_NORMAL;

    if      (bold_ && italic_)
      style = CFONT_STYLE_BOLD_ITALIC;
    else if (bold_)
      style = CFONT_STYLE_BOLD;
    else if (italic_)
      style = CFONT_STYLE_ITALIC;
    else
      style = CFONT_STYLE_NORMAL;

    //---

    if (underscore_)
      style = CFontStyle(uint(style) | uint(CFONT_STYLE_UNDERLINE));

    if (strikeout_)
      style = CFontStyle(uint(style) | uint(CFONT_STYLE_STRIKEOUT));

    return style;
  }

  bool getBlink() const { return blink_; }

 private:
  CEscapeColor getFgColor() const {
    if (! fg_is_rgb_)
      return static_cast<CEscapeColor>(fg_);
    else
      return CEscapeColor::FG;
  }

  CEscapeColor getBgColor() const {
    if (! bg_is_rgb_)
      return static_cast<CEscapeColor>(bg_);
    else
      return CEscapeColor::BG;
  }

  CRGB getFgRGB() const {
    if (fg_is_rgb_)
      return CRGB::decodeRGB(static_cast<uint>(fg_));
    else
      return CRGB(1,1,1);
  }

  CRGB getBgRGB() const {
    if (bg_is_rgb_)
      return CRGB::decodeRGB(static_cast<uint>(bg_));
    else
      return CRGB(0,0,0);
  }

 private:
  // Color
  uint bg_ { 0 };
  uint fg_ { 0 };

  bool bg_is_rgb_ { false };
  bool fg_is_rgb_ { false };

  bool dim_    { false };
  bool invert_ { false };
  bool hidden_ { false };

  // Font
  bool bold_       { false };
  bool italic_     { false };
  bool underscore_ { false };
  bool strikeout_  { false };

  // Misc
  bool blink_ { false };
};

#endif
