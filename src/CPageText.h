#ifndef CTEXT_AREA_H
#define CTEXT_AREA_H

#include <CRGBA.h>
#include <CRGBName.h>
#include <CFont.h>
#include <CCellStyle.h>
#include <CTextPos.h>
#include <CLogFile.h>
#include <CImageLib.h>

class CWindow;
class CPageTextLine;
class CTextCell;
class CTextLinkCell;
class CTextImageCell;
class CTty;
class CKeyEvent;
class CPageTextEscapeNotifier;

class CPageText {
 public:
  struct Pixel {
    int   x, y;
    CRGBA color;

    Pixel(int x1=0, int y1=0, const std::string &color1="white") :
     x(x1), y(y1), color(CRGBName::toRGBA(color1)) {
    }
  };

  typedef std::vector<CPageTextLine *> LineList;
  typedef std::vector<Pixel>           PixelList;

 protected:
  struct Page {
    LineList lines;
  };

  struct Selection {
    bool     set;
    CTextPos start;
    CTextPos end;

    Selection() : set(false) { }
  };

 public:
  CPageText();

  virtual ~CPageText();

  virtual CWindow *getWindow() const;

  virtual CPageTextEscapeNotifier *getEscapeNotifier() const;

  virtual void setDebug(bool debug);
  virtual void setTrace(bool trace);

  virtual bool getDebug() { return debug_; }
  virtual bool getTrace() { return trace_; }

  virtual void log(char c);
  virtual void log(const std::string &str);

  virtual void setWindow(CWindow *window);

  virtual void beep();

  virtual void setFontSet(const std::string &family, uint size);

  virtual void getFontSet(std::string &family, uint &size);

  virtual void resetAll();

  virtual CRGBA getBg() const;

  virtual void setBg(const CRGBA &bg);

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
  virtual uint getCol() const;

  virtual void setPosition(const CTextPos &pos);

  virtual void setRow(uint row);
  virtual void setCol(uint col);

  virtual uint getPageRows() const;
  virtual uint getPageCols() const;

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

  virtual CRGBA getBg(const CCellStyle &style);
  virtual CRGBA getFg(const CCellStyle &style);

  virtual CFontPtr getFont(const CCellStyle &style);

  virtual CTextCell *getCell(const CTextPos &pos);

  virtual LineList::const_iterator beginOldLine() const;
  virtual LineList::const_iterator endOldLine  () const;

  virtual LineList::const_iterator beginLine() const;
  virtual LineList::const_iterator endLine  () const;

  virtual CPageTextLine *getCurrentLine();

  virtual PixelList::const_iterator beginPixels() const;
  virtual PixelList::const_iterator endPixels  () const;

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

  virtual void addImage(const std::string &fileName, CImagePtr image);

  virtual void addPixel(int x, int y, const std::string &color);

  virtual void setDirName(const std::string &dirName);

  const std::string &getDirName() const { return dirName_; }

  virtual bool loadCmd(const std::string &fileName);

  virtual bool loadEsc(const std::string &fileName);

  virtual void addEscapeChars(const char *str, uint len);

  virtual std::string getEventText(const CKeyEvent *);

  virtual void notifyKey();
  virtual void notifyTty();

  virtual void processString(const char *str);

  virtual bool flush();

  virtual void paste(const std::string &str);

  virtual void setShowToolBar  (bool show);
  virtual void setShowScrollBar(bool show);
  virtual void setShowStatusBar(bool show);

  virtual bool getShowToolBar  () const { return showToolBar_; }
  virtual bool getShowScrollBar() const { return showScrollBar_; }
  virtual bool getShowStatusBar() const { return showStatusBar_; }

  virtual void notifyPageSize(int, int) { }

  virtual CTty *getTty() const { return NULL; }

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
  CTextPos                 pos_;
  uint                     page_rows_;
  uint                     page_cols_;
  Page                     cur_page_;  // current page
  Page                     alt_page_;  // alternative page
  LineList                 old_lines_; // lines scrolled off-screen
  bool                     is_alt_;
  Selection                selection_;
  PixelList                pixels_;
  CWindow                 *window_;
  CPageTextEscapeNotifier *notifier_;
  bool                     showScrollBar_;
  bool                     showToolBar_;
  bool                     showStatusBar_;
  bool                     debug_;
  bool                     trace_;
  std::string              dirName_;
  CLogFile                 log_;
};

class CPageTextLine {
 public:
  typedef std::vector<CTextCell *>       CellList;
  typedef std::vector<CTextLinkCell *>   LinkList;
  typedef std::map<int,CTextImageCell *> ImageList;

 public:
  CPageTextLine(CPageText *area);
 ~CPageTextLine();

  CPageText *getArea() const { return area_; }

  virtual const CCellStyle &getStyle() const { return area_->getStyle(); }

  CTextCell *getCell(uint col);

  CellList::const_iterator beginCell() const;
  CellList::const_iterator endCell  () const;

  LinkList::const_iterator beginLink() const;
  LinkList::const_iterator endLink  () const;

  ImageList::const_iterator beginImage() const;
  ImageList::const_iterator endImage  () const;

  void eraseLeft(uint col);
  void eraseRight(uint col);
  void erase();

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
  CPageText *area_;
  CellList   cells_;
  LinkList   links_;
  ImageList  images_;
};

// row span, col span
class CTextCell {
 public:
  enum Type {
    NO_TYPE,
    CHAR_TYPE,
    IMAGE_TYPE,
    LINK_TYPE
  };

 public:
  CTextCell(CPageTextLine *line, Type type=NO_TYPE);

  CPageTextLine *getLine() const { return line_; }

  Type getType() const { return type_; }

  virtual const CCellStyle &getStyle() const { return line_->getStyle(); }

  CTextCell *convertTo(Type type);

 protected:
  CPageTextLine *line_;
  Type           type_;
};

class CTextCharCell : public CTextCell {
 public:
  CTextCharCell(CPageTextLine *line, char c='\0') :
   CTextCell(line, CHAR_TYPE), c_(c) {
  }

  char getChar() const;

  void setChar(char c);

  const CCellStyle &getStyle() const { return style_; }

  void setStyle(const CCellStyle &style);

  void resetStyle();

 protected:
  char       c_;
  CCellStyle style_;
};

class CTextImageCell : public CTextCell {
 public:
  CTextImageCell(CPageTextLine *line, uint col=0, const std::string &fileName="",
                 CImagePtr image=CImagePtr()) :
   CTextCell(line, IMAGE_TYPE), col_(col), fileName_(fileName), image_(image) {
  }

  uint getCol() const { return col_; }

  void setCol(uint col) { col_ = col; }

  const std::string &getFileName() const { return fileName_; }

  void setFileName(const std::string &fileName);

  CImagePtr getImage() const { return image_; }

  void setImage(CImagePtr image);

 protected:
  uint        col_;
  std::string fileName_;
  CImagePtr   image_;
};

class CTextLinkCell : public CTextCell {
 public:
  CTextLinkCell(CPageTextLine *line, uint col=0) :
   CTextCell(line, LINK_TYPE), col_(col) {
  }

  uint getCol() const { return col_; }

  void setCol(uint col) { col_ = col; }

  const std::string &getLinkDest() const { return linkDest_; }
  const std::string &getLinkName() const { return linkName_; }

  void setLinkDest(const std::string &linkDest);
  void setLinkName(const std::string &linkName);

 protected:
  uint        col_;
  std::string linkDest_;
  std::string linkName_;
};

#endif
