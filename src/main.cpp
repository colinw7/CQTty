#include <CQTty.h>
#include <CQPageText.h>
#include <CQImage.h>
#include <CQFont.h>
#include <CQWindow.h>
#include <QApplication>

#include <iostream>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  CQImage::setPrototype();

  CQFontMgrInst->setPrototype();

  CQWindow::setFactory();

  //CQDirViewFactory::init();

  //CQEdit::init();

  bool debug = false;
  bool trace = false;
  int  num   = 0;

  for (int i = 1; i < argc; ++i) {
    if      (strcmp(argv[i], "-debug") == 0)
      debug = true;
    else if (strcmp(argv[i], "-trace") == 0)
      trace = true;
    else if (strcmp(argv[i], "-num") == 0) {
      ++i;

      if (i < argc)
        num = atoi(argv[i]);
    }
    else
      std::cerr << "Invalid arg '" << argv[i] << "'" << std::endl;
  }

  if (num <= 0) num = 1;

  for (int i = 0; i < num; ++i) {
    CQTty *edit = new CQTty;

    edit->getText()->setDebug(debug);
    edit->getText()->setTrace(trace);

    edit->resize(800, 1000);

    edit->show();
  }

  return app.exec();
}
