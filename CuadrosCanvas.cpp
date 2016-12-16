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

void CuadrosCanvas::mousePressEvent(QMouseEvent *event) {
  makeCurrent();
  renderer.leftMouseDownAt(event->x(), height() - event->y());
  update();
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::mouseMoveEvent(QMouseEvent *event) {
  makeCurrent();
  renderer.leftMouseDraggedTo(event->x(), height() - event->y());
  update();
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::mouseReleaseEvent(QMouseEvent *event) {
  makeCurrent();
  update();
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::wheelEvent(QWheelEvent *event) {
  makeCurrent();
  renderer.scroll(event->angleDelta().y());
  update();
}

/* ------------------------------------------------------------------------- */

