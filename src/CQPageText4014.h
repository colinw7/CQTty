#ifndef CQPageText4014_H
#define CQPageText4014_H

#include <CQPageText.h>
#include <CEscapeColors.h>
#include <QWidget>

class CQPageText4014 : public QWidget {
  Q_OBJECT

 public:
  struct Char {
    int  row     { 0 };
    int  col     { 0 };
    char c       { '\0' };
    int  charSet { 1 };

    Char(int row=0, int col=0, char c='\0', int charSet=1) :
     row(row), col(col), c(c), charSet(charSet) {
    }
  };

 public:
  CQPageText4014(CQPageText *area);

  int row() const { return row_; }
  int col() const { return col_; }

  int numRows() const;
  int numCols() const;

  int numRows(int charSet) const;
  int numCols(int charSet) const;

  int getCharSet() const { return charSet_; }
  void setCharSet(int charSet) { charSet_ = charSet; }

  bool getGIN() const { return gin_; }
  void setGIN(bool b);

  void cursorLeft (int n=1);
  void cursorRight(int n=1);
  void cursorUp   (int n=1);
  void cursorDown (int n=1);

  void clear();

  void addLine(int x1, int y1, int x2, int y2, const CEscapeColor &color,
               const CEscapeLineStyle &lineStyle);

  void addChar(char c);

  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *);
  void mouseMotionEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

 private:
  void drawChar(QPainter *painter, const Char &c) const;

  void windowToPixel(double wx, double wy, double *px, double *py, bool flipY) const;
  void pixelToWindow(double px, double py, double *wx, double *wy, bool flipY) const;

  void charPosToPixel(double row, double col, double *px, double *py) const;

  double charWidth () const;
  double charHeight() const;

  double charWidth (int charSet) const;
  double charHeight(int charSet) const;

 private:
  static const int tek_width_  = 4096;
  static const int tek_height_ = 3072;

  typedef std::vector<CQPageText::PixelLine> PixelLines;
  typedef std::vector<Char>                  Chars;

  CQPageText* area_ { nullptr };
  int         scale_ { 2 };
  PixelLines  pixelLines_;
  Chars       chars_;
  int         row_ { 0 }, col_ { 0 };
  int         charSet_ { 1 };
  bool        gin_ { false };
};

#endif
