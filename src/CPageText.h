#ifndef CTEXT_AREA_H
#define CTEXT_AREA_H

#include <CRGBA.h>
#include <CRGBName.h>
#include <CFont.h>
#include <CCellStyle.h>
#include <CTextPos.h>
#include <CLogFile.h>
#include <CImagePtr.h>

class CWindow;
class CPageTextLine;
class CTextCell;
class CTextLinkCell;
class CTextImageCell;
class CTty;
class CKeyEvent;
class CPageTextEscapeNotifier;

//------

class CPageText {
 public:
  struct PixelPoint {
    int          x { 0 }, y { 0 };
    CEscapeColor color;

    PixelPoint(int x=0, int y=0, const CEscapeColor &color=CEscapeColor::WHITE) :
     x(x), y(y), color(color) {
    }
  };

  struct PixelLine {
    int              x1 { 0 }, y1 { 0 };
    int              x2 { 0 }, y2 { 0 };
    CEscapeColor     color;
    CEscapeLineStyle lineStyle;

    PixelLine(int x1=0, int y1=0, int x2=0, int y2=0,
              const CEscapeColor &color=CEscapeColor::WHITE,
              const CEscapeLineStyle &lineStyle=CEscapeLineStyle::SOLID) :
     x1(x1), y1(y1), x2(x2), y2(y2), color(color), lineStyle(lineStyle) {
    }
  };

  typedef std::vector<CPageTextLine *> LineList;
  typedef std::vector<PixelPoint>      PixelPoints;
  typedef std::vector<PixelLine>       PixelLines;

 protected:
  struct Page {
    LineList lines;
  };

  struct Selection {
    bool     set { false };
    CTextPos start;
    CTextPos end;

    Selection() { }
  };

 public:
  CPageText();

  virtual ~CPageText();

  virtual CWindow *getWindow() const;

  virtual CPageTextEscapeNotifier *getEscapeNotifier() const;

  //---

  virtual bool getDebug() { return debug_; }
  virtual void setDebug(bool debug);

  virtual bool getTrace() { return trace_; }
  virtual void setTrace(bool trace);

  //---

  virtual void log(char c);
  virtual void log(const std::string &str);

  //---

  virtual void set4014(bool b) { is4014_ = b; }
  virtual bool is4014() const { return is4014_; }

  //---

  virtual void setWindow(CWindow *window);

  virtual void beep();

  virtual void getFontSet(std::string &family, uint &size);
  virtual void setFontSet(const std::string &family, uint size);

  virtual void resetAll();

  virtual CRGBA getBg() const;
  virtual CRGBA getBg(const CCellStyle &style);
  virtual void setBg(const CRGBA &bg);

  virtual CRGBA getFg() const;
  virtual CRGBA getFg(const CCellStyle &style);

  virtual const Page &getCurrentPage() const;

  virtual Page &getCurrentPage();

  virtual bool isAlt() const { return is_alt_; }
  virtual void setIsAlt(bool alt);

  virtual bool hasSelection() const { return selection_.set; }

  bool selectWord(int row, int col);

  virtual const CTextPos &getSelectionStart() const { return selection_.start; }
  virtual const CTextPos &getSelectionEnd  () const { return selection_.end  ; }

  virtual void clearSelection();
  virtual void setSelectionStart(const CTextPos &start);
  virtual void setSelectionEnd(const CTextPos &end);

  virtual std::string getSelectionText() const;

  virtual bool isSelected(int row, int col) const;

  virtual char getCharSet(int id) const;

  virtual const CCellStyle &getStyle() const;
  virtual void setStyle(const CCellStyle &style);

  virtual const CTextPos &getPosition() const;

  virtual uint getRow() const;
  virtual void setRow(uint row);

  virtual uint getCol() const;
  virtual void setCol(uint col);

  virtual void setPosition(const CTextPos &pos);

  virtual uint getPageRows() const;
  virtual void setPageRows(uint n);

  virtual uint getPageCols() const;
  virtual void setPageCols(uint n);

  virtual void getPageSize(uint *rows, uint *cols);
  virtual void setPageSize(uint rows, uint cols);

  virtual uint getCharWidth () const;
  virtual uint getCharHeight() const;

  virtual void pixelsToChars(int w, int h, int *cols, int *rows);
  virtual void charsToPixels(int cols, int rows, int *w, int *h);

  virtual void pixelToPos(int x, int y, int *row, int *col);

  virtual uint getPageWidth () const;
  virtual uint getPageHeight() const;

  virtual uint getNumOldLines() const;

  virtual CPageTextLine *getOldLine(uint pos) const;
  virtual bool addOldLine(CPageTextLine *line);

  virtual uint getOldLinesHeight() const;

  virtual uint getLinesHeight() const;

  virtual CFontPtr getFont(const CCellStyle &style);

  virtual CTextCell *getCell(const CTextPos &pos);

  virtual LineList::const_iterator beginOldLine() const;
  virtual LineList::const_iterator endOldLine  () const;

  virtual const CPageText::LineList &lines() const;
  virtual CPageText::LineList &lines();

  virtual LineList::const_iterator beginLine() const;
  virtual LineList::const_iterator endLine  () const;

  virtual CPageTextLine *getLine(uint row);

  virtual CPageTextLine *getCurrentLine();

  virtual const PixelPoints &pixelPoints() const { return pixelPoints_; }

  virtual PixelPoints::const_iterator beginPixelPoints() const;
  virtual PixelPoints::const_iterator endPixelPoints  () const;

  virtual const PixelLines &pixelLines() const { return pixelLines_; }

  virtual PixelLines::const_iterator beginPixelLines() const;
  virtual PixelLines::const_iterator endPixelLines  () const;

  virtual void clear();
  virtual void clearAbove();
  virtual void clearBelow();
  virtual void clearSaved();

  virtual void dumpScreen(CFile &file);

  virtual void fill(char c);

  virtual void insertLine(uint num);
  virtual void deleteLine(uint num);

  virtual void scrollUp();
  virtual void scrollDown();

  virtual void insertText(const std::string &str);

  virtual void insertChar(char c);

  virtual void setChar(char c);

  virtual void addLink(const std::string &linkDest, const std::string &linkName);

  virtual void addImage(const std::string &fileName, const CImagePtr &image);

  virtual void addPixel(int x, int y, const CEscapeColor &color);

  virtual void addLine(int x1, int y1, int x2, int y2, const CEscapeColor &color,
                       const CEscapeLineStyle &style);

  virtual void add4014Line(int x1, int y1, int x2, int y2, const CEscapeColor &color,
                           const CEscapeLineStyle &style);

  virtual void setDirName(const std::string &dirName);

  const std::string &getDirName() const { return dirName_; }

  virtual bool loadCmd(const std::string &fileName);
  virtual bool loadCmdLine(const std::string &line);

  virtual bool loadEsc(const std::string &fileName);

  virtual void addEscapeChars(const char *str, uint len);

  virtual std::string getEventText(const CKeyEvent *);

  virtual void notifyKey();
  virtual void notifyTty();

  virtual void processString(const char *str);

  virtual bool flush();

  virtual void paste(const std::string &str);

  virtual bool getShowToolBar() const { return showToolBar_; }
  virtual void setShowToolBar(bool show);

  virtual bool getShowScrollBar() const { return showScrollBar_; }
  virtual void setShowScrollBar(bool show);

  virtual bool getShowStatusBar() const { return showStatusBar_; }
  virtual void setShowStatusBar(bool show);

  virtual void notifyPageSize(int, int) { }
  virtual void notifyPosition(const CTextPos &) { }
  virtual void notifyCharSize(int, int) { }

  virtual void notifyStyle(const CTextPos &) { }

  virtual CTty *getTty() const { return nullptr; }

  //---------

  // virtuals to be implemented by graphical page text
  virtual void scrollBottom() { }

  virtual void notifyToolBar   () { }
  virtual void notifyScrollBar () { }
  virtual void notifyStatusBar () { }

  virtual void notifyDirChanged(const std::string &) { }

  virtual void update() { }

  //---------

 protected:
  virtual void makePosValid();
  virtual void makeSizeValid();
  virtual void makePosValid(const CTextPos &pos);

  virtual void checkLines();

 protected:
  CFontSet                 fonts_;
  CCellStyle               style_;
  CTextPos                 pos_ { 0, 0 };
  uint                     page_rows_ { 0 };
  uint                     page_cols_ { 0 };
  Page                     cur_page_;  // current page
  Page                     alt_page_;  // alternative page
  LineList                 old_lines_; // lines scrolled off-screen
  bool                     is_alt_ { false };
  bool                     is4014_ { false };
  Selection                selection_;
  PixelPoints              pixelPoints_;
  PixelLines               pixelLines_;
  PixelLines               pixel4014Lines_;
  CWindow                 *window_ { nullptr };
  CPageTextEscapeNotifier *notifier_ { nullptr };
  bool                     showScrollBar_ { true };
  bool                     showToolBar_ { false };
  bool                     showStatusBar_ { false };
  bool                     debug_ { false };
  bool                     trace_ { false };
  std::string              dirName_;
  CLogFile                 log_;
};

//------

class CPageTextLine {
 public:
  typedef std::vector<CTextCell *>       CellList;
  typedef std::map<int,CTextLinkCell *>  LinkList;
  typedef std::map<int,CTextImageCell *> ImageList;

 public:
  CPageTextLine(CPageText *area);

  virtual ~CPageTextLine();

  CPageText *getArea() const { return area_; }

  virtual const CCellStyle &getStyle() const { return area_->getStyle(); }

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

  void eraseLeft(uint col);
  void eraseRight(uint col);
  void erase();

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

//------

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

  virtual const CCellStyle &getStyle() const { return line_->getStyle(); }

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
