#include <CQApp.h>
#include <CQTty.h>
#include <CQPageText.h>
#ifdef DIR_LIST
#include <CQDirView.h>
#endif
#ifdef FILE_EDIT
#include <CQEdit.h>
#endif

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

#ifdef DIR_LIST
  CQDirViewFactory::init();
#endif

#ifdef FILE_EDIT
  CQEdit::init();
#endif

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
