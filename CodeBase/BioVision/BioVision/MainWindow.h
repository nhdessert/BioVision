/*!
 * \class MainWindow
 *
 * MainWindow is the main BioVision window through which the user interacts with the program
 *
 * MainWindow inherits from Qt's \QMainWindow. It is constructed by WindowManager and is the
 * primary way that the user interacts with the program. All functionality in the program must
 * be able to be accessed by MainWindow. Note that if a funtion ends with the word Slot it is a
 * slot that will be called when a certain signal is emitted. Signals and slots are connected when
 * MainWindow is constructed.  Most GUI button presses/events are handled through a slot, and MainWindow passes
 * the data and anything else needed back to system to handle the request.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class WindowManager;

#include <QMainWindow>
#include <QFileDialog>
#include <QTreeWidget>
#include <QMouseEvent>
#include <QMessageBox>
#include <phonon>
#include "WindowManager.h"
#include "Project.h"
#include "Video.h"
#include "VideoPlayer.h"
#include <QtDeclarative>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    //Constructors
    explicit MainWindow(QWidget *parent = 0, WindowManager *windowManager = 0);
    ~MainWindow();

    // Progress
    void setProgress(int progress);

    // Projects tree view
    void refreshProjectBrowser();

    // Carousel Methods
    void addImageToCarousel(QString name, QString index);
    void clearCarousel();
    Q_INVOKABLE void expandCarouselFrame(int index, QString url);

    // Conversions for Analyze settings
    int getPreviewSpeedSelected();
    int getPreviewWindowSizeSelected();
    int getImageOutputSizeSelected();

public slots:
    // Projects
    void newProjectSlot();
    void loadProjectSlot();
    void saveProjectSlot();
    void saveProjectAsSlot();
    void saveAllProjectsSlot();
    void removeProjectSlot();
    void hideProjectSlot();
    void viewWorkspaceSlot();
    void openExcelFileSlot();

    // Videos
    void addVideoSlot();
    void removeVideoSlot();

    // Threshold Bar
    void thresholdChangedSlot(int value);
    void moveSliderSlot(QString value);

    // Phonon-related
    void playPauseSlot();
    void setTotalTimeSlot(qint64 totalTime);
    void updateCurrentTimeSlot(qint64 currentTime);

    // Regions
    void editRegionSlot();
    void deleteRegionSlot();

    // Launch Windows
    void launchDocumentationSlot();
    void launchAboutSlot();
    void launchOptionsSlot();

    // Handle clicks on the project tree.
    void projectBrowserPressedSlot(QTreeWidgetItem *item, int counter);
    void projectBrowserDoubleClickedSlot(QTreeWidgetItem *item, int counter);
    void projectBrowserRightClickedSlot(const QPoint& pos);

    // Analyze
    void analyzeSlot();
    void cancelSlot();

    // Video start and end times, edit points slot
    void addEditPointSlot();
    void removeLastEditPointSlot();
    void startTimeChangeCheckerSlot();
    void endTimeChangeCheckerSlot();
    float calculateEditTimeSliderDrawingPosition(float currentEditTimeInSeconds);

    // Analyze & Preview Analyze Options slots
    void previewSlowSpeedCheckSlot();
    void previewMediumSpeedCheckSlot();
    void previewFastSpeedCheckSlot();

    void previewWindowSizeNativeSlot();
    void previewWindowSizeMediumSlot();
    void previewWindowSizeLargeSlot();

    void analyzeImageSaveNativeSlot();
    void analyzeImageSaveSmallSlot();
    void analyzeImageSaveMediumSlot();

private:
    //Properties
    Ui::MainWindow *ui;
    WindowManager *_windowManager;
    QString _activeProjectName;
    QString _activeVideoName;
    QString _activeRegionName;
    bool _isRegionBeingDrawn;
    int _regionBeginX;
    int _regionEndX;
    int _regionBeginY;
    int _regionEndY;
    double _xScaling;
    double _yScaling;
    double _xResolution;
    double _yResolution;
    double _xBlackSpace;
    double _yBlackSpace;
    double _xConverter;
    double _yConverter;
    Phonon::MediaSource *_mediaSource;
    QObject *_resultCarouselObject;
    int _sensitivity;

    //Dustin Added//
    std::vector <QTime> _videoEditTimes;
    std::vector <int> _videoEditTimesInSeconds;
    QTime _previousStartTime;
    QTime _previousEndTime;
    std::vector <int> _editTimeDrawingPositions;

    //added for region drawing in GUI
    QColor colorList[10];
    std::vector<int> allActiveXCoords;
    std::vector<int> allActiveYCoords;
    std::vector<int> allActiveRegionWidths;
    std::vector<int> allActiveRegionHeights;
    //Dustin Added//

    //Methods
    void makeProjectActive(QTreeWidgetItem *item);
    void makeVideoActive(QTreeWidgetItem *item);
    void makeRegionActive(QTreeWidgetItem *item);
    void inactivateProject();
    void inactivateVideo();
    void inactivateRegion();
    void getAllProjects();

    // Mouse events- region drawing
    bool isMouseEventInVideoPlayer(QMouseEvent *mouseEvent);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);

    // Resizing of the window
    void resizeEvent(QResizeEvent *resizeEvent);
    void moveAndResizeToolbarElements();
    void moveAndResizePlayerElements();
    void moveAndResizeStatusElements();
    void moveAndResizeCarouselElements();
    void moveAndResizePlayerToolsElements();
    void moveAndResizeFileElements();

    // Utility functions/conversions
    QString parseVideoName(QString videoPath);
    int qTimeToSeconds(QTime);
    QString convertMilToHourMinSec(qint64);
    int uiXToFrameX(int uiX);
    int uiYToFrameY(int uiY);
    int frameXToUiX(int frameX);
    int frameYToUiY(int frameY);
    double min(double x, double y);
    double max(double x, double y);
    int roundToNearest(double x);
    void setupResolutionConverters();
    void initializeResultCarousel();

    //Dustin Added, testing a new way to draw region rectangles onto the GUI
    void paintEvent(QPaintEvent *e);

    // Handles prompting for saving projects when the window closes.
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
