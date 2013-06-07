#include <QtGui/QApplication>
#include "OnkyoRemote.h"


int main(int argc, char** argv) {
  QApplication app(argc, argv);
  OnkyoRemote foo;
  foo.show();
  return app.exec();
}
