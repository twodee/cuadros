#include <iostream>

#include "CuadrosCanvas.h"

/* ------------------------------------------------------------------------- */

CuadrosCanvas::CuadrosCanvas(QWidget *parent) :
  QOpenGLWidget(parent),
  renderer() {
  // Qt doesn't by default emit mouse events unless a button is down. We want
  // events on hovering too.
  setMouseTracking(true);
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
 
  int x = event->x();
  int y = height() - event->y(); 

  if (renderer.isMouseOverImage(x, y)) {
    setCursor(Qt::CrossCursor);
  } else {
    setCursor(Qt::ArrowCursor);
  }

  if (event->buttons()) {
    if (event->buttons() & Qt::LeftButton) {
      renderer.leftMouseDraggedTo(x, y);
    } else if (event->buttons() & Qt::RightButton) {
      renderer.rightMouseDraggedTo(x, y);
    }
    update();
  }
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

