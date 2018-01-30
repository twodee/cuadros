#ifndef CUADROSWINDOW_H
#define CUADROSWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QSplitter>
#include <QSpinBox>
#include <QComboBox>

#include <twodee/QVector3.h>
#include <twodee/QVector4.h>

#include "CuadrosCanvas.h"

/* ------------------------------------------------------------------------- */

class CuadrosWindow : public QMainWindow {
  Q_OBJECT

  public:
    explicit CuadrosWindow(QWidget *parent = 0);
    ~CuadrosWindow();

    void open(const std::string &path);
    void open(int width, int height);

  private:
    void syncColor();

    CuadrosCanvas *canvas;
    CuadrosRenderer *renderer;
    QSplitter *horizontal_splitter;

    QComboBox *settings_picker;
    QComboBox *interpolation_picker;
    QSpinBox *frame_spinner;

    QSlider *rgb_sliders[3];
    QSlider *hsv_sliders[3];
    QSpinBox *rgb_boxes[3];
    QSpinBox *hsv_boxes[3];
    QPushButton *swatch;

    QString last_directory;
    td::QVector4<int> rgb;
};

/* ------------------------------------------------------------------------- */

#endif
