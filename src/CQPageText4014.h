#ifndef CQPageText4014_H
#define CQPageText4014_H

#include <CQPageText.h>
#include <CEscapeColors.h>
#include <QWidget>

class CQPageText4014 : public QWidget {
  Q_OBJECT

 public:
  CQPageText4014(CQPageText *area);

  void addLine(int x1, int y1, int x2, int y2, const CEscapeColor &color,
               const CEscapeLineStyle &lineStyle);

  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *);

 private:
  typedef std::vector<CQPageText::PixelLine> PixelLines;

  CQPageText* area_ { nullptr };
  PixelLines  pixelLines_;
};

#endif
