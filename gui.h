#include <QMainWindow>

#include "pbm.h"

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
    PBM *image;
    QImage *qimage;
    QLabel *label;
    QMenu *filters_menu;
    QAction *save_as_act;

    void open_image();
    void load_image();
    void load_pixels();
    void save_image_as();

    enum Filters {
        MEDIAN,
    };

    void apply_filter(Filters filter);
};
