#include <CQPageTextTest.h>
#include <CQPageText.h>
#include <CQPageTextWidget.h>
#include <CQPageTextToolBar.h>
#include <CQPageTextStatus.h>
#include <CQApp.h>

#include <QIcon>
#include <QVBoxLayout>

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  auto *test = new CQPageTextTest;

  test->show();

  auto *area = test->getPageText();

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if      (strcmp(&argv[i][1], "cmd") == 0) {
        ++i;

        if (i < argc)
          area->loadCmd(argv[i]);
      }
      else if (strcmp(&argv[i][1], "esc") == 0) {
        ++i;

        if (i < argc)
          area->loadEsc(argv[i]);
      }
    }
    else
      std::cerr << "Invalid arg \'" << argv[i] << "\'" << std::endl;
  }

  return app.exec();
}

//---

CQPageTextTest::
CQPageTextTest() :
 config_("CQPageText")
{
  setObjectName("test");

  auto *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  area_ = new CQPageTextWidget;

  auto *toolBar = area_->createToolBar();

  toolBar->addButton(QIcon(), "");
  toolBar->addStretch();

  auto *statusBar = area_->createStatusBar();

  statusBar->addStretch();
  statusBar->addCmdEdit();
  statusBar->addPosLabel();
  statusBar->addSizeLabel();

  connect(area_, SIGNAL(pageSizeSignal(int, int)), this, SLOT(fitAreaSlot()));

  layout->addWidget(area_);

  area_->getArea()->setMoveOnClick(true);

  //---

  area_->getArea()->setConfig(&config_);

  area_->getArea()->loadConfig();
}

void
CQPageTextTest::
resizeEvent(QResizeEvent *)
{
  if (area_->isVisible()) {
    int rows, cols;

    area_->sizeToRowCols(width(), height(), &rows, &cols);

    area_->getArea()->setPageSize(rows, cols);
  }
}

void
CQPageTextTest::
fitAreaSlot()
{
  QSize s = sizeHint();

  resize(s);
}

QSize
CQPageTextTest::
sizeHint() const
{
  uint rows, cols;

  area_->getArea()->getPageSize(&rows, &cols);

  int w, h;

  area_->rowColsToSize(rows, cols, &w, &h);

  return QSize(w, h);
}
