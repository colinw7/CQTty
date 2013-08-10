#include <CQPageText.h>
#include <CQPageTextState.h>
#include <CPageTextEscapeNotifier.h>
#include <CPageText.h>

#include <CQUtil.h>
#include <CQWindow.h>
#include <CFileUtil.h>

#include <QMenu>
#include <QGridLayout>
#include <QScrollBar>
#include <QMouseEvent>
#include <QToolTip>
#include <QTimer>
#include <QPainter>
#include <QContextMenuEvent>

using std::string;

CQPageTextWidget::
CQPageTextWidget(QWidget *parent) :
 QWidget(parent)
{
  area_ = new CQPageText(this);

  QGridLayout *grid = new QGridLayout(this);
  grid->setMargin(2); grid->setSpacing(2);

  text_ = new CQPageTextCanvas(this);

  hscroll_ = new QScrollBar(Qt::Horizontal);
  vscroll_ = new QScrollBar(Qt::Vertical  );

  QObject::connect(hscroll_, SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot()));
  QObject::connect(vscroll_, SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot()));

  grid->addWidget(text_   , 0, 0);
  grid->addWidget(hscroll_, 1, 0);
  grid->addWidget(vscroll_, 0, 1);

  area_->setWindow(text_);

  blinkTimer_ = new QTimer;

  connect(blinkTimer_, SIGNAL(timeout()), this, SLOT(blinkTimeout()));

  blinkTimer_->start(750);
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
loadCmd(const string &fileName)
{
  area_->loadCmd(fileName);
}

void
CQPageTextWidget::
loadEsc(const string &fileName)
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
emitDisplayImage(const string &path)
{
  emit displayImage(QString(path.c_str()));
}

void
CQPageTextWidget::
emitDisplayFile(const string &path)
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

  vscroll_->setValue(vscroll_->maximum());
}

void
CQPageTextWidget::
showScrollBar(bool show)
{
  hscroll_->setVisible(show);
  vscroll_->setVisible(show);
}

void
CQPageTextWidget::
showToolBar(bool)
{
}

void
CQPageTextWidget::
showStatusBar(bool)
{
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
notifyPageSize(int rows, int cols)
{
  emit pageSizeSignal(rows, cols);
}

void
CQPageTextWidget::
notifyDirChanged(const string &dirName)
{
  QString dirName1(dirName.c_str());

  emit dirChangeSignal(dirName1);
}

//----------

CQPageTextCanvas::
CQPageTextCanvas(CQPageTextWidget *area) :
 area_(area), pressed_(false), selecting_(false), focus_(false), stateDialog_(NULL)
{
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

    if (area->getEscapeNotifier() && area->getEscapeNotifier()->getCursorBlink()) {
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

  char_width_  = area->getCharWidth ();
  char_height_ = area->getCharHeight();

  if (! do_blink_) {
    int min_rows = 2;
    int min_cols = 4;

    setMinSize(ui_width_ + min_cols*char_width_, ui_height_ + min_rows*char_height_);

    setBaseSize(ui_width_, ui_height_);

    setResizeInc(char_width_, char_height_);
  }

  //------

  if (! do_blink_)
    drawGraphics(&ipainter);

  //------

  drawCells(&ipainter);

  //------

  if (area->getEscapeNotifier() && area->getEscapeNotifier()->getCursorVisible())
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
resizeEvent(QResizeEvent *)
{
  qimage_ = QImage(QSize(width(), height()), QImage::Format_ARGB32);

  CQPageText *area = area_->getArea();

  char_width_  = area->getCharWidth ();
  char_height_ = area->getCharHeight();

  int rows = std::max(height()/char_height_, 1U);
  int cols = std::max(width ()/char_width_ , 1U);

  area->setPageSize(rows, cols);

  area_->notifyPageSize(rows, cols);
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

  QWidget *parent = CQUtil::getToplevelWidget(this);

  ui_width_  = parent->width () - width ();
  ui_height_ = parent->height() - height();
}

void
CQPageTextCanvas::
drawGraphics(QPainter *painter)
{
  CQPageText *area = area_->getArea();

  CQPageText::PixelList::const_iterator p1, p2;

  for (p1 = area->beginPixels(), p2 = area->endPixels(); p1 != p2; ++p1) {
    const CQPageText::Pixel &pixel = *p1;

    painter->setPen(CQUtil::rgbaToColor(pixel.color));

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

  int min_y = -char_height_;
  int max_y = height() + char_height_ - 1;

  int y = -y_offset_;

  CQPageText::LineList::const_iterator pl1, pl2;

  for (pl1 = area->beginOldLine(), pl2 = area->endOldLine(); pl1 != pl2; ++pl1) {
    const CPageTextLine *line = *pl1;

    if (y >= min_y && y <= max_y)
      drawLine(painter, y, line);

    y += char_height_;
  }

  for (pl1 = area->beginLine(), pl2 = area->endLine(); pl1 != pl2; ++pl1) {
    const CPageTextLine *line = *pl1;

    if (y >= min_y && y <= max_y)
      drawLine(painter, y, line);

    y += char_height_;
  }

  //------

  if (! do_blink_) {
    uint num_image_cells = image_cells_.size();

    for (uint i = 0; i < num_image_cells; ++i) {
      const ImageCell &image_cell = image_cells_[i];

      drawCell(painter, image_cell.x, image_cell.y, image_cell.cell);
    }

    uint num_link_cells = link_cells_.size();

    for (uint i = 0; i < num_link_cells; ++i) {
      const LinkCell &link_cell = link_cells_[i];

      drawCell(painter, link_cell.x, link_cell.y, link_cell.cell);
    }
  }
}

void
CQPageTextCanvas::
drawLine(QPainter *painter, int y, const CPageTextLine *line)
{
  int x = -x_offset_;

  CPageTextLine::CellList::const_iterator pc1, pc2;

  for (pc1 = line->beginCell(), pc2 = line->endCell(); pc1 != pc2; ++pc1) {
    const CTextCell *cell = *pc1;

    if      (cell->getType() == CTextCell::CHAR_TYPE) {
      const CTextCharCell *char_cell = dynamic_cast<const CTextCharCell *>(cell);

      drawCell(painter, x, y, char_cell);
    }

    x += char_width_;
  }

  if (! do_blink_) {
    CPageTextLine::LinkList::const_iterator pl1, pl2;

    for (pl1 = line->beginLink(), pl2 = line->endLink(); pl1 != pl2; ++pl1) {
      const CTextLinkCell *link_cell = *pl1;

      int x = link_cell->getCol()*char_width_;

      link_cells_.push_back(LinkCell(x, y, link_cell));
    }

    CPageTextLine::ImageList::const_iterator pi1, pi2;

    for (pi1 = line->beginImage(), pi2 = line->endImage(); pi1 != pi2; ++pi1) {
      const CTextImageCell *image_cell = (*pi1).second;

      int x = image_cell->getCol()*char_width_;

      image_cells_.push_back(ImageCell(x, y, image_cell));
    }
  }
}

void
CQPageTextCanvas::
drawCell(QPainter *painter, int x, int y, const CTextCharCell *char_cell)
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

  CIBBox2D bbox(CIPoint2D(x, y), CISize2D(char_width_, char_height_));

  painter->fillRect(CQUtil::toQRect(bbox), CQUtil::rgbaToColor(bg));

  CRGBA fg = (! invert ? area->getFg(style) : area->getBg(style));

  painter->setPen(CQUtil::rgbaToColor(fg));

  QFont font = CQUtil::toQFont(area->getFont(style));

  painter->setFont(font);

  QFontMetrics fm(font);

  char c = char_cell->getChar();

  if (c == '\0') c = ' ';

  painter->drawText(QPoint(x, y + fm.ascent()), QString(c));
}

void
CQPageTextCanvas::
drawCell(QPainter *painter, int x, int y, const CTextImageCell *image_cell)
{
  if (do_blink_) return;

  CImagePtr image = image_cell->getImage();

  uint w = image->getWidth ();
  uint h = image->getHeight();

  uint cols = (w + char_width_  - 1)/char_width_;
  uint rows = (h + char_height_ - 1)/char_height_;

  int dx = (cols*char_width_  - w)/2;
  int dy = (rows*char_height_ - h)/2;

  painter->drawImage(QPoint(x + dx, y + dy), CQUtil::toQImage(image));

  const string &str = image_cell->getFileName();

  CIBBox2D bbox(CIPoint2D(x, y), CISize2D(cols*char_width_, rows*char_height_));

  addRegion(Region(bbox, str, image_cell));
}

void
CQPageTextCanvas::
drawCell(QPainter *, int x, int y, const CTextLinkCell *link_cell)
{
  if (do_blink_) return;

  const string &str = link_cell->getLinkName();

  CIBBox2D bbox(CIPoint2D(x, y), CISize2D(str.size()*char_width_, char_height_));

  addRegion(Region(bbox, str, link_cell));
}

void
CQPageTextCanvas::
drawCursor(QPainter *painter)
{
  CQPageText *area = area_->getArea();

  const CTextPos &pos = area->getPosition();

  const CTextCharCell *char_cell = dynamic_cast<const CTextCharCell *>(area->getCell(pos));

  int x =                             pos.getCol()*char_width_  - x_offset_;
  int y = area->getOldLinesHeight() + pos.getRow()*char_height_ - y_offset_;

  CIBBox2D bbox(CIPoint2D(x, y), CISize2D(char_width_ - 1, char_height_ - 1));

  CRGBA fg = area->getEscapeNotifier()->getCursorColor();
  CRGBA bg = area->getBg();

  if (! do_blink_ || blink_num_ == 0)
    painter->fillRect(CQUtil::toQRect(bbox), QBrush(CQUtil::rgbaToColor(fg)));
  else
    painter->fillRect(CQUtil::toQRect(bbox), QBrush(CQUtil::rgbaToColor(bg)));

  if (char_cell) {
    const CCellStyle &style = char_cell->getStyle();

    if (! do_blink_ || blink_num_ == 0)
      painter->setPen(CQUtil::rgbaToColor(bg));
    else
      painter->setPen(CQUtil::rgbaToColor(area->getFg(style)));

    //painter->setBackground((blink_num_ == 0 ? fg : bg);

    QFont font = CQUtil::toQFont(area->getFont(style));

    painter->setFont(font);

    QFontMetrics fm(font);

    char c = char_cell->getChar();

    if (c == '\0') c = ' ';

    painter->drawText(QPoint(x, y + fm.ascent()), QString(c));
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

  if (area->getEscapeNotifier() && area->getEscapeNotifier()->getSendMousePress()) {
    int button = e->button() - 1;

    int row, col;

    area->pixelToPos(x, y, &row, &col);

    area->getEscapeNotifier()->sendMousePress(button, col + 1, row + 1);
  }

  if      (e->button() == Qt::LeftButton) {
    int row, col;

    area->pixelToPos(x, y, &row, &col);

    area->setSelectionStart(CTextPos(row, col));

    update();

    selecting_ = true;
  }
  else if (e->button() == Qt::MidButton) {
    CQPageText *area = area_->getArea();

    string str = area->getSelectionText();

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

  if (area->getEscapeNotifier() && area->getEscapeNotifier()->getSendMousePress()) {
    int button = e->button() - 1;

    int row, col;

    area->pixelToPos(x, y, &row, &col);

    area->getEscapeNotifier()->sendMouseRelease(button, col + 1, row + 1);
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

      const CTextLinkCell  *link_cell  = NULL;
      const CTextImageCell *image_cell = NULL;

      if      ((link_cell = dynamic_cast<const CTextLinkCell *>(cell)) != NULL) {
        CFileType type = CFileUtil::getType(link_cell->getLinkDest());

        if      (type & CFILE_TYPE_IMAGE) {
          area_->emitDisplayImage(link_cell->getLinkDest());

          return;
        }
        else if (type & CFILE_TYPE_INODE_DIR) {
          string cmd = "cd " + link_cell->getLinkDest() + "\n";

          area_->processString(cmd.c_str());

          return;
        }
        else {
          area_->emitDisplayFile(link_cell->getLinkDest());

          return;
        }
      }
      else if ((image_cell = dynamic_cast<const CTextImageCell *>(cell)) != NULL) {
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
    string str = "_<paste/>\\";

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

  const string &str = area->getEventText(ke);

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

  const CTextCell *cell = NULL;

  if (region)
    cell = region->cell;

  const CTextLinkCell  *link_cell  = NULL;
  const CTextImageCell *image_cell = NULL;

  if      ((link_cell = dynamic_cast<const CTextLinkCell *>(cell)) != NULL) {
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
  else if ((image_cell = dynamic_cast<const CTextImageCell *>(cell)) != NULL) {
    QAction *viewAction = new QAction("View", this);

    menu->addAction(viewAction);

    QAction *editAction = new QAction("Edit", this);

    menu->addAction(editAction);
  }
  else {
    QAction *toolBarAction   = new QAction("Show ToolBar", this);
    QAction *statusBarAction = new QAction("Show Status Bar", this);
    QAction *scrollBarAction = new QAction("Show Scroll Bar", this);
    QAction *statusAction    = new QAction("Status", this);

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
  area_->getArea()->setShowToolBar(! area_->getArea()->getShowToolBar());
}

void
CQPageTextCanvas::
toggleStatusBar()
{
  area_->getArea()->setShowStatusBar(! area_->getArea()->getShowStatusBar());
}

void
CQPageTextCanvas::
toggleScrollBar()
{
  area_->getArea()->setShowScrollBar(! area_->getArea()->getShowScrollBar());
}

void
CQPageTextCanvas::
displayStatus()
{
  if (! stateDialog_)
    stateDialog_ = new CQPageTextState(area_->getArea());

  stateDialog_->updateState();

  stateDialog_->show();
}

CQPageTextCanvas::Region *
CQPageTextCanvas::
getRegionAtPoint(int x, int y)
{
  RegionList::iterator p1, p2;

  for (p1 = regions_.begin(), p2 = regions_.end(); p1 != p2; ++p1) {
    Region &region = *p1;

    if (! region.bbox.inside(CIPoint2D(x, y))) continue;

    return &region;
  }

  return NULL;
}

//------

CQPageText::
CQPageText(CQPageTextWidget *widget) :
 CPageText(), widget_(widget)
{
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
notifyDirChanged(const string &dirName)
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
