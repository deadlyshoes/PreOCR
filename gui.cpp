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
    QAction *median_act = new QAction("&Median", this);
    connect(median_act, &QAction::triggered, this, [=]{apply_filter(Filters::MEDIAN);});
    QAction *dilation_act = new QAction("&Dilation", this);
    connect(dilation_act, &QAction::triggered, this, [=]{apply_filter(Filters::DILATION);});
    QAction *erosion_act = new QAction("&Erosion", this);
    connect(erosion_act, &QAction::triggered, this, [=]{apply_filter(Filters::CLOSING);});
    QAction *opening_act = new QAction("&Opening", this);
    connect(opening_act, &QAction::triggered, this, [=]{apply_filter(Filters::OPENING);});
    QAction *closing_act = new QAction("&Closing", this);
    connect(closing_act, &QAction::triggered, this, [=]{apply_filter(Filters::CLOSING);});
    filters_menu->addAction(median_act);
    filters_menu->addAction(dilation_act);
    filters_menu->addAction(erosion_act);
    filters_menu->addAction(opening_act);
    filters_menu->addAction(closing_act);
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
    image = new PBM(image_path.toStdString());
    if (image) {
        MainWindow::load_image();
        filters_menu->setEnabled(true);
        save_as_act->setEnabled(true);
    }
}

void MainWindow::load_image() {
    int width = image->width;
    int height = image->height;

    qimage = new QImage(width, height, QImage::Format_Mono);

    MainWindow::load_pixels();
}

void MainWindow::load_pixels() {
    int width = image->width;
    int height = image->height;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            qimage->setPixel(j, i, (image->data[i][j] + 1) % 2);
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
        case MEDIAN:
            image->median(3);
            break;
        case DILATION:
            {
                std::vector<std::vector<int>> default_se{{0, 1, 0}, {1, 1, 1}, {0, 1, 0}}; // 3x3 cross
                image->dilation(default_se);
            }
            break;
        case EROSION:
            {
                std::vector<std::vector<int>> default_se{{0, 1, 0}, {1, 1, 1}, {0, 1, 0}}; // 3x3 cross
                image->erosion(default_se);
            }
            break;
        case OPENING:
            {
                std::vector<std::vector<int>> default_se{{0, 0, 1, 0, 0}, {0, 1, 1, 1, 0}, {1, 1, 1, 1, 1}, {0, 1, 1, 1, 0}, {0, 0, 1, 0, 0}}; // 3x3 cross
                image->opening(default_se);
            }
            break;
        case CLOSING:
            {
                std::vector<std::vector<int>> default_se{{0, 0, 1, 0, 0}, {0, 1, 1, 1, 0}, {1, 1, 1, 1, 1}, {0, 1, 1, 1, 0}, {0, 0, 1, 0, 0}}; // 3x3 cross
                image->closing(default_se);
            }
            break;
        default:
            break;
    }

    load_pixels();
}
