#include <CQPageText4014.h>
#include <CPageTextEscapeNotifier.h>
#include <CQUtil.h>
#include <CQUtilRGBA.h>
#include <CQUtilEvent.h>
#include <CVFont.h>

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

// 4014 Graphics is 4096, 3072
//
//                 Rows Columns
// ESC 9 : 0 Small       , 38   81
// ESC 8 : 1 Normal/Large, 35   74
// ESC : : 2 #2          , 58   121
// ESC ; : 3 #3          , 64   133
CQPageText4014::
CQPageText4014(CQPageText *area) :
 area_(area)
{
  setFixedSize(tek_width_/scale_, tek_height_/scale_);

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);
}

int
CQPageText4014::
numRows() const
{
  return numRows(charSet_);
}

int
CQPageText4014::
numRows(int charSet) const
{
  switch (charSet) {
    case 0: return 38;
    default:
    case 1: return 35;
    case 2: return 58;
    case 3: return 64;
  }
}

int
CQPageText4014::
numCols() const
{
  return numCols(charSet_);
}

int
CQPageText4014::
numCols(int charSet) const
{
  switch (charSet) {
    case 0: return 81;
    default:
    case 1: return 74;
    case 2: return 121;
    case 3: return 133;
  }
}

void
CQPageText4014::
setGIN(bool b)
{
  gin_ = b;

  update();
}

void
CQPageText4014::
cursorLeft(int n)
{
  col_ -= n;

  if (col_ > 0)
    col_ = 0;
}

void
CQPageText4014::
cursorRight(int n)
{
  col_ += n;
}

void
CQPageText4014::
cursorUp(int n)
{
  row_ -= n;

  while (row_ < 0)
    row_ += numRows();
}

void
CQPageText4014::
cursorDown(int n)
{
  row_ += n;

  while (row_ >= numRows())
    row_ -= numRows();
}

void
CQPageText4014::
clear()
{
  pixelLines_.clear();

  chars_.clear();

  update();
}

void
CQPageText4014::
addLine(int x1, int y1, int x2, int y2, const CEscapeColor &color,
        const CEscapeLineStyle &style)
{
  pixelLines_.push_back(CQPageText::PixelLine(x1, y1, x2, y2, color, style));
}

void
CQPageText4014::
addChar(char c)
{
  chars_.push_back(Char(row_, col_, c, charSet_));
}

void
CQPageText4014::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  for (const auto &line : pixelLines_) {
    CRGBA c = CEscapeColorsInst->decode(line.color);

    QPen pen;

    pen.setColor(CQUtil::rgbaToColor(c));

    if      (line.lineStyle == CEscapeLineStyle::DOTTED)
      pen.setStyle(Qt::DotLine);
    else if (line.lineStyle == CEscapeLineStyle::DOT_DASH)
      pen.setStyle(Qt::DashDotLine);
    else if (line.lineStyle == CEscapeLineStyle::SHORT_DASH)
      pen.setStyle(Qt::DashLine);
    else if (line.lineStyle == CEscapeLineStyle::LONG_DASH) {
      pen.setStyle(Qt::CustomDashLine);
      pen.setDashPattern(QVector<qreal>() << 8 << 4);
    }

    painter.setPen(pen);

    double px1, py1, px2, py2;

    windowToPixel(line.x1, line.y1, &px1, &py1, /*flipY*/true);
    windowToPixel(line.x2, line.y2, &px2, &py2, /*flipY*/true);

    painter.drawLine(QPointF(px1, py1), QPointF(px2, py2));
  }

  //---

  for (const auto &c : chars_) {
    drawChar(&painter, c);
  }
}

void
CQPageText4014::
drawChar(QPainter *painter, const Char &c) const
{
  double wx = c.col*charWidth (c.charSet);
  double wy = c.row*charHeight(c.charSet);

  double cw = charWidth (c.charSet);
  double ch = charHeight(c.charSet);

  auto mapFontPoint = [&](const CPoint2D &p) -> QPointF {
    QPointF p1(wx + cw*p.x, wy + ch*(1.0 - p.y)/CVFont::charHeight);

    double px, py;

    windowToPixel(p1.x(), p1.y(), &px, &py, /*flipY*/false);

    return QPointF(px, py);
  };

  const CVFontDef &fontDef = CVFont::getFontDef(c.c);

  QPen pen(Qt::white);

  painter->setPen(pen);

  for (const auto &shape : fontDef.shapes()) {
    if      (shape->type() == CVFontShape::Type::LINE) {
      const CVFontLine *line = dynamic_cast<const CVFontLine *>(shape);

      QPointF p1 = mapFontPoint(line->start());
      QPointF p2 = mapFontPoint(line->end  ());

      painter->drawLine(p1, p2);
    }
    else if (shape->type() == CVFontShape::Type::CURVE) {
      const CVFontCurve *curve = dynamic_cast<const CVFontCurve *>(shape);

      QPainterPath path;

      QPointF p1 = mapFontPoint(curve->p1());
      QPointF p2 = mapFontPoint(curve->p2());
      QPointF p3 = mapFontPoint(curve->p3());
      QPointF p4 = mapFontPoint(curve->p4());

      path.moveTo (p1);
      path.cubicTo(p2, p3, p4);

      painter->strokePath(path, pen);
    }
  }
}

void
CQPageText4014::
keyPressEvent(QKeyEvent *e)
{
  CKeyEvent *ke = CQUtil::convertEvent(e);

  // handle special pasted key sequence
  if (ke->getType() == CKEY_TYPE_Insert && ke->isShiftKey()) {
    std::string str = "^[_<paste/>^[\\";

    area_->addEscapeChars(str.c_str(), str.size());

    return;
  }

  const std::string &str = area_->getEventText(ke);

  if (! str.empty())
    area_->processString(str.c_str());
}

void
CQPageText4014::
mousePressEvent(QMouseEvent *e)
{
  update();

  if (gin_) {
    double wx, wy;

    pixelToWindow(e->x(), e->y(), &wx, &wy, /*flipY*/true);

    area_->getEscapeNotifier()->send4014MousePress(e->button(), wx, wy);

    gin_ = false;
  }
}

void
CQPageText4014::
mouseMoveEvent(QMouseEvent *)
{
}

void
CQPageText4014::
mouseReleaseEvent(QMouseEvent *)
{
}

void
CQPageText4014::
windowToPixel(double wx, double wy, double *px, double *py, bool flipY) const
{
  *px = wx/scale_;

  if (flipY)
    *py = (tek_height_ - wy)/scale_;
  else
    *py = wy/scale_;
}

void
CQPageText4014::
pixelToWindow(double px, double py, double *wx, double *wy, bool flipY) const
{
  *wx = px*scale_;

  if (flipY)
    *wy = tek_height_ - py*scale_;
  else
    *wy = py*scale_;
}

void
CQPageText4014::
charPosToPixel(double row, double col, double *px, double *py) const
{
  double wx = col*charWidth ();
  double wy = row*charHeight();

  *px = wx/scale_;
  *py = wy/scale_;
}

double
CQPageText4014::
charWidth() const
{
  return tek_width_/numCols();
}

double
CQPageText4014::
charHeight() const
{
  return tek_height_/numRows();
}

double
CQPageText4014::
charWidth(int charSet) const
{
  return tek_width_/numCols(charSet);
}

double
CQPageText4014::
charHeight(int charSet) const
{
  return tek_height_/numRows(charSet);
}
