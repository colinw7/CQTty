#ifndef CQPageTextCanvas_H
#define CQPageTextCanvas_H

#include <CQWindow.h>
#include <CIBBox2D.h>

class CQPageTextWidget;
class CQPageTextState;
class CPageTextLine;
class CTextCell;
class CTextImageCell;
class CTextLinkCell;
class CTextCharCell;
class CTextStyleCell;
class CTextUtfCharCell;

class CQPageTextCanvas : public CQWindow {
  Q_OBJECT

  Q_PROPERTY(uint charWidth  READ charWidth)
  Q_PROPERTY(uint charHeight READ charHeight)
  Q_PROPERTY(uint charAscent READ charAscent)
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
  uint charAscent() const { return charAscent_; }

  uint xOffset() const { return x_offset_; }
  uint yOffset() const { return y_offset_; }

  //---

  void addRegion(const Region &region) { regions_.push_back(region); }

  void doBlink();

  void paintEvent(QPaintEvent *);

  void resize(uint width, uint height) override;

  void resizeEvent(QResizeEvent *);

  void updateScrollBars();

  void updateState();

  void drawGraphics(QPainter *painter);

  void drawCells(QPainter *painter);

  void drawLine(QPainter *painter, int y, const CPageTextLine *line);

  void drawCharCell   (QPainter *painter, int x, int y, const CTextCharCell *char_cell);
  void drawUtfCharCell(QPainter *painter, int x, int y, const CTextUtfCharCell *char_cell);
  void drawImageCell  (QPainter *painter, int x, int y, const CTextImageCell *image_cell);
  void drawLinkCell   (QPainter *painter, int x, int y, const CTextLinkCell *link_cell);
  void drawEmptyCell  (QPainter *painter, int x, int y, const CTextCell *cell);

  void drawStyleCharCell(QPainter *painter, int x, int y,
                         const CTextStyleCell *style_cell, ulong c);

  void drawCursor(QPainter *painter);

  void drawStyleCharCellChar(QPainter *painter, int x, int y,
                             const CTextStyleCell *style_cell, ulong c);

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
  uint              charWidth_ { 8 }, charHeight_ { 12 }, charAscent_ { 2 };
  uint              x_offset_ { 0 }, y_offset_ { 0 };
  bool              pressed_ { false };
  bool              selecting_ { false };
  bool              focus_ { false };
  CQPageTextState*  stateDialog_ { nullptr };
};

#endif
