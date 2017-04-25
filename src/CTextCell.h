#ifndef CTextCell_H
#define CTextCell_H

#include <CCellStyle.h>
#include <CImagePtr.h>
#include <CIBBox2D.h>
#include <vector>

class CPageTextLine;

// row span, col span
class CTextCell {
 public:
  enum class Type {
    NONE,
    CHAR,
    IMAGE,
    LINK
  };

 public:
  CTextCell(CPageTextLine *line, Type type=Type::NONE);

  virtual ~CTextCell() { }

  CPageTextLine *getLine() const { return line_; }

  Type getType() const { return type_; }

  virtual const CCellStyle &getStyle() const;

  CTextCell *convertTo(Type type);

  CIBBox2D getBBox(int x, int y, int w, int h, bool doubleHeight) const;

 protected:
  CPageTextLine *line_ { nullptr };
  Type           type_ { Type::NONE };
};

//------

class CTextCharCell : public CTextCell {
 public:
  CTextCharCell(CPageTextLine *line, char c='\0') :
   CTextCell(line, Type::CHAR), c_(c) {
  }

  char getChar() const;

  void setChar(char c);

  const CCellStyle &getStyle() const { return style_; }
  void setStyle(const CCellStyle &style);

  void resetStyle();

 protected:
  char       c_ { '\0' };
  CCellStyle style_;
};

//------

class CTextImageCell : public CTextCell {
 public:
  CTextImageCell(CPageTextLine *line, uint col=0, const std::string &fileName="",
                 const CImagePtr &image=CImagePtr()) :
   CTextCell(line, Type::IMAGE), col_(col), fileName_(fileName), image_(image) {
  }

  uint getCol() const { return col_; }

  void setCol(uint col) { col_ = col; }

  const std::string &getFileName() const { return fileName_; }

  void setFileName(const std::string &fileName);

  CImagePtr getImage() const { return image_; }

  void setImage(CImagePtr image);

 protected:
  uint        col_ { 0 };
  std::string fileName_;
  CImagePtr   image_;
};

//------

class CTextLinkCell : public CTextCell {
 public:
  CTextLinkCell(CPageTextLine *line, uint col=0) :
   CTextCell(line, Type::LINK), col_(col) {
  }

  uint getCol() const { return col_; }
  void setCol(uint col) { col_ = col; }

  const std::string &getLinkDest() const { return linkDest_; }
  const std::string &getLinkName() const { return linkName_; }

  void setLinkDest(const std::string &linkDest);
  void setLinkName(const std::string &linkName);

 protected:
  uint        col_ { 0 };
  std::string linkDest_;
  std::string linkName_;
};

#endif
