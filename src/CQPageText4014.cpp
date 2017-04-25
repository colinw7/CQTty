#include <CQPageText4014.h>
#include <CQUtil.h>

#include <QPainter>

// In Alpha mode, the 4010 displays 35 lines of 74 characters
// Graphics is 1024, 780
CQPageText4014::
CQPageText4014(CQPageText *area) :
 area_(area)
{
  setFixedSize(1024, 780);
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

    painter.drawLine(QPoint(line.x1, line.y1), QPoint(line.x2, line.y2));
  }
}

void
CQPageText4014::
mousePressEvent(QMouseEvent *)
{
  update();
}
