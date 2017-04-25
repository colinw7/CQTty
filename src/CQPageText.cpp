#include <CQPageText.h>
#include <CQPageTextWidget.h>
#include <CQPageText4014.h>
#include <CQPageTextState.h>
#include <CPageTextEscapeNotifier.h>
#include <CPageText.h>
#include <CPageTextLine.h>
#include <CTextCell.h>
#include <CEscapeColors.h>
#include <CEscape.h>

CQPageText::
CQPageText(CQPageTextWidget *widget) :
 QObject(widget), CPageText(), widget_(widget)
{
  setObjectName("text");
}

void
CQPageText::
set4014(bool b)
{
  if (b) {
    if (! text4014_)
      text4014_ = new CQPageText4014(this);

    text4014_->show();
  }

  CPageText::set4014(b);
}

void
CQPageText::
add4014Line(int x1, int y1, int x2, int y2, const CEscapeColor &color,
            const CEscapeLineStyle &style)
{
  if (text4014_)
    text4014_->addLine(x1, y1, x2, y2, color, style);

  CPageText::add4014Line(x1, y1, x2, y2, color, style);
}

void
CQPageText::
scrollBottom()
{
  widget_->scrollBottom();
}

void
CQPageText::
notifyToolBar()
{
  bool show = getShowToolBar();

  widget_->showToolBar(show);
}

void
CQPageText::
notifyScrollBar()
{
  bool show = getShowScrollBar();

  widget_->showScrollBar(show);
}

void
CQPageText::
notifyStatusBar()
{
  bool show = getShowStatusBar();

  widget_->showStatusBar(show);
}

void
CQPageText::
notifyPageSize(int rows, int cols)
{
  widget_->notifyPageSize(rows, cols);
}

void
CQPageText::
notifyPosition(const CTextPos &pos)
{
  widget_->notifyPosition(pos);
}

void
CQPageText::
notifyCharSize(int width, int height)
{
  widget_->notifyCharSize(width, height);
}

void
CQPageText::
notifyStyle(const CTextPos &pos)
{
  widget_->notifyStyle(pos);
}

void
CQPageText::
notifyStateChange()
{
  widget_->notifyStateChange();

  CPageText::notifyStateChange();
}

void
CQPageText::
notifyDirChanged(const std::string &dirName)
{
  widget_->notifyDirChanged(dirName);
}

void
CQPageText::
update()
{
  widget_->update();
}

CTty *
CQPageText::
getTty() const
{
  return widget_->getTty();
}
