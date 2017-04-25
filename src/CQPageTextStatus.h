#ifndef CQPageTextStatus_H
#define CQPageTextStatus_H

#include <QWidget>

class CQPageTextWidget;

class QLineEdit;
class QLabel;

class CQPageTextStatus : public QWidget {
  Q_OBJECT

 public:
  CQPageTextStatus(CQPageTextWidget *widget);

  void addMsgLabel();

  void addCmdEdit();

  void addSizeLabel();

  void addPosLabel();

  void updateLabels();

  void addStretch();

  void setMsg(const QString &str);

 private slots:
  void cmdSlot();

 private:
  CQPageTextWidget *widget_    { nullptr };
  QLabel           *msgLabel_  { nullptr };
  QLineEdit        *cmdEdit_   { nullptr };
  QLabel           *sizeLabel_ { nullptr };
  QLabel           *posLabel_  { nullptr };
};

#endif
