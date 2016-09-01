#include <iostream>
#include <QApplication>

#include "CuadrosWindow.h"

/* ------------------------------------------------------------------------- */

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  QSurfaceFormat format;
  format.setVersion(4, 1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  /* format.setDepthBufferSize(24); */
  QSurfaceFormat::setDefaultFormat(format);

  CuadrosWindow window;
  window.show();
  return app.exec();
}

/* ------------------------------------------------------------------------- */

