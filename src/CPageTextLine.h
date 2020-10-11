#ifndef CPageTextLine_H
#define CPageTextLine_H

#include <CCellStyle.h>
#include <vector>
#include <map>

class CPageText;
class CTextCell;
class CTextLinkCell;
class CTextImageCell;
class CFile;

class CPageTextLine {
 public:
  using CellList  = std::vector<CTextCell *>;
  using LinkList  = std::map<int,CTextLinkCell *>;
  using ImageList = std::map<int,CTextImageCell *>;

 public:
  CPageTextLine(CPageText *area);

  virtual ~CPageTextLine();

  CPageText *getArea() const { return area_; }

  virtual const CCellStyle &getStyle() const;

  CTextCell *getCell(uint col);

  //---

  const CellList &cells() const { return cells_; }

  CellList::const_iterator beginCell() const;
  CellList::const_iterator endCell  () const;

  //---

  const LinkList links() const { return links_; }

  LinkList::const_iterator beginLink() const;
  LinkList::const_iterator endLink  () const;

  //---

  const ImageList &images() const { return images_; }

  ImageList::const_iterator beginImage() const;
  ImageList::const_iterator endImage  () const;

  //---

  const CCellLineWidthStyle &widthStyle() const { return widthStyle_; }
  void setWidthStyle(const CCellLineWidthStyle &v) { widthStyle_ = v; }

  const CCellLineHeightStyle &heightStyle() const { return heightStyle_; }
  void setHeightStyle(const CCellLineHeightStyle &v) { heightStyle_ = v; }

  //---

  void reset();

  //---

  void eraseLeft(uint col);
  void eraseRight(uint col);
  void erase();

  void resetLineStyle();

  //---

  void dumpLine(CFile &file);

  void fill(char c);

  void shiftLeft (uint col);
  void shiftRight(uint col);

  char getChar(uint i) const;

  void clearCell(uint i, char c);

  void makeColValid(uint col);

  void replaceCell(CTextCell *old_cell, CTextCell *new_cell);

  void addLink (CTextLinkCell  *link_cell);
  void addImage(CTextImageCell *image_cell);

  bool getWordBounds(int col, uint &col1, uint &col2);

 protected:
  CPageText*           area_ { nullptr };
  CellList             cells_;
  LinkList             links_;
  ImageList            images_;
  CCellLineWidthStyle  widthStyle_  { CCellLineWidthStyle::SINGLE };
  CCellLineHeightStyle heightStyle_ { CCellLineHeightStyle::SINGLE };
};

#endif
