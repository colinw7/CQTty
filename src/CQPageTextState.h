#ifndef CQPAGE_TEXT_STATE_H
#define CQPAGE_TEXT_STATE_H

#include <QDialog>

class CQPageText;
class QCheckBox;
class QLineEdit;
class CQColorChooser;
class QGridLayout;
class CQIntegerEdit;

class CQPageTextState : public QDialog {
  Q_OBJECT

 public:
  CQPageTextState(CQPageText *text);

 private:
  CQColorChooser *addColor(const std::string &name, int pos, int ind, QGridLayout *layout);

  QLineEdit *addEdit(const std::string &name, int pos, QGridLayout *layout);

 public slots:
  void updateState();

  void updateInverseVideo();
  void updateCursorVisible();
  void updateCursorBlink();
  void updateAlt();

  void colorChanged(const QColor &);

  void updateFont();

 private:
  CQPageText *text_                  { nullptr };
  QCheckBox  *inverseVideo_          { nullptr };
  QCheckBox  *sendMousePress_        { nullptr };
  QCheckBox  *sendMouseRelease_      { nullptr };
  QCheckBox  *sendMouseMotion_       { nullptr };
  QCheckBox  *sendFocusInOut_        { nullptr };
  QCheckBox  *scrollBottomKey_       { nullptr };
  QCheckBox  *scrollBottomTty_       { nullptr };
  QCheckBox  *applicationCursorKeys_ { nullptr };
  QCheckBox  *insertMode_            { nullptr };
  QCheckBox  *lineWrap_              { nullptr };
  QCheckBox  *ansiVt52Mode_          { nullptr };
  QCheckBox  *tek4014Mode_           { nullptr };
  QCheckBox  *keyPadMode_            { nullptr };
  QCheckBox  *lfNlMode_              { nullptr };
  QCheckBox  *ffNpMode_              { nullptr };
  QCheckBox  *smoothScroll_          { nullptr };
  QCheckBox  *originMode_            { nullptr };
  QCheckBox  *autoRepeat_            { nullptr };
  QCheckBox  *cursorVisible_         { nullptr };
  QCheckBox  *cursorBlink_           { nullptr };
  QCheckBox  *reverseWrap_           { nullptr };
  QCheckBox  *allow80132_            { nullptr };
  QCheckBox  *control8Bit_           { nullptr };
  QCheckBox  *altPage_               { nullptr };

  QCheckBox      *scrollSet_    { nullptr };
  CQIntegerEdit  *scrollTop_    { nullptr };
  CQIntegerEdit  *scrollBottom_ { nullptr };

  CQColorChooser *bgColor_ { nullptr };
  CQColorChooser *fgColor_ { nullptr };

  CQColorChooser *cursorColor_ { nullptr };

  CQColorChooser *color1_ { nullptr };
  CQColorChooser *color2_ { nullptr };
  CQColorChooser *color3_ { nullptr };
  CQColorChooser *color4_ { nullptr };
  CQColorChooser *color5_ { nullptr };
  CQColorChooser *color6_ { nullptr };
  CQColorChooser *color7_ { nullptr };
  CQColorChooser *color8_ { nullptr };

  CQColorChooser *altColor1_ { nullptr };
  CQColorChooser *altColor2_ { nullptr };
  CQColorChooser *altColor3_ { nullptr };
  CQColorChooser *altColor4_ { nullptr };
  CQColorChooser *altColor5_ { nullptr };
  CQColorChooser *altColor6_ { nullptr };
  CQColorChooser *altColor7_ { nullptr };
  CQColorChooser *altColor8_ { nullptr };

  QLineEdit     *fontName_ { nullptr };
  CQIntegerEdit *fontSize_ { nullptr };

  QCheckBox     *iconized_    { nullptr };
  QCheckBox     *maximized_   { nullptr };
  QLineEdit     *windowTitle_ { nullptr };
  QLineEdit     *iconName_    { nullptr };
  QLineEdit     *windowPos_   { nullptr };
  QLineEdit     *pixelSize_   { nullptr };
  QLineEdit     *charSize_    { nullptr };
};

#endif
