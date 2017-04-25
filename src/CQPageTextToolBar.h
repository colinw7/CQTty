#ifndef CQPageTextToolBar_H
#define CQPageTextToolBar_H

#include <QWidget>

class CQPageTextWidget;
class CQPageTextButton;

class CQPageTextToolBar : public QWidget {
  Q_OBJECT

 public:
  CQPageTextToolBar(CQPageTextWidget *widget);

  CQPageTextWidget *widget() const { return widget_; }

  void addButton(const QIcon &icon, const QString &cmd);
  void addStretch();

 private:
  typedef std::vector<CQPageTextButton *> Buttons;

  CQPageTextWidget *widget_ { nullptr };
  Buttons           buttons_;
};

#endif
