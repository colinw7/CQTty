#ifndef CESCAPE_COLORS_H
#define CESCAPE_COLORS_H

#include <CEscapeType.h>
#include <CRGBA.h>

struct CEscapeColors {
 private:
  enum { MAX_COLORS = 8 };

  CRGBA bg_, fg_;
  CRGBA colors_[8];

 public:
  CEscapeColors() {
    bg_ = CRGBA(0,0,0);
    fg_ = CRGBA(1,1,1);

    colors_[CESCAPE_COLOR_BLACK  ] = CRGBA(0,0,0);
    colors_[CESCAPE_COLOR_RED    ] = CRGBA(1,0,0);
    colors_[CESCAPE_COLOR_GREEN  ] = CRGBA(0,1,0);
    colors_[CESCAPE_COLOR_YELLOW ] = CRGBA(1,1,0);
    colors_[CESCAPE_COLOR_BLUE   ] = CRGBA(0,0,1);
    colors_[CESCAPE_COLOR_MAGENTA] = CRGBA(1,0,1);
    colors_[CESCAPE_COLOR_CYAN   ] = CRGBA(0,1,1);
    colors_[CESCAPE_COLOR_WHITE  ] = CRGBA(1,1,1);
  }

  const CRGBA &getBg() const { return bg_; }
  const CRGBA &getFg() const { return fg_; }

  void setBg(const CRGBA &bg) { bg_ = bg; }
  void setFg(const CRGBA &fg) { fg_ = fg; }

  const CRGBA &getColor(CEscapeColor type) const {
    static CRGBA c;

    bool dim = (type & CESCAPE_COLOR_DIM);

    if      (type == CESCAPE_COLOR_BG || type == (CESCAPE_COLOR_BG | CESCAPE_COLOR_DIM))
      c = getBg();
    else if (type == CESCAPE_COLOR_FG || type == (CESCAPE_COLOR_FG | CESCAPE_COLOR_DIM))
      c = getFg();
    else {
      uint itype = (type & 0x07);

      assert(itype < MAX_COLORS);

      c = colors_[itype];
    }

    if (dim)
      c *= 0.8;

    return c;
  }

  void setColor(CEscapeColor type, const CRGBA &rgba) {
    if (type == CESCAPE_COLOR_BG) { setBg(rgba); return; }
    if (type == CESCAPE_COLOR_FG) { setFg(rgba); return; }

    int id = type;

    assert(id >= 0 && id < MAX_COLORS);

    colors_[id] = rgba;
  }
};

#endif
