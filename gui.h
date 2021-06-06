#include <QMainWindow>

#include "ppm.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    PPM *image;
    QImage *qimage;
    QLabel *label;
    QMenu *filters_menu;

    void open_image();
    void load_image();
    void load_pixels();

    enum Filters {
        EQUAL,
        MEDIAN,
    };

    void apply_filter(Filters filter);
};
