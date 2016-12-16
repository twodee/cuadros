#include <iostream>

#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "CuadrosWindow.h"

/* ------------------------------------------------------------------------- */

CuadrosWindow::CuadrosWindow(QWidget *parent) :
  QMainWindow(parent) {

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
  settings_picker->addItem("Display");

  QStackedWidget *settings_pager = new QStackedWidget();

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

  // Add pages
  settings_pager->addWidget(display_page);

  QVBoxLayout *settings_panel_layout = new QVBoxLayout(settings_panel);
  settings_panel_layout->setSpacing(6);
  settings_panel_layout->setContentsMargins(0, 6, 0, 0);
  settings_panel_layout->addWidget(settings_picker);
  settings_panel_layout->addWidget(settings_pager);

  this->setCentralWidget(horizontal_splitter);

  // Actions
  connect(interpolation_picker, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int i) {
    canvas->makeCurrent(); // no effect without this
    renderer->setInterpolation(i);
    canvas->update();
  });
}

/* ------------------------------------------------------------------------- */

CuadrosWindow::~CuadrosWindow() {
}

/* ------------------------------------------------------------------------- */

void CuadrosWindow::open(const std::string &path) {
  renderer->show(path);
}

/* ------------------------------------------------------------------------- */

