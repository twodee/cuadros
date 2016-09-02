#ifndef CUADROSCANVAS_H
#define CUADROSCANVAS_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_1_Core>

#include "CuadrosRenderer.h"

class CuadrosCanvas : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {
  public:
    explicit CuadrosCanvas(QWidget *parent = nullptr);

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

  private:
    CuadrosRenderer renderer;
};


#endif
