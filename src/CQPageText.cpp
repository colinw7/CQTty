#include <CQPageText.h>
#include <CQPageTextState.h>
#include <CPageTextEscapeNotifier.h>
#include <CPageText.h>
#include <CEscapeColors.h>
#include <CEscape.h>

#include <CQUtil.h>
#include <CQWindow.h>
#include <CFileUtil.h>

#include <QMenu>
#include <QScrollBar>
#include <QToolButton>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QToolTip>
#include <QTimer>
#include <QPainter>
#include <QContextMenuEvent>

CQPageTextWidget::
CQPageTextWidget(QWidget *parent) :
 QWidget(parent)
{
  setObjectName("widget");

  //---

  area_ = new CQPageText(this);

  text_ = new CQPageTextCanvas(this);

  hscroll_ = new QScrollBar(Qt::Horizontal, this);
  vscroll_ = new QScrollBar(Qt::Vertical  , this);

  connect(hscroll_, SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot()));
  connect(vscroll_, SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot()));

  area_->setWindow(text_);

  //---

  blinkTimer_ = new QTimer;

  connect(blinkTimer_, SIGNAL(timeout()), this, SLOT(blinkTimeout()));

  blinkTimer_->start(750);

  //---

  updatePlacement();
}

CQPageTextToolBar *
CQPageTextWidget::
createToolBar()
{
  toolBar_ = new CQPageTextToolBar(this);

  toolBar_->setVisible(false);

  return toolBar_;
}

CQPageTextStatus *
CQPageTextWidget::
createStatusBar()
{
  statusBar_ = new CQPageTextStatus(this);

  statusBar_->setVisible(false);

  return statusBar_;
}

void
CQPageTextWidget::
setDebug(bool debug)
{
  return area_->setDebug(debug);
}

void
CQPageTextWidget::
setTrace(bool trace)
{
  return area_->setTrace(trace);
}

void
CQPageTextWidget::
loadCmd(const std::string &fileName)
{
  area_->loadCmd(fileName);
}

void
CQPageTextWidget::
loadEsc(const std::string &fileName)
{
  area_->loadEsc(fileName);
}

void
CQPageTextWidget::
addEscapeChars(const char *str, uint len)
{
  area_->addEscapeChars(str, len);
}

void
CQPageTextWidget::
hscrollSlot()
{
  update();
}

void
CQPageTextWidget::
vscrollSlot()
{
  update();
}

void
CQPageTextWidget::
emitDisplayImage(const std::string &path)
{
  emit displayImage(QString(path.c_str()));
}

void
CQPageTextWidget::
emitDisplayFile(const std::string &path)
{
  emit displayFile(QString(path.c_str()));
}

void
CQPageTextWidget::
blinkTimeout()
{
  text_->doBlink();
}

void
CQPageTextWidget::
scrollBottom()
{
  text_->updateScrollBars();

  getVScroll()->setValue(getVScroll()->maximum());
}

void
CQPageTextWidget::
showScrollBar(bool show)
{
  getHScroll()->setVisible(show);
  getVScroll()->setVisible(show);

  updatePlacement();
}

void
CQPageTextWidget::
showToolBar(bool show)
{
  getToolBar()->setVisible(show);

  updatePlacement();
}

void
CQPageTextWidget::
showStatusBar(bool show)
{
  getStatusBar()->setVisible(show);

  updatePlacement();
}

void
CQPageTextWidget::
processString(const char *str)
{
  area_->processString(str);
}

void
CQPageTextWidget::
flush()
{
  area_->flush();
}

void
CQPageTextWidget::
notifyPageSize(int, int)
{
  updatePlacement();
}

void
CQPageTextWidget::
notifyPosition(const CTextPos &)
{
  getStatusBar()->updateLabels();

  text_->update();
}

void
CQPageTextWidget::
notifyCharSize(int, int)
{
  updatePlacement();
}

void
CQPageTextWidget::
notifyStyle(const CTextPos &)
{
  text_->update();
}

void
CQPageTextWidget::
notifyDirChanged(const std::string &dirName)
{
  QString dirName1(dirName.c_str());

  emit dirChangeSignal(dirName1);
}

void
CQPageTextWidget::
resizeEvent(QResizeEvent *)
{
  //updatePlacement();
}

void
CQPageTextWidget::
showEvent(QShowEvent *)
{
  updatePlacement();
}

void
CQPageTextWidget::
updatePlacement()
{
  uint rows, cols;

  area_->getPageSize(&rows, &cols);

  int textw = cols*area_->getCharWidth ();
  int texth = rows*area_->getCharHeight();

  //---

  int th = (area_->getShowToolBar  () ? (toolBar_   ? toolBar_  ->sizeHint().height() : 0) : 0);
  int sh = (area_->getShowStatusBar() ? (statusBar_ ? statusBar_->sizeHint().height() : 0) : 0);

  int hsize = (area_->getShowScrollBar() ? getHScroll()->sizeHint().height() : 0);
  int vsize = (area_->getShowScrollBar() ? getVScroll()->sizeHint().width () : 0);

  //---

  if (area_->getShowToolBar()) {
    if (toolBar_) {
      toolBar_->move(0, 0);
      toolBar_->resize(textw + vsize, th);
    }
  }


  if (area_->getShowStatusBar()) {
    if (statusBar_) {
      statusBar_->move(0, texth + th + hsize);
      statusBar_->resize(textw + vsize, sh);
    }
  }

  //---

  if (area_->getShowScrollBar()) {
    getHScroll()->move(0, texth + th);
    getHScroll()->resize(textw, hsize);

    getVScroll()->move(textw, th);
    getVScroll()->resize(vsize, texth);
  }

  //---

  text_->QWidget::move(0, th);
  text_->QWidget::resize(textw, texth);

  //---

  ui_width_  = vsize;
  ui_height_ = sh + th + hsize;

  //---

  setFixedSize(textw + ui_width_, texth + ui_height_);

  //---

  updateHints();

  //---

  if (getStatusBar())
    getStatusBar()->updateLabels();

  //---

  emit pageSizeSignal(rows, cols);
}

void
CQPageTextWidget::
updateHints()
{
  int min_rows = 2;
  int min_cols = 4;

  text_->setMinSize(uiWidth () + min_cols*area_->getCharWidth (),
                    uiHeight() + min_rows*area_->getCharHeight());

  text_->setBaseSize(uiWidth(), uiHeight());

  text_->setResizeInc(area_->getCharWidth(), area_->getCharHeight());
}

void
CQPageTextWidget::
sizeToRowCols(int w, int h, int *rows, int *cols) const
{
  int th = (area_->getShowToolBar  () ? (toolBar_   ? toolBar_  ->sizeHint().height() : 0) : 0);
  int sh = (area_->getShowStatusBar() ? (statusBar_ ? statusBar_->sizeHint().height() : 0) : 0);

  int hsize = (area_->getShowScrollBar() ? getHScroll()->sizeHint().height() : 0);
  int vsize = (area_->getShowScrollBar() ? getVScroll()->sizeHint().width () : 0);

  //---

  int textw = w - vsize;
  int texth = h - th - sh - hsize;

  //---

  *rows = texth/area_->getCharHeight();
  *cols = textw/area_->getCharWidth ();
}

void
CQPageTextWidget::
rowColsToSize(int rows, int cols, int *w, int *h) const
{
  int th = (area_->getShowToolBar  () ? (toolBar_   ? toolBar_  ->sizeHint().height() : 0) : 0);
  int sh = (area_->getShowStatusBar() ? (statusBar_ ? statusBar_->sizeHint().height() : 0) : 0);

  int hsize = (area_->getShowScrollBar() ? getHScroll()->sizeHint().height() : 0);
  int vsize = (area_->getShowScrollBar() ? getVScroll()->sizeHint().width () : 0);

  //---

  *w = cols*area_->getCharWidth () + vsize;
  *h = rows*area_->getCharHeight() + th + sh + hsize;
}

//----------

CQPageTextCanvas::
CQPageTextCanvas(CQPageTextWidget *area) :
 CQWindow(area), area_(area)
{
  setObjectName("canvas");

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);

  do_blink_  = false;
  blink_num_ = 0;
}

void
CQPageTextCanvas::
doBlink()
{
  if (focus_) {
    CQPageText *area = area_->getArea();

    CPageTextEscapeNotifier *notifier = area->getEscapeNotifier();

    if (notifier && notifier->getCursorBlink()) {
      do_blink_ = true;

      update();
    }
  }
}

void
CQPageTextCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  if (! do_blink_)
    updateScrollBars();

  //------

  if (do_blink_)
    painter.drawImage(QPoint(0, 0), qimage_);

  //------

  CQPageText *area = area_->getArea();

  //------

  bool clear = ! do_blink_;

  QPainter ipainter(&qimage_);

  if (clear)
    ipainter.fillRect(rect(), QBrush(CQUtil::rgbaToColor(area->getBg())));

  //------

  charWidth_  = area->getCharWidth ();
  charHeight_ = area->getCharHeight();

  //------

  if (! do_blink_)
    drawGraphics(&ipainter);

  //------

  drawCells(&ipainter);

  //------

  CPageTextEscapeNotifier *notifier = area->getEscapeNotifier();

  if (notifier && notifier->getCursorVisible())
    drawCursor(&ipainter);

  //------

  painter.drawImage(QPoint(0, 0), qimage_);

  //------

  if (do_blink_) {
    blink_num_ = 1 - blink_num_;

    do_blink_ = false;
  }
}

void
CQPageTextCanvas::
resize(uint width, uint height)
{
  CQPageText *area = area_->getArea();

  if (area_->getToolBar()->isVisible())
    height += area_->getToolBar()->height();

  if (area_->getStatusBar()->isVisible())
    height += area_->getStatusBar()->height();

  if (area->getShowScrollBar()) {
    height += area_->getHScroll()->height();
    width  += area_->getVScroll()->width();
  }

  area_->resize(width, height);

  area_->scrollBottom();
}

void
CQPageTextCanvas::
resizeEvent(QResizeEvent *)
{
  qimage_ = QImage(QSize(width(), height()), QImage::Format_ARGB32);

  CQPageText *area = area_->getArea();

  charWidth_  = area->getCharWidth ();
  charHeight_ = area->getCharHeight();

  int w = width ();
  int h = height();

  int rows = std::max(h/charHeight(), 1U);
  int cols = std::max(w/charWidth (), 1U);

  area->setPageSize(rows, cols);
}

void
CQPageTextCanvas::
updateScrollBars()
{
  CQPageText *area = area_->getArea();

  QScrollBar *hscroll = area_->getHScroll();
  QScrollBar *vscroll = area_->getVScroll();

  int w = width ();
  int h = height();

  int aw = area->getPageWidth ();
  int ah = area->getPageHeight() + area->getOldLinesHeight();

  int hs = std::min(w, aw);
  int vs = std::min(h, ah);

  hscroll->setMinimum(0);
  hscroll->setMaximum(aw - hs);
  hscroll->setPageStep(hs);

  vscroll->setMinimum(0);
  vscroll->setMaximum(ah - vs);
  vscroll->setPageStep(vs);

  x_offset_ = hscroll->value();
  y_offset_ = vscroll->value();
}

void
CQPageTextCanvas::
drawGraphics(QPainter *painter)
{
  CQPageText *area = area_->getArea();

  for (const auto &pixel : area->pixelPoints()) {
    CRGBA c = CEscapeColorsInst->decode(pixel.color);

    painter->setPen(CQUtil::rgbaToColor(c));

    painter->drawPoint(QPoint(pixel.x, pixel.y));
  }
}

void
CQPageTextCanvas::
drawCells(QPainter *painter)
{
  if (! do_blink_) {
    regions_.clear();

    image_cells_.clear();
    link_cells_ .clear();
  }

  //------

  CQPageText *area = area_->getArea();

  int min_y = -charHeight();
  int max_y = height() + charHeight() - 1;

  int y = -y_offset_;

  CQPageText::LineList::const_iterator pl1, pl2;

  for (pl1 = area->beginOldLine(), pl2 = area->endOldLine(); pl1 != pl2; ++pl1) {
    const CPageTextLine *line = *pl1;

    if (y >= min_y && y <= max_y)
      drawLine(painter, y, line);

    y += charHeight();
  }

  for (const auto &line : area->lines()) {
    if (y >= min_y && y <= max_y)
      drawLine(painter, y, line);

    y += charHeight();
  }

  //------

  if (! do_blink_) {
    uint num_image_cells = image_cells_.size();

    for (uint i = 0; i < num_image_cells; ++i) {
      const ImageCell &image_cell = image_cells_[i];

      drawImageCell(painter, image_cell.x, image_cell.y, image_cell.cell);
    }

    uint num_link_cells = link_cells_.size();

    for (uint i = 0; i < num_link_cells; ++i) {
      const LinkCell &link_cell = link_cells_[i];

      drawLinkCell(painter, link_cell.x, link_cell.y, link_cell.cell);
    }
  }
}

void
CQPageTextCanvas::
drawLine(QPainter *painter, int y, const CPageTextLine *line)
{
  int x = -x_offset_;

  for (const auto &cell : line->cells()) {
    if      (cell->getType() == CTextCell::Type::CHAR) {
      const CTextCharCell *char_cell = dynamic_cast<const CTextCharCell *>(cell);

      drawCharCell(painter, x, y, char_cell);
    }
    else if (cell->getType() == CTextCell::Type::NONE) {
      drawEmptyCell(painter, x, y, cell);
    }

    if (line->widthStyle() == CCellLineWidthStyle::DOUBLE)
      x += 2*charWidth();
    else
      x += charWidth();
  }

  if (! do_blink_) {
    for (const auto &idCell : line->links()) {
      const CTextLinkCell *link_cell = idCell.second;

      int x = link_cell->getCol()*charWidth();

      if (link_cell->getLine()->widthStyle() == CCellLineWidthStyle::DOUBLE)
        x *= 2;

      link_cells_.push_back(LinkCell(x, y, link_cell));
    }

    CPageTextLine::ImageList::const_iterator pi1, pi2;

    for (const auto &idImage : line->images()) {
      const CTextImageCell *image_cell = idImage.second;

      int x = image_cell->getCol()*charWidth();

      if (image_cell->getLine()->widthStyle() == CCellLineWidthStyle::DOUBLE)
        x *= 2;

      image_cells_.push_back(ImageCell(x, y, image_cell));
    }
  }
}

void
CQPageTextCanvas::
drawCharCell(QPainter *painter, int x, int y, const CTextCharCell *char_cell)
{
  const CCellStyle &style = char_cell->getStyle();

  bool invert = false;

  if (do_blink_) {
    if (! style.getBlink())
      return;

    invert = (blink_num_ == 1);
  }

  CQPageText *area = area_->getArea();

  int row, col;

  area->pixelToPos(x + x_offset_, y + y_offset_, &row, &col);

  if (area->isSelected(row, col))
    invert = ! invert;

  CRGBA bg = (! invert ? area->getBg(style) : area->getFg(style));

  CIBBox2D bbox = char_cell->getBBox(x, y, charWidth(), charHeight(), false);

  painter->fillRect(CQUtil::toQRect(bbox), CQUtil::rgbaToColor(bg));

  CRGBA fg = (! invert ? area->getFg(style) : area->getBg(style));

  painter->setPen(CQUtil::rgbaToColor(fg));

  QFont font = CQUtil::toQFont(area->getFont(style));

  painter->setFont(font);

  drawCharCellChar(painter, x, y, char_cell);
}

void
CQPageTextCanvas::
drawCharCellChar(QPainter *painter, int x, int y, const CTextCharCell *char_cell)
{
  QFontMetrics fm(painter->font());

  char c = char_cell->getChar();

  if (c == '\0') c = ' ';

  QString text(c);

  CPageTextLine *line = char_cell->getLine();

  if (line->widthStyle () != CCellLineWidthStyle ::SINGLE ||
      line->heightStyle() != CCellLineHeightStyle::SINGLE) {
    int tw = fm.width(text);
    int th = fm.height();

    QImage qimage(QSize(tw, th), QImage::Format_ARGB32);

    qimage.fill(0);

    QPainter ipainter(&qimage);

    ipainter.setPen(painter->pen());
    ipainter.setFont(painter->font());

    ipainter.drawText(QPoint(0, fm.ascent()), text);

    if (line->heightStyle() != CCellLineHeightStyle::SINGLE) {
      int y1 = 0;
      int y2 = th/2;

      if (line->heightStyle() == CCellLineHeightStyle::DOUBLE_BOTTOM) {
        y1 = th/2;
        y2 = th - 1;
      }

      for (int py = y1; py <= y2; ++py) {
        int py1 = 2*(py - y1);

        for (int px = 0; px < tw; ++px) {
          QRgb rgb = qimage.pixel(px, py);
          if (! rgb) continue;

          painter->setPen(rgb);

          painter->drawPoint(x + px, y + py1 + 0);
          painter->drawPoint(x + px, y + py1 + 1);
        }
      }
    }
    else {
      for (int py = 0; py < th; ++py) {
        for (int px = 0; px < tw; ++px) {
          QRgb rgb = qimage.pixel(px, py);
          if (! rgb) continue;

          painter->setPen(rgb);

          painter->drawPoint(x + 2*px + 0, y + py);
          painter->drawPoint(x + 2*px + 1, y + py);
        }
      }
    }
  }
  else {
    painter->drawText(QPoint(x, y + fm.ascent()), text);
  }
}

void
CQPageTextCanvas::
drawImageCell(QPainter *painter, int x, int y, const CTextImageCell *image_cell)
{
  if (do_blink_) return;

  CImagePtr image = image_cell->getImage();

  uint w = image->getWidth ();
  uint h = image->getHeight();

  uint cols = (w + charWidth () - 1)/charWidth ();
  uint rows = (h + charHeight() - 1)/charHeight();

  int dx = (cols*charWidth () - w)/2;
  int dy = (rows*charHeight() - h)/2;

  painter->drawImage(QPoint(x + dx, y + dy), CQUtil::toQImage(image));

  const std::string &str = image_cell->getFileName();

  CIBBox2D bbox(CIPoint2D(x, y), CISize2D(cols*charWidth(), rows*charHeight()));

  addRegion(Region(bbox, str, image_cell));
}

void
CQPageTextCanvas::
drawLinkCell(QPainter *, int x, int y, const CTextLinkCell *link_cell)
{
  if (do_blink_) return;

  const std::string &str = link_cell->getLinkName();

  CIBBox2D bbox(CIPoint2D(x, y), CISize2D(str.size()*charWidth(), charHeight()));

  addRegion(Region(bbox, str, link_cell));
}

void
CQPageTextCanvas::
drawEmptyCell(QPainter *painter, int x, int y, const CTextCell *cell)
{
  CQPageText *area = area_->getArea();

  int row, col;

  area->pixelToPos(x + x_offset_, y + y_offset_, &row, &col);

  bool invert = false;

  if (area->isSelected(row, col))
    invert = ! invert;

  CRGBA bg = (! invert ? area->getBg() : area->getFg());

  CIBBox2D bbox = cell->getBBox(x, y, charWidth(), charHeight(), false);

  painter->fillRect(CQUtil::toQRect(bbox), CQUtil::rgbaToColor(bg));
}

void
CQPageTextCanvas::
drawCursor(QPainter *painter)
{
  CQPageText *area = area_->getArea();

  const CTextPos &pos = area->getPosition();

  const CTextCell *cell = area->getCell(pos);

  const CTextCharCell *char_cell = dynamic_cast<const CTextCharCell *>(cell);

  int xf = (cell->getLine()->widthStyle() == CCellLineWidthStyle::DOUBLE ? 2 : 1);

  int x = xf*pos.getCol()*charWidth() - x_offset_;
  int y = area->getOldLinesHeight() + pos.getRow()*charHeight() - y_offset_;

  //---

  CIBBox2D bbox = cell->getBBox(x, y, charWidth(), charHeight(), true);

  //---

  CPageTextEscapeNotifier *notifier = area->getEscapeNotifier();

  CRGBA fg = (notifier ? notifier->getCursorColor() : CRGBA(1,1,1));
  CRGBA bg = area->getBg();

  if (! do_blink_ || blink_num_ == 0)
    painter->fillRect(CQUtil::toQRect(bbox), QBrush(CQUtil::rgbaToColor(fg)));
  else
    painter->fillRect(CQUtil::toQRect(bbox), QBrush(CQUtil::rgbaToColor(bg)));

  //---

  if (char_cell) {
    const CCellStyle &style = char_cell->getStyle();

    if (! do_blink_ || blink_num_ == 0)
      painter->setPen(CQUtil::rgbaToColor(bg));
    else
      painter->setPen(CQUtil::rgbaToColor(area->getFg(style)));

    //painter->setBackground((blink_num_ == 0 ? fg : bg);

    QFont font = CQUtil::toQFont(area->getFont(style));

    painter->setFont(font);

    drawCharCellChar(painter, x, y, char_cell);

    if      (char_cell->getLine()->heightStyle() == CCellLineHeightStyle::DOUBLE_TOP) {
      CTextPos pos1(pos.getRow() + 1, pos.getCol());

      const CTextCell *cell1 = area->getCell(pos1);

      const CTextCharCell *char_cell1 = dynamic_cast<const CTextCharCell *>(cell1);

      if (char_cell1)
        drawCharCellChar(painter, x, y + charHeight(), char_cell1);
    }
    else if (char_cell->getLine()->heightStyle() == CCellLineHeightStyle::DOUBLE_BOTTOM) {
      if (pos.getRow() > 0) {
        CTextPos pos1(pos.getRow() - 1, pos.getCol());

        const CTextCell *cell1 = area->getCell(pos1);

        const CTextCharCell *char_cell1 = dynamic_cast<const CTextCharCell *>(cell1);

        if (char_cell1)
          drawCharCellChar(painter, x, y - charHeight(), char_cell1);
      }
    }
  }
}

void
CQPageTextCanvas::
mousePressEvent(QMouseEvent *e)
{
  pressed_ = true;

  CQPageText *area = area_->getArea();

  int x = e->x() + x_offset_;
  int y = e->y() + y_offset_;

  CPageTextEscapeNotifier *notifier = area->getEscapeNotifier();

  if (notifier && notifier->getSendMousePress()) {
    int button = e->button() - 1;

    int row, col;

    area->pixelToPos(x, y, &row, &col);

    notifier->sendMousePress(button, col + 1, row + 1);
  }

  if      (e->button() == Qt::LeftButton) {
    int row, col;

    area->pixelToPos(x, y, &row, &col);

    area->setSelectionStart(CTextPos(row, col));

    update();

    selecting_ = true;

    if (area->isMoveOnClick()) {
      std::string str = CEscape::CUP(row + 1, col + 1);

      area_->processString(str.c_str());
    }
  }
  else if (e->button() == Qt::MidButton) {
    CQPageText *area = area_->getArea();

    std::string str = area->getSelectionText();

    area_->processString(str.c_str());
  }
}

void
CQPageTextCanvas::
mouseReleaseEvent(QMouseEvent *e)
{
  pressed_ = false;

  CQPageText *area = area_->getArea();

  int x = e->x() + x_offset_;
  int y = e->y() + y_offset_;

  CPageTextEscapeNotifier *notifier = area->getEscapeNotifier();

  if (notifier && notifier->getSendMousePress()) {
    int button = e->button() - 1;

    int row, col;

    area->pixelToPos(x, y, &row, &col);

    notifier->sendMouseRelease(button, col + 1, row + 1);
  }

  if (selecting_) {
    selecting_ = false;

    int row, col;

    area->pixelToPos(x, y, &row, &col);

    area->setSelectionEnd(CTextPos(row, col));

    update();
  }
}

void
CQPageTextCanvas::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_) {
    if (selecting_) {
      int x = e->x() + x_offset_;
      int y = e->y() + y_offset_;

      CQPageText *area = area_->getArea();

      int row, col;

      area->pixelToPos(x, y, &row, &col);

      area->setSelectionEnd(CTextPos(row, col));

      update();
    }
  }
  else {
    int x = e->x();
    int y = e->y();

    Region *region = getRegionAtPoint(x, y);

    if (region) {
      QPoint p1 = mapToGlobal(QPoint(region->bbox.getUL().x, region->bbox.getUL().y));
      QPoint p2 = mapToGlobal(QPoint(region->bbox.getLR().x, region->bbox.getLR().y));

      QToolTip::showText((p1 + p2)/2, region->text.c_str(), this, QRect(p1, p2));
    }
  }
}

void
CQPageTextCanvas::
mouseDoubleClickEvent(QMouseEvent *e)
{
  if      (e->button() == Qt::LeftButton) {
    int x = e->x();
    int y = e->y();

    Region *region = getRegionAtPoint(x, y);

    if (region) {
      const CTextCell *cell = region->cell;

      const CTextLinkCell  *link_cell  = nullptr;
      const CTextImageCell *image_cell = nullptr;

      if      ((link_cell = dynamic_cast<const CTextLinkCell *>(cell)) != nullptr) {
        CFileType type = CFileUtil::getType(link_cell->getLinkDest());

        if      (type & CFILE_TYPE_IMAGE) {
          area_->emitDisplayImage(link_cell->getLinkDest());

          return;
        }
        else if (type & CFILE_TYPE_INODE_DIR) {
          std::string cmd = "cd " + link_cell->getLinkDest() + "\n";

          area_->processString(cmd.c_str());

          return;
        }
        else {
          area_->emitDisplayFile(link_cell->getLinkDest());

          return;
        }
      }
      else if ((image_cell = dynamic_cast<const CTextImageCell *>(cell)) != nullptr) {
        area_->emitDisplayImage(image_cell->getFileName());

        return;
      }
    }

    CQPageText *area = area_->getArea();

    int row, col;

    area->pixelToPos(x + x_offset_, y + y_offset_, &row, &col);

    if (area->selectWord(row, col))
      update();
  }
}

void
CQPageTextCanvas::
keyPressEvent(QKeyEvent *e)
{
  CKeyEvent *ke = CQUtil::convertEvent(e);

  CQPageText *area = area_->getArea();

  // handle special pasted key sequence
  if (ke->getType() == CKEY_TYPE_Insert && ke->isShiftKey()) {
    std::string str = "_<paste/>\\";

    area->addEscapeChars(str.c_str(), str.size());

    return;
  }

  // handle file browser shortcut
  if (ke->isAltKey()) {
    if      (ke->getType() == CKEY_TYPE_f) {
      area_->showFileBrowser();

      return;
    }
    else if (ke->getType() == CKEY_TYPE_i) {
      area_->showImage();

      return;
    }
  }

  const std::string &str = area->getEventText(ke);

  if (! str.empty())
    area_->processString(str.c_str());
}

void
CQPageTextCanvas::
contextMenuEvent(QContextMenuEvent *e)
{
  int x = e->x();
  int y = e->y();

  Region *region = getRegionAtPoint(x, y);

  QMenu *menu = new QMenu;

  const CTextCell *cell = nullptr;

  if (region)
    cell = region->cell;

  const CTextLinkCell  *link_cell  = nullptr;
  const CTextImageCell *image_cell = nullptr;

  if      ((link_cell = dynamic_cast<const CTextLinkCell *>(cell)) != nullptr) {
    CFileType type = CFileUtil::getType(link_cell->getLinkDest());

    if      (type & CFILE_TYPE_IMAGE) {
      QAction *viewAction = new QAction("View", this);

      menu->addAction(viewAction);

      //connect(editAction, SIGNAL(triggered()), this, SLOT(menuEditSlot()));

      QAction *editAction = new QAction("Edit", this);

      menu->addAction(editAction);
    }
    else if (type & CFILE_TYPE_INODE_DIR) {
      QAction *cdAction = new QAction("Change Directiory", this);

      menu->addAction(cdAction);
    }
    else {
      QAction *viewAction = new QAction("View", this);

      menu->addAction(viewAction);

      QAction *editAction = new QAction("Edit", this);

      menu->addAction(editAction);
    }
  }
  else if ((image_cell = dynamic_cast<const CTextImageCell *>(cell)) != nullptr) {
    QAction *viewAction = new QAction("View", this);

    menu->addAction(viewAction);

    QAction *editAction = new QAction("Edit", this);

    menu->addAction(editAction);
  }
  else {
    CQPageText *area = area_->getArea();

    QAction *toolBarAction   = new QAction("Show ToolBar", this);
    QAction *statusBarAction = new QAction("Show Status Bar", this);
    QAction *scrollBarAction = new QAction("Show Scroll Bar", this);
    QAction *statusAction    = new QAction("Status", this);

    toolBarAction  ->setCheckable(true);
    statusBarAction->setCheckable(true);
    scrollBarAction->setCheckable(true);

    toolBarAction  ->setChecked(area_->getToolBar  ()->isVisible());
    statusBarAction->setChecked(area_->getStatusBar()->isVisible());
    scrollBarAction->setChecked(area->getShowScrollBar());

    menu->addAction(toolBarAction);
    menu->addAction(statusBarAction);
    menu->addAction(scrollBarAction);
    menu->addAction(statusAction);

    connect(toolBarAction  , SIGNAL(triggered()), this, SLOT(toggleToolBar  ()));
    connect(statusBarAction, SIGNAL(triggered()), this, SLOT(toggleStatusBar()));
    connect(scrollBarAction, SIGNAL(triggered()), this, SLOT(toggleScrollBar()));
    connect(statusAction   , SIGNAL(triggered()), this, SLOT(displayStatus  ()));
  }

  menu->popup(e->globalPos());
}

void
CQPageTextCanvas::
focusInEvent(QFocusEvent *)
{
  focus_ = true;
}

void
CQPageTextCanvas::
focusOutEvent(QFocusEvent *)
{
  focus_ = false;
}

void
CQPageTextCanvas::
toggleToolBar()
{
  CQPageText *area = area_->getArea();

  area->setShowToolBar(! area->getShowToolBar());
}

void
CQPageTextCanvas::
toggleStatusBar()
{
  CQPageText *area = area_->getArea();

  area->setShowStatusBar(! area->getShowStatusBar());
}

void
CQPageTextCanvas::
toggleScrollBar()
{
  CQPageText *area = area_->getArea();

  area->setShowScrollBar(! area->getShowScrollBar());
}

void
CQPageTextCanvas::
displayStatus()
{
  CQPageText *area = area_->getArea();

  if (! stateDialog_)
    stateDialog_ = new CQPageTextState(area);

  stateDialog_->updateState();

  stateDialog_->show();
}

CQPageTextCanvas::Region *
CQPageTextCanvas::
getRegionAtPoint(int x, int y)
{
  for (auto &region : regions_) {
    if (! region.bbox.inside(CIPoint2D(x, y)))
      continue;

    return &region;
  }

  return nullptr;
}

//------

CQPageText::
CQPageText(CQPageTextWidget *widget) :
 QObject(widget), CPageText(), widget_(widget)
{
  setObjectName("text");
}

void
CQPageText::
set4014(bool b)
{
  if (b) {
    if (! text4014_)
      text4014_ = new CQPageText4014(this);

    text4014_->show();
  }

  CPageText::set4014(b);
}

void
CQPageText::
add4014Line(int x1, int y1, int x2, int y2, const CEscapeColor &color,
            const CEscapeLineStyle &style)
{
  if (text4014_)
    text4014_->addLine(x1, y1, x2, y2, color, style);

  CPageText::add4014Line(x1, y1, x2, y2, color, style);
}

void
CQPageText::
scrollBottom()
{
  widget_->scrollBottom();
}

void
CQPageText::
notifyToolBar()
{
  bool show = getShowToolBar();

  widget_->showToolBar(show);
}

void
CQPageText::
notifyScrollBar()
{
  bool show = getShowScrollBar();

  widget_->showScrollBar(show);
}

void
CQPageText::
notifyStatusBar()
{
  bool show = getShowStatusBar();

  widget_->showStatusBar(show);
}

void
CQPageText::
notifyPageSize(int rows, int cols)
{
  widget_->notifyPageSize(rows, cols);
}

void
CQPageText::
notifyPosition(const CTextPos &pos)
{
  widget_->notifyPosition(pos);
}

void
CQPageText::
notifyCharSize(int width, int height)
{
  widget_->notifyCharSize(width, height);
}

void
CQPageText::
notifyStyle(const CTextPos &pos)
{
  widget_->notifyStyle(pos);
}

void
CQPageText::
notifyDirChanged(const std::string &dirName)
{
  widget_->notifyDirChanged(dirName);
}

void
CQPageText::
update()
{
  widget_->update();
}

CTty *
CQPageText::
getTty() const
{
  return widget_->getTty();
}

//------

CQPageTextToolBar::
CQPageTextToolBar(CQPageTextWidget *widget) :
 QWidget(widget), widget_(widget)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);
}

void
CQPageTextToolBar::
addButton()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  QToolButton *button = new QToolButton;

  layout->addWidget(button);
}

void
CQPageTextToolBar::
addStretch()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  layout->addStretch(1);
}

//------

CQPageTextStatus::
CQPageTextStatus(CQPageTextWidget *widget) :
 QWidget(widget), widget_(widget)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);
}

void
CQPageTextStatus::
addStretch()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  layout->addStretch(1);
}

void
CQPageTextStatus::
addMsgLabel()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  msgLabel_ = new QLabel;

  layout->addWidget(msgLabel_);
}

void
CQPageTextStatus::
setMsg(const QString &str)
{
  msgLabel_->setText(str);
}

void
CQPageTextStatus::
addCmdEdit()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  cmdEdit_ = new QLineEdit;

  connect(cmdEdit_, SIGNAL(returnPressed()), this, SLOT(cmdSlot()));

  layout->addWidget(cmdEdit_);
}

void
CQPageTextStatus::
addPosLabel()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  posLabel_ = new QLabel;

  layout->addWidget(posLabel_);
}

void
CQPageTextStatus::
addSizeLabel()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  sizeLabel_ = new QLabel;

  layout->addWidget(sizeLabel_);
}

void
CQPageTextStatus::
cmdSlot()
{
  QString cmd = cmdEdit_->text();

  CQPageText *text = widget_->getArea();

  text->loadCmdLine(cmd.toStdString());
}

void
CQPageTextStatus::
updateLabels()
{
  CQPageText *text = widget_->getArea();

  int rows = text->getPageRows();
  int cols = text->getPageCols();

  if (sizeLabel_)
    sizeLabel_->setText(QString("%1x%2").arg(rows).arg(cols));

  ///

  CQPageText *area = widget_->getArea();

  const CTextPos &pos = area->getPosition();

  int x = pos.getCol();
  int y = pos.getRow();

  if (posLabel_)
    posLabel_->setText(QString("%1x%2").arg(x).arg(y));
}

//------

// In Alpha mode, the 4010 displays 35 lines of 74 characters
// Graphics is 1024, 780
CQPageText4014::
CQPageText4014(CQPageText *area) :
 area_(area)
{
  setFixedSize(1024, 780);
}

void
CQPageText4014::
addLine(int x1, int y1, int x2, int y2, const CEscapeColor &color,
        const CEscapeLineStyle &style)
{
  pixelLines_.push_back(CQPageText::PixelLine(x1, y1, x2, y2, color, style));
}

void
CQPageText4014::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  for (const auto &line : pixelLines_) {
    CRGBA c = CEscapeColorsInst->decode(line.color);

    QPen pen;

    pen.setColor(CQUtil::rgbaToColor(c));

    if      (line.lineStyle == CEscapeLineStyle::DOTTED)
      pen.setStyle(Qt::DotLine);
    else if (line.lineStyle == CEscapeLineStyle::DOT_DASH)
      pen.setStyle(Qt::DashDotLine);
    else if (line.lineStyle == CEscapeLineStyle::SHORT_DASH)
      pen.setStyle(Qt::DashLine);
    else if (line.lineStyle == CEscapeLineStyle::LONG_DASH) {
      pen.setStyle(Qt::CustomDashLine);
      pen.setDashPattern(QVector<qreal>() << 8 << 4);
    }

    painter.setPen(pen);

    painter.drawLine(QPoint(line.x1, line.y1), QPoint(line.x2, line.y2));
  }
}

void
CQPageText4014::
mousePressEvent(QMouseEvent *)
{
  update();
}
