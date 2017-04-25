#include <CQTty.h>
#include <CTty.h>

#include <CQTtyPageTextWidget.h>
#include <CQPageTextToolBar.h>
#include <CQPageTextStatus.h>
#include <CQPageText.h>

#ifdef IMAGE_PREVIEW
#include <CQImagePreview.h>
#endif

#ifdef DIR_LIST
#include <CQDirList.h>
#include <CQDirView.h>
#endif

#ifdef FILE_EDIT
#include <CQEdit.h>
#endif

#include <CImageLib.h>

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QTimer>

#include <svg/home_svg.h>

CQTty::
CQTty(QWidget *parent) :
 QWidget(parent), config_("CQTty"), ind_(0)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  //------

  stack_ = new QStackedWidget;

  //------

  text_ = new CQTtyPageTextWidget(this);

  connect(text_, SIGNAL(displayImage(const QString &)),
          this, SLOT(displayImage(const QString &)));
  connect(text_, SIGNAL(displayFile(const QString &)),
          this, SLOT(displayFile(const QString &)));
  connect(text_, SIGNAL(pageSizeSignal(int,int)),
          this, SLOT(setTtyPageSize(int,int)));
  connect(text_, SIGNAL(dirChangeSignal(const QString &)),
          this, SLOT(updateCurrentDir(const QString &)));

  stack_->addWidget(text_);

  CQPageTextToolBar *toolBar = text_->createToolBar();

  toolBar->addButton(CQPixmapCacheInst->getIcon("HOME"), "cd ${HOME}\n");
  toolBar->addStretch();

  CQPageTextStatus *statusBar = text_->createStatusBar();

  statusBar->addMsgLabel();
  statusBar->addStretch();

  connect(text_, SIGNAL(pageSizeSignal(int, int)), this, SLOT(fitAreaSlot()));

  //------

  tty_ = new CTty();

  tty_->init();

  tty_->setReadWait(1);

  //------

#ifdef IMAGE_PREVIEW
  imagePreview_ = new CQImagePreview;

  stack_->addWidget(imagePreview_);

  connect(imagePreview_, SIGNAL(cancelSignal()), this, SLOT(showDefault()));
#endif

  //------

#ifdef DIR_LIST
  dirList_ = new CQDirList;

  connect(dirList_, SIGNAL(cancelSignal()),
          this, SLOT(showDefault()));
  connect(dirList_, SIGNAL(showImage(const QString &)),
          this, SLOT(displayImage(const QString &)));
  connect(dirList_, SIGNAL(showFile(const QString &)),
          this, SLOT(displayFile(const QString &)));

  connect(CQDirViewInst, SIGNAL(dirChangedSignal(const QString &)),
          this, SLOT(changeCurrentDir(const QString &)));

  stack_->addWidget(dirList_);
#endif

  //------

#ifdef FILE_EDIT
  edit_ = new CQEdit;

  connect(edit_, SIGNAL(quitCommand()), this, SLOT(showDefault()));

  stack_->addWidget(edit_);
#endif

  //------

  layout->addWidget(stack_);

  //------

  tty_->setPageSize(text_->getArea()->getPageRows(), text_->getArea()->getPageCols());

  //------

  loadConfig();

  //------

  timer_ = new QTimer;

  connect(timer_, SIGNAL(timeout()), this, SLOT(flush()));

  timer_->start(75);
}

void
CQTty::
loadConfig()
{
  std::string fontFamily;

  if (! config_.getValue("fontFamily", "", fontFamily))
    fontFamily = "courier";

  int fontSize;

  if (! config_.getValue("fontSize", "", &fontSize))
    fontSize = 12;

  text_->getArea()->setFontSet(fontFamily, fontSize);
}

CQPageTextWidget *
CQTty::
getText() const
{
  return text_;
}

void
CQTty::
displayImage(const QString &fileName)
{
  CImagePtr image = CImageMgrInst->lookupImage(fileName.toStdString());

  if (image.isValid()) {
#ifdef IMAGE_PREVIEW
    imagePreview_->setImage(image);

    showImage();
#endif
  }
}

void
CQTty::
displayFile(const QString &fileName)
{
#ifdef FILE_EDIT
  edit_->getFile()->loadLines(fileName.toStdString());

  showEdit();
#else
  showText();

  CQPageText *area = text_->getArea();

  std::string cmd = "vi " + fileName.toStdString() + "\n";

  area->processString(cmd.c_str());
#endif
}

void
CQTty::
setTtyPageSize(int rows, int cols)
{
  tty_->setPageSize(rows, cols);
}

void
CQTty::
updateCurrentDir(const QString &dirName)
{
#ifdef DIR_LIST
  CDirViewInst->setDirName(dirName.toStdString());
#endif

  text_->getStatusBar()->setMsg(dirName);
}

void
CQTty::
changeCurrentDir(const QString &dirName)
{
  CQPageText *area = text_->getArea();

  if (area->getDirName() != dirName.toStdString()) {
    std::string cmd = "cd " + dirName.toStdString() + "\n";

    area->processString(cmd.c_str());
  }
}

void
CQTty::
flush()
{
  text_->flush();
}

void
CQTty::
showText()
{
  ind_ = 0;

  stack_->setCurrentIndex(0);
}

void
CQTty::
showImage()
{
  ind_ = stack_->currentIndex();

  stack_->setCurrentIndex(1);
}

void
CQTty::
showFileBrowser()
{
  ind_ = stack_->currentIndex();

  stack_->setCurrentIndex(2);
}

void
CQTty::
showEdit()
{
  ind_ = stack_->currentIndex();

  stack_->setCurrentIndex(3);
}

void
CQTty::
showDefault()
{
  stack_->setCurrentIndex(ind_);

  ind_ = 0;
}

void
CQTty::
resizeEvent(QResizeEvent *)
{
  if (text_->isVisible()) {
    int rows, cols;

    text_->sizeToRowCols(width(), height(), &rows, &cols);

    text_->getArea()->setPageSize(rows, cols);
  }
}

void
CQTty::
fitAreaSlot()
{
  QSize s = sizeHint();

  resize(s);
}

QSize
CQTty::
sizeHint() const
{
  uint rows, cols;

  text_->getArea()->getPageSize(&rows, &cols);

  int w, h;

  text_->rowColsToSize(rows, cols, &w, &h);

  return QSize(w, h);
}
