#include <iostream>

#include "CuadrosCanvas.h"

/* ------------------------------------------------------------------------- */

CuadrosCanvas::CuadrosCanvas(QWidget *parent) :
  QOpenGLWidget(parent) {
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f);
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::resizeGL(int width, int height) {
}

/* ------------------------------------------------------------------------- */

