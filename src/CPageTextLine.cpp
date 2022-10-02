#include <CPageTextLine.h>
#include <CPageText.h>
#include <CTextCell.h>
#include <CImageLib.h>

CPageTextLine::
CPageTextLine(CPageText *area) :
 area_(area)
{
}

CPageTextLine::
~CPageTextLine()
{
  for (auto &cell : cells_)
    delete cell;

  for (auto &link : links_)
    delete link.second;

  for (auto &image : images_)
    delete image.second;
}

const CCellStyle &
CPageTextLine::
getStyle() const
{
  return area_->getStyle();
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
  uint numCells = uint(cells_.size());

  for (uint i = 0; i <= col && i < numCells; ++i)
    clearCell(i, '\0');

  if (col >= numCells - 1)
    resetLineStyle();
}

void
CPageTextLine::
eraseRight(uint col)
{
  uint numCells = uint(cells_.size());

  for (uint i = col; i < numCells; ++i)
    clearCell(i, '\0');

  if (col == 0)
    resetLineStyle();
}

void
CPageTextLine::
erase()
{
  eraseRight(0);
}

void
CPageTextLine::
reset()
{
  eraseRight(0);

  assert(links_ .empty());
  assert(images_.empty());
}

void
CPageTextLine::
resetLineStyle()
{
  widthStyle_  = CCellLineWidthStyle::SINGLE;
  heightStyle_ = CCellLineHeightStyle::SINGLE;
}

void
CPageTextLine::
dumpLine(CFile &file)
{
  uint numCells = uint(cells_.size());

  for (uint i = 0; i < numCells; ++i) {
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
  uint numCells = uint(cells_.size());

  for (uint i = 0; i < numCells; ++i)
    clearCell(i, c);
}

void
CPageTextLine::
shiftLeft(uint col)
{
  uint numCells = uint(cells_.size());

  uint col1 = col;
  uint col2 = col;

  for ( ; col2 < numCells; ++col2) {
    auto *cell = cells_[col2];

    auto *charCell = dynamic_cast<CTextCharCell *>(cell);

    if (! charCell || charCell->getChar() == '\0')
      break;
  }

  makeColValid(col2);

  auto *oldCell = cells_[col1];

  for (int c = col1; c < int(col2); ++c)
    cells_[c] = cells_[c + 1];

  cells_[col2] = oldCell;

  clearCell(col2, ' ');
}

void
CPageTextLine::
shiftRight(uint col)
{
  uint numCells = uint(cells_.size());

  uint col1 = col;
  uint col2 = col;

  for ( ; col2 < numCells; ++col2) {
    auto *cell = cells_[col2];

    auto *charCell = dynamic_cast<CTextCharCell *>(cell);

    if (! charCell || charCell->getChar() == '\0')
      break;
  }

  makeColValid(col2);

  auto *oldCell = cells_[col2];

  for (int c = col2; c > int(col1); --c)
    cells_[c] = cells_[c - 1];

  cells_[col1] = oldCell;

  clearCell(col1, ' ');
}

char
CPageTextLine::
getChar(uint col) const
{
  auto *cell = cells_[col];

  auto *charCell = dynamic_cast<CTextCharCell *>(cell);

  if (charCell)
    return charCell->getChar();
  else
    return '\0';
}

void
CPageTextLine::
clearCell(uint col, char c)
{
  makeColValid(col);

  auto *oldCell = cells_[col];

  auto *newCell = oldCell->convertTo(CTextCell::Type::CHAR);

  auto *charCell = dynamic_cast<CTextCharCell *>(newCell);

  charCell->resetStyle();

  charCell->setChar(c);

  if (newCell != oldCell) {
    cells_[col] = newCell;

    delete oldCell;
  }

  ImageList::iterator pi = images_.find(col);

  if (pi != images_.end()) {
    delete (*pi).second;

    images_.erase(pi);
  }

  LinkList::iterator pl = links_.find(col);

  if (pl != links_.end()) {
    delete (*pl).second;

    links_.erase(pl);
  }
}

void
CPageTextLine::
makeColValid(uint col)
{
  uint numCells = uint(cells_.size());

  while (col >= numCells) {
    cells_.push_back(new CTextCell(this));

    ++numCells;
  }
}

void
CPageTextLine::
replaceCell(CTextCell *oldCell, CTextCell *newCell)
{
  assert(oldCell != newCell);

  uint numCells = uint(cells_.size());

  for (uint i = 0; i < numCells; ++i) {
    if (cells_[i] != oldCell)
      continue;

    cells_[i] = newCell;

    return;
  }

  assert(false);
}

void
CPageTextLine::
addLink(CTextLinkCell *linkCell)
{
  int col = linkCell->getCol();

  LinkList::iterator pl = links_.find(col);

  if (pl != links_.end())
    delete (*pl).second;

  links_[col] = linkCell;
}

void
CPageTextLine::
addImage(CTextImageCell *imageCell)
{
  int col = imageCell->getCol();

  ImageList::const_iterator pi = images_.find(col);

  if (pi != images_.end())
    delete (*pi).second;

  images_[col] = imageCell;
}

bool
CPageTextLine::
getWordBounds(int col, uint &col1, uint &col2)
{
  uint numCells = uint(cells_.size());

  if (col < 0 || col >= int(numCells)) return false;

  auto *cell = cells_[col];

  auto *charCell = dynamic_cast<CTextCharCell *>(cell);
  if (! charCell) return false;

  char c = charCell->getChar();

  if (c == '\0' || isspace(c)) return false;

  col1 = col;
  col2 = col;

  while (col1 > 0) {
    auto *cell1 = cells_[col1 - 1];

    auto *charCell1 = dynamic_cast<CTextCharCell *>(cell1);
    if (! charCell1) break;

    char c1 = charCell1->getChar();

    if (c1 == '\0' || isspace(c1)) break;

    --col1;
  }

  while (col2 < numCells - 1) {
    auto *cell1 = cells_[col2 + 1];

    auto *charCell1 = dynamic_cast<CTextCharCell *>(cell1);
    if (! charCell1) break;

    char c1 = charCell1->getChar();

    if (c1 == '\0' || isspace(c1)) break;

    ++col2;
  }

  return true;
}
