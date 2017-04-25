#include <CQPageTextStatus.h>
#include <CQPageTextWidget.h>
#include <CQPageText.h>

#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>

CQPageTextStatus::
CQPageTextStatus(CQPageTextWidget *widget) :
 QWidget(widget), widget_(widget)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);
}

void
CQPageTextStatus::
addStretch()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  layout->addStretch(1);
}

void
CQPageTextStatus::
addMsgLabel()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  msgLabel_ = new QLabel;

  layout->addWidget(msgLabel_);
}

void
CQPageTextStatus::
setMsg(const QString &str)
{
  msgLabel_->setText(str);
}

void
CQPageTextStatus::
addCmdEdit()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  cmdEdit_ = new QLineEdit;

  connect(cmdEdit_, SIGNAL(returnPressed()), this, SLOT(cmdSlot()));

  layout->addWidget(cmdEdit_);
}

void
CQPageTextStatus::
addPosLabel()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  posLabel_ = new QLabel;

  layout->addWidget(posLabel_);
}

void
CQPageTextStatus::
addSizeLabel()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  sizeLabel_ = new QLabel;

  layout->addWidget(sizeLabel_);
}

void
CQPageTextStatus::
cmdSlot()
{
  QString cmd = cmdEdit_->text();

  CQPageText *text = widget_->getArea();

  text->loadCmdLine(cmd.toStdString());
}

void
CQPageTextStatus::
updateLabels()
{
  CQPageText *text = widget_->getArea();

  int rows = text->getPageRows();
  int cols = text->getPageCols();

  if (sizeLabel_)
    sizeLabel_->setText(QString("%1x%2").arg(rows).arg(cols));

  ///

  CQPageText *area = widget_->getArea();

  const CTextPos &pos = area->getPosition();

  int x = pos.getCol();
  int y = pos.getRow();

  if (posLabel_)
    posLabel_->setText(QString("%1x%2").arg(x).arg(y));
}
