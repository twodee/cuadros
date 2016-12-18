#ifndef CUADROSWINDOW_H
#define CUADROSWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QComboBox>

#include "CuadrosCanvas.h"

/* ------------------------------------------------------------------------- */

class CuadrosWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit CuadrosWindow(QWidget *parent = 0);
    ~CuadrosWindow();

    void open(const std::string &path);

  private:
    CuadrosCanvas *canvas;
    CuadrosRenderer *renderer;
    QSplitter *horizontal_splitter;

    QComboBox *settings_picker;
    QComboBox *interpolation_picker;

    QString last_directory;
};

/* ------------------------------------------------------------------------- */

#endif
