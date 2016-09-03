#include <iostream>

#include "CuadrosWindow.h"

/* ------------------------------------------------------------------------- */

CuadrosWindow::CuadrosWindow(QWidget *parent) :
  QMainWindow(parent) {

  horizontal_splitter = new QSplitter(this);
  horizontal_splitter->setOrientation(Qt::Horizontal);

  canvas = new CuadrosCanvas(horizontal_splitter);

  QSizePolicy p1(QSizePolicy::Preferred, QSizePolicy::Preferred);
  p1.setHorizontalStretch(1);
  p1.setVerticalStretch(1);
  canvas->setSizePolicy(p1);

  QWidget *settings_panel = new QWidget(horizontal_splitter);;

  horizontal_splitter->addWidget(canvas);
  horizontal_splitter->addWidget(settings_panel);

  this->setCentralWidget(horizontal_splitter);
}

/* ------------------------------------------------------------------------- */

CuadrosWindow::~CuadrosWindow() {
}

/* ------------------------------------------------------------------------- */

