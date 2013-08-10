#include <CQTty.h>

#include <CTty.h>
#include <CQPageText.h>

#ifdef IMAGE_PREVIEW
#include <CQImagePreview.h>
#endif

#ifdef DIR_VIEW
#include <CQDirView.h>
#include <CQDirList.h>
#endif

#ifdef EDIT_VIEW
#include <CQEdit.h>
#endif

#include <CImageLib.h>

#include <QToolBar>
#include <QStatusBar>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QTimer>

class CQTtyPageTextWidget : public CQPageTextWidget {
 public:
  CQTtyPageTextWidget(CQTty *qtty) :
   CQPageTextWidget(NULL), tty_(qtty) {
  }

  void showToolBar(bool show) {
    tty_->showToolBar(show);
  }

  void showStatusBar(bool show) {
    tty_->showStatusBar(show);
  }

  void showFileBrowser() {
    tty_->showFileBrowser();
  }

  void showImage() {
    tty_->showImage();
  }

  CTty *getTty() const { return tty_->getTty(); }

 private:
  CQTty *tty_;
};

CQTty::
CQTty(QWidget *parent) :
 QWidget(parent), config_("CQTty"), ind_(0)
{
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  //------

  toolbar_ = new QToolBar;

  toolbar_->setFixedHeight(20);

  //------

  stack_ = new QStackedWidget;

  text_ = new CQTtyPageTextWidget(this);

  connect(text_, SIGNAL(displayImage(const QString &)),
          this, SLOT(displayImage(const QString &)));
  connect(text_, SIGNAL(displayFile(const QString &)),
          this, SLOT(displayFile(const QString &)));
  connect(text_, SIGNAL(pageSizeSignal(int,int)),
          this, SLOT(setTtyPageSize(int,int)));
  connect(text_, SIGNAL(dirChangeSignal(const QString &)),
          this, SLOT(updateCurrentDir(const QString &)));

  tty_ = new CTty();

  tty_->init();

  tty_->setReadWait(1);

  stack_->addWidget(text_);

  //------

#ifdef IMAGE_PREVIEW
  imagePreview_ = new CQImagePreview;

  stack_->addWidget(imagePreview_);

  connect(imagePreview_, SIGNAL(cancelSignal()), this, SLOT(showDefault()));
#endif

  //------

#ifdef DIR_VIEW
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

#ifdef EDIT_VIEW
  edit_ = new CQEdit;

  connect(edit_, SIGNAL(quitCommand()),
          this, SLOT(showDefault()));

  stack_->addWidget(edit_);
#endif

  //------

  status_ = new QStatusBar;

  status_->setFixedHeight(20);

  //------

  layout->addWidget(toolbar_);
  layout->addWidget(stack_);
  layout->addWidget(status_);

  //------

  showToolBar  (text_->getArea()->getShowToolBar  ());
  showStatusBar(text_->getArea()->getShowStatusBar());

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
    fontSize = 10;

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
#ifdef IMAGE_PREVIEW
  CImagePtr image = CImageMgrInst->lookupImage(fileName.toStdString());

  if (image.isValid()) {
    imagePreview_->setImage(image);

    showImage();
  }
#else
  std::cerr << "Image: " << fileName.toStdString() << std::endl;
#endif
}

void
CQTty::
displayFile(const QString &fileName)
{
#ifndef EDIT_VIEW
  showText();

  CQPageText *area = text_->getArea();

  std::string cmd = "vi " + fileName.toStdString() + "\n";

  area->processString(cmd.c_str());
#else
  edit_->getFile()->loadLines(fileName.toStdString());

  showEdit();
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
#ifdef DIR_VIEW
  CDirViewInst->setDirName(dirName.toStdString());
#endif

  status_->showMessage(dirName);
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
showToolBar(bool show)
{
  toolbar_->setVisible(show);
}

void
CQTty::
showStatusBar(bool show)
{
  status_->setVisible(show);
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
