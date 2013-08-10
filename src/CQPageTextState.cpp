#include <CQPageTextState.h>
#include <CQPageText.h>
#include <CPageTextEscapeNotifier.h>
#include <CQColorChooser.h>
#include <CQIntegerEdit.h>
#include <CQUtil.h>

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

// TODO:
//  Tabs
//  Window (title, size, ...)

CQPageTextState::
CQPageTextState(CQPageText *text) :
 text_(text)
{
  QVBoxLayout *layout = new QVBoxLayout(this);

  QTabWidget *tab = new QTabWidget();

  layout->addWidget(tab);

  //-----

  QWidget *tabWidget1 = new QWidget();

  tab->addTab(tabWidget1, "State");

  QVBoxLayout *layout1 = new QVBoxLayout(tabWidget1);

  QGroupBox *stateGroup = new QGroupBox("State");

  QGridLayout *grid = new QGridLayout(stateGroup);

  inverseVideo_          = new QCheckBox("Inverse Video");
  sendMousePress_        = new QCheckBox("Send Mouse Press");
  sendMouseRelease_      = new QCheckBox("Send Mouse Release");
  scrollBottomKey_       = new QCheckBox("Scroll Bottom Key");
  scrollBottomTty_       = new QCheckBox("Scroll Bottom TTY");
  applicationCursorKeys_ = new QCheckBox("Application Cursor Keys");
  insertMode_            = new QCheckBox("Insert Mode");
  lineWrap_              = new QCheckBox("Line Wrap");
  ansiVt52Mode_          = new QCheckBox("ANSI VT52 Mode");
  keyPadMode_            = new QCheckBox("Key Pad Mode");
  lfNlMode_              = new QCheckBox("LF/NL Mode");
  ffNpMode_              = new QCheckBox("FF/NP Mode");
  smoothScroll_          = new QCheckBox("Smooth Scroll");
  originMode_            = new QCheckBox("Origin Mode");
  autoRepeat_            = new QCheckBox("Auto Repeat");
  cursorVisible_         = new QCheckBox("Cursor Visible");
  cursorBlink_           = new QCheckBox("Cursor Blink");
  reverseWrap_           = new QCheckBox("Reverse Wrap");
  allow80132_            = new QCheckBox("Allow 80/132");
  altPage_               = new QCheckBox("Alternate Page");

  connect(inverseVideo_ , SIGNAL(clicked()), this, SLOT(updateInverseVideo ()));
  connect(cursorVisible_, SIGNAL(clicked()), this, SLOT(updateCursorVisible()));
  connect(cursorBlink_  , SIGNAL(clicked()), this, SLOT(updateCursorBlink  ()));
  connect(altPage_      , SIGNAL(clicked()), this, SLOT(updateAlt          ()));

  grid->addWidget(inverseVideo_         ,  0, 0);
  grid->addWidget(sendMousePress_       ,  1, 0);
  grid->addWidget(sendMouseRelease_     ,  1, 1);
  grid->addWidget(scrollBottomKey_      ,  2, 0);
  grid->addWidget(scrollBottomTty_      ,  2, 1);
  grid->addWidget(applicationCursorKeys_,  3, 0);
  grid->addWidget(insertMode_           ,  4, 0);
  grid->addWidget(lineWrap_             ,  5, 0);
  grid->addWidget(ansiVt52Mode_         ,  6, 0);
  grid->addWidget(keyPadMode_           ,  7, 0);
  grid->addWidget(lfNlMode_             ,  8, 0);
  grid->addWidget(ffNpMode_             ,  8, 1);
  grid->addWidget(smoothScroll_         ,  9, 0);
  grid->addWidget(originMode_           ,  9, 1);
  grid->addWidget(autoRepeat_           , 10, 0);
  grid->addWidget(cursorVisible_        , 11, 0);
  grid->addWidget(cursorBlink_          , 11, 1);
  grid->addWidget(reverseWrap_          , 12, 0);
  grid->addWidget(allow80132_           , 13, 0);
  grid->addWidget(altPage_              , 14, 0);

  layout1->addWidget(stateGroup);

  QHBoxLayout *scrollAreaLayout = new QHBoxLayout;

  scrollSet_ = new QCheckBox("Scroll Area");

  scrollBottom_ = new CQIntegerEdit;
  scrollTop_    = new CQIntegerEdit;

  scrollAreaLayout->addWidget(scrollSet_);
  scrollAreaLayout->addWidget(scrollTop_);
  scrollAreaLayout->addWidget(scrollBottom_);

  layout1->addLayout(scrollAreaLayout);

  //-----

  QWidget *tabWidget2 = new QWidget();

  tab->addTab(tabWidget2, "Colors/Fonts");

  QVBoxLayout *layout2 = new QVBoxLayout(tabWidget2);

  QGridLayout *grid2 = new QGridLayout;

  bgColor_     = addColor("Bg"     ,  0, grid2);
  fgColor_     = addColor("Fg"     ,  1, grid2);
  cursorColor_ = addColor("Cursor" ,  2, grid2);
  color1_      = addColor("Black"  ,  3, grid2);
  color2_      = addColor("Red"    ,  4, grid2);
  color3_      = addColor("Green"  ,  5, grid2);
  color4_      = addColor("Yellow" ,  6, grid2);
  color5_      = addColor("Blue"   ,  7, grid2);
  color6_      = addColor("Magenta",  8, grid2);
  color7_      = addColor("Cyan"   ,  9, grid2);
  color8_      = addColor("White"  , 10, grid2);

  layout2->addLayout(grid2);

  QHBoxLayout *fontLayout = new QHBoxLayout;

  QLabel *fontLabel = new QLabel("Font");

  fontName_ = new QLineEdit;
  fontSize_ = new CQIntegerEdit;

  connect(fontName_, SIGNAL(returnPressed()), this, SLOT(updateFont()));
  connect(fontSize_, SIGNAL(returnPressed()), this, SLOT(updateFont()));

  fontLayout->addWidget(fontLabel);
  fontLayout->addWidget(fontName_);
  fontLayout->addWidget(fontSize_);

  layout2->addLayout(fontLayout);

  layout2->addStretch();

  //-----

  QWidget *tabWidget3 = new QWidget();

  tab->addTab(tabWidget3, "Window");

  QVBoxLayout *layout3 = new QVBoxLayout(tabWidget3);

  iconized_  = new QCheckBox("Iconized");
  maximized_ = new QCheckBox("Maximized");

  layout3->addWidget(iconized_);
  layout3->addWidget(maximized_);

  QGridLayout *grid3 = new QGridLayout;

  windowTitle_ = addEdit("Window Title", 0, grid3);
  iconName_    = addEdit("Icon Name"   , 1, grid3);
  windowPos_   = addEdit("Position"    , 2, grid3);
  pixelSize_   = addEdit("Pixel Size"  , 3, grid3);
  charSize_    = addEdit("Char Size"   , 4, grid3);

  layout3->addLayout(grid3);

  layout3->addStretch();

  //------

  QHBoxLayout *buttonLayout = new QHBoxLayout;

  QPushButton *updateButton = new QPushButton("Update");
  QPushButton *doneButton   = new QPushButton("Done");

  connect(updateButton, SIGNAL(clicked()), this, SLOT(updateState()));
  connect(doneButton  , SIGNAL(clicked()), this, SLOT(hide()));

  buttonLayout->addWidget(updateButton);
  buttonLayout->addWidget(doneButton);
  buttonLayout->addStretch();

  layout->addLayout(buttonLayout);
}

CQColorChooser *
CQPageTextState::
addColor(const std::string &name, int pos, QGridLayout *layout)
{
  QLabel *label = new QLabel(name.c_str());

  CQColorChooser *chooser = new CQColorChooser;

  uint styles = CQColorChooser::Text | CQColorChooser::ColorButton;

  chooser->setStyles(styles);

  connect(chooser, SIGNAL(colorChanged(const QColor &)), this, SLOT(colorChanged(const QColor &)));

  layout->addWidget(label  , pos, 0);
  layout->addWidget(chooser, pos, 1);

  return chooser;
}

QLineEdit *
CQPageTextState::
addEdit(const std::string &name, int pos, QGridLayout *layout)
{
  QLabel *label = new QLabel(name.c_str());

  QLineEdit *edit = new QLineEdit;

  //connect(edit, SIGNAL(returnPressed()), this, SLOT(windowChanged()));

  layout->addWidget(label, pos, 0);
  layout->addWidget(edit , pos, 1);

  return edit;
}

void
CQPageTextState::
updateState()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  inverseVideo_         ->setChecked(notifier->getInverseVideo());
  sendMousePress_       ->setChecked(notifier->getSendMousePress());
  sendMouseRelease_     ->setChecked(notifier->getSendMouseRelease());
  scrollBottomKey_      ->setChecked(notifier->getScrollBottomOnKey());
  scrollBottomTty_      ->setChecked(notifier->getScrollBottomOnTty());
  applicationCursorKeys_->setChecked(notifier->getApplicationCursorKeys());
  insertMode_           ->setChecked(notifier->getInsertMode());
  lineWrap_             ->setChecked(notifier->getLineWrap());
  ansiVt52Mode_         ->setChecked(notifier->getAnsiVT52Mode());
  keyPadMode_           ->setChecked(notifier->getKeyPadMode());
  lfNlMode_             ->setChecked(notifier->getLfNlMode());
  ffNpMode_             ->setChecked(notifier->getFfNpMode());
  smoothScroll_         ->setChecked(notifier->getSmoothScroll());
  originMode_           ->setChecked(notifier->getOriginMode());
  autoRepeat_           ->setChecked(notifier->getAutoRepeat());
  cursorVisible_        ->setChecked(notifier->getCursorVisible());
  cursorBlink_          ->setChecked(notifier->getCursorBlink());
  reverseWrap_          ->setChecked(notifier->getReverseWrap());
  allow80132_           ->setChecked(notifier->getAllow80To132());

  altPage_->setChecked(text_->isAlt());

  scrollSet_   ->setChecked(notifier->hasScrollArea());
  scrollTop_   ->setText(QString("%1").arg(notifier->getScrollTop()));
  scrollBottom_->setText(QString("%1").arg(notifier->getScrollBottom()));

  //------

  bgColor_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_BG)));
  fgColor_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_FG)));

  cursorColor_->setColor(CQUtil::rgbaToColor(notifier->getCursorColor()));

  color1_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_BLACK  )));
  color2_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_RED    )));
  color3_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_GREEN  )));
  color4_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_YELLOW )));
  color5_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_BLUE   )));
  color6_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_MAGENTA)));
  color7_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_CYAN   )));
  color8_->setColor(CQUtil::rgbaToColor(notifier->getColor(CESCAPE_COLOR_WHITE  )));

  std::string fontFamily;
  uint        fontSize;

  text_->getFontSet(fontFamily, fontSize);

  fontName_->setText(fontFamily.c_str());
  fontSize_->setValue(fontSize);

  //------

  iconized_ ->setChecked(false);
  maximized_->setChecked(false);

  std::string name;

  notifier->getWindow()->getWindowTitle(name);

  windowTitle_->setText(name.c_str());

  notifier->getWindow()->getIconTitle(name);

  iconName_->setText(name.c_str());

  int x, y;

  notifier->getWindow()->getPosition(&x, &y);

  windowPos_->setText(QString("%1 %2").arg(x).arg(y));

  uint width, height;

  notifier->getWindow()->getSize(&width, &height);

  pixelSize_->setText(QString("%1 %2").arg(width).arg(height));

  charSize_->setText(QString("%1 %2").arg(notifier->getNumDispRows()).
                                      arg(notifier->getNumDispCols()));
}

void
CQPageTextState::
updateInverseVideo()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setInverseVideo(inverseVideo_->isChecked());

  text_->update();
}

void
CQPageTextState::
updateCursorVisible()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setCursorVisible(cursorVisible_->isChecked());

  text_->update();
}

void
CQPageTextState::
updateCursorBlink()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setCursorBlink(cursorBlink_->isChecked());

  text_->update();
}

void
CQPageTextState::
updateAlt()
{
  text_->setIsAlt(altPage_->isChecked());

  text_->update();
}

void
CQPageTextState::
colorChanged(const QColor &)
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setColor(CESCAPE_COLOR_BG, CQUtil::colorToRGBA(bgColor_->color()));
  notifier->setColor(CESCAPE_COLOR_FG, CQUtil::colorToRGBA(fgColor_->color()));

  notifier->setCursorColor(CQUtil::colorToRGBA(cursorColor_->color()));

  notifier->setColor(CESCAPE_COLOR_BLACK  , CQUtil::colorToRGBA(color1_->color()));
  notifier->setColor(CESCAPE_COLOR_RED    , CQUtil::colorToRGBA(color2_->color()));
  notifier->setColor(CESCAPE_COLOR_GREEN  , CQUtil::colorToRGBA(color3_->color()));
  notifier->setColor(CESCAPE_COLOR_YELLOW , CQUtil::colorToRGBA(color4_->color()));
  notifier->setColor(CESCAPE_COLOR_BLUE   , CQUtil::colorToRGBA(color5_->color()));
  notifier->setColor(CESCAPE_COLOR_MAGENTA, CQUtil::colorToRGBA(color6_->color()));
  notifier->setColor(CESCAPE_COLOR_CYAN   , CQUtil::colorToRGBA(color7_->color()));
  notifier->setColor(CESCAPE_COLOR_WHITE  , CQUtil::colorToRGBA(color8_->color()));

  text_->update();
}

void
CQPageTextState::
updateFont()
{
  std::string fontFamily = fontName_->text().toStdString();
  uint        fontSize   = fontSize_->getValue();

  text_->setFontSet(fontFamily, fontSize);

  text_->update();
}
