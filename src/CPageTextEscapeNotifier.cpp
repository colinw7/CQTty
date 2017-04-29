#include <CPageTextEscapeNotifier.h>
#include <CPageText.h>
#include <CPageTextLine.h>

CPageTextEscapeNotifier::
CPageTextEscapeNotifier(CPageText *area) :
 area_(area)
{
}

void
CPageTextEscapeNotifier::
set4014(bool b)
{
  area_->set4014(b);

  CEscapeHandler::set4014(b);
}

const CIPoint2D &
CPageTextEscapeNotifier::
getDataPos() const
{
  int row, col;

  getDispPos(&row, &col);

  pos_.x = col;
  pos_.y = row;

  return pos_;
}

void
CPageTextEscapeNotifier::
setDataPos(const CIPoint2D &pos)
{
  setDispPos(pos.y, pos.x);
}

void
CPageTextEscapeNotifier::
getDispPos(int *row, int *col) const
{
  const CTextPos &pos = area_->getPosition();

  *row = pos.getRow();
  *col = pos.getCol();
}

void
CPageTextEscapeNotifier::
setDispPos(int row, int col)
{
  CTextPos pos;

  pos.setRowCol(row, col);

  area_->setPosition(pos);
}

void
CPageTextEscapeNotifier::
setDispRow(int row)
{
  area_->setRow(row);
}

void
CPageTextEscapeNotifier::
setDispCol(int col)
{
  area_->setCol(col);
}

void
CPageTextEscapeNotifier::
setDispSize(uint rows, uint cols)
{
  area_->setPageSize(rows, cols);
}

void
CPageTextEscapeNotifier::
getDispSize(uint *rows, uint *cols) const
{
  *rows = area_->getPageRows();
  *cols = area_->getPageCols();
}

void
CPageTextEscapeNotifier::
resetAll()
{
  resetState();

  clearTabAll();

  resetScrollArea();

  area_->resetAll();
}

void
CPageTextEscapeNotifier::
setLineWidthStyle(uint row, const CCellLineWidthStyle &lineStyle)
{
  CPageTextLine *line = area_->getLine(row);

  line->setWidthStyle(lineStyle);
}

void
CPageTextEscapeNotifier::
setLineHeightStyle(uint row, const CCellLineHeightStyle &lineStyle)
{
  CPageTextLine *line = area_->getLine(row);

  line->setHeightStyle(lineStyle);
}

void
CPageTextEscapeNotifier::
setCell(uint row, uint col, char c, const CCellStyle &style)
{
  setDispPos(row, col);

  area_->setStyle(style);

  area_->setChar(c);
}

void
CPageTextEscapeNotifier::
setUtfCell(uint row, uint col, ulong c, const CCellStyle &style)
{
  setDispPos(row, col);

  area_->setStyle(style);

  area_->setUtfChar(c);
}

void
CPageTextEscapeNotifier::
resetCell()
{
  CPageTextLine *line = area_->getCurrentLine();

  line->clearCell(area_->getCol(), ' ');
}

void
CPageTextEscapeNotifier::
pixelsToChars(int w, int h, int *cols, int *rows)
{
  area_->pixelsToChars(w, h, cols, rows);
}

void
CPageTextEscapeNotifier::
charsToPixels(int cols, int rows, int *w, int *h)
{
  area_->charsToPixels(cols, rows, w, h);
}

uint
CPageTextEscapeNotifier::
getNumDispRows() const
{
  return getNumDataRows();
}

uint
CPageTextEscapeNotifier::
getNumDispCols() const
{
  return getNumDataCols();
}

uint
CPageTextEscapeNotifier::
getNumDataRows() const
{
  return area_->getPageRows();
}

uint
CPageTextEscapeNotifier::
getNumDataCols() const
{
  return area_->getPageCols();
}

void
CPageTextEscapeNotifier::
shiftLeft()
{
  CPageTextLine *line = area_->getCurrentLine();

  line->shiftLeft(area_->getCol());
}

void
CPageTextEscapeNotifier::
shiftRight()
{
  CPageTextLine *line = area_->getCurrentLine();

  line->shiftRight(area_->getCol());
}

const CCellStyle &
CPageTextEscapeNotifier::
getStyle() const
{
  return area_->getStyle();
}

void
CPageTextEscapeNotifier::
setStyle(const CCellStyle &style)
{
  area_->setStyle(style);
}

void
CPageTextEscapeNotifier::
notifyChar(uint, uint, char)
{
}

void
CPageTextEscapeNotifier::
notifyUtfChar(uint, uint, ulong)
{
}

void
CPageTextEscapeNotifier::
notifyEnter(char)
{
}

void
CPageTextEscapeNotifier::
notifyStateChange()
{
  area_->notifyStateChange();
}

void
CPageTextEscapeNotifier::
paste(const std::string &str)
{
  area_->paste(str);
}

void
CPageTextEscapeNotifier::
setTrace(bool trace)
{
  area_->setTrace(trace);
}

bool
CPageTextEscapeNotifier::
getTrace() const
{
  return area_->getTrace();
}

void
CPageTextEscapeNotifier::
setDebug(bool debug)
{
  area_->setDebug(debug);
}

bool
CPageTextEscapeNotifier::
getDebug() const
{
  return area_->getDebug();
}

void
CPageTextEscapeNotifier::
setAlternative(bool alt)
{
  area_->setIsAlt(alt);
}

void
CPageTextEscapeNotifier::
insertLine(uint num)
{
  area_->insertLine(num);
}

void
CPageTextEscapeNotifier::
deleteLine(uint num)
{
  area_->deleteLine(num);
}

// TODO: change API use getChar
// TODO: only works if last output a character
void
CPageTextEscapeNotifier::
replicatePrevChar()
{
  CPageTextLine *line = area_->getCurrentLine();

  int col = area_->getCol();

  if (col > 0) {
    char c = line->getChar(col - 1);

    setCell(area_->getRow(), col, c, getStyle());
  }
}

void
CPageTextEscapeNotifier::
eraseLineLeft()
{
  CPageTextLine *line = area_->getCurrentLine();

  line->eraseLeft(area_->getCol());
}

void
CPageTextEscapeNotifier::
eraseLineRight()
{
  CPageTextLine *line = area_->getCurrentLine();

  line->eraseRight(area_->getCol());
}

void
CPageTextEscapeNotifier::
eraseLineAll()
{
  CPageTextLine *line = area_->getCurrentLine();

  line->erase();
}

void
CPageTextEscapeNotifier::
scrollUp()
{
  area_->scrollUp();
}

void
CPageTextEscapeNotifier::
scrollDown()
{
  area_->scrollDown();
}

void
CPageTextEscapeNotifier::
clearScreenBelow()
{
  eraseLineRight();

  area_->clearBelow();
}

void
CPageTextEscapeNotifier::
clearScreenAbove()
{
  CPageTextLine *line = area_->getCurrentLine();

  line->eraseLeft(area_->getCol());

  area_->clearAbove();
}

void
CPageTextEscapeNotifier::
clearScreen()
{
  area_->clear();
}

void
CPageTextEscapeNotifier::
clearScreenSaved()
{
  area_->clearSaved();
}

void
CPageTextEscapeNotifier::
fill(char c)
{
  area_->fill(c);
}

void
CPageTextEscapeNotifier::
startPrintLog()
{
  // TODO
}

void
CPageTextEscapeNotifier::
stopPrintLog()
{
  // TODO
}

void
CPageTextEscapeNotifier::
printLine()
{
  CFile file(".line.txt");

  CPageTextLine *line = area_->getCurrentLine();

  line->dumpLine(file);
}

void
CPageTextEscapeNotifier::
printScreen()
{
  CFile file(".screen.txt");

  area_->dumpScreen(file);
}

void
CPageTextEscapeNotifier::
printComposedScreen()
{
  // TODO
  printScreen();
}

void
CPageTextEscapeNotifier::
printAllPages()
{
  // TODO
  printScreen();
}

CWindow *
CPageTextEscapeNotifier::
getWindow() const
{
  return area_->getWindow();
}

void
CPageTextEscapeNotifier::
beep()
{
  area_->beep();
}

void
CPageTextEscapeNotifier::
setShowToolBar(bool show)
{
  area_->setShowToolBar(show);
}

void
CPageTextEscapeNotifier::
setShowScrollBar(bool show)
{
  area_->setShowScrollBar(show);
}

void
CPageTextEscapeNotifier::
setShowStatusBar(bool show)
{
  area_->setShowStatusBar(show);
}

void
CPageTextEscapeNotifier::
addSizedImageChar(const std::string &, CImageFile *, int, int, int, int)
{
  assert(false);
}

void
CPageTextEscapeNotifier::
addImageChar(const std::string &, CImageFile *, int, int, int, int)
{
  assert(false);
}

void
CPageTextEscapeNotifier::
addImage(const std::string &name, CImageFile *file)
{
  area_->addImage(name, file->getImage());
}

void
CPageTextEscapeNotifier::
addImage(const CImagePtr &image)
{
  area_->addImage("", image);
}

void
CPageTextEscapeNotifier::
addPixel(int x, int y, const CEscapeColor &color)
{
  area_->addPixel(x, y, color);
}

void
CPageTextEscapeNotifier::
addLine(int x1, int y1, int x2, int y2, const CEscapeColor &color, const CEscapeLineStyle &style)
{
  area_->addLine(x1, y1, x2, y2, color, style);
}

void
CPageTextEscapeNotifier::
addLink(const std::string &name, const std::string &path, const std::string &)
{
  area_->addLink(path + "/" + name, name);
}

void
CPageTextEscapeNotifier::
setLinkCell(uint, uint, char, const CCellStyle &, const std::string &, const std::string &)
{
  assert(false);
}

void
CPageTextEscapeNotifier::
setUtfLinkCell(uint, uint, ulong, const CCellStyle &, const std::string &, const std::string &)
{
  assert(false);
}

void
CPageTextEscapeNotifier::
previewFile(const std::string &)
{
  assert(false);
}

void
CPageTextEscapeNotifier::
setDirName(const std::string &dirName)
{
  area_->setDirName(dirName);
}

void
CPageTextEscapeNotifier::
processString(const char *str)
{
  area_->processString(str);
}

void
CPageTextEscapeNotifier::
logDebug(const std::string &str) const
{
  if (area_->getDebug())
    area_->log(str);
}

void
CPageTextEscapeNotifier::
logTrace(char c) const
{
  if (area_->getTrace())
    area_->log(c);
}

void
CPageTextEscapeNotifier::
logTrace(const std::string &str) const
{
  if (area_->getTrace())
    area_->log(str);
}

void
CPageTextEscapeNotifier::
exec4014BEL()
{
  area_->exec4014BEL();
}

void
CPageTextEscapeNotifier::
exec4014BS()
{
  area_->exec4014BS();
}

void
CPageTextEscapeNotifier::
exec4014TAB()
{
  area_->exec4014TAB();
}

void
CPageTextEscapeNotifier::
exec4014LF()
{
  area_->exec4014LF();
}

void
CPageTextEscapeNotifier::
exec4014VT()
{
  area_->exec4014VT();
}

void
CPageTextEscapeNotifier::
exec4014FF()
{
  area_->exec4014FF();
}

void
CPageTextEscapeNotifier::
exec4014CR()
{
  area_->exec4014CR();
}

void
CPageTextEscapeNotifier::
exec4014CUF()
{
  area_->exec4014CUF();
}

void
CPageTextEscapeNotifier::
set4014CharSet(int charSet)
{
  area_->set4014CharSet(charSet);
}

void
CPageTextEscapeNotifier::
set4014GIN(bool b)
{
  area_->set4014GIN(b);
}

void
CPageTextEscapeNotifier::
clear4014()
{
  area_->clear4014();
}

void
CPageTextEscapeNotifier::
draw4014Line(int x1, int y1, int x2, int y2, const CEscapeColor &color,
             const CEscapeLineStyle &style)
{
  area_->draw4014Line(x1, y1, x2, y2, color, style);
}

void
CPageTextEscapeNotifier::
draw4014Char(char c)
{
  area_->draw4014Char(c);
}
