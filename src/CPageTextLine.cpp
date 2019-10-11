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
  uint num_cells = cells_.size();

  for (uint i = 0; i <= col && i < num_cells; ++i)
    clearCell(i, '\0');

  if (col >= num_cells - 1)
    resetLineStyle();
}

void
CPageTextLine::
eraseRight(uint col)
{
  uint num_cells = cells_.size();

  for (uint i = col; i < num_cells; ++i)
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

    if (! char_cell || char_cell->getChar() == '\0')
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

    if (! char_cell || char_cell->getChar() == '\0')
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

  CTextCell *new_cell = old_cell->convertTo(CTextCell::Type::CHAR);

  CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(new_cell);

  char_cell->resetStyle();

  char_cell->setChar(c);

  if (new_cell != old_cell) {
    cells_[col] = new_cell;

    delete old_cell;
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
    if (cells_[i] != old_cell)
      continue;

    cells_[i] = new_cell;

    return;
  }

  assert(false);
}

void
CPageTextLine::
addLink(CTextLinkCell *link_cell)
{
  int col = link_cell->getCol();

  LinkList::iterator pl = links_.find(col);

  if (pl != links_.end())
    delete (*pl).second;

  links_[col] = link_cell;
}

void
CPageTextLine::
addImage(CTextImageCell *image_cell)
{
  int col = image_cell->getCol();

  ImageList::const_iterator pi = images_.find(col);

  if (pi != images_.end())
    delete (*pi).second;

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
  if (! char_cell) return false;

  char c = char_cell->getChar();

  if (c == '\0' || isspace(c)) return false;

  col1 = col;
  col2 = col;

  while (col1 > 0) {
    CTextCell *cell = cells_[col1 - 1];

    CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);
    if (! char_cell) break;

    char c = char_cell->getChar();

    if (c == '\0' || isspace(c)) break;

    --col1;
  }

  while (col2 < num_cells - 1) {
    CTextCell *cell = cells_[col2 + 1];

    CTextCharCell *char_cell = dynamic_cast<CTextCharCell *>(cell);
    if (! char_cell) break;

    char c = char_cell->getChar();

    if (c == '\0' || isspace(c)) break;

    ++col2;
  }

  return true;
}
