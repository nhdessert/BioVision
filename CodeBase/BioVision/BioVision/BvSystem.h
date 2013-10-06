/*!
 * \class BvSystem
 *
 * BvSystem is the communication hub between the different modules of BioVision, and handles passing data back and forth.
 * It acts in some ways as a facade, in some ways as an architectural layer.  It derives from QApplication and is instantiated
 * in main.cpp.  The instantiation of BvSystem actually launches the program.
 *
 * BvSystem has a reference to each manager class- ThreadManager, ProjectManager, and WindowManager, and therefore BvSystem
 * can communicate with each of them and pass requests to each of them.  Usually System takes requests from the GUI section
 * (WindowManager) and passes those on to different managers for processing.  In this way, it serves as a go-between.
 * For details, see the function list.
 */

#ifndef BVSYSTEM_H
#define BVSYSTEM_H

class ThreadManager;
class WindowManager;
class ProjectManager;

#include "ProjectManager.h"
#include "ThreadManager.h"
#include "WindowManager.h"
#include "Result.h"
#include "OpenCV.h"
#include "Analyzer.h"
#include "DetailAnalyzer.h"
#include "VideoCopier.h"

#include <QString>
#include <QApplication>
#include <string>

class BvSystem : public QApplication
{

    Q_OBJECT

public:

    // Constructor & Destructor
    BvSystem(int &argc, char **argv);
    virtual ~BvSystem();

    // Functions for the workspace directory.  These go to ProjectManager.
    QString getWorkspace();
    void setWorkspace(QString workspacePath);

    //Requests to Access or Manipulate ProjectManager data
    Project* getProject(QString projName);
    std::vector<Project*> getAllProjects();
    Video* getVideo(QString projName, QString vidName);
    std::vector<Video*> getAllVideos(QString projName);
    BvRegion* getRegion(QString projName, QString vidName, QString regionName);
    std::vector<BvRegion*>* getAllRegions(QString projName, QString vidName);
    void removeVideoFromProject(QString projName, QString vidName);
    bool addVideoToProject(QString projName, QString pathToVideoFile, QString vidName);
    QString getVideoPath(QString projName, QString vidName);
    int getVideoWidth(QString projName, QString vidName);
    int getVideoHeight(QString projName, QString vidName);
    void getAllMetaData(QString videoFilePath);
    void removeRegion(QString projName, QString vidName, QString regionName);
    void saveProject(QString projName /*QString filePath Or does a project have a path to where is should be saved?*/);
    void saveAsProject(QString projName, QString projPath);
    void saveAllProjects();
    void openProject(QString filePath);
    void hideProject(QString projName);
    bool removeProject(QString projName);
    bool makeNewProject(QString projName);
    void autoLoadProjects();

    // Region data.
    bool setRegion(QString projName, QString vidName, QString oldName, QString newName, int threshold, QString notes, int x, int y, int width, int height);
    bool checkNumberOfRegions(QString projName, QString vidName);
    std::vector<int>* getAllRegionsXcoords(QString projName, QString vidName);
    std::vector<int>* getAllRegionsYcoords(QString projName, QString vidName);
    std::vector<int>* getAllRegionsWidths(QString projName, QString vidName);
    std::vector<int>* getAllRegionsHeights(QString projName, QString vidName);
    std::vector<int>* getAllRegionsThresholds(QString projName, QString vidName);
    int getRegionThreshold(QString projName, QString vidName, QString regionName);
    QString getRegionNotes(QString projName, QString vidName, QString regionName);

    // Analysis data manipulation
    bool checkExperimentName(QString projName, QString vidName, QString runName);
    bool outputResults(QString projName, QString vidName, QString experimentName, bool images, bool overwriteExperiment=0);
    void setOverwriteOldExperiment(bool overwrite);
    // Functions to set and get video analysis start and stop times.
    void setVideoStartAndStopTimes(QString projName, QString vidName, QTime currentStartTime, QTime currentEndTime);
    void addVideoEditTime(QString projName, QString vidName, QTime newEditTime);
    void removeLastVideoEditTime(QString projName, QString vidName);
    std::vector<QTime> getVideoTimes(QString projName, QString vidName);
    int getNumberOfRegionsInVideo(QString projName, QString vidName);
    std::vector <int> getRegionDataForRegionWindow(QString projName, QString vidName, QString regionName);

    //Send an analyze request to the ThreadManager.
    QString sendAnalyzeRequest(QString projName, QString vidName, int startSec, int endSec, std::deque<int> videoEditTimesInSeconds, int motionSensitivity, bool isPreviewSelected,
                            int previewSpeed, int previewSize, int imageOutputSize, bool isOutputImages, bool isFullFrameAnalysis);

    //Send a file copying request to the ThreadManager.
    QString sendVideoCopyRequest(QString projName, QString videoPath, QString vidName);

    // Cancels a task.
    void cancelTask();

    // Create image when regions are selected .
    std::string saveFrameWhenRegionCreated(QString videoPath, int videoTimeInMilliseconds, int frameX1, int frameY1, int frameWidth, int frameHeight, int regionNumber);

    //Clear carousel.
    void clearCarousel();

public Q_SLOTS:
    void progressUpdateSlot(int);
    void clearCarouselSlot();
    void updateCarouselSlot(QString, QString index);
    void displayErrorWindowSlot();
    void handleResultSlot(Result*);

private:
    /*!
     * \brief _projectManager reference to the ProjectManager object to handle data saving and loading.
     */
    ProjectManager* _projectManager;

    /*!
     * \brief _threadManager reference to the ThreadManager object to handle asynchronous tasks.
     */
    ThreadManager* _threadManager;

    /*!
     * \brief _windowManager reference to the WindowManager to handle GUI-related functionality.
     */
    WindowManager* _windowManager;

    // video metadata, updated every time a video is added.
    int _numberOfFrames;
    int _frameRate;
    int _frameWidth;
    int _frameHeight;
    int _threshold; //added by Molly

    // Temporary variables that hold data to pass back and forth.
    bool _overwrite;
    QString _analyzeProjectName;
    QString _analyzeVideoName;
    QString _analyzeExperimentName;
    QImage* _imageOfVideo;

    //End Managers
};
#endif // !defined(EA_5F0F50FF_3F91_4456_ABAC_879718D75C66__INCLUDED_)
