#include <CQPageTextState.h>
#include <CQPageText.h>
#include <CQPageTextCanvas.h>
#include <CQPageTextWidget.h>
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
 QDialog(text->widget()), text_(text)
{
  setObjectName("state");

  QVBoxLayout *layout = new QVBoxLayout(this);

  QTabWidget *tab = new QTabWidget();

  layout->addWidget(tab);

  //-----

  QWidget *tabWidget1 = new QWidget();

  tab->addTab(tabWidget1, "State");

  QVBoxLayout *layout1 = new QVBoxLayout(tabWidget1);

  QGroupBox *stateGroup = new QGroupBox("State");

  QGridLayout *grid = new QGridLayout(stateGroup);

  inverseVideo_     = new QCheckBox("Inverse Video");
  sendMousePress_   = new QCheckBox("Send Mouse Press");
  sendMouseRelease_ = new QCheckBox("Send Mouse Release");
  sendMouseMotion_  = new QCheckBox("Send Mouse Motion");
  sendFocusInOut_   = new QCheckBox("Send Focus In/Out");
  scrollBottomKey_  = new QCheckBox("Scroll Bottom Key");
  scrollBottomTty_  = new QCheckBox("Scroll Bottom TTY");
  bracketedPaste_   = new QCheckBox("Bracketed Paste");
  appCursorKeys_    = new QCheckBox("Application Cursor Keys");
  insertMode_       = new QCheckBox("Insert Mode");
  lineWrap_         = new QCheckBox("Line Wrap");
  ansiVt52Mode_     = new QCheckBox("ANSI VT52 Mode");
  tek4014Mode_      = new QCheckBox("Tek 4014 Mode");
  keyPadMode_       = new QCheckBox("Key Pad Mode");
  lfNlMode_         = new QCheckBox("LF/NL Mode");
  ffNpMode_         = new QCheckBox("FF/NP Mode");
  smoothScroll_     = new QCheckBox("Smooth Scroll");
  originMode_       = new QCheckBox("Origin Mode");
  autoRepeat_       = new QCheckBox("Auto Repeat");
  cursorVisible_    = new QCheckBox("Cursor Visible");
  cursorBlink_      = new QCheckBox("Cursor Blink");
  reverseWrap_      = new QCheckBox("Reverse Wrap");
  allow80132_       = new QCheckBox("Allow 80/132");
  control8Bit_      = new QCheckBox("Control 8 bit");
  altPage_          = new QCheckBox("Alternate Page");

  connect(inverseVideo_    , SIGNAL(clicked()), this, SLOT(updateInverseVideo    ()));
  connect(sendMousePress_  , SIGNAL(clicked()), this, SLOT(updateSendMousePress  ()));
  connect(sendMouseRelease_, SIGNAL(clicked()), this, SLOT(updateSendMouseRelease()));
  connect(sendMouseMotion_ , SIGNAL(clicked()), this, SLOT(updateSendMouseMotion ()));
  connect(sendFocusInOut_  , SIGNAL(clicked()), this, SLOT(updateSendFocusInOut  ()));
  connect(scrollBottomKey_ , SIGNAL(clicked()), this, SLOT(updateScrollBottomKey ()));
  connect(scrollBottomTty_ , SIGNAL(clicked()), this, SLOT(updateScrollBottomTty ()));
  connect(bracketedPaste_  , SIGNAL(clicked()), this, SLOT(updateBracketedPaste  ()));
  connect(appCursorKeys_   , SIGNAL(clicked()), this, SLOT(updateAppCursorKeys   ()));
  connect(insertMode_      , SIGNAL(clicked()), this, SLOT(updateInsertMode      ()));
  connect(lineWrap_        , SIGNAL(clicked()), this, SLOT(updateLineWrap        ()));
  connect(ansiVt52Mode_    , SIGNAL(clicked()), this, SLOT(updateAnsiVt52Mode    ()));
  connect(tek4014Mode_     , SIGNAL(clicked()), this, SLOT(updateTek4014Mode     ()));
  connect(keyPadMode_      , SIGNAL(clicked()), this, SLOT(updateKeyPadMode      ()));
  connect(lfNlMode_        , SIGNAL(clicked()), this, SLOT(updateLfNlMode        ()));
  connect(ffNpMode_        , SIGNAL(clicked()), this, SLOT(updateFfNpMode        ()));
  connect(smoothScroll_    , SIGNAL(clicked()), this, SLOT(updateSmoothScroll    ()));
  connect(originMode_      , SIGNAL(clicked()), this, SLOT(updateOriginMode      ()));
  connect(autoRepeat_      , SIGNAL(clicked()), this, SLOT(updateAutoRepeat      ()));
  connect(cursorVisible_   , SIGNAL(clicked()), this, SLOT(updateCursorVisible   ()));
  connect(cursorBlink_     , SIGNAL(clicked()), this, SLOT(updateCursorBlink     ()));
  connect(reverseWrap_     , SIGNAL(clicked()), this, SLOT(updateReverseWrap     ()));
  connect(allow80132_      , SIGNAL(clicked()), this, SLOT(updateAllow80123      ()));
  connect(control8Bit_     , SIGNAL(clicked()), this, SLOT(updateControl8Bit     ()));
  connect(altPage_         , SIGNAL(clicked()), this, SLOT(updateAlt             ()));

  grid->addWidget(inverseVideo_    ,  0, 0);
  grid->addWidget(sendMousePress_  ,  1, 0);
  grid->addWidget(sendMouseRelease_,  1, 1);
  grid->addWidget(sendMouseMotion_ ,  2, 0);
  grid->addWidget(sendFocusInOut_  ,  2, 1);
  grid->addWidget(scrollBottomKey_ ,  3, 0);
  grid->addWidget(scrollBottomTty_ ,  3, 1);
  grid->addWidget(bracketedPaste_  ,  4, 0);
  grid->addWidget(appCursorKeys_   ,  4, 1);
  grid->addWidget(insertMode_      ,  5, 0);
  grid->addWidget(lineWrap_        ,  6, 0);
  grid->addWidget(ansiVt52Mode_    ,  7, 0);
  grid->addWidget(tek4014Mode_     ,  7, 1);
  grid->addWidget(keyPadMode_      ,  8, 0);
  grid->addWidget(lfNlMode_        ,  9, 0);
  grid->addWidget(ffNpMode_        ,  9, 1);
  grid->addWidget(smoothScroll_    , 10, 0);
  grid->addWidget(originMode_      , 10, 1);
  grid->addWidget(autoRepeat_      , 11, 0);
  grid->addWidget(cursorVisible_   , 12, 0);
  grid->addWidget(cursorBlink_     , 12, 1);
  grid->addWidget(reverseWrap_     , 13, 0);
  grid->addWidget(allow80132_      , 14, 0);
  grid->addWidget(control8Bit_     , 14, 1);
  grid->addWidget(altPage_         , 15, 0);

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

  bgColor_     = addColor("Bg"         , 0, 0, grid2);
  fgColor_     = addColor("Fg"         , 0, 1, grid2);
  cursorColor_ = addColor("Cursor"     , 1, 0, grid2);
  color1_      = addColor("Black"      , 2, 0, grid2);
  color2_      = addColor("Red"        , 3, 0, grid2);
  color3_      = addColor("Green"      , 4, 0, grid2);
  color4_      = addColor("Yellow"     , 5, 0, grid2);
  color5_      = addColor("Blue"       , 6, 0, grid2);
  color6_      = addColor("Magenta"    , 7, 0, grid2);
  color7_      = addColor("Cyan"       , 8, 0, grid2);
  color8_      = addColor("White"      , 9, 0, grid2);
  altColor1_   = addColor("Alt Black"  , 2, 1, grid2);
  altColor2_   = addColor("Alt Red"    , 3, 1, grid2);
  altColor3_   = addColor("Alt Green"  , 4, 1, grid2);
  altColor4_   = addColor("Alt Yellow" , 5, 1, grid2);
  altColor5_   = addColor("Alt Blue"   , 6, 1, grid2);
  altColor6_   = addColor("Alt Magenta", 7, 1, grid2);
  altColor7_   = addColor("Alt Cyan"   , 8, 1, grid2);
  altColor8_   = addColor("Alt White"  , 9, 1, grid2);

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

  QPushButton *saveButton   = new QPushButton("Save");
  QPushButton *updateButton = new QPushButton("Update");
  QPushButton *doneButton   = new QPushButton("Done");

  connect(saveButton  , SIGNAL(clicked()), this, SLOT(saveConfig()));
  connect(updateButton, SIGNAL(clicked()), this, SLOT(updateState()));
  connect(doneButton  , SIGNAL(clicked()), this, SLOT(hide()));

  buttonLayout->addWidget(saveButton);
  buttonLayout->addWidget(updateButton);
  buttonLayout->addWidget(doneButton);
  buttonLayout->addStretch();

  layout->addLayout(buttonLayout);
}

CQColorChooser *
CQPageTextState::
addColor(const std::string &name, int pos, int ind, QGridLayout *layout)
{
  QLabel *label = new QLabel(name.c_str());

  CQColorChooser *chooser = new CQColorChooser;

  uint styles = CQColorChooser::Text | CQColorChooser::ColorButton;

  chooser->setStyles(styles);

  connect(chooser, SIGNAL(colorChanged(const QColor &)), this, SLOT(colorChanged(const QColor &)));

  if (ind == 0) {
    layout->addWidget(label  , pos, 0);
    layout->addWidget(chooser, pos, 1);
  }
  else {
    layout->addWidget(label  , pos, 2);
    layout->addWidget(chooser, pos, 3);
  }

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
saveConfig()
{
  text_->saveConfig();
}

void
CQPageTextState::
updateState()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  inverseVideo_    ->setChecked(notifier->getInverseVideo());
  sendMousePress_  ->setChecked(notifier->getSendMousePress());
  sendMouseRelease_->setChecked(notifier->getSendMouseRelease());
  sendMouseMotion_ ->setChecked(notifier->getSendMouseMotion());
  sendFocusInOut_  ->setChecked(notifier->getSendFocusInOut());
  scrollBottomKey_ ->setChecked(notifier->getScrollBottomOnKey());
  scrollBottomTty_ ->setChecked(notifier->getScrollBottomOnTty());
  bracketedPaste_  ->setChecked(notifier->getBracketedPasteMode());
  appCursorKeys_   ->setChecked(notifier->getApplicationCursorKeys());
  insertMode_      ->setChecked(notifier->getInsertMode());
  lineWrap_        ->setChecked(notifier->getLineWrap());
  ansiVt52Mode_    ->setChecked(notifier->getAnsiVT52Mode());
  tek4014Mode_     ->setChecked(notifier->getTek4014());
  keyPadMode_      ->setChecked(notifier->getKeyPadMode());
  lfNlMode_        ->setChecked(notifier->getLfNlMode());
  ffNpMode_        ->setChecked(notifier->getFfNpMode());
  smoothScroll_    ->setChecked(notifier->getSmoothScroll());
  originMode_      ->setChecked(notifier->getOriginMode());
  autoRepeat_      ->setChecked(notifier->getAutoRepeat());
  cursorVisible_   ->setChecked(notifier->getCursorVisible());
  cursorBlink_     ->setChecked(notifier->getCursorBlink());
  reverseWrap_     ->setChecked(notifier->getReverseWrap());
  allow80132_      ->setChecked(notifier->getAllow80To132());
  control8Bit_     ->setChecked(notifier->getControl8Bit());

  altPage_->setChecked(text_->isAlt());

  scrollSet_   ->setChecked(notifier->hasScrollArea());
  scrollTop_   ->setText(QString("%1").arg(notifier->getScrollTop()));
  scrollBottom_->setText(QString("%1").arg(notifier->getScrollBottom()));

  //------

  bgColor_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::BG)));
  fgColor_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::FG)));

  cursorColor_->setColor(CQUtil::rgbaToColor(notifier->getCursorColor()));

  color1_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::BLACK  )));
  color2_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::RED    )));
  color3_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::GREEN  )));
  color4_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::YELLOW )));
  color5_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::BLUE   )));
  color6_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::MAGENTA)));
  color7_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::CYAN   )));
  color8_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::WHITE  )));

  altColor1_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::ALT_BLACK  )));
  altColor2_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::ALT_RED    )));
  altColor3_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::ALT_GREEN  )));
  altColor4_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::ALT_YELLOW )));
  altColor5_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::ALT_BLUE   )));
  altColor6_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::ALT_MAGENTA)));
  altColor7_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::ALT_CYAN   )));
  altColor8_->setColor(CQUtil::rgbaToColor(notifier->getColor(CEscapeColor::ALT_WHITE  )));

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
}

void
CQPageTextState::
updateSendMousePress()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setSendMousePress(sendMousePress_->isChecked());
}

void
CQPageTextState::
updateSendMouseRelease()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setSendMouseRelease(sendMouseRelease_->isChecked());
}

void
CQPageTextState::
updateSendMouseMotion()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setSendMouseMotion(sendMouseMotion_->isChecked());
}

void
CQPageTextState::
updateSendFocusInOut()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setSendFocusInOut(sendFocusInOut_->isChecked());
}

void
CQPageTextState::
updateScrollBottomKey()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setScrollBottomOnKey(scrollBottomKey_->isChecked());
}

void
CQPageTextState::
updateScrollBottomTty()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setScrollBottomOnTty(scrollBottomTty_->isChecked());
}

void
CQPageTextState::
updateBracketedPaste()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setBracketedPasteMode(bracketedPaste_->isChecked());
}

void
CQPageTextState::
updateAppCursorKeys()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setApplicationCursorKeys(appCursorKeys_->isChecked());
}

void
CQPageTextState::
updateInsertMode()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setInsertMode(insertMode_->isChecked());
}

void
CQPageTextState::
updateLineWrap()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setLineWrap(lineWrap_->isChecked());
}

void
CQPageTextState::
updateAnsiVt52Mode()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setAnsiVT52Mode(ansiVt52Mode_->isChecked());
}

void
CQPageTextState::
updateTek4014Mode()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setTek4014(tek4014Mode_->isChecked());
}

void
CQPageTextState::
updateKeyPadMode()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setKeyPadMode(keyPadMode_->isChecked());
}

void
CQPageTextState::
updateLfNlMode()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setLfNlMode(lfNlMode_->isChecked());
}

void
CQPageTextState::
updateFfNpMode()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setFfNpMode(ffNpMode_->isChecked());
}

void
CQPageTextState::
updateSmoothScroll()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setSmoothScroll(smoothScroll_->isChecked());
}

void
CQPageTextState::
updateOriginMode()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setOriginMode(originMode_->isChecked());
}

void
CQPageTextState::
updateAutoRepeat()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setAutoRepeat(autoRepeat_->isChecked());
}

void
CQPageTextState::
updateCursorVisible()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setCursorVisible(cursorVisible_->isChecked());
}

void
CQPageTextState::
updateCursorBlink()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setCursorBlink(cursorBlink_->isChecked());
}

void
CQPageTextState::
updateReverseWrap()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setReverseWrap(reverseWrap_->isChecked());
}

void
CQPageTextState::
updateAllow80123()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setAllow80To132(allow80132_->isChecked());
}

void
CQPageTextState::
updateControl8Bit()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setControl8Bit(control8Bit_->isChecked());
}

void
CQPageTextState::
updateAlt()
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setAlternative(altPage_->isChecked());
}

void
CQPageTextState::
colorChanged(const QColor &)
{
  CPageTextEscapeNotifier *notifier = text_->getEscapeNotifier();

  notifier->setColor(CEscapeColor::BG, CQUtil::colorToRGBA(bgColor_->color()));
  notifier->setColor(CEscapeColor::FG, CQUtil::colorToRGBA(fgColor_->color()));

  notifier->setCursorColor(CQUtil::colorToRGBA(cursorColor_->color()));

  notifier->setColor(CEscapeColor::BLACK  , CQUtil::colorToRGBA(color1_->color()));
  notifier->setColor(CEscapeColor::RED    , CQUtil::colorToRGBA(color2_->color()));
  notifier->setColor(CEscapeColor::GREEN  , CQUtil::colorToRGBA(color3_->color()));
  notifier->setColor(CEscapeColor::YELLOW , CQUtil::colorToRGBA(color4_->color()));
  notifier->setColor(CEscapeColor::BLUE   , CQUtil::colorToRGBA(color5_->color()));
  notifier->setColor(CEscapeColor::MAGENTA, CQUtil::colorToRGBA(color6_->color()));
  notifier->setColor(CEscapeColor::CYAN   , CQUtil::colorToRGBA(color7_->color()));
  notifier->setColor(CEscapeColor::WHITE  , CQUtil::colorToRGBA(color8_->color()));

  notifier->setColor(CEscapeColor::ALT_BLACK  , CQUtil::colorToRGBA(altColor1_->color()));
  notifier->setColor(CEscapeColor::ALT_RED    , CQUtil::colorToRGBA(altColor2_->color()));
  notifier->setColor(CEscapeColor::ALT_GREEN  , CQUtil::colorToRGBA(altColor3_->color()));
  notifier->setColor(CEscapeColor::ALT_YELLOW , CQUtil::colorToRGBA(altColor4_->color()));
  notifier->setColor(CEscapeColor::ALT_BLUE   , CQUtil::colorToRGBA(altColor5_->color()));
  notifier->setColor(CEscapeColor::ALT_MAGENTA, CQUtil::colorToRGBA(altColor6_->color()));
  notifier->setColor(CEscapeColor::ALT_CYAN   , CQUtil::colorToRGBA(altColor7_->color()));
  notifier->setColor(CEscapeColor::ALT_WHITE  , CQUtil::colorToRGBA(altColor8_->color()));

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
