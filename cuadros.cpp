#include <iostream>
#include <QApplication>
#include <QCommandLineParser>

#include "CuadrosWindow.h"

/* ------------------------------------------------------------------------- */

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  QSurfaceFormat format;
  format.setVersion(4, 1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  /* format.setDepthBufferSize(24); */
  QSurfaceFormat::setDefaultFormat(format);

  QCommandLineParser parser;
  parser.setApplicationDescription("An image editor for me.");
  parser.addPositionalArgument("file", "Image file.");
  parser.process(app);

  const QStringList args = parser.positionalArguments();

  CuadrosWindow window;
  window.open(args.begin()->toStdString());
  window.show();
  window.resize(768, 512);

  return app.exec();
}

/* ------------------------------------------------------------------------- */

