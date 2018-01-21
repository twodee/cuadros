#include <iostream>

#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>

#include "CuadrosWindow.h"

/* ------------------------------------------------------------------------- */

CuadrosWindow::CuadrosWindow(QWidget *parent) :
  QMainWindow(parent),
  last_directory(QDir::homePath()),
  rgb(0, 0, 0) {

  horizontal_splitter = new QSplitter(this);
  horizontal_splitter->setOrientation(Qt::Horizontal);

  canvas = new CuadrosCanvas(horizontal_splitter);
  renderer = canvas->getRenderer();

  QSizePolicy p1(QSizePolicy::Preferred, QSizePolicy::Preferred);
  p1.setHorizontalStretch(1);
  p1.setVerticalStretch(1);
  canvas->setSizePolicy(p1);

  QWidget *settings_panel = new QWidget(horizontal_splitter);;

  horizontal_splitter->addWidget(canvas);
  horizontal_splitter->addWidget(settings_panel);

  QSpacerItem *vertical_spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  settings_picker = new QComboBox();
  settings_picker->addItem("Drawing");
  settings_picker->addItem("Display");

  QStackedWidget *settings_pager = new QStackedWidget();

  // --------------------------------------------------------------------------

  QWidget *display_page = new QWidget();

  QLabel *interpolation_label = new QLabel("Interpolation");
  interpolation_picker = new QComboBox();
  interpolation_picker->addItem("Nearest");
  interpolation_picker->addItem("Linear");

  QGridLayout *display_page_layout = new QGridLayout(display_page);
  display_page_layout->setSpacing(-1);
  display_page_layout->setContentsMargins(0, 0, 0, 0);
  display_page_layout->addWidget(interpolation_label, 0, 0, 1, 1);
  display_page_layout->addWidget(interpolation_picker, 0, 1, 1, 1);
  display_page_layout->addItem(vertical_spacer, 1, 0, 1, 2);

  // --------------------------------------------------------------------------

  QWidget *drawing_page = new QWidget();

  QLabel *rgb_labels[] = {
    new QLabel("R"),
    new QLabel("G"),
    new QLabel("B")
  };

  QLabel *hsv_labels[] = {
    new QLabel("H"),
    new QLabel("S"),
    new QLabel("V")
  };

  for (int d = 0; d < 3; ++d) {
    rgb_sliders[d] = new QSlider(Qt::Horizontal, drawing_page);
    hsv_sliders[d] = new QSlider(Qt::Horizontal, drawing_page);
    rgb_boxes[d] = new QSpinBox(drawing_page);
    hsv_boxes[d] = new QSpinBox(drawing_page);
    rgb_sliders[d]->setMaximum(255);
    hsv_sliders[d]->setMaximum(255);
    rgb_boxes[d]->setMaximum(255);
    hsv_boxes[d]->setMaximum(255);
  }

  swatch = new QPushButton(drawing_page);
  swatch->setFlat(true);
  swatch->setAutoFillBackground(true);
  swatch->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

  QGridLayout *drawing_page_layout = new QGridLayout(drawing_page);
  drawing_page_layout->setSpacing(-1);
  drawing_page_layout->setContentsMargins(0, 0, 0, 0);

  for (int d = 0; d < 3; ++d) {
    drawing_page_layout->addWidget(rgb_labels[d], d, 0, 1, 1);
    drawing_page_layout->addWidget(hsv_labels[d], d + 3, 0, 1, 1);
    drawing_page_layout->addWidget(rgb_sliders[d], d, 1, 1, 1);
    drawing_page_layout->addWidget(hsv_sliders[d], d + 3, 1, 1, 1);
    drawing_page_layout->addWidget(rgb_boxes[d], d, 2, 1, 1);
    drawing_page_layout->addWidget(hsv_boxes[d], d + 3, 2, 1, 1);
  }

  drawing_page_layout->addWidget(swatch, 6, 0, 1, 3);

  // --------------------------------------------------------------------------

  // Add pages
  settings_pager->addWidget(drawing_page);
  settings_pager->addWidget(display_page);

  QVBoxLayout *settings_panel_layout = new QVBoxLayout(settings_panel);
  settings_panel_layout->setSpacing(6);
  settings_panel_layout->setContentsMargins(0, 6, 0, 0);
  settings_panel_layout->addWidget(settings_picker);
  settings_panel_layout->addWidget(settings_pager);

  this->setCentralWidget(horizontal_splitter);

  // Status bar
  QStatusBar *statusBar = new QStatusBar(this);
  setStatusBar(statusBar);

  // Menubar
  QMenuBar *menuBar = new QMenuBar();
  setMenuBar(menuBar);

  QAction *action_save_as = new QAction(this);
  action_save_as->setText("Save as");
  action_save_as->setShortcut(Qt::SHIFT + Qt::CTRL + Qt::Key_S);

  QMenu *menuFile = new QMenu("File");
  menuFile->addAction(action_save_as);

  QMenu *menuView = new QMenu("View");
  menuView->addAction(action_save_as);

  setWindowTitle("Cuadros");

  menuBar->addMenu(menuFile);
  menuBar->addMenu(menuView);
  menuBar->addAction(menuFile->menuAction());
  menuBar->addAction(menuView->menuAction());

  // Actions
  connect(interpolation_picker, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
    canvas->makeCurrent(); // no effect without this
    renderer->setInterpolation(i);
    canvas->update();
  });

  connect(settings_picker, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), settings_pager, &QStackedWidget::setCurrentIndex);

  connect(action_save_as, &QAction::triggered, [=]() {
    QString path = QFileDialog::getSaveFileName(this, "Save File", last_directory, "Images (*.png *.jpg)");
    if (path.length() != 0) {
      last_directory = QFileInfo(path).absolutePath();
      renderer->saveAs(path.toStdString());
    }
  });

  auto on_rgb_slider_changed = [=](int i) {
    for (int d = 0; d < 3; ++d) {
      rgb[d] = rgb_sliders[d]->value();
    }
    syncColor();
  };

  auto on_rgb_box_changed = [=](int i) {
    for (int d = 0; d < 3; ++d) {
      rgb[d] = rgb_boxes[d]->value();
    }
    syncColor();
  };

  for (int d = 0; d < 3; ++d) {
    connect(rgb_sliders[d], &QSlider::valueChanged, on_rgb_slider_changed);
    connect(rgb_boxes[d], static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), on_rgb_box_changed);
  }

  syncColor();

  // Really, I just want to disable Qt's auto focusing on the first spinbox.
  canvas->setFocus();
}

/* ------------------------------------------------------------------------- */

void CuadrosWindow::syncColor() {
  for (int d = 0; d < 3; ++d) {
    rgb_boxes[d]->setValue(rgb[d]);
    rgb_sliders[d]->setValue(rgb[d]);
  }
  renderer->setColor(rgb);
  QPalette palette;
  palette.setColor(QPalette::Button, QColor(rgb[0], rgb[1], rgb[2]));
  swatch->setPalette(palette);
}

/* ------------------------------------------------------------------------- */

CuadrosWindow::~CuadrosWindow() {
}

/* ------------------------------------------------------------------------- */

void CuadrosWindow::open(const std::string &path) {
  renderer->show(path);
}

/* ------------------------------------------------------------------------- */

void CuadrosWindow::open(int width, int height) {
  renderer->show(width, height);
}

/* ------------------------------------------------------------------------- */

