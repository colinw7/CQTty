#include <CPageText.h>
#include <CPageTextEscapeNotifier.h>
#include <CPageTextLine.h>
#include <CTextCell.h>

#include <CStrParse.h>
#include <CFontMgr.h>
#include <CEscape.h>
#include <CWindow.h>
#include <CEnv.h>
#include <CTty.h>
#include <CDir.h>

CPageText::
CPageText() :
 log_(".page_text.log")
{
  pageRows_ = 60;
  pageCols_ = 100;

  setFontSet("courier", 12);

  CEnvInst.set("LINES"  , pageRows_);
  CEnvInst.set("COLUMNS", pageCols_);

  dirName_ = CDir::getCurrent();
}

CPageText::
~CPageText()
{
  for (auto &old_line : oldLines_)
    delete old_line;
}

void
CPageText::
setDebug(bool debug)
{
  debug_ = debug;

  if (debug_)
    log_.init();
}

void
CPageText::
setTrace(bool trace)
{
  trace_ = trace;

  if (trace_)
    log_.init();
}

void
CPageText::
logDebug(const std::string &str)
{
  log_.write(str);
}

void
CPageText::
logTrace(const std::string &str)
{
  log_.write(str);
}

void
CPageText::
logError(const std::string &str)
{
  log_.write(str);
}

CWindow *
CPageText::
getWindow() const
{
  return window_;
}

void
CPageText::
setWindow(CWindow *window)
{
  window_ = window;
}

void
CPageText::
beep()
{
  if (window_)
    window_->beep();
}

CPageTextEscapeNotifier *
CPageText::
getEscapeNotifier() const
{
  if (! notifier_) {
    CPageText *th = const_cast<CPageText *>(this);

    th->notifier_ = new CPageTextEscapeNotifier(th);
  }

  return notifier_;
}

void
CPageText::
setFontSet(const std::string &family, uint size)
{
  fonts_.setup(family, size);

  uint char_width  = getCharWidth ();
  uint char_height = getCharHeight();

  CEnvInst.set("CTERM_CHAR_WIDTH" , char_width );
  CEnvInst.set("CTERM_CHAR_HEIGHT", char_height);

  notifyCharSize(char_width, char_height);
}

void
CPageText::
getFontSet(std::string &family, uint &size)
{
  family = fonts_.getFamily();
  size   = fonts_.getSize();
}

void
CPageText::
resetAll()
{
  style_.reset();

  setPosition(CTextPos(0, 0));

  setIsAlt(false);

  clearSelection();
}

CRGBA
CPageText::
getBg() const
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  if (! notifier->getInverseVideo())
    return notifier->getColor(CEscapeColor::BG);
  else
    return notifier->getColor(CEscapeColor::BG).inverse();
}

CRGBA
CPageText::
getBg(const CCellStyle &style)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  if (! notifier->getInverseVideo()) {
    if (! style.isBgRGB())
      return notifier->getColor(style.getBg());
    else {
      CRGB rgb;

      style.getBg(rgb);

      return rgb;
    }
  }
  else {
    if (! style.isBgRGB())
      return notifier->getColor(style.getBg()).inverse();
    else {
      CRGB rgb;

      style.getBg(rgb);

      return rgb.inverse();
    }
  }
}

void
CPageText::
setBg(const CRGBA &bg)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  notifier->setColor(CEscapeColor::BG, bg);
}

CRGBA
CPageText::
getFg() const
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  if (! notifier->getInverseVideo())
    return notifier->getColor(CEscapeColor::FG);
  else
    return notifier->getColor(CEscapeColor::FG).inverse();
}

CRGBA
CPageText::
getFg(const CCellStyle &style)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  if (! notifier->getInverseVideo()) {
    if (! style.isFgRGB())
      return notifier->getColor(style.getFg());
    else {
      CRGB rgb;

      style.getFg(rgb);

      return rgb;
    }
  }
  else {
    if (! style.isFgRGB())
      return notifier->getColor(style.getFg()).inverse();
    else {
      CRGB rgb;

      style.getFg(rgb);

      return rgb.inverse();
    }
  }
}

const CPageText::Page &
CPageText::
getCurrentPage() const
{
  if (! isAlt_)
    return currentPage_;
  else
    return altPage_;
}

CPageText::Page &
CPageText::
getCurrentPage()
{
  if (! isAlt_)
    return currentPage_;
  else
    return altPage_;
}

void
CPageText::
setIsAlt(bool alt)
{
  isAlt_ = alt;
}

void
CPageText::
clearSelection()
{
  selection_.set = false;
}

void
CPageText::
setSelectionStart(const CTextPos &start)
{
  selection_.set   = true;
  selection_.start = start;
  selection_.end   = start;
}

void
CPageText::
setSelectionEnd(const CTextPos &end)
{
  assert(selection_.set);

  selection_.end = end;
}

bool
CPageText::
selectWord(int row, int col)
{
  selection_.set = false;

  uint num_old_lines = getNumOldLines();

  if (row >= 0 && row < int(num_old_lines)) {
    CPageTextLine *line = getOldLine(row);

    uint col1, col2;

    if (line->getWordBounds(col, col1, col2)) {
      selection_.set   = true;
      selection_.start = CTextPos(row, col1);
      selection_.end   = CTextPos(row, col2);
    }
  }
  else {
    int row1 = row - num_old_lines;

    LineList &lines = getCurrentPage().lines;

    uint num_lines = uint(lines.size());

    if (row1 >= 0 && row1 < int(num_lines)) {
      CPageTextLine *line = lines[row1];

      uint col1, col2;

      if (line->getWordBounds(col, col1, col2)) {
        selection_.set   = true;
        selection_.start = CTextPos(row, col1);
        selection_.end   = CTextPos(row, col2);
      }
    }
  }

  return selection_.set;
}

std::string
CPageText::
getSelectionText() const
{
  if (! hasSelection())
    return "";

  std::string selText;

  int row = 0;
  int col = 0;

  bool in_selection = false;

  LineList::const_iterator pl1, pl2;

  for (pl1 = beginOldLine(), pl2 = endOldLine(); pl1 != pl2; ++pl1) {
    const CPageTextLine *line = *pl1;

    col = 0;

    for (const auto &cell : line->cells()) {
      if      (cell->getType() == CTextCell::Type::CHAR) {
        const CTextCharCell *char_cell = dynamic_cast<const CTextCharCell *>(cell);

        if (isSelected(row, col)) {
          if (col == 0 && in_selection)
            selText += "\n";

          selText += char_cell->getChar();

          in_selection = true;
        }
        else
          in_selection = false;
      }

      ++col;
    }

    ++row;
  }

  for (const auto &line : lines()) {
    col = 0;

    for (const auto &cell : line->cells()) {
      if      (cell->getType() == CTextCell::Type::CHAR) {
        const CTextCharCell *char_cell = dynamic_cast<const CTextCharCell *>(cell);

        if (isSelected(row, col)) {
          if (col == 0 && in_selection)
            selText += "\n";

          selText += char_cell->getChar();

          in_selection = true;
        }
        else
          in_selection = false;
      }

      ++col;
    }

    ++row;
  }

  return selText;
}

bool
CPageText::
isSelected(int row, int col) const
{
  if (! hasSelection()) return false;

  CTextPos start = selection_.start;
  CTextPos end   = selection_.end;

  if (start.getRow() > end.getRow() ||
      (start.getRow() == end.getRow() && start.getCol() > end.getCol()))
    std::swap(start, end);

  if (row > start.getRow() && row < end.getRow()) return true;

  if (start.getRow() == end.getRow()) {
    if (row == start.getRow() && col >= start.getCol() && col <= end.getCol()) return true;
  }
  else {
    if (row == start.getRow() && col >= start.getCol()) return true;
    if (row == end  .getRow() && col <= end  .getCol()) return true;
  }

  return false;
}

const CCellStyle &
CPageText::
getStyle() const
{
  return style_;
}

void
CPageText::
setStyle(const CCellStyle &style)
{
  style_ = style;
}

const CTextPos &
CPageText::
getPosition() const
{
  return pos_;
}

uint
CPageText::
getRow() const
{
  return getPosition().getRow();
}

uint
CPageText::
getCol() const
{
  return getPosition().getCol();
}

void
CPageText::
setPosition(const CTextPos &pos)
{
  makePosValid(pos);

  //assert(pos.getRow() >= 0 && pos.getRow() < int(getPageRows()));

  pos_ = pos;

  notifyPosition(pos_);
}

void
CPageText::
setRow(uint row)
{
  setPosition(CTextPos(row, getPosition().getCol()));
}

void
CPageText::
setCol(uint col)
{
  setPosition(CTextPos(getPosition().getRow(), col));
}

uint
CPageText::
getPageRows() const
{
  return pageRows_;
}

void
CPageText::
setPageRows(uint n)
{
  setPageSize(n, getPageCols());
}

uint
CPageText::
getPageCols() const
{
  return pageCols_;
}

void
CPageText::
setPageCols(uint n)
{
  setPageSize(getPageRows(), n);
}

void
CPageText::
getPageSize(uint *rows, uint *cols)
{
  *rows = getPageRows();
  *cols = getPageCols();
}

void
CPageText::
setPageSize(uint rows, uint cols)
{
  if (rows == getPageRows() && cols == getPageCols())
    return;

  //setPosition(CTextPos(0, 0));

  CTextPos pos = getPosition();

  makeSizeValid();

  LineList &lines = getCurrentPage().lines;

  if      (rows < getPageRows()) {
    uint num_extra = getPageRows() - rows;

    for (uint i = 0; i < num_extra; ++i) {
      CPageTextLine *old_line = lines[0];

      uint num_lines = uint(lines.size());

      for (uint j = 1; j < num_lines; ++j)
        lines[j - 1] = lines[j];

      lines.pop_back();

      if (! addOldLine(old_line))
        delete old_line;
    }
  }
  else if (rows > getPageRows()) {
    if (! isAlt()) {
      uint num_extra = rows - getPageRows();

      while (num_extra > 0 && ! oldLines_.empty()) {
        CPageTextLine *old_line = oldLines_.back();

        oldLines_.pop_back();

        uint num_lines = uint(lines.size());

        lines.push_back(nullptr);

        for (int j = num_lines - 1; j >= 0; --j)
          lines[j + 1] = lines[j];

        lines[0] = old_line;

        --num_extra;
      }
    }
  }

  pageRows_ = rows;
  pageCols_ = cols;

  CEnvInst.set("LINES"  , rows);
  CEnvInst.set("COLUMNS", cols);

  pos.setRow(std::min(std::max(pos.getRow(), 0), int(getPageRows()) - 1));
  pos.setCol(std::min(std::max(pos.getCol(), 0), int(getPageCols()) - 1));

  setPosition(pos);

  notifyPageSize(rows, cols);
}

uint
CPageText::
getCharWidth() const
{
  return uint(fonts_.getFont(CFONT_STYLE_NORMAL)->getCharWidth());
}

uint
CPageText::
getCharHeight() const
{
  return uint(fonts_.getFont(CFONT_STYLE_NORMAL)->getCharHeight());
}

uint
CPageText::
getCharAscent() const
{
  return uint(fonts_.getFont(CFONT_STYLE_NORMAL)->getCharAscent());
}

void
CPageText::
pixelsToChars(int w, int h, int *cols, int *rows)
{
  int char_width  = getCharWidth ();
  int char_height = getCharHeight();

  *cols = (w + char_width  - 1)/char_width ;
  *rows = (h + char_height - 1)/char_height;
}

void
CPageText::
charsToPixels(int cols, int rows, int *w, int *h)
{
  int char_width  = getCharWidth ();
  int char_height = getCharHeight();

  *w = cols*char_width ;
  *h = rows*char_height;
}

void
CPageText::
pixelToPos(int w, int h, int *row, int *col)
{
  int char_width  = getCharWidth ();
  int char_height = getCharHeight();

  *col = w/char_width ;
  *row = h/char_height;
}

uint
CPageText::
getPageWidth() const
{
  return getPageCols()*getCharWidth();
}

uint
CPageText::
getPageHeight() const
{
  return getPageRows()*getCharHeight();
}

uint
CPageText::
getNumOldLines() const
{
  if (! isAlt())
    return uint(oldLines_.size());
  else
    return 0;
}

CPageTextLine *
CPageText::
getOldLine(uint i) const
{
  if (! isAlt())
    return oldLines_[i];
  else
    return nullptr;
}

bool
CPageText::
addOldLine(CPageTextLine *line)
{
  if (! isAlt()) {
    oldLines_.push_back(line);

    return true;
  }
  else
    return false;
}

uint
CPageText::
getOldLinesHeight() const
{
  return getNumOldLines()*getCharHeight();
}

uint
CPageText::
getLinesHeight() const
{
  const LineList &lines = getCurrentPage().lines;

  return uint(lines.size())*getCharHeight();
}

CFontPtr
CPageText::
getFont(const CCellStyle &style)
{
  CFontStyle fontStyle = style.getFontStyle();

  return fonts_.getFont(fontStyle);
}

CTextCell *
CPageText::
getCell(const CTextPos &pos)
{
  makePosValid(pos);

  return getLine(pos.getRow())->getCell(pos.getCol());
}

CPageText::LineList::const_iterator
CPageText::
beginOldLine() const
{
  if (! isAlt())
    return oldLines_.begin();
  else
    return oldLines_.end();
}

CPageText::LineList::const_iterator
CPageText::
endOldLine() const
{
  return oldLines_.end();
}

const CPageText::LineList &
CPageText::
lines() const
{
  return getCurrentPage().lines;
}

CPageText::LineList &
CPageText::
lines()
{
  return getCurrentPage().lines;
}

CPageText::LineList::const_iterator
CPageText::
beginLine() const
{
  return lines().begin();
}

CPageText::LineList::const_iterator
CPageText::
endLine() const
{
  return lines().end();
}

CPageText::PixelPoints::const_iterator
CPageText::
beginPixelPoints() const
{
  return pixelPoints_.begin();
}

CPageText::PixelPoints::const_iterator
CPageText::
endPixelPoints() const
{
  return pixelPoints_.end();
}

CPageText::PixelLines::const_iterator
CPageText::
beginPixelLines() const
{
  return pixelLines_.begin();
}

CPageText::PixelLines::const_iterator
CPageText::
endPixelLines() const
{
  return pixelLines_.end();
}

CPageTextLine *
CPageText::
getCurrentLine()
{
  return getLine(getRow());
}

CPageTextLine *
CPageText::
getLine(uint row)
{
  makePosValid(CTextPos(row, getPageCols() - 1));

  assert(row < lines().size());

  return lines()[row];
}

void
CPageText::
clear()
{
  // Scroll Up

  for (auto &line : lines())
    line->reset();

  pixelPoints_.clear();
  pixelLines_ .clear();
}

void
CPageText::
clearAbove()
{
  LineList &lines     = this->lines();
  uint      num_lines = uint(lines.size());

  for (uint i = 0; i < uint(getRow()) && i < num_lines; ++i)
    lines[i]->erase();
}

void
CPageText::
clearBelow()
{
  LineList &lines     = this->lines();
  uint      num_lines = uint(lines.size());

  for (uint i = getRow() + 1; i < num_lines; ++i)
    lines[i]->erase();
}

void
CPageText::
clearSaved()
{
  for (auto &old_line : oldLines_)
    delete old_line;

  oldLines_.clear();
}

void
CPageText::
dumpScreen(CFile &file)
{
  for (auto &line : lines())
    line->dumpLine(file);
}

void
CPageText::
fill(char c)
{
  makeSizeValid();

  int cols = getPageCols();

  for (auto &line : lines()) {
    line->makeColValid(cols);

    line->fill(c);
  }
}

void
CPageText::
insertLine(uint num)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  makeSizeValid();

  // get scroll region
  int top_line    = notifier->getScrollTop   () - 1;
  int bottom_line = notifier->getScrollBottom() - 1;

  // ignore if outside scroll region
  uint row = getRow();

  if (int(row) < top_line || int(row) >= bottom_line) return;

  // move lines below cursor position down
  LineList &lines = this->lines();

  for (uint n = 0; n < num; ++n) {
    CPageTextLine *old_line = lines[bottom_line];

    for (int i = bottom_line - 1; i >= int(row); --i)
      lines[i + 1] = lines[i];

    old_line->erase();

    lines[row] = old_line;
  }

  checkLines();
}

void
CPageText::
deleteLine(uint num)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  makeSizeValid();

  // get scroll region
  int top_line    = notifier->getScrollTop   () - 1;
  int bottom_line = notifier->getScrollBottom() - 1;

  // ignore if outside scroll region
  uint row = getRow();

  if (int(row) < top_line || int(row) > bottom_line) return;

  // move lines at cursor position up
  LineList &lines = this->lines();

  for (uint n = 0; n < num; ++n) {
    CPageTextLine *old_line = lines[row];

    for (int i = row; i < bottom_line; ++i)
      lines[i] = lines[i + 1];

    old_line->erase();

    lines[bottom_line] = old_line;
  }

  checkLines();
}

void
CPageText::
scrollUp()
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  makeSizeValid();

  int top_line    = notifier->getScrollTop   () - 1;
  int bottom_line = notifier->getScrollBottom() - 1;

  LineList &lines = this->lines();

  CPageTextLine *old_line = lines[top_line];

  for (int i = top_line + 1; i <= bottom_line; ++i)
    lines[i - 1] = lines[i];

  lines[bottom_line] = new CPageTextLine(this);

  if (! notifier->hasScrollArea())
    oldLines_.push_back(old_line);
  else
    delete old_line;

  checkLines();
}

void
CPageText::
scrollDown()
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  makeSizeValid();

  checkLines();

  int top_line    = notifier->getScrollTop   () - 1;
  int bottom_line = notifier->getScrollBottom() - 1;

  LineList &lines = this->lines();

  CPageTextLine *old_line = lines[bottom_line];

  for (int i = bottom_line; i > top_line; --i) {
    lines[i] = lines[i - 1];

    checkLines();
  }

  old_line->erase();

  checkLines();

  lines[top_line] = old_line;

  checkLines();
}

void
CPageText::
insertText(const std::string &str)
{
  uint num_chars = uint(str.size());

  for (uint i = 0; i < num_chars; ++i)
    insertChar(str[i]);
}

void
CPageText::
insertChar(char c)
{
  setChar(c);

  setCol(getCol() + 1);
}

void
CPageText::
setChar(char c)
{
  makePosValid();

  CTextCell *old_cell = getCell(getPosition());

  CTextCell *new_cell = old_cell->convertTo(CTextCell::Type::CHAR);

  CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(new_cell);

  char_cell->setStyle(getStyle());

  char_cell->setChar(c);

  if (new_cell != old_cell) {
    old_cell->getLine()->replaceCell(old_cell, new_cell);

    delete old_cell;
  }
}

void
CPageText::
setUtfChar(ulong c)
{
  makePosValid();

  CTextCell *old_cell = getCell(getPosition());

  CTextCell *new_cell = old_cell->convertTo(CTextCell::Type::UTF_CHAR);

  CTextUtfCharCell *char_cell = dynamic_cast<CTextUtfCharCell *>(new_cell);

  char_cell->setStyle(getStyle());

  char_cell->setUtfChar(c);

  if (new_cell != old_cell) {
    old_cell->getLine()->replaceCell(old_cell, new_cell);

    delete old_cell;
  }
}

void
CPageText::
addLink(const std::string &linkDest, const std::string &linkName)
{
  makePosValid();

  CPageTextLine *line = getCurrentLine();

  CTextLinkCell *link_cell = new CTextLinkCell(line, getCol());

  link_cell->setLinkDest(linkDest);
  link_cell->setLinkName(linkName);

  line->addLink(link_cell);
}

void
CPageText::
addImage(const std::string &fileName, const CImagePtr &image)
{
  makePosValid();

  CPageTextLine *line = getCurrentLine();

  CTextImageCell *image_cell = new CTextImageCell(line, getCol(), fileName, image);

  line->addImage(image_cell);
}

void
CPageText::
addPixel(int x, int y, const CEscapeColor &color)
{
#if 0
  std::string::size_type pos = color.find(',');

  if (pos != std::string::npos) {
    std::string color1 = color;

    while (pos != std::string::npos) {
      std::string color2 = color1.substr(0, pos);

      pixelPoints_.push_back(PixelPoint(x, y, color2));

      ++x;

      color1 = color1.substr(pos + 1);

      pos = color1.find(',');
    }
  }
  else {
    pixelPoints_.push_back(PixelPoint(x, y, color));
  }
#else
  pixelPoints_.push_back(PixelPoint(x, y, color));
#endif
}

void
CPageText::
addLine(int x1, int y1, int x2, int y2, const CEscapeColor &color, const CEscapeLineStyle &style)
{
  pixelLines_.push_back(PixelLine(x1, y1, x2, y2, color, style));
}

void
CPageText::
setDirName(const std::string &dirName)
{
  if (dirName != dirName_) {
    dirName_ = dirName;

    notifyDirChanged(dirName_);
  }
}

void
CPageText::
makePosValid()
{
  makePosValid(getPosition());
}

void
CPageText::
makeSizeValid()
{
  makePosValid(CTextPos(getPageRows() - 1, getPageCols() - 1));
}

void
CPageText::
makePosValid(const CTextPos &pos)
{
  LineList &lines     = this->lines();
  uint      num_lines = uint(lines.size());

  uint row = pos.getRow();

  while (row >= num_lines) {
    lines.push_back(new CPageTextLine(this));

    ++num_lines;
  }

  CPageTextLine *line = lines[row];

  line->makeColValid(pos.getCol());
}

bool
CPageText::
loadCmd(const std::string &fileName)
{
  CFile file(fileName);

  std::string line;

  while (file.readLine(line)) {
    loadCmdLine(line);
  }

  return true;
}

bool
CPageText::
loadCmdLine(const std::string &line)
{
  CStrParse parse(line);

  parse.skipSpace();

  std::string cmd;

  if (! parse.readIdentifier(cmd))
    return false;

  parse.skipSpace();

  if      (cmd == "move") {
    int row;

    if (! parse.readInteger(&row))
      return false;

    parse.skipSpace();

    int col;

    if (! parse.readInteger(&col))
      return false;

    setPosition(CTextPos(row, col));
  }
  else if (cmd == "text") {
    std::string str;

    if (! parse.readString(str, true))
      return false;

    insertText(str);
  }
  else if (cmd == "bg") {
    std::string color;

    if (! parse.readNonSpace(color))
      return false;

    CRGBA bg = CRGBName::toRGBA(color);

    setBg(bg);
  }
  else if (cmd == "cell_bg" || cmd == "cell_fg") {
    std::string color;

    if (! parse.readNonSpace(color))
      return false;

    CRGBA fg = CRGBName::toRGBA(color);

    const CTextPos &pos = getPosition();

    CTextCell *cell = getCell(pos);

    CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);

    if (char_cell) {
      CCellStyle style = char_cell->getStyle();

      if (cmd == "cell_bg")
        style.setBg(fg.getRGB());
      else
        style.setFg(fg.getRGB());

      char_cell->setStyle(style);
    }

    notifyStyle(pos);
  }
  else if (cmd == "dim"  || cmd == "invert"     || cmd == "hidden" ||
           cmd == "bold" || cmd == "underscore" || cmd == "blink") {
    const CTextPos &pos = getPosition();

    CTextCell *cell = getCell(pos);

    CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);

    if (char_cell) {
      CCellStyle style = char_cell->getStyle();

      if      (cmd == "dim")
        style.setDim(! style.isDim());
      else if (cmd == "invert")
        style.setInvert(! style.isInvert());
      else if (cmd == "hidden")
        style.setHidden(! style.isHidden());
      else if (cmd == "bold")
        style.setBold(! style.isBold());
      else if (cmd == "underscore")
        style.setUnderscore(! style.isUnderscore());
      else if (cmd == "blink")
        style.setBlink(! style.isBlink());

      char_cell->setStyle(style);
    }

    notifyStyle(pos);
  }
  else if (cmd == "image") {
    std::string imageName;

    if (! parse.readString(imageName, true))
      return false;

    CImagePtr image = CImageMgrInst->lookupImage(imageName);

    addImage(imageName, image);
  }
  else if (cmd == "link") {
    std::string linkDest;

    if (! parse.readString(linkDest, true))
      return false;

    parse.skipSpace();

    std::string linkName;

    if (! parse.readString(linkName, true))
      return false;

    addLink(linkDest, linkName);
  }
  else {
    std::string str = CEscape::stringToEscape(line);

    if (str != "") {
      addEscapeChars(str.c_str(), int(str.size()));
    }
    else
      std::cerr << "Invalid command name \"" << cmd << "\"" << std::endl;
  }

  return true;
}

bool
CPageText::
loadEsc(const std::string &fileName)
{
  CFile file(fileName);

  std::string buffer;

  int c = 0;

  while ((c = file.getC()) != EOF) {
    if (c == '') {
      buffer += char(c);

      while ((c = file.getC()) != EOF) {
        buffer += char(c);

        if (buffer.size() > bufferSize_)
          break;
      }

      addEscapeChars(buffer.c_str(), int(buffer.size()));

      buffer.clear();
    }
    else {
#if 0
      notifier->addOutputChar(char(c));
#else
      buffer += char(c);

      while ((c = file.getC()) != EOF) {
        buffer += char(c);

        if (buffer.size() > bufferSize_)
          break;
      }

      addEscapeChars(buffer.c_str(), int(buffer.size()));

      buffer.clear();
    }
#endif
  }

  return true;
}

void
CPageText::
addEscapeChars(const char *str, uint len)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  notifier->addOutputChars(str, len);
}

std::string
CPageText::
getEventText(const CKeyEvent *e)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  if (notifier->getApplicationCursorKeys())
    return e->getEscapeTextSS3();
  else
    return e->getEscapeTextCSI();
}

void
CPageText::
notifyKey()
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  if (notifier->getScrollBottomOnKey())
    scrollBottom();

  update();
}

void
CPageText::
notifyTty()
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  if (notifier->getScrollBottomOnTty())
    scrollBottom();

  update();
}

void
CPageText::
processString(const char *str)
{
  CTty *tty = getTty();

  if (getTrace()) {
    CPageTextEscapeNotifier *notifier = getEscapeNotifier();

    notifier->logTrace(str);
  }

  if (tty)
    tty->write(str);
  else
    addEscapeChars(str, int(strlen(str)));

  flush();
}

void
CPageText::
pasteString(const char *str)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  if (notifier->getBracketedPasteMode()) {
    std::string str1 = "[200~" + std::string(&str[0]) + "[201~";

    processString(str1.c_str());
  }
  else
    processString(str);
}

bool
CPageText::
flush()
{
  CTty *tty = getTty();

  std::string output;

  if (tty)
    tty->read(output);

  if (output.empty()) return false;

  if (getTrace()) {
    CPageTextEscapeNotifier *notifier = getEscapeNotifier();

    notifier->logTrace(output);
  }

  addEscapeChars(output.c_str(), int(output.size()));

  notifyTty();

  return true;
}

void
CPageText::
paste(const std::string &str)
{
  if (! str.empty())
    pasteString(str.c_str());
  else
    pasteString(getSelectionText().c_str());
}

void
CPageText::
setShowToolBar(bool show)
{
  showToolBar_ = show;

  notifyToolBar();
}

void
CPageText::
setShowScrollBar(bool show)
{
  showScrollBar_ = show;

  notifyScrollBar();
}

void
CPageText::
setShowStatusBar(bool show)
{
  showStatusBar_ = show;

  notifyStatusBar();
}

void
CPageText::
notifyStateChange()
{
}

//------

void
CPageText::
exec4014BEL()
{
}

void
CPageText::
exec4014BS()
{
}

void
CPageText::
exec4014TAB()
{
}

void
CPageText::
exec4014LF()
{
}

void
CPageText::
exec4014VT()
{
}

void
CPageText::
exec4014FF()
{
}

void
CPageText::
exec4014CR()
{
}

void
CPageText::
exec4014CUF()
{
}

void
CPageText::
set4014GIN(bool)
{
}

void
CPageText::
set4014CharSet(int)
{
}

void
CPageText::
clear4014()
{
}

void
CPageText::
draw4014Line(int x1, int y1, int x2, int y2, const CEscapeColor &color,
             const CEscapeLineStyle &style)
{
  pixel4014Lines_.push_back(PixelLine(x1, y1, x2, y2, color, style));
}

void
CPageText::
draw4014Char(char)
{
}

CIPoint2D
CPageText::
get4014DataPos() const
{
  return CIPoint2D();
}

int
CPageText::
get4014NumDataRows() const
{
  return 0;
}

int
CPageText::
get4014NumDataCols() const
{
  return 0;
}

//------

void
CPageText::
checkLines()
{
  for (const auto &line : lines())
    assert(line);
}
