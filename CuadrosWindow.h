#ifndef CUADROSWINDOW_H
#define CUADROSWINDOW_H

#include <QMainWindow>
#include <QSplitter>

#include "CuadrosCanvas.h"

/* ------------------------------------------------------------------------- */

class CuadrosWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit CuadrosWindow(QWidget *parent = 0);
    ~CuadrosWindow();

  private:
    CuadrosCanvas *canvas;
    QSplitter *horizontal_splitter;
};

/* ------------------------------------------------------------------------- */

#endif
