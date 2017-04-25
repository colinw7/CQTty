#include <CQPageTextToolBar.h>
#include <CQPageTextWidget.h>
#include <CQPageTextButton.h>

#include <QHBoxLayout>

CQPageTextToolBar::
CQPageTextToolBar(CQPageTextWidget *widget) :
 QWidget(widget), widget_(widget)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);
}

void
CQPageTextToolBar::
addButton(const QIcon &icon, const QString &cmd)
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  CQPageTextButton *button = new CQPageTextButton(this, cmd);

  button->setIcon(icon);

  buttons_.push_back(button);

  layout->addWidget(button);
}

void
CQPageTextToolBar::
addStretch()
{
  QHBoxLayout *layout = qobject_cast<QHBoxLayout *>(this->layout());

  layout->addStretch(1);
}
