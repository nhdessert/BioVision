/*!
 * \class WindowManager is responsible for controlling the different windows and dialogs associated with BioVision.  In
 * a sense, this is the 'parent' class of all of the UI, as it instantiates MainWindow and all the other windows,
 * though MainWindow handles a lot of the individual slots.  MainWindow mostly calls back to WindowManager to perform a
 * given task, and WindowManager calls back to the system.  MainWindow or BvSystem also calls upon WindowManager to display each of the
 * different windows- region window, options window, etc.
 */

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

class MainWindow;
class OptionsWindow;
class RegionWindow;
class AnalyzeCheckDialog;
class EnlargedFrameWindow;

#include <QWidget>
#include <vector>
#include <deque>
#include "BvSystem.h"
#include "RegionWindow.h"
#include "AboutWindow.h"
#include "OptionsWindow.h"
#include "AnalyzeCheckDialog.h"
#include "MainWindow.h"
#include "EnlargedFrameWindow.h"

class WindowManager
{

public:
    //Constructors
    WindowManager(BvSystem *bvSystem = NULL);
	virtual ~WindowManager();

    //Methods
    void checkWorkspace();
    QString getWorkspace();

    // Methods to launch dialogs & windows:
    void launchRegionWindow(QString projName, QString vidName, QString regionName, int videoTimeInMilliseconds, int newRegionNumber, int x=0, int y=0, int width=0, int height=0);
    void launchDocumentationWindow();
    void launchAboutWindow();
    void launchOptionsWindow();
    bool launchAnalyzeCheckDialog(QString projName, QString videoName, QString detailedText);
    bool launchAnalyzeFinishedDialog(QString size);
    void launchEnlargedFrameWindow(QString fileName);

    // Updating Progress
    void setSimpleAnalyzeMax(int max);
    void updateProgress(int progress);

    // Analyze & threading tasks.
    void sendAnalyzeRequest(QString projName, QString vidName, int startSec, int stopSec, std::deque<int> videoEditTimesInSeconds, int motionSensitivity, bool isPreviewSelected,
                            int previewSpeed, int previewSize, int imageOutputSize, bool isOutputImages, bool isFullFrameAnalysis);
    void sendVideoCopyRequest(QString projName, QString videoPath, QString vidName);
    void cancelTask();
    void updateCarousel(QString imageName, QString imageIndex);
    void displayVidCopyError();
    void displayErrorWindow();

    // Projects & Options
    void saveProject(QString projName);
    void saveAsProject(QString projName, QString projPath);
    void saveAllProjects();
    std::vector<Project*> getAllProjects();
    bool addVideo(QString projName, QString vidPath, QString vidName);
    void removeVideo(QString projName, QString vidName);
    bool makeNewProject(QString projName);
    void deleteRegion(QString projName, QString vidName, QString regionName);
    void loadProject(QString filePath);
    void hideProject(QString projName);
    bool removeProject(QString projName);
    void saveOptions(QString workspacePath);
    void setRegion(QString projName, QString vidName, QString oldName, QString newName, int threshold, QString notes, int x=0, int y=0, int width=0, int height=0);
    bool saveExperimentData(QString projName, QString vidName, QString experimentName);
    void setOverwriteOldExperiment(bool overwrite);
    void promptSaveProjects();

    // Region image saving
    std::string saveFrameWhenRegionCreated(QString videoPath, int videoTimeInMilliseconds, int frameX1, int frameY1, int frameWidth, int frameHeight, int regionNumber);

    // Get & sets
    int getVideoWidth( QString projName, QString vidName);
    int getVideoHeight( QString projName, QString vidName);
    QString getVideoPath(QString projName, QString vidName);
    void setVideoStartAndStopTimes(QString projName, QString vidName, QTime currentStartTime, QTime currentEndTime);
    void addVideoEditTime(QString projName, QString vidName, QTime newEditTime);
    void removeLastVideoEditTime(QString projName, QString vidName);
    std::vector <QTime> getAllVideoEditTimes(QString projName, QString vidName);
    int getNumberOfRegionsInVideo(QString projName, QString vidName);
    std::vector <int> getRegionDataForRegionWindow(QString projName, QString vidName, QString regionName);

    //Clear carousel
    void clearCarousel();


private:
    //Properties
    BvSystem *_bvSystem;
    AboutWindow *_aboutWindow;
    MainWindow *_mainWindow;
    OptionsWindow *_optionsWindow;
    RegionWindow *_regionWindow;
    AnalyzeCheckDialog *_analyzeCheckDialog;
    EnlargedFrameWindow *_enlargedFrameWindow;
};
#endif
