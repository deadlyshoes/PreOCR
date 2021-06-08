#include <QtWidgets>

#include "gui.h"

MainWindow::MainWindow() {
    QMenu *file_menu = menuBar()->addMenu("&File");
    QAction *open_act = new QAction("&Open", this);
    connect(open_act, &QAction::triggered, this, &MainWindow::open_image);
    save_as_act = new QAction("&Save As...", this);
    connect(save_as_act, &QAction::triggered, this, &MainWindow::save_image_as);
    file_menu->addAction(open_act);
    file_menu->addAction(save_as_act);
    save_as_act->setEnabled(false);
    file_menu->addSeparator();
    file_menu->addAction("E&xit", this, &QWidget::close);

    filters_menu = menuBar()->addMenu("&Filters");
    QAction *equal_act = new QAction("&Histogram Equalization", this);
    connect(equal_act, &QAction::triggered, this, [=]{apply_filter(Filters::EQUAL);});
    QAction *median_act = new QAction("&Median", this);
    connect(median_act, &QAction::triggered, this, [=]{apply_filter(Filters::MEDIAN);});
    filters_menu->addAction(equal_act);
    filters_menu->addAction(median_act);
    filters_menu->setEnabled(false);

    label = new QLabel;

    QScrollArea *scroll_area = new QScrollArea;
    scroll_area->setFrameShape(QFrame::NoFrame);
    scroll_area->setWidget(label);
    scroll_area->setWidgetResizable(true);

    setCentralWidget(scroll_area);
}

MainWindow::~MainWindow() {

}

void MainWindow::open_image() {
    QString image_path = QFileDialog::getOpenFileName(this, "Open");
    image = new PPM(image_path.toStdString());
    if (image) {
        MainWindow::load_image();
        filters_menu->setEnabled(true);
        save_as_act->setEnabled(true);
    }
}

void MainWindow::load_image() {
    int bpp = image->header.bpp;
    int width = image->header.width;
    int height = image->header.height;

    if (bpp == 1)
        qimage = new QImage(width, height, QImage::Format_Mono);
    else if (bpp == 8)
        qimage = new QImage(width, height, QImage::Format_Grayscale8);
    else if (bpp == 24)
        qimage = new QImage(width, height, QImage::Format_RGB32);

    MainWindow::load_pixels();
}

void MainWindow::load_pixels() {
    int bpp = image->header.bpp;
    int width = image->header.width;
    int height = image->header.height;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (bpp == 1) {
                qimage->setPixel(j, i, (image->data[i][j][0] + 1) % 2);
            } else if (bpp < 24) {
                qimage->setPixel(j, i, image->data[i][j][0]);
            } else {
                QRgb value = qRgb(image->data[i][j][0], image->data[i][j][1], image->data[i][j][2]);
                qimage->setPixel(j, i, value);
            }
        }
    }

    label->setPixmap(QPixmap::fromImage(*qimage));
}

void MainWindow::save_image_as() {
    QString new_image_path = QFileDialog::getSaveFileName(this, "Save As");
    image->write(new_image_path.toStdString());
}

void MainWindow::apply_filter(Filters filter) {
    switch (filter) {
        case EQUAL:
            image->equal();
            break;
        case MEDIAN:
            image->median(3);
            break;
        default:
            break;
    }

    load_pixels();
}
