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
  if (event->buttons() & Qt::LeftButton) {
    renderer.leftMouseDownAt(event->x(), height() - event->y());
  } else if (event->buttons() & Qt::RightButton) {
    renderer.rightMouseDownAt(event->x(), height() - event->y());
  }
  update();
}

/* ------------------------------------------------------------------------- */

void CuadrosCanvas::mouseMoveEvent(QMouseEvent *event) {
  makeCurrent();
  if (event->buttons() & Qt::LeftButton) {
    renderer.leftMouseDraggedTo(event->x(), height() - event->y());
  } else if (event->buttons() & Qt::RightButton) {
    renderer.rightMouseDraggedTo(event->x(), height() - event->y());
  }
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

