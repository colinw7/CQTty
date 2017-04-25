#include <CQPageTextButton.h>
#include <CQPageTextToolBar.h>
#include <CQPageTextWidget.h>
#include <CQPageText.h>

CQPageTextButton::
CQPageTextButton(CQPageTextToolBar *toolbar, const QString &cmd) :
 QToolButton(toolbar), toolbar_(toolbar), cmd_(cmd)
{
  connect(this, SIGNAL(clicked()), this, SLOT(exec()));
}

void
CQPageTextButton::
exec()
{
  toolbar_->widget()->getArea()->processString(cmd_.toLatin1().constData());
}
