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

 private:
  CQPageText *text_;
  QCheckBox  *inverseVideo_;
  QCheckBox  *sendMousePress_;
  QCheckBox  *sendMouseRelease_;
  QCheckBox  *scrollBottomKey_;
  QCheckBox  *scrollBottomTty_;
  QCheckBox  *applicationCursorKeys_;
  QCheckBox  *insertMode_;
  QCheckBox  *lineWrap_;
  QCheckBox  *ansiVt52Mode_;
  QCheckBox  *keyPadMode_;
  QCheckBox  *lfNlMode_;
  QCheckBox  *ffNpMode_;
  QCheckBox  *smoothScroll_;
  QCheckBox  *originMode_;
  QCheckBox  *autoRepeat_;
  QCheckBox  *cursorVisible_;
  QCheckBox  *cursorBlink_;
  QCheckBox  *reverseWrap_;
  QCheckBox  *allow80132_;
  QCheckBox  *altPage_;

  QCheckBox      *scrollSet_;
  CQIntegerEdit  *scrollTop_;
  CQIntegerEdit  *scrollBottom_;

  CQColorChooser *bgColor_;
  CQColorChooser *fgColor_;

  CQColorChooser *cursorColor_;

  CQColorChooser *color1_;
  CQColorChooser *color2_;
  CQColorChooser *color3_;
  CQColorChooser *color4_;
  CQColorChooser *color5_;
  CQColorChooser *color6_;
  CQColorChooser *color7_;
  CQColorChooser *color8_;

  QLineEdit     *fontName_;
  CQIntegerEdit *fontSize_;

  QCheckBox     *iconized_;
  QCheckBox     *maximized_;
  QLineEdit     *windowTitle_;
  QLineEdit     *iconName_;
  QLineEdit     *windowPos_;
  QLineEdit     *pixelSize_;
  QLineEdit     *charSize_;

 public:
  CQPageTextState(CQPageText *text);

 private:
  CQColorChooser *addColor(const std::string &name, int pos, QGridLayout *layout);

  QLineEdit *addEdit(const std::string &name, int pos, QGridLayout *layout);

 public slots:
  void updateState();

  void updateInverseVideo();
  void updateCursorVisible();
  void updateCursorBlink();
  void updateAlt();

  void colorChanged(const QColor &);

  void updateFont();
};

#endif
