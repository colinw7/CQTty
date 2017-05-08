#include <CQPageText.h>
#include <CQPageTextWidget.h>
#include <CQPageText4014.h>
#include <CQPageTextError.h>
#include <CQPageTextState.h>
#include <CPageTextEscapeNotifier.h>
#include <CPageText.h>
#include <CPageTextLine.h>
#include <CTextCell.h>
#include <CEscapeColors.h>
#include <CEscape.h>
#include <CConfig.h>

CQPageText::
CQPageText(CQPageTextWidget *widget) :
 QObject(widget), CPageText(), widget_(widget)
{
  setObjectName("text");
}

void
CQPageText::
logError(const std::string &str)
{
  if (! errorDlg_) {
    errorDlg_ = new CQPageTextError(this);

    errorDlg_->show();
  }

  errorDlg_->addError(str);
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
setIsAlt(bool alt)
{
  CPageText::setIsAlt(alt);

  update();
}

void
CQPageText::
clear()
{
  CPageText::clear();

  update();
}

void
CQPageText::
clearAbove()
{
  CPageText::clearAbove();

  update();
}

void
CQPageText::
clearBelow()
{
  CPageText::clearBelow();

  update();
}

void
CQPageText::
clearSaved()
{
  CPageText::clearSaved();

  update();
}

//---

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

  update();
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
  widget_->repaint();

  if (text4014_)
    text4014_->update();
}

CTty *
CQPageText::
getTty() const
{
  return widget_->getTty();
}

void
CQPageText::
exec4014BS()
{
  if (text4014_)
    text4014_->cursorLeft();

  CPageText::exec4014BS();
}

void
CQPageText::
exec4014TAB()
{
  if (text4014_)
    text4014_->cursorRight(8);

  CPageText::exec4014TAB();
}

void
CQPageText::
exec4014LF()
{
  if (text4014_)
    text4014_->cursorDown();

  CPageText::exec4014LF();
}

void
CQPageText::
exec4014FF()
{
  if (text4014_)
    text4014_->cursorDown();

  CPageText::exec4014FF();
}

void
CQPageText::
exec4014CR()
{
  if (text4014_)
    text4014_->cursorLeft(text4014_->col());

  CPageText::exec4014CR();
}

void
CQPageText::
exec4014CUF()
{
  if (text4014_)
    text4014_->cursorRight();

  CPageText::exec4014CUF();
}

void
CQPageText::
set4014GIN(bool b)
{
  if (text4014_)
    text4014_->setGIN(b);

  CPageText::set4014GIN(b);
}

void
CQPageText::
set4014CharSet(int charSet)
{
  if (text4014_)
    text4014_->setCharSet(charSet);

  CPageText::set4014CharSet(charSet);
}

void
CQPageText::
clear4014()
{
  if (text4014_)
    text4014_->clear();

  CPageText::clear4014();
}

void
CQPageText::
draw4014Line(int x1, int y1, int x2, int y2, const CEscapeColor &color,
             const CEscapeLineStyle &style)
{
  if (text4014_)
    text4014_->addLine(x1, y1, x2, y2, color, style);

  CPageText::draw4014Line(x1, y1, x2, y2, color, style);
}

void
CQPageText::
draw4014Char(char c)
{
  if (text4014_)
    text4014_->addChar(c);

  CPageText::draw4014Char(c);
}

CIPoint2D
CQPageText::
get4014DataPos() const
{
  if (text4014_)
    return CIPoint2D(text4014_->col(), text4014_->row());

  return CPageText::get4014DataPos();
}

int
CQPageText::
get4014NumDataRows() const
{
  if (text4014_)
    return text4014_->numRows();

  return CPageText::get4014NumDataRows();
}

int
CQPageText::
get4014NumDataCols() const
{
  if (text4014_)
    return text4014_->numCols();

  return CPageText::get4014NumDataCols();
}

bool
CQPageText::
loadConfig()
{
  if (! config_)
    return false;

  //---

  std::string fontFamily;

  if (! config_->getValue("fontFamily", "", fontFamily))
    fontFamily = "courier";

  int fontSize;

  if (! config_->getValue("fontSize", "", &fontSize))
    fontSize = 12;

  setFontSet(fontFamily, fontSize);

  //---

  int rows = 60, columns = 100;

  if (! config_->getValue("rows", "", &rows))
    rows = 60;

  if (! config_->getValue("columns", "", &columns))
    columns = 100;

  setPageSize(rows, columns);

  //---

  loadConfigColor("colorBg"        , CEscapeColor::BG);
  loadConfigColor("colorFg"        , CEscapeColor::FG);
  loadConfigColor("colorBlack"     , CEscapeColor::BLACK);
  loadConfigColor("colorRed"       , CEscapeColor::RED);
  loadConfigColor("colorGreen"     , CEscapeColor::GREEN);
  loadConfigColor("colorYellow"    , CEscapeColor::YELLOW);
  loadConfigColor("colorBlue"      , CEscapeColor::BLUE);
  loadConfigColor("colorMagenta"   , CEscapeColor::MAGENTA);
  loadConfigColor("colorCyan"      , CEscapeColor::CYAN);
  loadConfigColor("colorWhite"     , CEscapeColor::WHITE);
  loadConfigColor("colorAltBlack"  , CEscapeColor::ALT_BLACK);
  loadConfigColor("colorAltRed"    , CEscapeColor::ALT_RED);
  loadConfigColor("colorAltGreen"  , CEscapeColor::ALT_GREEN);
  loadConfigColor("colorAltYellow" , CEscapeColor::ALT_YELLOW);
  loadConfigColor("colorAltBlue"   , CEscapeColor::ALT_BLUE);
  loadConfigColor("colorAltMagenta", CEscapeColor::ALT_MAGENTA);
  loadConfigColor("colorAltCyan"   , CEscapeColor::ALT_CYAN);
  loadConfigColor("colorAltWhite"  , CEscapeColor::ALT_WHITE);

  return true;
}

bool
CQPageText::
loadConfigColor(const std::string &name, CEscapeColor color)
{
  std::string value;

  if (! config_->getValue(name, "", value))
    return false;

  getEscapeNotifier()->setColor(color, CRGBName::toRGBA(value));

  return true;
}

bool
CQPageText::
saveConfig()
{
  if (! config_)
    return false;

  //---

  bool rc = true;

  //---

  std::string fontFamily = "courier";
  uint        fontSize   = 12;

  getFontSet(fontFamily, fontSize);

  if (! config_->setValue("fontFamily", "", fontFamily))
    rc = false;

  if (! config_->setValue("fontSize", "", int(fontSize)))
    rc = false;

  //---

  uint rows = 60, columns = 100;

  getPageSize(&rows, &columns);

  if (! config_->setValue("rows", "", int(rows)))
    rc = false;

  if (! config_->setValue("columns", "", int(columns)))
    rc = false;

  //---

  saveConfigColor("colorBg"        , CEscapeColor::BG);
  saveConfigColor("colorFg"        , CEscapeColor::FG);
  saveConfigColor("colorBlack"     , CEscapeColor::BLACK);
  saveConfigColor("colorRed"       , CEscapeColor::RED);
  saveConfigColor("colorGreen"     , CEscapeColor::GREEN);
  saveConfigColor("colorYellow"    , CEscapeColor::YELLOW);
  saveConfigColor("colorBlue"      , CEscapeColor::BLUE);
  saveConfigColor("colorMagenta"   , CEscapeColor::MAGENTA);
  saveConfigColor("colorCyan"      , CEscapeColor::CYAN);
  saveConfigColor("colorWhite"     , CEscapeColor::WHITE);
  saveConfigColor("colorAltBlack"  , CEscapeColor::ALT_BLACK);
  saveConfigColor("colorAltRed"    , CEscapeColor::ALT_RED);
  saveConfigColor("colorAltGreen"  , CEscapeColor::ALT_GREEN);
  saveConfigColor("colorAltYellow" , CEscapeColor::ALT_YELLOW);
  saveConfigColor("colorAltBlue"   , CEscapeColor::ALT_BLUE);
  saveConfigColor("colorAltMagenta", CEscapeColor::ALT_MAGENTA);
  saveConfigColor("colorAltCyan"   , CEscapeColor::ALT_CYAN);
  saveConfigColor("colorAltWhite"  , CEscapeColor::ALT_WHITE);

  //---

  config_->save();

  return rc;
}

bool
CQPageText::
saveConfigColor(const std::string &name, CEscapeColor color)
{
  CRGBA c = getEscapeNotifier()->getColor(color);

  std::string value = c.getRGB().stringEncode();

  if (! config_->setValue(name, "", value))
    return false;

  return true;
}
