#include <CQPageTextWidget.h>
#include <CQPageText.h>
#include <CQPageTextCanvas.h>
#include <CQPageTextToolBar.h>
#include <CQPageTextStatus.h>
#include <QScrollBar>
#include <QTimer>

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
notifyStateChange()
{
  text_->updateState();
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
