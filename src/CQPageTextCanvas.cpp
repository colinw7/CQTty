#include <CQPageTextCanvas.h>
#include <CQPageTextWidget.h>
#include <CQPageTextToolBar.h>
#include <CQPageTextStatus.h>
#include <CQPageText4014.h>
#include <CQPageTextState.h>
#include <CQPageText.h>
#include <CPageTextEscapeNotifier.h>
#include <CPageTextLine.h>
#include <CTextCell.h>
#include <CEscape.h>
#include <CEscapeColors.h>
#include <CUtf8.h>

#include <CQUtil.h>
#include <CFileUtil.h>

#include <QPainter>
#include <QScrollBar>
#include <QToolTip>
#include <QMenu>
#include <QToolButton>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>

CQPageTextCanvas::
CQPageTextCanvas(CQPageTextWidget *area) :
 CQWindow(area), area_(area)
{
  setObjectName("canvas");

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);
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
repaint()
{
  dirty_ = true;

  update();
}

void
CQPageTextCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  if (dirty_ || ! do_blink_)
    updateScrollBars();

  //------

  if (! dirty_ && do_blink_)
    painter.drawImage(QPoint(0, 0), qimage_);

  //------

  CQPageText *area = area_->getArea();

  //------

  bool clear = dirty_ || ! do_blink_;

  QPainter ipainter;

  ipainter.begin(&qimage_);

  if (clear)
    ipainter.fillRect(rect(), QBrush(CQUtil::rgbaToColor(area->getBg())));

  //------

  charWidth_  = area->getCharWidth ();
  charHeight_ = area->getCharHeight();
  charAscent_ = area->getCharAscent();

  //------

  if (dirty_ || ! do_blink_)
    drawGraphics(&ipainter);

  //------

  drawCells(&ipainter);

  //------

  CPageTextEscapeNotifier *notifier = area->getEscapeNotifier();

  if (notifier && notifier->getCursorVisible())
    drawCursor(&ipainter);

  //------

  ipainter.end();

  painter.drawImage(QPoint(0, 0), qimage_);

  //------

  if (do_blink_) {
    blink_num_ = 1 - blink_num_;

    do_blink_ = false;
  }

  dirty_ = false;
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

  QScrollBar *hbar = area_->getHScroll();
  QScrollBar *vbar = area_->getVScroll();

  int w = width ();
  int h = height();

  int aw = area->getPageWidth ();
  int ah = area->getPageHeight() + area->getOldLinesHeight();

  int hs = std::min(w, aw);
  int vs = std::min(h, ah);

  hbar->setMinimum(0);
  hbar->setMaximum(aw - hs);
  hbar->setPageStep(hs);

  vbar->setMinimum(0);
  vbar->setMaximum(ah - vs);
  vbar->setPageStep(vs);
  vbar->setSingleStep(charHeight_);

  x_offset_ = hbar->value();
  y_offset_ = vbar->value();
}

void
CQPageTextCanvas::
updateState()
{
  if (stateDialog_)
    stateDialog_->updateState();
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
  if (dirty_ || ! do_blink_) {
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

  if (dirty_ || ! do_blink_) {
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
    else if (cell->getType() == CTextCell::Type::UTF_CHAR) {
      const CTextUtfCharCell *char_cell = dynamic_cast<const CTextUtfCharCell *>(cell);

      drawUtfCharCell(painter, x, y, char_cell);
    }
    else if (cell->getType() == CTextCell::Type::NONE) {
      drawEmptyCell(painter, x, y, cell);
    }

    if (line->widthStyle() == CCellLineWidthStyle::DOUBLE)
      x += 2*charWidth();
    else
      x += charWidth();
  }

  if (dirty_ || ! do_blink_) {
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
  drawStyleCharCell(painter, x, y, char_cell, char_cell->getChar());
}

void
CQPageTextCanvas::
drawUtfCharCell(QPainter *painter, int x, int y, const CTextUtfCharCell *char_cell)
{
  drawStyleCharCell(painter, x, y, char_cell, char_cell->getUtfChar());
}

void
CQPageTextCanvas::
drawStyleCharCell(QPainter *painter, int x, int y, const CTextStyleCell *style_cell, ulong c)
{
  const CCellStyle &style = style_cell->getStyle();

  bool invert = false;

  if (! dirty_ && do_blink_) {
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

  CIBBox2D bbox = style_cell->getBBox(x, y, charWidth(), charHeight(), false);

  painter->fillRect(CQUtil::toQRect(bbox), CQUtil::rgbaToColor(bg));

  CRGBA fg = (! invert ? area->getFg(style) : area->getBg(style));

  painter->setPen(CQUtil::rgbaToColor(fg));

  QFont font = CQUtil::toQFont(area->getFont(style));

  painter->setFont(font);

  drawStyleCharCellChar(painter, x, y, style_cell, c);
}

void
CQPageTextCanvas::
drawStyleCharCellChar(QPainter *painter, int x, int y, const CTextStyleCell *cell, ulong c)
{
  std::string str;

  if (! CUtf8::append(str, c))
    return;

  QString text(str.c_str());

  CPageTextLine *line = cell->getLine();

  if (line->widthStyle () != CCellLineWidthStyle ::SINGLE ||
      line->heightStyle() != CCellLineHeightStyle::SINGLE) {
    int tw = charWidth ();
    int th = charHeight();

    QImage qimage(QSize(tw, th), QImage::Format_ARGB32);

    qimage.fill(0);

    QPainter ipainter(&qimage);

    ipainter.setPen(painter->pen());
    ipainter.setFont(painter->font());

    ipainter.drawText(QPoint(0, charAscent()), text);

    if (line->heightStyle() != CCellLineHeightStyle::SINGLE) {
      int y1 = 0;
      int y2 = th/2;

      if (line->heightStyle() == CCellLineHeightStyle::DOUBLE_BOTTOM) {
        y1 = th/2;
        y2 = th - 1;
      }

      if (line->widthStyle() != CCellLineWidthStyle::SINGLE) {
        for (int py = y1; py <= y2; ++py) {
          int py1 = 2*(py - y1);

          for (int px = 0; px < tw; ++px) {
            QRgb rgb = qimage.pixel(px, py);
            if (! rgb) continue;

            painter->setPen(rgb);

            painter->drawPoint(x + 2*px + 0, y + py1 + 0);
            painter->drawPoint(x + 2*px + 0, y + py1 + 1);
            painter->drawPoint(x + 2*px + 1, y + py1 + 0);
            painter->drawPoint(x + 2*px + 1, y + py1 + 1);
          }
        }
      }
      else {
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
    painter->drawText(QPoint(x, y + charAscent()), text);
  }
}

void
CQPageTextCanvas::
drawImageCell(QPainter *painter, int x, int y, const CTextImageCell *image_cell)
{
  if (! dirty_ && do_blink_)
    return;

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
  if (! dirty_ && do_blink_)
    return;

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

  const CTextStyleCell *style_cell = nullptr;

  if (cell->getType() == CTextCell::Type::CHAR ||
      cell->getType() == CTextCell::Type::UTF_CHAR) {
    style_cell = dynamic_cast<const CTextStyleCell *>(cell);
  }

  int xf = (cell->getLine()->widthStyle() == CCellLineWidthStyle::DOUBLE ? 2 : 1);

  int x = xf*pos.getCol()*charWidth() - x_offset_;
  int y = area->getOldLinesHeight() + pos.getRow()*charHeight() - y_offset_;

  //---

  CIBBox2D bbox = cell->getBBox(x, y, charWidth(), charHeight(), true);

  //---

  CPageTextEscapeNotifier *notifier = area->getEscapeNotifier();

  CRGBA fg = (notifier ? notifier->getCursorColor() : CRGBA(1,1,1));
  CRGBA bg = area->getBg();

  if (dirty_ || (! do_blink_ || blink_num_ == 0))
    painter->fillRect(CQUtil::toQRect(bbox), QBrush(CQUtil::rgbaToColor(fg)));
  else
    painter->fillRect(CQUtil::toQRect(bbox), QBrush(CQUtil::rgbaToColor(bg)));

  //---

  if (style_cell) {
    const CCellStyle &style = style_cell->getStyle();

    if (dirty_ || (! do_blink_ || blink_num_ == 0))
      painter->setPen(CQUtil::rgbaToColor(bg));
    else
      painter->setPen(CQUtil::rgbaToColor(area->getFg(style)));

    //painter->setBackground((blink_num_ == 0 ? fg : bg);

    QFont font = CQUtil::toQFont(area->getFont(style));

    painter->setFont(font);

    long c = (cell->getType() == CTextCell::Type::UTF_CHAR ?
      dynamic_cast<const CTextUtfCharCell *>(cell)->getUtfChar() :
      dynamic_cast<const CTextCharCell    *>(cell)->getChar());

    drawStyleCharCellChar(painter, x, y, style_cell, c);

    if      (cell->getLine()->heightStyle() == CCellLineHeightStyle::DOUBLE_TOP) {
      CTextPos pos1(pos.getRow() + 1, pos.getCol());

      const CTextCell *cell1 = area->getCell(pos1);

      const CTextStyleCell *style_cell1 = nullptr;

      if (cell1->getType() == CTextCell::Type::CHAR ||
          cell1->getType() == CTextCell::Type::UTF_CHAR)
        style_cell1 = dynamic_cast<const CTextStyleCell *>(cell1);

      if (style_cell1) {
        long c1 = (cell1->getType() == CTextCell::Type::UTF_CHAR ?
          dynamic_cast<const CTextUtfCharCell *>(cell1)->getUtfChar() :
          dynamic_cast<const CTextCharCell    *>(cell1)->getChar());

        drawStyleCharCellChar(painter, x, y + charHeight(), style_cell1, c1);
      }
    }
    else if (cell->getLine()->heightStyle() == CCellLineHeightStyle::DOUBLE_BOTTOM) {
      if (pos.getRow() > 0) {
        CTextPos pos1(pos.getRow() - 1, pos.getCol());

        const CTextCell *cell1 = area->getCell(pos1);

        const CTextStyleCell *style_cell1 = nullptr;

        if (cell1->getType() == CTextCell::Type::CHAR ||
            cell1->getType() == CTextCell::Type::UTF_CHAR)
          style_cell1 = dynamic_cast<const CTextStyleCell *>(cell1);

        if (style_cell1) {
          long c1 = (cell1->getType() == CTextCell::Type::UTF_CHAR ?
            dynamic_cast<const CTextUtfCharCell *>(cell1)->getUtfChar() :
            dynamic_cast<const CTextCharCell    *>(cell1)->getChar());

          drawStyleCharCellChar(painter, x, y - charHeight(), style_cell1, c1);
        }
      }
    }
  }
}

void
CQPageTextCanvas::
mousePressEvent(QMouseEvent *e)
{
  mouseState_.reset();

  mouseState_.pressed = true;

  //---

  CQPageText *area = area_->getArea();

  int x = e->x() + x_offset_;
  int y = e->y() + y_offset_;

  area->pixelToPos(x, y, &mouseState_.pressRow, &mouseState_.pressCol);

  //----

  CPageTextEscapeNotifier *notifier = area->getEscapeNotifier();

  if (notifier && notifier->getSendMousePress()) {
    int button = e->button() - 1;

    notifier->sendMousePress(button, mouseState_.pressCol + 1, mouseState_.pressRow + 1);
  }

  //----

  if      (e->button() == Qt::LeftButton) {
    mouseState_.select    = true;
    mouseState_.selecting = false;

    if (area->isMoveOnClick()) {
      std::string str = CEscape::CUP(mouseState_.pressRow + 1, mouseState_.pressCol + 1);

      area_->processString(str.c_str());
    }

    if (area->hasSelection()) {
      area->clearSelection();

      update();
    }
  }
  else if (e->button() == Qt::MidButton) {
    CQPageText *area = area_->getArea();

    std::string str = area->getSelectionText();

    area_->pasteString(str.c_str());
  }
}

void
CQPageTextCanvas::
mouseReleaseEvent(QMouseEvent *e)
{
  CQPageText *area = area_->getArea();

  int x = e->x() + x_offset_;
  int y = e->y() + y_offset_;

  int releaseRow, releaseCol;

  area->pixelToPos(x, y, &releaseRow, &releaseCol);

  //----

  CPageTextEscapeNotifier *notifier = area->getEscapeNotifier();

  if (notifier && notifier->getSendMousePress()) {
    int button = e->button() - 1;

    notifier->sendMouseRelease(button, releaseCol + 1, releaseRow + 1);
  }

  //----

  if (mouseState_.selecting) {
    mouseState_.selecting = false;

    area->setSelectionEnd(CTextPos(releaseRow, releaseCol));

    update();
  }

  //----

  mouseState_.reset();
}

void
CQPageTextCanvas::
mouseMoveEvent(QMouseEvent *e)
{
  CQPageText *area = area_->getArea();

  int x = e->x();
  int y = e->y();

  int xo = x + x_offset_;
  int yo = y + y_offset_;

  int moveRow, moveCol;

  area->pixelToPos(xo, yo, &moveRow, &moveCol);

  //---

  if (mouseState_.pressed) {
    if (mouseState_.select && ! mouseState_.selecting) {
      area->setSelectionStart(CTextPos(mouseState_.pressRow, mouseState_.pressCol));

      mouseState_.selecting = true;
    }

    if (mouseState_.selecting) {
      area->setSelectionEnd(CTextPos(moveRow, moveCol));

      update();
    }
  }
  else {
    if (area->getShowLinks()) {
      Region *region = getRegionAtPoint(x, y);

      if (region) {
        QPoint p1 = mapToGlobal(QPoint(region->bbox.getUL().x, region->bbox.getUL().y));
        QPoint p2 = mapToGlobal(QPoint(region->bbox.getLR().x, region->bbox.getLR().y));

        QToolTip::showText((p1 + p2)/2, region->text.c_str(), this, QRect(p1, p2));
      }
    }
  }
}

void
CQPageTextCanvas::
mouseDoubleClickEvent(QMouseEvent *e)
{
  CQPageText *area = area_->getArea();

  int x = e->x();
  int y = e->y();

  int xo = x + x_offset_;
  int yo = y + y_offset_;

  int clickRow, clickCol;

  area->pixelToPos(xo, yo, &clickRow, &clickCol);

  //---

  if      (e->button() == Qt::LeftButton) {
    if (area->getShowLinks()) {
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
    }

    CQPageText *area = area_->getArea();

    if (area->selectWord(clickRow, clickCol))
      update();
  }
}

void
CQPageTextCanvas::
wheelEvent(QWheelEvent *event)
{
  CQPageText *area = area_->getArea();

  if (area->getShowScrollBar()) {
    QScrollBar *vbar = area_->getVScroll();

    double num = -(1.0*event->delta())/vbar->singleStep();

    int value = vbar->value();

    if (num > 0)
      value += std::round(num)*vbar->singleStep();
    else
      value -= std::round(-num)*vbar->singleStep();

    int value1 = std::min(std::max(value, vbar->minimum()), vbar->maximum());

    vbar->setValue(value1);

    event->accept();
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
  CQPageText *area = area_->getArea();

  QMenu *menu = new QMenu;

  bool linkFound = false;

  if (area->getShowLinks()) {
    int x = e->x();
    int y = e->y();

    Region *region = getRegionAtPoint(x, y);

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

        linkFound = true;
      }
      else if (type & CFILE_TYPE_INODE_DIR) {
        QAction *cdAction = new QAction("Change Directiory", this);

        menu->addAction(cdAction);

        linkFound = true;
      }
      else {
        QAction *viewAction = new QAction("View", this);

        menu->addAction(viewAction);

        QAction *editAction = new QAction("Edit", this);

        menu->addAction(editAction);

        linkFound = true;
      }
    }
    else if ((image_cell = dynamic_cast<const CTextImageCell *>(cell)) != nullptr) {
      QAction *viewAction = new QAction("View", this);

      menu->addAction(viewAction);

      QAction *editAction = new QAction("Edit", this);

      menu->addAction(editAction);

      linkFound = true;
    }
  }

  //---

  if (! linkFound) {
    QAction *toolBarAction   = new QAction("Show ToolBar", this);
    QAction *statusBarAction = new QAction("Show Status Bar", this);
    QAction *scrollBarAction = new QAction("Show Scroll Bar", this);
    QAction *linksAction     = new QAction("Enable Links", this);
    QAction *statusAction    = new QAction("Status", this);

    toolBarAction  ->setCheckable(true);
    statusBarAction->setCheckable(true);
    scrollBarAction->setCheckable(true);
    linksAction    ->setCheckable(true);

    toolBarAction  ->setChecked(area_->getToolBar  ()->isVisible());
    statusBarAction->setChecked(area_->getStatusBar()->isVisible());
    scrollBarAction->setChecked(area->getShowScrollBar());
    linksAction    ->setChecked(area->getShowLinks());

    menu->addAction(toolBarAction);
    menu->addAction(statusBarAction);
    menu->addAction(scrollBarAction);
    menu->addAction(linksAction);
    menu->addAction(statusAction);

    connect(toolBarAction  , SIGNAL(triggered()), this, SLOT(toggleToolBar  ()));
    connect(statusBarAction, SIGNAL(triggered()), this, SLOT(toggleStatusBar()));
    connect(scrollBarAction, SIGNAL(triggered()), this, SLOT(toggleScrollBar()));
    connect(linksAction    , SIGNAL(triggered()), this, SLOT(toggleLinks    ()));
    connect(statusAction   , SIGNAL(triggered()), this, SLOT(displayStatus  ()));
  }

  //---

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
toggleLinks()
{
  CQPageText *area = area_->getArea();

  area->setShowLinks(! area->getShowLinks());
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
