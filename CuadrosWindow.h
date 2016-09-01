#ifndef CUADROSWINDOW_H
#define CUADROSWINDOW_H

#include <QMainWindow>

#include "CuadrosCanvas.h"

/* ------------------------------------------------------------------------- */

class CuadrosWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit CuadrosWindow(QWidget *parent = 0);
    ~CuadrosWindow();

  private:
    CuadrosCanvas *canvas;
};

/* ------------------------------------------------------------------------- */

#endif
