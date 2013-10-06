/*!
 * \class RegionWindow is responsible for launching a new GUI window whenever the user creates a region within a video.
 * The region they created will be displayed to them as an image, as well as numbric data about the region, and the user
 * is then able to name the region, add notes to it, and adjust the region's threshold(the precentage of pixels within that
 * region that must containe motion for it to be flagged for data collection)
 */

#ifndef REGIONWINDOW_H
#define REGIONWINDOW_H

class WindowManager;

#include <QDialog>
#include <QString>
#include <QGraphicsScene>
#include <QPen>
#include "WindowManager.h"

namespace Ui {
class RegionWindow;
}

class RegionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegionWindow(QWidget *parent = 0);
    explicit RegionWindow(QWidget* parent, WindowManager *windowManager, QString projName,
                          QString vidName, QString regionName, int threshold, QString notes,
                          int x, int y, int width, int height, QString pathToPicture);

    ~RegionWindow();

public slots:
    void sliderChangedSlot(int value);
    void moveSliderSlot(QString value);
    void saveSlot();
    void checkInputSlot(QString text);

private:
    //Properties
    Ui::RegionWindow *ui;
    WindowManager *_windowManager;
    QString _projName;
    QString _vidName;
    QString _regionName;
    int _threshold;
    QString _notes;
    QString _pathToPicture;
    QPixmap *_image;
    QPen *_pen;
    QGraphicsScene *_scene;
    QGraphicsPixmapItem *_imageItem;

    // Region Dimension properties
    int _x;
    int _y;
    int _width;
    int _height;
    double _xBlackSpace;
    double _yBlackSpace;
    double _xResolution;
    double _yResolution;
    double _xScaling;
    double _yScaling;
    double _xConverter;
    double _yConverter;

    //Functions
    void setGraphicsDisplay(QString Path);
    int frameXToUiX(int frameX);
    int frameYToUiY(int frameY);
    void setupResolutionConverters();
    double min(double x, double y);
    double max(double x, double y);
    int roundToNearest(double x);
};


#endif // REGIONWINDOW_H

