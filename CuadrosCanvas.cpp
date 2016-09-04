#include <iostream>

#include "CuadrosCanvas.h"

/* ------------------------------------------------------------------------- */

CuadrosCanvas::CuadrosCanvas(QWidget *parent) :
  QOpenGLWidget(parent),
  renderer() {
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::initializeGL() {
  initializeOpenGLFunctions();
  renderer.initializeGL();
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::paintGL() {
  renderer.render();
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::resizeGL(int width, int height) {
  renderer.resize(width, height);
}

/* ------------------------------------------------------------------------- */

CuadrosRenderer *CuadrosCanvas::getRenderer() {
  return &renderer;
}

/* ------------------------------------------------------------------------- */

