#include <iostream>

#include "CuadrosWindow.h"

/* ------------------------------------------------------------------------- */

CuadrosWindow::CuadrosWindow(QWidget *parent) :
  QMainWindow(parent) {
  canvas = new CuadrosCanvas(this);

  QSizePolicy p1(QSizePolicy::Preferred, QSizePolicy::Preferred);
  p1.setHorizontalStretch(1);
  p1.setVerticalStretch(1);
  canvas->setSizePolicy(p1);

  this->setCentralWidget(canvas);
}

/* ------------------------------------------------------------------------- */

CuadrosWindow::~CuadrosWindow() {
}

/* ------------------------------------------------------------------------- */

