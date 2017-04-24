#ifndef CQPAGE_DATA_H
#define CQPAGE_DATA_H

#include <QWidget>
#include <CTextPos.h>

class CQPageText;
class CQPageTextCanvas;
class CQPageTextToolBar;
class CQPageTextStatus;
class CQPageText4014;
class QScrollBar;
class QLabel;
class QLineEdit;
class QTimer;
class QPainter;
class CTty;

class CQPageTextWidget : public QWidget {
  Q_OBJECT

  Q_PROPERTY(uint uiWidth  READ uiWidth)
  Q_PROPERTY(uint uiHeight READ uiHeight)

 public:
  CQPageTextWidget(QWidget *parent=nullptr);

  virtual ~CQPageTextWidget() { }

  void setDebug(bool debug);
  void setTrace(bool trace);

  virtual CQPageTextToolBar *createToolBar();
  virtual CQPageTextStatus  *createStatusBar();

  CQPageTextToolBar* getToolBar  () const { return toolBar_  ; }
  CQPageTextStatus*  getStatusBar() const { return statusBar_; }
  CQPageText*        getArea     () const { return area_     ; }
  QScrollBar*        getHScroll  () const { return hscroll_  ; }
  QScrollBar*        getVScroll  () const { return vscroll_  ; }

  int uiWidth () const { return ui_width_ ; }
  int uiHeight() const { return ui_height_; }

  void loadCmd(const std::string &fileName);
  void loadEsc(const std::string &fileName);

  void addEscapeChars(const char *str, uint len);

  void emitDisplayImage(const std::string &path);
  void emitDisplayFile (const std::string &path);

  void scrollBottom();

  virtual void showToolBar  (bool show);
  virtual void showScrollBar(bool show);
  virtual void showStatusBar(bool show);

  virtual void showFileBrowser() { }
  virtual void showImage() { }

  virtual CTty *getTty() const { return nullptr; }

  void processString(const char *str);

  void flush();

  void notifyPageSize(int rows, int cols);
  void notifyPosition(const CTextPos &pos);
  void notifyStyle(const CTextPos &pos);
  void notifyCharSize(int width, int height);

  void notifyDirChanged(const std::string &dirName);

  void updatePlacement();

  void updateHints();

  void sizeToRowCols(int w, int h, int *rows, int *cols) const;
  void rowColsToSize(int rows, int cols, int *w, int *h) const;

 private:
  void resizeEvent(QResizeEvent *);
  void showEvent(QShowEvent *);

 private slots:
  void hscrollSlot();
  void vscrollSlot();

  void blinkTimeout();

 signals:
  void pageSizeSignal(int rows, int cols);
  void dirChangeSignal(const QString &path);

  void displayImage(const QString &path);
  void displayFile (const QString &path);

 private:
  CQPageTextToolBar* toolBar_    { nullptr };
  CQPageTextStatus*  statusBar_  { nullptr };
  CQPageText*        area_       { nullptr };
  CQPageTextCanvas*  text_       { nullptr };
  QScrollBar*        hscroll_    { nullptr };
  QScrollBar*        vscroll_    { nullptr };
  QTimer*            blinkTimer_ { nullptr };
  int                ui_width_   { 0 };
  int                ui_height_  { 0 };
};

//------

#include <CPageText.h>

class CQPageText : public QObject, public CPageText {
  Q_OBJECT

  Q_PROPERTY(bool debug         READ getDebug         WRITE setDebug        )
  Q_PROPERTY(bool trace         READ getTrace         WRITE setTrace        )
  Q_PROPERTY(bool isAlt         READ isAlt            WRITE setIsAlt        )
  Q_PROPERTY(uint row           READ getRow           WRITE setRow          )
  Q_PROPERTY(uint col           READ getCol           WRITE setCol          )
  Q_PROPERTY(uint pageRows      READ getPageRows      WRITE setPageRows     )
  Q_PROPERTY(uint pageCols      READ getPageCols      WRITE setPageCols     )
  Q_PROPERTY(uint charWidth     READ getCharWidth)
  Q_PROPERTY(uint charHeight    READ getCharHeight)
  Q_PROPERTY(uint pageWidth     READ getPageWidth)
  Q_PROPERTY(uint pageHeight    READ getPageHeight)
  Q_PROPERTY(bool showToolbar   READ getShowToolBar   WRITE setShowToolBar  )
  Q_PROPERTY(bool showScrollBar READ getShowScrollBar WRITE setShowScrollBar)
  Q_PROPERTY(bool showStatusBar READ getShowStatusBar WRITE setShowStatusBar)
  Q_PROPERTY(bool moveOnClick   READ isMoveOnClick    WRITE setMoveOnClick)

 public:
  CQPageText(CQPageTextWidget *widget);

  bool isMoveOnClick() const { return moveOnClick_; }
  void setMoveOnClick(bool b) { moveOnClick_ = b; }

  void set4014(bool b) override;

  void add4014Line(int x1, int y1, int x2, int y2, const CEscapeColor &color,
                   const CEscapeLineStyle &style) override;

  void scrollBottom() override;

  void notifyToolBar  () override;
  void notifyScrollBar() override;
  void notifyStatusBar() override;

  void notifyPageSize(int rows, int cols) override;
  void notifyPosition(const CTextPos &pos) override;
  void notifyStyle(const CTextPos &pos) override;
  void notifyCharSize(int width, int height) override;

  void notifyDirChanged(const std::string &dirName) override;

  void update() override;

  CTty *getTty() const override;

 private:
  CQPageTextWidget *widget_   { nullptr };
  CQPageText4014   *text4014_ { nullptr };
  bool              moveOnClick_ { false };
};

//------

#include <CQWindow.h>

class CQPageTextState;

class CQPageTextCanvas : public CQWindow {
  Q_OBJECT

  Q_PROPERTY(uint charWidth  READ charWidth)
  Q_PROPERTY(uint charHeight READ charHeight)
  Q_PROPERTY(uint xOffset    READ xOffset)
  Q_PROPERTY(uint yOffset    READ yOffset)

 private:
  struct Region {
    CIBBox2D         bbox;
    std::string      text;
    const CTextCell *cell { nullptr };

    Region(const CIBBox2D &bbox1, const std::string &text1, const CTextCell *cell1) :
     bbox(bbox1), text(text1), cell(cell1) {
    }
  };

  //---

  struct ImageCell {
    int                   x { 0 }, y { 0 };
    const CTextImageCell *cell { nullptr };

    ImageCell(int x1, int y1, const CTextImageCell *cell1) :
     x(x1), y(y1), cell(cell1) {
    }
  };

  //---

  struct LinkCell {
    int                  x { 0 }, y { 0 };
    const CTextLinkCell *cell { nullptr };

    LinkCell(int x1, int y1, const CTextLinkCell *cell1) :
     x(x1), y(y1), cell(cell1) {
    }
  };

  //---

  typedef std::vector<Region>    RegionList;
  typedef std::vector<ImageCell> ImageCellList;
  typedef std::vector<LinkCell>  LinkCellList;

 public:
  CQPageTextCanvas(CQPageTextWidget *area);

  uint charWidth () const { return charWidth_ ; }
  uint charHeight() const { return charHeight_; }

  uint xOffset() const { return x_offset_; }
  uint yOffset() const { return y_offset_; }

  //---

  void addRegion(const Region &region) { regions_.push_back(region); }

  void doBlink();

  void paintEvent(QPaintEvent *);

  void resize(uint width, uint height) override;

  void resizeEvent(QResizeEvent *);

  void updateScrollBars();

  void drawGraphics(QPainter *painter);

  void drawCells(QPainter *painter);

  void drawLine(QPainter *painter, int y, const CPageTextLine *line);

  void drawCharCell (QPainter *painter, int x, int y, const CTextCharCell *char_cell);
  void drawImageCell(QPainter *painter, int x, int y, const CTextImageCell *image_cell);
  void drawLinkCell (QPainter *painter, int x, int y, const CTextLinkCell *link_cell);
  void drawEmptyCell(QPainter *painter, int x, int y, const CTextCell *cell);

  void drawCursor(QPainter *painter);

  void drawCharCellChar(QPainter *painter, int x, int y, const CTextCharCell *char_cell);

  void mousePressEvent  (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;
  void mouseMoveEvent   (QMouseEvent *) override;

  void mouseDoubleClickEvent(QMouseEvent *) override;

  void keyPressEvent(QKeyEvent *e) override;

  void contextMenuEvent(QContextMenuEvent *event);

  void focusInEvent (QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);

  Region *getRegionAtPoint(int x, int y);

 public slots:
  void toggleToolBar();
  void toggleStatusBar();
  void toggleScrollBar();

  void displayStatus();

 private:
  CQPageTextWidget* area_ { nullptr };
  QImage            qimage_;
  RegionList        regions_;
  ImageCellList     image_cells_;
  LinkCellList      link_cells_;
  bool              do_blink_ { false };
  int               blink_num_ { 0 };
  uint              charWidth_ { 0 }, charHeight_ { 0 };
  uint              x_offset_ { 0 }, y_offset_ { 0 };
  bool              pressed_ { false };
  bool              selecting_ { false };
  bool              focus_ { false };
  CQPageTextState*  stateDialog_ { nullptr };
};

//------

class CQPageTextStatus : public QWidget {
  Q_OBJECT

 public:
  CQPageTextStatus(CQPageTextWidget *widget);

  void addMsgLabel();

  void addCmdEdit();

  void addSizeLabel();

  void addPosLabel();

  void updateLabels();

  void addStretch();

  void setMsg(const QString &str);

 private slots:
  void cmdSlot();

 private:
  CQPageTextWidget *widget_    { nullptr };
  QLabel           *msgLabel_  { nullptr };
  QLineEdit        *cmdEdit_   { nullptr };
  QLabel           *sizeLabel_ { nullptr };
  QLabel           *posLabel_  { nullptr };
};

//------

class CQPageTextToolBar : public QWidget {
  Q_OBJECT

 public:
  CQPageTextToolBar(CQPageTextWidget *widget);

  void addButton();
  void addStretch();

 private:
  CQPageTextWidget *widget_ { nullptr };
};

//------

class CQPageText4014 : public QWidget {
  Q_OBJECT

 public:
  CQPageText4014(CQPageText *area);

  void addLine(int x1, int y1, int x2, int y2, const CEscapeColor &color,
               const CEscapeLineStyle &lineStyle);

  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *);

 private:
  typedef std::vector<CQPageText::PixelLine> PixelLines;

  CQPageText* area_ { nullptr };
  PixelLines  pixelLines_;
};

#endif
