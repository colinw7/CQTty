#ifndef CQPageTextButton_H
#define CQPageTextButton_H

#include <QToolButton>

class CQPageTextToolBar;

class CQPageTextButton : public QToolButton {
  Q_OBJECT

 public:
  CQPageTextButton(CQPageTextToolBar *toolbar, const QString &cmd);

 private slots:
  void exec();

 private:
  CQPageTextToolBar *toolbar_ { nullptr };
  QString            cmd_;
};

#endif
