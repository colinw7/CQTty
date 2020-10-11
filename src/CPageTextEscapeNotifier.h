#ifndef CTEXT_AREA_ESCAPE_NOTIFIER
#define CTEXT_AREA_ESCAPE_NOTIFIER

#include <CEscapeHandler.h>

class CPageText;

class CPageTextEscapeNotifier : public CEscapeHandler {
 public:
  CPageTextEscapeNotifier(CPageText *area);

  void set4014(bool b) override;

  const CIPoint2D &getDataPos() const override;
  void setDataPos(const CIPoint2D &pos) override;

  // Display Position Get/Set
  void getDispPos(int *row, int *col) const override;
  void setDispPos(int row, int col) override;

  void setDispRow(int row) override;
  void setDispCol(int col) override;

  void setDispSize(uint, uint) override;
  void getDispSize(uint *, uint *) const override;

  void resetAll() override;

  void setLineWidthStyle(uint row, const CCellLineWidthStyle &lineStyle) override;
  void setLineHeightStyle(uint row, const CCellLineHeightStyle &lineStyle) override;

  void setCell(uint row, uint col, char c, const CCellStyle &style) override;
  void setUtfCell(uint row, uint col, ulong c, const CCellStyle &style) override;

  void addSizedImageChar(const std::string &, CImageFile *file, int, int, int, int) override;
  void addImageChar(const std::string &, CImageFile *file, int, int, int, int) override;
  void addImage(const std::string &, CImageFile *file) override;
  void addImage(const CImagePtr &image) override;

  void addPixel(int x, int y, const CEscapeColor &color) override;

  void addLine(int x1, int y1, int x2, int y2, const CEscapeColor &color,
               const CEscapeLineStyle &style) override;

  void setLinkCell(uint x, uint y, char c, const CCellStyle &style,
                   const std::string &type, const std::string &value) override;
  void setUtfLinkCell(uint x, uint y, ulong c, const CCellStyle &style,
                      const std::string &type, const std::string &value) override;

  void addLink(const std::string &name, const std::string &path,
               const std::string &) override;

  void resetCell() override;

  void pixelsToChars(int w, int h, int *cols, int *rows) override;
  void charsToPixels(int cols, int rows, int *w, int *h) override;

  uint getNumDispRows() const override;
  uint getNumDispCols() const override;

  uint getNumDataRows() const override;
  uint getNumDataCols() const override;

  void shiftLeft () override;
  void shiftRight() override;

  const CCellStyle &getStyle() const override;
  void setStyle(const CCellStyle &style) override;

  void notifyChar(uint, uint, char) override;
  void notifyUtfChar(uint, uint, ulong) override;

  void notifyEnter(char c) override;

  void notifyStateChange() override;

  void paste(const std::string &str) override;

  void setTrace(bool) override;
  bool getTrace() const override;

  void setDebug(bool) override;
  bool getDebug() const override;

  void setAlternative(bool alt) override;

  void insertLine(uint num) override;
  void deleteLine(uint num) override;

  void replicatePrevChar() override;

  void eraseLineLeft() override;
  void eraseLineRight() override;
  void eraseLineAll() override;

  void scrollUp  () override;
  void scrollDown() override;

  void clearScreenBelow() override;
  void clearScreenAbove() override;
  void clearScreen     () override;
  void clearScreenSaved() override;

  void fill(char) override;

  void startPrintLog() override;
  void stopPrintLog() override;
  void printLine() override;
  void printScreen() override;
  void printComposedScreen() override;
  void printAllPages() override;

  CWindow *getWindow() const override;

  void setWindowTextSize(uint, uint);

  void beep() override;

  void setShowToolBar  (bool) override;
  void setShowScrollBar(bool) override;
  void setShowStatusBar(bool) override;

  void previewFile(const std::string &) override;
  void setDirName(const std::string &) override;

  void processString(const char *) override;

  void logDebug(const std::string &) const override;
  void logTrace(const std::string &) const override;
  void logError(const std::string &) const override;

  //---

  // 4014
  void exec4014BEL() override;
  void exec4014BS() override;
  void exec4014TAB() override;
  void exec4014LF() override;
  void exec4014VT() override;
  void exec4014FF() override;
  void exec4014CR() override;
  void exec4014CUF() override;

  void set4014GIN(bool) override;

  void set4014CharSet(int) override;

  void clear4014() override;

  void draw4014Line(int, int, int, int, const CEscapeColor &, const CEscapeLineStyle &) override;
  void draw4014Char(char) override;

  CIPoint2D get4014DataPos() const override;

  int get4014NumDataRows() const override;
  int get4014NumDataCols() const override;

 private:
  CPageText* area_         { nullptr };
  bool       cursor_blink_ { false };

  mutable CIPoint2D pos_ { 0, 0 };
};

#endif
