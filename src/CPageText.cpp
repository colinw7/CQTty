#include <CPageText.h>
#include <CPageTextEscapeNotifier.h>
#include <CStrParse.h>
#include <CFontMgr.h>
#include <CWindow.h>
#include <CTty.h>
#include <CFuncs.h>
#include <CDir.h>
#include <COSEnv.h>

CPageText::
CPageText() :
 pos_          (0,0),
 is_alt_       (false),
 window_       (NULL),
 notifier_     (NULL),
 showScrollBar_(true),
 showToolBar_  (false),
 showStatusBar_(false),
 debug_        (false),
 trace_        (false),
 log_          (".page_text.log")
{
  page_rows_ = 60;
  page_cols_ = 100;

  setFontSet("courier", 8);

  COSEnv::setenv("LINES"  , page_rows_);
  COSEnv::setenv("COLUMNS", page_cols_);

  dirName_ = CDir::getCurrent();
}

CPageText::
~CPageText()
{
  for_each(old_lines_.begin(), old_lines_.end(), CDeletePointer());
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
log(char c)
{
  log_.write(c);
}

void
CPageText::
log(const std::string &str)
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

  COSEnv::setenv("CTERM_CHAR_WIDTH" , char_width );
  COSEnv::setenv("CTERM_CHAR_HEIGHT", char_height);
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
    return notifier->getColor(CESCAPE_COLOR_BG);
  else
    return notifier->getColor(CESCAPE_COLOR_BG).inverse();
}

void
CPageText::
setBg(const CRGBA &bg)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  notifier->setColor(CESCAPE_COLOR_BG, bg);
}

const CPageText::Page &
CPageText::
getCurrentPage() const
{
  if (is_alt_)
    return cur_page_;
  else
    return alt_page_;
}

CPageText::Page &
CPageText::
getCurrentPage()
{
  if (is_alt_)
    return cur_page_;
  else
    return alt_page_;
}

void
CPageText::
setIsAlt(bool alt)
{
  is_alt_ = alt;
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

    uint num_lines = lines.size();

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
  if (! hasSelection()) return "";

  std::string selText;

  int row = 0;
  int col = 0;

  bool in_selection = false;

  LineList::const_iterator pl1, pl2;

  for (pl1 = beginOldLine(), pl2 = endOldLine(); pl1 != pl2; ++pl1) {
    const CPageTextLine *line = *pl1;

    col = 0;

    CPageTextLine::CellList::const_iterator pc1, pc2;

    for (pc1 = line->beginCell(), pc2 = line->endCell(); pc1 != pc2; ++pc1) {
      const CTextCell *cell = *pc1;

      if      (cell->getType() == CTextCell::CHAR_TYPE) {
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

  for (pl1 = beginLine(), pl2 = endLine(); pl1 != pl2; ++pl1) {
    const CPageTextLine *line = *pl1;

    col = 0;

    CPageTextLine::CellList::const_iterator pc1, pc2;

    for (pc1 = line->beginCell(), pc2 = line->endCell(); pc1 != pc2; ++pc1) {
      const CTextCell *cell = *pc1;

      if      (cell->getType() == CTextCell::CHAR_TYPE) {
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

char
CPageText::
getCharSet(int id) const
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  return notifier->getCharSet(id);
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
  assert(pos.getRow() >= 0 && pos.getRow() < int(page_rows_));

  pos_ = pos;
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
  return page_rows_;
}

uint
CPageText::
getPageCols() const
{
  return page_cols_;
}

void
CPageText::
setPageSize(uint rows, uint cols)
{
  setPosition(CTextPos(0, 0));

  makeSizeValid();

  if (rows < page_rows_) {
    LineList &lines = getCurrentPage().lines;

    uint num_extra = page_rows_ - rows;

    for (uint i = 0; i < num_extra; ++i) {
      CPageTextLine *old_line = lines[0];

      uint num_lines = lines.size();

      for (uint j = 1; j < num_lines; ++j)
        lines[j - 1] = lines[j];

      lines.pop_back();

      if (! addOldLine(old_line))
        delete old_line;
    }
  }

  page_rows_ = rows;
  page_cols_ = cols;

  COSEnv::setenv("LINES"  , rows);
  COSEnv::setenv("COLUMNS", cols);

  notifyPageSize(rows, cols);
}

uint
CPageText::
getCharWidth() const
{
  return fonts_.getFont(CFONT_STYLE_NORMAL)->getCharWidth();
}

uint
CPageText::
getCharHeight() const
{
  return fonts_.getFont(CFONT_STYLE_NORMAL)->getCharHeight();
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
    return old_lines_.size();
  else
    return 0;
}

CPageTextLine *
CPageText::
getOldLine(uint i) const
{
  if (! isAlt())
    return old_lines_[i];
  else
    return NULL;
}

bool
CPageText::
addOldLine(CPageTextLine *line)
{
  if (! isAlt()) {
    old_lines_.push_back(line);

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

  return lines.size()*getCharHeight();
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

  return getCurrentLine()->getCell(pos.getCol());
}

CPageText::LineList::const_iterator
CPageText::
beginOldLine() const
{
  if (! isAlt())
    return old_lines_.begin();
  else
    return old_lines_.end();
}

CPageText::LineList::const_iterator
CPageText::
endOldLine() const
{
  return old_lines_.end();
}

CPageText::LineList::const_iterator
CPageText::
beginLine() const
{
  return getCurrentPage().lines.begin();
}

CPageText::LineList::const_iterator
CPageText::
endLine() const
{
  return getCurrentPage().lines.end();
}

CPageText::PixelList::const_iterator
CPageText::
beginPixels() const
{
  return pixels_.begin();
}

CPageText::PixelList::const_iterator
CPageText::
endPixels() const
{
  return pixels_.end();
}

CPageTextLine *
CPageText::
getCurrentLine()
{
  makePosValid();

  return getCurrentPage().lines[getRow()];
}

void
CPageText::
clear()
{
  LineList &lines = getCurrentPage().lines;

  uint num_lines = lines.size();

  for (uint i = 0; i < num_lines; ++i)
    lines[i]->erase();

  pixels_.clear();
}

void
CPageText::
clearAbove()
{
  LineList &lines = getCurrentPage().lines;

  uint num_lines = lines.size();

  for (uint i = 0; i < uint(getRow()) && i < num_lines; ++i)
    lines[i]->erase();
}

void
CPageText::
clearBelow()
{
  LineList &lines = getCurrentPage().lines;

  uint num_lines = lines.size();

  for (uint i = getRow() + 1; i < num_lines; ++i)
    lines[i]->erase();
}

void
CPageText::
clearSaved()
{
  for_each(old_lines_.begin(), old_lines_.end(), CDeletePointer());

  old_lines_.clear();

  update();
}

void
CPageText::
dumpScreen(CFile &file)
{
  LineList &lines = getCurrentPage().lines;

  uint num_lines = lines.size();

  for (uint i = 0; i < num_lines; ++i)
    lines[i]->dumpLine(file);
}

void
CPageText::
fill(char c)
{
  int cols = getPageCols();

  LineList &lines = getCurrentPage().lines;

  uint num_lines = lines.size();

  for (uint i = 0; i < num_lines; ++i) {
    lines[i]->makeColValid(cols);

    lines[i]->fill(c);
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
  LineList &lines = getCurrentPage().lines;

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
  LineList &lines = getCurrentPage().lines;

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

  LineList &lines = getCurrentPage().lines;

  CPageTextLine *old_line = lines[top_line];

  for (int i = top_line + 1; i <= bottom_line; ++i)
    lines[i - 1] = lines[i];

  lines[bottom_line] = new CPageTextLine(this);

  if (! notifier->hasScrollArea())
    old_lines_.push_back(old_line);
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

  LineList &lines = getCurrentPage().lines;

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
  uint num_chars = str.size();

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

  CTextCell *new_cell = old_cell->convertTo(CTextCell::CHAR_TYPE);

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
addImage(const std::string &fileName, CImagePtr image)
{
  makePosValid();

  CPageTextLine *line = getCurrentLine();

  CTextImageCell *image_cell = new CTextImageCell(line, getCol(), fileName, image);

  line->addImage(image_cell);
}

void
CPageText::
addPixel(int x, int y, const std::string &color)
{
  std::string::size_type pos = color.find(',');

  if (pos != std::string::npos) {
    std::string color1 = color;

    while (pos != std::string::npos) {
      std::string color2 = color1.substr(0, pos);

      pixels_.push_back(Pixel(x, y, color2));

      ++x;

      color1 = color1.substr(pos + 1);

      pos = color1.find(',');
    }
  }
  else
    pixels_.push_back(Pixel(x, y, color));
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
  LineList &lines = getCurrentPage().lines;

  uint num_lines = lines.size();

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
    CStrParse parse(line);

    parse.skipSpace();

    std::string cmd;

    if (! parse.readIdentifier(cmd))
      continue;

    parse.skipSpace();

    if      (cmd == "move") {
      int row;

      if (! parse.readInteger(&row)) continue;

      parse.skipSpace();

      int col;

      if (! parse.readInteger(&col)) continue;

      setPosition(CTextPos(row, col));
    }
    else if (cmd == "text") {
      std::string str;

      if (! parse.readString(str, true)) continue;

      insertText(str);
    }
    else if (cmd == "bg") {
      std::string color;

      if (! parse.readNonSpace(color))
        continue;

      //CRGBA bg = CRGBA(color);

      //setBg(bg);
    }
    else if (cmd == "fg") {
      std::string color;

      if (! parse.readNonSpace(color))
        continue;

      //CRGBA fg = CRGBA(color);

      //setFg(fg);
    }
    else if (cmd == "font") {
      std::string fontName;

      if (! parse.readNonSpace(fontName))
        continue;

      //CFontPtr font = CFontMgrInst->lookupFont(fontName);

      //setFont(font);
    }
    else if (cmd == "image") {
      std::string imageName;

      if (! parse.readString(imageName, true))
        continue;

      CImagePtr image = CImageMgrInst->lookupImage(imageName);

      addImage(imageName, image);
    }
    else if (cmd == "link") {
      std::string linkDest;

      if (! parse.readString(linkDest, true))
        continue;

      parse.skipSpace();

      std::string linkName;

      if (! parse.readString(linkName, true))
        continue;

      addLink(linkDest, linkName);
    }
    else {
      std::cerr << "Invalid command name \"" << cmd << "\"" << std::endl;
    }
  }

  return true;
}

bool
CPageText::
loadEsc(const std::string &fileName)
{
  CPageTextEscapeNotifier *notifier = getEscapeNotifier();

  CFile file(fileName);

  std::string buffer;

  int c = 0;

  while ((c = file.getC()) != EOF) {
    if (c == '') {
      buffer += c;

      while ((c = file.getC()) != EOF) {
        buffer += c;

        if (buffer.size() > 512)
          break;
      }

      notifier->addOutputChars(buffer.c_str(), buffer.size());

      buffer.clear();
    }
    else
      notifier->addOutputChar(char(c));
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

  flush();
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

  addEscapeChars(output.c_str(), output.size());

  notifyTty();

  return true;
}

void
CPageText::
paste(const std::string &str)
{
  if (! str.empty())
    processString(str.c_str());
  else
    processString(getSelectionText().c_str());
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
checkLines()
{
  LineList &lines = getCurrentPage().lines;

  uint num_lines = lines.size();

  for (uint i = 0; i < num_lines; ++i)
    assert(lines[i]);
}

//---------

CPageTextLine::
CPageTextLine(CPageText *area) :
 area_(area)
{
}

CPageTextLine::
~CPageTextLine()
{
  for_each(cells_.begin(), cells_.end(), CDeletePointer());
  for_each(links_.begin(), links_.end(), CDeletePointer());
}

CTextCell *
CPageTextLine::
getCell(uint col)
{
  makeColValid(col);

  return cells_[col];
}

CPageTextLine::CellList::const_iterator
CPageTextLine::
beginCell() const
{
  return cells_.begin();
}

CPageTextLine::CellList::const_iterator
CPageTextLine::
endCell() const
{
  return cells_.end();
}

CPageTextLine::LinkList::const_iterator
CPageTextLine::
beginLink() const
{
  return links_.begin();
}

CPageTextLine::LinkList::const_iterator
CPageTextLine::
endLink() const
{
  return links_.end();
}

CPageTextLine::ImageList::const_iterator
CPageTextLine::
beginImage() const
{
  return images_.begin();
}

CPageTextLine::ImageList::const_iterator
CPageTextLine::
endImage() const
{
  return images_.end();
}

void
CPageTextLine::
eraseLeft(uint col)
{
  uint num_cells = cells_.size();

  for (uint i = 0; i <= col && i < num_cells; ++i)
    clearCell(i, '\0');
}

void
CPageTextLine::
eraseRight(uint col)
{
  uint num_cells = cells_.size();

  for (uint i = col; i < num_cells; ++i)
    clearCell(i, '\0');
}

void
CPageTextLine::
erase()
{
  eraseRight(0);
}

void
CPageTextLine::
dumpLine(CFile &file)
{
  uint num_cells = cells_.size();

  for (uint i = 0; i < num_cells; ++i) {
    char c = getChar(i);

    if (c)
      file.write(c);
  }

  file.write('\n');
}

void
CPageTextLine::
fill(char c)
{
  uint num_cells = cells_.size();

  for (uint i = 0; i < num_cells; ++i)
    clearCell(i, c);
}

void
CPageTextLine::
shiftLeft(uint col)
{
  uint num_cells = cells_.size();

  uint col1 = col;
  uint col2 = col;

  for ( ; col2 < num_cells; ++col2) {
    CTextCell *cell = cells_[col2];

    CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);

    if (char_cell == NULL || char_cell->getChar() == '\0')
      break;
  }

  makeColValid(col2);

  CTextCell *old_cell = cells_[col1];

  for (int c = col1; c < int(col2); ++c)
    cells_[c] = cells_[c + 1];

  cells_[col2] = old_cell;

  clearCell(col2, ' ');
}

void
CPageTextLine::
shiftRight(uint col)
{
  uint num_cells = cells_.size();

  uint col1 = col;
  uint col2 = col;

  for ( ; col2 < num_cells; ++col2) {
    CTextCell *cell = cells_[col2];

    CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);

    if (char_cell == NULL || char_cell->getChar() == '\0')
      break;
  }

  makeColValid(col2);

  CTextCell *old_cell = cells_[col2];

  for (int c = col2; c > int(col1); --c)
    cells_[c] = cells_[c - 1];

  cells_[col1] = old_cell;

  clearCell(col1, ' ');
}

char
CPageTextLine::
getChar(uint col) const
{
  CTextCell *cell = cells_[col];

  CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);

  if (char_cell)
    return char_cell->getChar();
  else
    return '\0';
}

void
CPageTextLine::
clearCell(uint col, char c)
{
  makeColValid(col);

  CTextCell *old_cell = cells_[col];

  CTextCell *new_cell = old_cell->convertTo(CTextCell::CHAR_TYPE);

  CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(new_cell);

  char_cell->resetStyle();

  char_cell->setChar(c);

  if (new_cell != old_cell) {
    cells_[col] = new_cell;

    delete old_cell;
  }

  ImageList::iterator p = images_.find(col);

  if (p != images_.end()) {
    delete (*p).second;

    images_.erase(p);
  }
}

void
CPageTextLine::
makeColValid(uint col)
{
  uint num_cells = cells_.size();

  while (col >= num_cells) {
    cells_.push_back(new CTextCell(this));

    ++num_cells;
  }
}

void
CPageTextLine::
replaceCell(CTextCell *old_cell, CTextCell *new_cell)
{
  assert(old_cell != new_cell);

  uint num_cells = cells_.size();

  for (uint i = 0; i < num_cells; ++i) {
    if (cells_[i] != old_cell) continue;

    cells_[i] = new_cell;

    return;
  }

  assert(false);
}

void
CPageTextLine::
addLink(CTextLinkCell *link_cell)
{
  links_.push_back(link_cell);
}

void
CPageTextLine::
addImage(CTextImageCell *image_cell)
{
  int col = image_cell->getCol();

  ImageList::const_iterator p = images_.find(col);

  if (p != images_.end())
    delete (*p).second;

  images_[col] = image_cell;
}

bool
CPageTextLine::
getWordBounds(int col, uint &col1, uint &col2)
{
  uint num_cells = cells_.size();

  if (col < 0 || col >= int(num_cells)) return false;

  CTextCell *cell = cells_[col];

  CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);

  if (char_cell == NULL) return false;

  char c = char_cell->getChar();

  if (c == '\0' || isspace(c)) return false;

  col1 = col;
  col2 = col;

  while (col1 > 0) {
    CTextCell *cell = cells_[col1 - 1];

    CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);

    if (char_cell == NULL) break;

    char c = char_cell->getChar();

    if (c == '\0' || isspace(c)) break;

    --col1;
  }

  while (col2 < num_cells - 1) {
    CTextCell *cell = cells_[col2 + 1];

    CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);

    if (char_cell == NULL) break;

    char c = char_cell->getChar();

    if (c == '\0' || isspace(c)) break;

    ++col2;
  }

  return true;
}

//---------

CTextCell::
CTextCell(CPageTextLine *line, Type type) :
 line_(line), type_(type)
{
}

CTextCell *
CTextCell::
convertTo(Type type)
{
  if (type_ == type)
    return this;

  CTextCell *cell1 = NULL;

  switch (type) {
    case CHAR_TYPE:
      cell1 = new CTextCharCell(line_);
      break;
    default:
      break;
  }

  return cell1;
}

//---------

char
CTextCharCell::
getChar() const
{
  return c_;
}

void
CTextCharCell::
setChar(char c)
{
  c_ = c;
}

void
CTextCharCell::
setStyle(const CCellStyle &style)
{
  style_ = style;
}

void
CTextCharCell::
resetStyle()
{
  style_.reset();
}

//---------

void
CTextImageCell::
setFileName(const std::string &fileName)
{
  fileName_ = fileName;
}

void
CTextImageCell::
setImage(CImagePtr image)
{
  image_ = image;
}

//---------

void
CTextLinkCell::
setLinkDest(const std::string &linkDest)
{
  linkDest_ = linkDest;
}

void
CTextLinkCell::
setLinkName(const std::string &linkName)
{
  linkName_ = linkName;
}
