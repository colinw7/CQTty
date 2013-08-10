#ifndef CTEXT_AREA_ESCAPE_NOTIFIER
#define CTEXT_AREA_ESCAPE_NOTIFIER

#include <CEscapeHandler.h>

class CPageText;

class CPageTextEscapeNotifier : public CEscapeHandler {
 private:
  CPageText *area_;
  bool       cursor_blink_;

  mutable CIPoint2D pos_;

 public:
  CPageTextEscapeNotifier(CPageText *area);

  const CIPoint2D &getDataPos() const;
  void setDataPos(const CIPoint2D &pos);

  // Display Position Get/Set
  void getDispPos(int *row, int *col) const;
  void setDispPos(int row, int col);

  void setDispRow(int row);
  void setDispCol(int col);

  void setDispSize(uint, uint);
  void getDispSize(uint *, uint *) const;

  void resetAll();

  void setCell(uint row, uint col, char c, const CCellStyle &style);

  void addSizedImageChar(const std::string &, CImageFile *file, int, int, int, int);
  void addImageChar(const std::string &, CImageFile *file, int, int, int, int);
  void addImage(const std::string &, CImageFile *file);

  void addPixel(int x, int y, const std::string &color);

  void setLinkCell(uint, uint, char, const CCellStyle &, const std::string &, const std::string &);
  void addLink(const std::string &name, const std::string &path, const std::string &);

  void resetCell();

  void pixelsToChars(int w, int h, int *cols, int *rows);
  void charsToPixels(int cols, int rows, int *w, int *h);

  uint getNumDispRows() const;
  uint getNumDispCols() const;

  uint getNumDataRows() const;
  uint getNumDataCols() const;

  void shiftLeft ();
  void shiftRight();

  const CCellStyle &getStyle() const;

  void setStyle(const CCellStyle &style);

  void notifyChar(uint, uint, char);
  void notifyEnter();

  void paste(const std::string &str);

  void setTrace(bool);
  bool getTrace() const;

  void setDebug(bool);
  bool getDebug() const;

  void setAlternative(bool alt);

  void insertLine(uint num);
  void deleteLine(uint num);

  void replicatePrevChar();

  void eraseLineLeft();
  void eraseLineRight();
  void eraseLineAll();

  void scrollUp  ();
  void scrollDown();

  void clearScreenBelow();
  void clearScreenAbove();
  void clearScreen     ();
  void clearScreenSaved();

  void fill(char);

  void startPrintLog();
  void stopPrintLog();
  void printLine();
  void printScreen();
  void printComposedScreen();
  void printAllPages();

  CWindow *getWindow() const;

  void setWindowTextSize(uint, uint);

  void beep();

  void setShowToolBar  (bool);
  void setShowScrollBar(bool);
  void setShowStatusBar(bool);

  void previewFile(const std::string &);
  void setDirName(const std::string &);

  void processString(const char *);

  void logDebug(const std::string &) const;
  void logTrace(char) const;
  void logTrace(const std::string &) const;
};

#endif
