#include <iostream>
#include <QApplication>
#include <QCommandLineParser>
#include <string>

#include "CuadrosWindow.h"

using std::string;

/* ------------------------------------------------------------------------- */

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  QSurfaceFormat format;
  format.setVersion(4, 1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  /* format.setDepthBufferSize(24); */
  QSurfaceFormat::setDefaultFormat(format);

  QCommandLineParser parser;
  QCommandLineOption new_option(QStringList() << "n" << "new", "New image's dimensions.", "dims");

  parser.setApplicationDescription("An image editor for me.");
  parser.addPositionalArgument("file", "Image file.");
  parser.addOption(new_option);
  parser.process(app);

  const QStringList args = parser.positionalArguments();

  if (parser.isSet(new_option)) {
    CuadrosWindow *window = new CuadrosWindow();
    stringstream ss(parser.value(new_option).toStdString());
    int width;
    int height;
    char c;
    ss >> width >> c >> height;
    window->open(width, height);
    window->show();
    window->resize(768, 512);
  }

  for (auto arg : args) {
    CuadrosWindow *window = new CuadrosWindow();
    window->open(arg.toStdString());
    window->show();
    window->resize(768, 512);
  }

  return app.exec();
}

/* ------------------------------------------------------------------------- */

