#include "WindowManager.h"

/*!
 * \brief WindowManager::WindowManager constructor takes a reference to the system to pass back requests.  It instantiates
 * and displays the MainWindow, and also instantiates the other windows (with the exception of RegionWindow, which is
 * created when the user selects a region.)  The constructor also calls checkWorkspace(), which makes sure that the user
 * has input a valid workspace directory, and, if not, displays a prompt allowing them to do so.  (this usually runs at the
 * start of the application.)
 *
 * \param bvSystem a reference to the system to allow passing requests to it.
 */
WindowManager::WindowManager(BvSystem *bvSystem)
{
    //Setup pointer to BvSystem
    _bvSystem = bvSystem;

    //Setup and show MainWindow
    _mainWindow = new MainWindow(0, this);
    _mainWindow->show();

    //Setup other windows
    _aboutWindow = new AboutWindow;
    _optionsWindow = new OptionsWindow(this);
    _analyzeCheckDialog = new AnalyzeCheckDialog(this);
    _enlargedFrameWindow = new EnlargedFrameWindow;

    // Don't setup region window-that is instantiated when the user selects a region.

    // This auto loads all projects if the workspace exists.
    checkWorkspace();
}

/*!
 * \brief WindowManager::~WindowManager destructor deletes references to windows.
 */
WindowManager::~WindowManager()
{
    delete _aboutWindow;
    delete _optionsWindow;
    delete _analyzeCheckDialog;
    delete _enlargedFrameWindow;
}

/*!
 * \brief WindowManager::checkWorkspace Asks the system to see if a workspace exists.
 *
 * If so, then auto-load all projects in the workspace.
 * If not- then prompt the user to set a workspace and set that workspace.
 */
void WindowManager::checkWorkspace()
{
    //If there is no workspace (first time load) go through the procedure to get it.
    if(getWorkspace()=="")
    {
        // Welcome the user.
        QMessageBox welcomeMsg;
        welcomeMsg.setText("Welcome to BioVision!");
        welcomeMsg.setInformativeText("The first step in using BioVision is to select a workspace directory.  This is where all of your projects and data will be stored.  Begin selection by clicking the ok button.  After you have done that, you can make a project and begin adding videos by clicking File -> New Project.");
        welcomeMsg.setStandardButtons(QMessageBox::Ok);
        welcomeMsg.exec();

        QString workspacePath = QFileDialog::getExistingDirectory(_mainWindow, "Please specify a workspace directory.", QDir::home().path());
        if(workspacePath != QString())
        {
            // Set the workspace.
            _bvSystem->setWorkspace(workspacePath);
        }
        else
        {
            QMessageBox errorMsg;
            errorMsg.setText("If you do not select a workspace you will be unable to create new projects.");
            errorMsg.setInformativeText("Set your workspace through the Options -> BioVision Settings menu.");
            errorMsg.setStandardButtons(QMessageBox::Ok);
            errorMsg.exec();
        }
    }
    // Otherwise, auto load all projects in the workspace.
    else
    {
        _bvSystem->autoLoadProjects();
        _mainWindow->refreshProjectBrowser();
    }
}

/*!
 * \brief WindowManager::getWorkspace calls to system to retrieve the workspace.
 *
 * \return the workspace.
 */
QString WindowManager::getWorkspace()
{
    return _bvSystem->getWorkspace();
}

/*!
 * \brief WindowManager::saveOptions Calls to system to persist all of the options that the user specified in the
 * options window.  Right now this is only the workspace, but more may be added later.
 *
 * \param newWorkspacePath The new workspace directory to be saved.
 */
void WindowManager::saveOptions(QString newWorkspacePath)
{
    _bvSystem->setWorkspace(newWorkspacePath);
}

/*!
 * \brief WindowManager::getAllProjects gets all of the projects from BvSystem- returns them to MainWindow to display.
 *
 * \return the vector of projects.
 */
std::vector<Project*> WindowManager::getAllProjects()
{
    return _bvSystem->getAllProjects();
}

/* REGION FUNCTIONALITY */

/*!
 * \brief WindowManager::launchRegionWindow is called when the user has dragged a region across the screen.
 * This function launches the window allowing the user to see the region they have created,
 * and to specify options for that region (name, threshold, notes).  The user may only save a region IF that region has
 * a name.
 *
 * This function also displays an image of the current frame with the region the user selected drawn on it.
 * See the function saveFrameWhenRegionCreated for a description of that.
 *
 * \see RegionWindow.
 *
 * \param projName The project associated.
 * \param vidName The video we are adding a region to.
 * \param regionName The name of the region (if this is an edit/update).
 * \param x The x of the region (pixels)
 * \param y y of the region (pixels)
 * \param width width of the region (pixels)
 * \param height height of the region (pixels)
 */
void WindowManager::launchRegionWindow(QString projName, QString vidName, QString regionName, int videoTimeInMilliseconds, int newRegionNumber, int x, int y, int width, int height)
{
    ///////////Dustin Added: code that saves an image file calling functions i made, and returns its filepath as a string
    QString videoFilePath = getVideoPath(projName, vidName);

    //save an image to display when the user makes a region, and have openCV draw the region onto the image
    std::string tempPath = saveFrameWhenRegionCreated(videoFilePath, videoTimeInMilliseconds, x, y, width, height, newRegionNumber);

    QString imageFilePath = QString::fromStdString(tempPath);
    ///////////Dustin Added: code that saves an image file calling functions i made, and returns its filepath as a string

    int threshold = _bvSystem->getRegionThreshold(projName, vidName, regionName);
    QString notes = _bvSystem->getRegionNotes(projName, vidName, regionName);
    _regionWindow = new RegionWindow(0, this, projName, vidName, regionName, threshold, notes, x, y, width, height, imageFilePath);
    _regionWindow->setAttribute(Qt::WA_DeleteOnClose);
    _regionWindow->show();
}

/*!
 * \brief WindowManager::deleteRegion removes a region from the project browser by calling back to system.
 *
 * \param projName name of the project the region belongs to.
 * \param vidName name of the video the region is a part of.
 * \param regionName The name of the region itself.
 */
void WindowManager::deleteRegion(QString projName, QString vidName, QString regionName)
{
    _bvSystem->removeRegion(projName, vidName, regionName);
}

/*!
 * \brief WindowManager::setRegion This function is called from RegionWindow's save button.  This function sends the request
 * to edit/add a region to the system.  It handles associated errors as well (cannot have more than 10 regions, or if
 * the region's name already exists).  Calls to system to actually attempt adding the region.
 *
 * \param projName
 * \param vidName
 * \param oldName The oldName of the region (this will be "" if this is a new region).
 * \param newName The newName of the region (the name the user entered).
 * \param threshold individual threshold setting for this region.
 * \param notes notes for this region.
 * \param x
 * \param y
 * \param width
 * \param height
 */
void WindowManager::setRegion(QString projName, QString vidName, QString oldName, QString newName, int threshold, QString notes, int x, int y, int width, int height)
{
    // if there are less than 10 regions OR if this is an update, we can go ahead and try to add.
    if(_bvSystem->checkNumberOfRegions(projName, vidName) || oldName!="")
    {
        // A problem occurred...  Display an error message and do not close the region window.
        if(!_bvSystem->setRegion(projName, vidName, oldName, newName, threshold, notes, x, y, width, height))
        {
            QMessageBox errorMsg;
            errorMsg.setText("This region's name already exists.");
            errorMsg.setInformativeText("Select a different name.");
            errorMsg.exec();
        }
        // Successful!  Refresh project browser and close the region window.
        else
        {
            _mainWindow->refreshProjectBrowser();
            _regionWindow->close();
        }
    }
    else
    {
        QMessageBox errorMsg;
        errorMsg.setText("You can only have 10 regions per video.");
        errorMsg.setInformativeText("Remove a region in order to add another one.");
        errorMsg.exec();
    }
}

/*!
 * \brief WindowManager::launchAboutWindow displays the about window when the user requests it.  Called from MainWindow.
 */
void WindowManager::launchAboutWindow()
{
    _aboutWindow->show();
}

/*!
 * \brief WindowManager::launchOptionsWindow displays the options Window when the user requests it.  Called from MainWindow.
 */
void WindowManager::launchOptionsWindow()
{
    _optionsWindow->setWindowTitle("Options");
    _optionsWindow->show();
}

/*!
 * \brief WindowManager::launchEnlargedFrameWindow Displays an image located at the filename parameter.  Used for showing
 * bigger versions of the images in the carousel when the user double clicks on one.
 *
 * \param fileName the file name of the image to display.
 */
void WindowManager::launchEnlargedFrameWindow(QString fileName)
{
    _enlargedFrameWindow->displayFrame(fileName);
    _enlargedFrameWindow->show();
}

/*!
 * \brief WindowManager::updateProgress updates the progress of the progress bar to the value of the progress parameter.
 *
 * \param progress a measure of the current progress of the task that we should update the progress bar to.
 */
void WindowManager::updateProgress(int progress)
{
    _mainWindow->setProgress(progress);
}

/*!
 * WindowManager::sendAnalyzeRequest passes a request for analyzing a video to the system, where it can be handled.
 * If the system returns a message (anything other than the empty string), this indicates that a task is already in progress,
 * so display a message notifying the user that they cannot analyze at this time.
 *
 */
void WindowManager::sendAnalyzeRequest(QString projName, QString vidName, int startSec, int endSec, std::deque<int> videoEditTimesInSeconds, int motionSensitivity, bool isPreviewSelected,
                                       int previewSpeed, int previewSize, int imageOutputSize, bool isOutputImages, bool isFullFrameAnalysis)
{
     //_mainWindow->clearCarousel();
    QString message = _bvSystem->sendAnalyzeRequest(projName, vidName, startSec, endSec, videoEditTimesInSeconds, motionSensitivity, isPreviewSelected, previewSpeed, previewSize, imageOutputSize, isOutputImages, isFullFrameAnalysis);
    if(!message.isEmpty())
    {
        QMessageBox errorMsg;
        errorMsg.setStandardButtons(QMessageBox::Ok);
        errorMsg.setText(message);
        errorMsg.exec();
    }
}

/*!
 * \brief WindowManager::sendVideoCopyRequest sends a request for a video to be copied to the workspace directory.
 * Like sendAnalyzeRequest, if the system returns a message, then the request has failed (another task is running)
 * so it displays that message to alert the user of that.
 *
 * \param projName The name of the project that the video belongs to.
 * \param videoPath the path of the video that we want to copy.
 * \param vidName The name of the video.
 */
void WindowManager::sendVideoCopyRequest(QString projName, QString videoPath, QString vidName)
{
    QString message = _bvSystem->sendVideoCopyRequest(projName, videoPath, vidName);

    if(!message.isEmpty())
    {
        QMessageBox errorMsg;
        errorMsg.setStandardButtons(QMessageBox::Ok);
        errorMsg.setText(message);
        errorMsg.exec();
    }
}

/*!
 * \brief WindowManager::updateCarousel send the imageName to MainWindow to display it.
 *
 * \param imageName the name of the image that has been written to file.
 */
void WindowManager::updateCarousel(QString imageName, QString imageIndex)
{
    _mainWindow->addImageToCarousel(imageName, imageIndex);
}

/*!
 * \brief WindowManager::cancelTask tells the system to cancel the current task.  Called from MainWindow.
 */
void WindowManager::cancelTask()
{
    _bvSystem->cancelTask();
}

/*!
 * \brief WindowManager::displayVidCopyError Launches a QMessageBox that displays an error notifying the user that
 * the video was not able to be copied.
 */
void WindowManager::displayVidCopyError()
{
    QMessageBox errorMsg;
    errorMsg.setText("Error copying video to the workspace.");
    errorMsg.setInformativeText("Is your project located in the workspace?  Or is this video already located in the workspace?");
    errorMsg.exec();
}

/*!
 * \brief WindowManager::launchAnalyzeCheckDialog prompts the user to enter an experiment name, and also notifies them
 * of all the settings that they have selected to run analyze with.
 *
 * \param videoName The name of the video that is going to be analyzed.
 * \param detailedText The details of analysis to display.
 * \return true if the user has authorized the analyze and input a run name.  False otherwise.
 */
bool WindowManager::launchAnalyzeCheckDialog(QString projName, QString videoName, QString detailedText)
{
    _analyzeCheckDialog->setAnalyzeInfo(projName, videoName, detailedText); //passes the data to display

    _analyzeCheckDialog->setWindowTitle("Analyze?");

    return (_analyzeCheckDialog->exec());
}

/*!
 * \brief WindowManager::setOverwriteOldExperiment Sets a boolean value that determines whether or not the user would like
 * to overwrite their old experiment data.
 *
 * \param overwrite the boolean value- true if overwrite is requested, false otherwise.
 */
void WindowManager::setOverwriteOldExperiment(bool overwrite)
{
    _bvSystem->setOverwriteOldExperiment(overwrite);
}

/*!
 * \brief WindowManager::launchAnalyzeFinishedDialog is called from BvSystem when it catches a signal that the analysis has
 * completed.  This function asks the user if they would like to save the images from the run that has just ended.  As
 * the images can often be of large quantity and/or size, it is important that the user be given control of this process.
 *
 * \param size the total size of the images to be copied (in readable format) estimated by a ProjectManager function.
 */
bool WindowManager::launchAnalyzeFinishedDialog(QString size)
{
    // prompt the user to save the images here.
    QMessageBox copyImagesPrompt;
    copyImagesPrompt.setText("Analysis has completed.  Would you like to save the images from this run in your workspace directory?");
    copyImagesPrompt.setInformativeText("Note that this will use approximately " + size + " of hard drive space.");
    copyImagesPrompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    copyImagesPrompt.setDefaultButton(QMessageBox::No);

    int ret = copyImagesPrompt.exec();

    switch(ret)
    {
        case QMessageBox::Yes:
            return true;
        case QMessageBox::No:
            return false;
        default:
            return false;
    }
}

/*!
 * \brief WindowManager::saveExperimentData asks system to saves the value of the run name
 *
 * \param experimentName the name the user input to save this run under.
 *
 * \return true if successful, false if the experiment name already exists.
 */
bool WindowManager::saveExperimentData(QString projName, QString vidName, QString experimentName)
{
    return (_bvSystem->checkExperimentName(projName, vidName, experimentName));
}

/*!
 * \brief WindowManager::saveProject asks the system to save a project.  Called from MainWindow.
 *
 * \param projName the name of the project to save.
 */
void WindowManager::saveProject(QString projName)
{
    _bvSystem->saveProject(projName);
}

/*!
 * \brief WindowManager::saveAsProject requests the system to save a project in the user's desired location.
 *
 * \param projName the name of the project to save.
 *
 * \param projPath the path to save it in.
 */
void WindowManager::saveAsProject(QString projName, QString projPath)
{
    _bvSystem->saveAsProject(projName, projPath);
}

/*!
 * \brief WindowManager::saveAllProjects Asks the system to save all projects.  Called from MainWindow.
 */
void WindowManager::saveAllProjects()
{
    _bvSystem->saveAllProjects();
}

/*!
 * \brief WindowManager::makeNewProject passes a request for a new project to the system for creation.
 *
 * \param projName The name of the new project.
 */
bool WindowManager::makeNewProject(QString projName)
{
    return (_bvSystem->makeNewProject(projName));
}

/*!
 * \brief WindowManager::hideProject Passes a request to hide a given project to the system.  Called from MainWindow.
 *
 * \param projName The project to be hidden.
 */
void WindowManager::hideProject(QString projName)
{
    _bvSystem->hideProject(projName);

    _mainWindow->refreshProjectBrowser();
}

/*!
 * \brief WindowManager::removeProject sends a request to BvSystem to remove the project.  Called from MainWindow.
 *
 *  also refreshes project browser when done, to show the new state of the browser.
 *
 * \param projName the name of the project to be removed.
 */
bool WindowManager::removeProject(QString projName)
{
    bool result = _bvSystem->removeProject(projName);

    _mainWindow->refreshProjectBrowser();

    return(result);
}

/*!
 * \brief WindowManager::promptSaveProjects prompts the user if they want to save their projects- called from MainWindow's
 * closeEvent.  If they do, then call to system to save the projects.
 */
void WindowManager::promptSaveProjects()
{
    QMessageBox prompt;
    prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    prompt.setText("Would you like to save your projects?");
    prompt.setInformativeText("Otherwise unsaved changes will be lost.");
    int ret = prompt.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
            // We want to save.
            _bvSystem->saveAllProjects();
        break;

        case QMessageBox::No:
            // User does not want to save- do nothing.
        break;

        default:
            // should never be reached
        break;
    }
}

/*!
 * \brief WindowManager::displayErrorWindow displays an error message that is the result of an error occuring during the
 * analysis process (either preview or regular).  It alerts the user that something is wrong.  Usually this is the result of
 * a video that has been poorly encoded, or some other OpenCV error that cannot always be predicted.  Re-encoding the file
 * using VLC solves most instances of the issue.
 */
void WindowManager::displayErrorWindow()
{
    QMessageBox errorMsg;
    errorMsg.setText("Video Analysis Error");
    errorMsg.setInformativeText("BioVision could not process the currently selected video file. Please try to re-encode this video file to a more commonly supported codec (such as H.264 or MPEG4) to analyze this video file.");
    errorMsg.exec();
}

/*!
 * \brief WindowManager::addVideo requests to system to add a video.  called from MainWindow.
 *
 * \param projName project name to add the video to.
 * \param vidPath the file path to the video.
 * \param vidName the name of the video.
 *
 * \return true if video was successfully added, false otherwise.
 */
bool WindowManager::addVideo(QString projName, QString vidPath, QString vidName)
{
    return(_bvSystem->addVideoToProject(projName, vidPath, vidName));
}

/*!
 * \brief WindowManager::removeVideo requests system to remove a video.  called from MainWindow.
 *
 * \param projName the name of the project containing the video.
 * \param vidName the name of the video.
 */
void WindowManager::removeVideo(QString projName, QString vidName)
{
    _bvSystem->removeVideoFromProject(projName, vidName);
}

/*!
 * \brief WindowManager::saveFrameWhenRegionCreated passes a request to system to save a frame from the video at videoFilePath,
 * at time videoTimeInMilliseconds, and draws a rectangle onto the image pased on passes in data
 *
 * \param videoFilePath: Video to save image from.
 * \param videoTimeInMilliseconds: Time to save the image at.
 * \param frameX1: The x1 coordinate of the rectangle to draw
 * \param frameY1: The y1 coordinate of the rectangle to draw
 * \param frameWidth: The width of the rectangle to draw
 * \param frameHeight: The height of the rectangle to draw
 * \param regionNumber: The number of the region, used to determine what color rectangle to draw
 *
 * \return Returns the filename/path of the image.
 */
std::string WindowManager::saveFrameWhenRegionCreated(QString videoFilePath, int videoTimeInMilliseconds, int frameX1, int frameY1, int frameWidth, int frameHeight, int regionNumber)
{
    return _bvSystem->saveFrameWhenRegionCreated(videoFilePath, videoTimeInMilliseconds, frameX1, frameY1, frameWidth, frameHeight, regionNumber);
}

/*!
 * \brief WindowManager::getVideoPath returns the file path to the video vidName that is part of the project projName.
 * \param projName is the name of the project.
 * \param vidName is the name of the video.
 * \return the file path to the video.
 */
QString WindowManager::getVideoPath(QString projName, QString vidName)
{
    return _bvSystem->getVideoPath(projName, vidName);
}

/*!
 * \brief WindowManager::loadProject asks system to load a project. Called from MainWindow.
 *
 * \param filePath the path to the project.
 */
void WindowManager::loadProject(QString filePath)
{
    _bvSystem->openProject(filePath);
}

/*!
 * \brief WindowManager::addVideoEditTime calls to system to add a video edit time to be saved.
 *
 * \param projName name of the project
 * \param vidName name of the video
 * \param newEditTime the edit time (in QTime)
 */
void WindowManager::addVideoEditTime(QString projName, QString vidName, QTime newEditTime)
{
    _bvSystem->addVideoEditTime(projName, vidName, newEditTime);
}

/*!
 * \brief WindowManager::removeLastVideoEditTime calls to system to remove a video edit time.
 *
 * \param projName project name
 *
 * \param vidName video name that the edit time is in.
 */
void WindowManager::removeLastVideoEditTime(QString projName, QString vidName)
{
    _bvSystem->removeLastVideoEditTime(projName, vidName);
}

/*!
 * \brief WindowManager::getVideoWidth returns the width in pixels of the video vidName that is part of the project projName.
 * \param projName is the name of the project.
 * \param vidName is the name of the video.
 * \return the width of the video
 */
int WindowManager::getVideoWidth(QString projName, QString vidName)
{
    return _bvSystem->getVideoWidth(projName, vidName);
}

/*!
 * \brief WindowManager::getVideoHeight returns the heigth in pixels of the video vidName that is part of the project projName.
 * \param projName is the name of the project.
 * \param vidName is the name of the video.
 *
 * \return the height of the video.
 */
int WindowManager::getVideoHeight(QString projName, QString vidName)
{
    return _bvSystem->getVideoHeight(projName, vidName);
}

/*!
 * Passes call through the system to set the start and end times of a video in projectManager
 *
 * \see ProjectManager::setVideoStartAndStopTimes for details
 */
void WindowManager::setVideoStartAndStopTimes(QString projName, QString vidName, QTime currentStartTime, QTime currentEndTime)
{
    _bvSystem->setVideoStartAndStopTimes(projName, vidName, currentStartTime, currentEndTime);
}

/*!
 * Passes call through the system to get all edit times of a video in projectManager
 *
 * \see ProjectManager::getAllVideoTimes for details
 */
std::vector <QTime> WindowManager::getAllVideoEditTimes(QString projName, QString vidName)
{
    return _bvSystem->getVideoTimes(projName, vidName);
}

/*!
 * Passes call through the system to get the number of regions in a video in projectManager
 *
 * \see ProjectManager::getNumberOfRegionsInVideo for details
 */
int WindowManager::getNumberOfRegionsInVideo(QString projName, QString vidName)
{
    return _bvSystem->getNumberOfRegionsInVideo(projName, vidName);
}

/*!
 * Passes call through the system to get the data of a specified region in a video in projectManager
 *
 * \see ProjectManager::getRegionDataForRegionWindow for details
 */
std::vector <int> WindowManager::getRegionDataForRegionWindow(QString projName, QString vidName, QString regionName)
{
    return _bvSystem->getRegionDataForRegionWindow(projName, vidName, regionName);
}

/*!
 * Passes call through the system to MainWindow to clear the frame carousel
 *
 * \see MainWindow::clearCarousel for details
 */
void WindowManager::clearCarousel()
{
    //Commented out for final release
    //qDebug() << "WindowManager clearCarousel call";

    _mainWindow->clearCarousel();
}
