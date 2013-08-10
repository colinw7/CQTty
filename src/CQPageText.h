#ifndef CQPAGE_DATA_H
#define CQPAGE_DATA_H

#include <QWidget>
#include <CIBBox2D.h>

class CQPageText;
class CQPageTextCanvas;
class QScrollBar;
class QTimer;
class QPainter;
class CTty;

class CQPageTextWidget : public QWidget {
  Q_OBJECT

 public:
  CQPageTextWidget(QWidget *parent=NULL);

  virtual ~CQPageTextWidget() { }

  void setDebug(bool debug);
  void setTrace(bool trace);

  CQPageText *getArea   () const { return area_   ; }
  QScrollBar *getHScroll() const { return hscroll_; }
  QScrollBar *getVScroll() const { return vscroll_; }

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

  virtual CTty *getTty() const { return NULL; }

  void processString(const char *str);

  void flush();

  void notifyPageSize(int rows, int cols);

  void notifyDirChanged(const std::string &dirName);

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
  CQPageText*       area_;
  CQPageTextCanvas* text_;
  QScrollBar*       hscroll_;
  QScrollBar*       vscroll_;
  QTimer*           blinkTimer_;
};

#include <CPageText.h>

class CQPageText : public CPageText {
 public:
  CQPageText(CQPageTextWidget *widget);

  void scrollBottom();

  void notifyToolBar  ();
  void notifyScrollBar();
  void notifyStatusBar();

  void notifyPageSize(int rows, int cols);

  void notifyDirChanged(const std::string &dirName);

  void update();

  CTty *getTty() const;

 private:
  CQPageTextWidget *widget_;
};

#include <CQWindow.h>

class CQPageTextState;

class CQPageTextCanvas : public CQWindow {
  Q_OBJECT

 private:
  struct Region {
    CIBBox2D         bbox;
    std::string      text;
    const CTextCell *cell;

    Region(const CIBBox2D &bbox1, const std::string &text1, const CTextCell *cell1) :
     bbox(bbox1), text(text1), cell(cell1) {
    }
  };

  struct ImageCell {
    int                   x, y;
    const CTextImageCell *cell;

    ImageCell(int x1, int y1, const CTextImageCell *cell1) :
     x(x1), y(y1), cell(cell1) {
    }
  };

  struct LinkCell {
    int                  x, y;
    const CTextLinkCell *cell;

    LinkCell(int x1, int y1, const CTextLinkCell *cell1) :
     x(x1), y(y1), cell(cell1) {
    }
  };

  typedef std::vector<Region>    RegionList;
  typedef std::vector<ImageCell> ImageCellList;
  typedef std::vector<LinkCell>  LinkCellList;

 public:
  CQPageTextCanvas(CQPageTextWidget *area);

  void addRegion(const Region &region) { regions_.push_back(region); }

  void doBlink();

  void paintEvent(QPaintEvent *);

  void resizeEvent(QResizeEvent *);

  void updateScrollBars();

  void drawGraphics(QPainter *painter);

  void drawCells(QPainter *painter);

  void drawLine(QPainter *painter, int y, const CPageTextLine *line);

  void drawCell(QPainter *painter, int x, int y, const CTextCharCell *char_cell);
  void drawCell(QPainter *painter, int x, int y, const CTextImageCell *image_cell);
  void drawCell(QPainter *painter, int x, int y, const CTextLinkCell *link_cell);

  void drawCursor(QPainter *painter);

  void mousePressEvent  (QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent   (QMouseEvent *);

  void mouseDoubleClickEvent(QMouseEvent *);

  void keyPressEvent(QKeyEvent *e);

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
  CQPageTextWidget* area_;
  QImage            qimage_;
  RegionList        regions_;
  ImageCellList     image_cells_;
  LinkCellList      link_cells_;
  bool              do_blink_;
  int               blink_num_;
  uint              char_width_, char_height_;
  uint              x_offset_, y_offset_;
  int               ui_width_, ui_height_;
  bool              pressed_;
  bool              selecting_;
  bool              focus_;
  CQPageTextState*  stateDialog_;
};

#endif
