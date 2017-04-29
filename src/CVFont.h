#ifndef CVFONT_H
#define CVFONT_H

typedef unsigned int uint;

struct CVFontPointDef {
  double x;
  double y;
};

struct CVFontLineDef {
  CVFontPointDef start;
  CVFontPointDef end;
};

struct CVFontCurveDef {
  double x1, y1;
  double x2, y2;
  double x3, y3;
  double x4, y4;
};

struct CVFontDef {
  double          width;
  double          ascender;
  double          descender;
  uint            num_lines;
  CVFontLineDef  *lines;
  uint            num_curves;
  CVFontCurveDef *curves;
};

class CVFont {
 public:
  static const CVFontDef &getFontDef(char c);

  template<typename LINE_PROC, typename CURVE_PROC>
  static void draw(const CVFontDef &font_def, const LINE_PROC &line_proc,
                   const CURVE_PROC &curve_proc) {
    for (uint i = 0; i < font_def.num_lines; ++i)
      line_proc(font_def.lines[i].start.x, font_def.lines[i].start.y,
                font_def.lines[i].end  .x, font_def.lines[i].end  .y);

    for (uint i = 0; i < font_def.num_curves; ++i)
      curve_proc(font_def.curves[i].x1, font_def.curves[i].y1,
                 font_def.curves[i].x2, font_def.curves[i].y2,
                 font_def.curves[i].x3, font_def.curves[i].y3,
                 font_def.curves[i].x4, font_def.curves[i].y4);
  }

 private:
  CVFont();
};

#endif
