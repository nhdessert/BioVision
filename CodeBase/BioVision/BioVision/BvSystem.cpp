///////////////////////////////////////////////////////////
//  BvSystem.cpp
//  Implementation of the Class BvSystem
//  Created on:      15-Jan-2013 5:49:25 PM
///////////////////////////////////////////////////////////

#include "BvSystem.h"
//extern bool _isCancelled;

/*!
 * \brief BvSystem::BvSystem constructs each manager in sequence.
 *
 * \param argc default param for QApplication, passed on to that constructor.
 * \param argv default param for QApplication, passed on to that constructor.
 */
BvSystem::BvSystem(int & argc, char ** argv) : QApplication(argc, argv)
{
    //Construct the manager classes.
    _threadManager = new ThreadManager(this); //Construct ThreadManager
    _projectManager = new ProjectManager(); //Construct ProjectManager

    //WindowManager construction calls getAllProjects
    //WindowManager must be constructed after ProjectManager
    _windowManager = new WindowManager(this); //Construct WindowManager

    _imageOfVideo = new QImage(0,0, QImage::Format_ARGB32);
}

/*!
 * \brief BvSystem::~BvSystem
 */
BvSystem::~BvSystem()
{
}

/*!
 * \brief BvSystem::getWorkspace retrieves the workspace from the project manager.
 *
 * \return A \QString containing the workspace path.
 */
QString BvSystem::getWorkspace()
{
    // return the workspace from projectManager.
    return _projectManager->getWorkspaceDirectory();
}

/*!
 * \brief BvSystem::setWorkspace asks ProjectManager to set a new value as the workspace directory.
 *
 * \param workspacePath the new workspace directory.
 */
void BvSystem::setWorkspace(QString workspacePath)
{
    _projectManager->setWorkspaceDirectory(workspacePath);
}

/*!
 * \brief BvSystem::getAllProjects calls project manager to get all of the projects to display in MainWindow.
 * \return the vector of projects to WindowManager.
 */
std::vector<Project*> BvSystem::getAllProjects()
{
    //Should probably pass a pointer to WindowManager in stead of the whole object.
     return *_projectManager->getAllProjects();
}

Video* BvSystem::getVideo(QString projName, QString vidName)
{
     return _projectManager->getVideo(projName, vidName);
}

std::vector<Video*> BvSystem::getAllVideos(QString projName)
{
    //Should probably pass a pointer to WindowManager in stead of the whole object.
    return *_projectManager->getAllVideos(projName);
}

BvRegion* BvSystem::getRegion(QString projName, QString vidName, QString regionName)
{
     return _projectManager->getRegion(projName, vidName, regionName);
}

std::vector<BvRegion*>* BvSystem::getAllRegions(QString projName, QString vidName)
{
    return _projectManager->getAllRegions(projName, vidName);
}

/*!
 * \brief BvSystem::removeVideoFromProject passes a request to remove a video to ProjectManager.
 *
 * \param projName The project the video is located in.
 * \param vidName The name of the video to remove.
 */
void BvSystem::removeVideoFromProject(QString projName, QString vidName)
{
    //A confirmation bool may be a good idea.
    _projectManager->removeVideoFromProject(projName, vidName);
}

/*!
 * \brief BvSystem::addVideoToProject Adds a video to the given project.  Gets meta data for the video using open cv and
 * passes that data to ProjectManager.
 *
 * \param projName Name of the project to add the video to.
 * \param pathToVideoFile The path to the video that we want to add.
 * \param vidName The video name.
 *
 * \return true if the video was successfully added, false otherwise.
 */
bool BvSystem::addVideoToProject(QString projName, QString pathToVideoFile, QString vidName)
{
    //function to retrieve metadata
    getAllMetaData(pathToVideoFile);

    // initialize threshold to 0
    _threshold = 0;

    return _projectManager->addVideoToProject(projName, pathToVideoFile, vidName, _numberOfFrames, _frameRate, _frameHeight, _frameWidth, _threshold);
}

/*!
 * BvSystem::sendVideoCopyRequest sends a request to the threadmanager to handle copying a video file from one location
 * to another.  ThreadManager posts back via the handleResultSlot to notify the system that the task has been completed.
 * From that slot the file path for the video is updated to the new one.
 *
 * This function obtains the new path of the video using the workspace directory, project name, and video name
 *
 * \param projName the name of the project the video is associated with.
 * \param videoPath The current path to the video.
 * \param vidName the name of the video.
 *
 * \return an error message if it fails, otherwise the empty string.
 */
QString BvSystem::sendVideoCopyRequest(QString projName, QString videoPath, QString vidName)
{
    bool isWindows;
    int lastSlashInPath = videoPath.toStdString().find_last_of('/');

    if(lastSlashInPath != -1)
        isWindows = false;
    else
        isWindows = true;

    QString newPath = _projectManager->getWorkspaceDirectory() + "\\" + projName + "\\" + vidName + "\\" + vidName;

    if(isWindows == false)
        newPath = _projectManager->getWorkspaceDirectory() + "/" + projName + "/" + vidName + "/" + vidName;
    BvThreadWorker *vidCopier = new VideoCopier(projName, videoPath, newPath);

    if(!_threadManager->startThread(vidCopier))
    {
        return _threadManager->getCurrentTaskMessage();
    }
    else
    {
        return "";
    }
}


/*!
 * \brief BvSystem::removeRegion passes a request to ProjectManager to delete the region with the given attributes.
 *
 * \param projName The project the video is located in.
 * \param vidName The video that contains the region.
 * \param regionName The name of the individual region.
 */
void BvSystem::removeRegion(QString projName, QString vidName, QString regionName)
{
    _projectManager->removeRegion(projName, vidName, regionName);
}

/*!
 * \brief BvSystem::saveProject Requests ProjectManager to save the project.
 *
 * \param projName Name of the project to save.
 */
void BvSystem::saveProject(QString projName)
{
    //A confirmation bool may be a good idea.
     _projectManager->saveProject(projName);
}

/*!
 * \brief BvSystem::saveAsProject requests ProjectManager to save a Project in another directory.
 *
 * \param projName Name of the project to save.
 * \param projPath Place to save it.
 */
void BvSystem::saveAsProject(QString projName, QString projPath)
{
   _projectManager->saveProjectAs(projName, projPath);
}

/*!
 * \brief BvSystem::saveAllProjects requests ProjectManager to save all of the projects it has.
 */
void BvSystem::saveAllProjects()
{
     _projectManager->saveAllProjects();
}

/*!
 * \brief BvSystem::openProject requests ProjectManager to open a project at the given filepath.
 *
 * \param filePath location of the .bv file.
 */
void BvSystem::openProject(QString filePath)
{
    _projectManager->openProject(filePath);
}

/*!
 * \brief BvSystem::makeNewProject requests projectManager to make a new Project.
 *
 * \param projName The name of the new project.
 *
 * \return true if project was made (had a unique name) false otherwise.
 */
bool BvSystem::makeNewProject(QString projName)
{
    return(_projectManager->makeNewProject(projName));
}

/*!
 * \brief BvSystem::hideProject tells project manager to remove the project identified by parameter projName from the vector
 * of projects to be auto-loaded.  This will not remove the project, it will just stop it from being auto-loaded when
 * BioVision starts.
 *
 * \param projName the project to be hidden.
 */
void BvSystem::hideProject(QString projName)
{
    _projectManager->projectToDirectory(projName, 0);
}

/*!
 * \brief BvSystem::removeProject calls project manager to remove a directory containing a project.
 * Grabs the workspace to pass the project manager function removeProject the full path of the project.
 * Has to pass the full path because removeProject is a recursive function.
 *
 * \param projName the name of the project to remove.
 *
 * \return true if successful, false otherwise.
 */
bool BvSystem::removeProject(QString projName)
{
    // Set whether or not we are on mac or windows.
    QString projDir;
    QString workspace = _projectManager->getWorkspaceDirectory();
    int lastSlashInPath = workspace.toStdString().find_last_of('/');

    if(lastSlashInPath != -1)
        projDir = workspace + "/" + projName;
    else
        projDir = workspace + "\\" + projName;

    // hide project first to remove it from the projects array.
    _projectManager->projectToDirectory(projName, 0);

    // then remove the project files- pass the path.
    return(_projectManager->removeProject(projDir));
}

/*!
 * \brief BvSystem::autoLoadProjects calls to project manager to open each project that is saved in the workspace.
 */
void BvSystem::autoLoadProjects()
{
    _projectManager->autoLoadProjects();
}

QString BvSystem::getVideoPath(QString projName, QString vidName)
{
    return _projectManager->getVideoPath(projName, vidName);
}

int BvSystem::getRegionThreshold(QString projName, QString vidName, QString regionName)
{
    return _projectManager->getRegionThreshold(projName, vidName, regionName);
}

QString BvSystem::getRegionNotes(QString projName, QString vidName, QString regionName)
{
   return _projectManager->getRegionNotes(projName, vidName, regionName);
}

/*!
 * \brief BvSystem::setRegion Forwards a request to project Manager to set a region with new data (receives this request
 * from window manager, who in turn receives it from the RegionWindow's Save slot).
 *
 * \param projName The name of the project that the region belongs to.
 * \param vidName The name of the video that the region belongs to.
 * \param oldName The old name of the region.
 * \param newName The new name of the region.
 * \param threshold The threshold of the region.
 * \param notes The region notes.
 * \param x The region's x value- will be 0 if this is not a region being added for the first time.
 * \param y The region's y value- will be 0 if not first add
 * \param width The region's width- will be 0 if not first add
 * \param height The region's height- will be 0 if not first add
 */
bool BvSystem::setRegion(QString projName, QString vidName, QString oldName, QString newName, int threshold, QString notes, int x, int y, int width, int height)
{
    // Modify the line below this to fit the new project manager function using all the above parameters.
    return _projectManager->setRegion(projName, vidName, oldName, newName, threshold, notes, x, y, width, height);
}

/*!
 * \brief BvSystem::checkNumberOfRegions calls ProjectManager to check how many regions have currently been added.
 *
 * \param projName The name of the project
 * \param vidName the name of the video the region will be added to.
 * \return true if the size is less than or equal to 10, false otherwise.
 */
bool BvSystem::checkNumberOfRegions(QString projName, QString vidName)
{
    return (_projectManager->checkRegionSize(projName, vidName));
}

std::vector<int>* BvSystem::getAllRegionsXcoords(QString projName, QString vidName)
{
    return _projectManager->getAllRegionsXcoords(projName, vidName);
}

std::vector<int>* BvSystem::getAllRegionsYcoords(QString projName, QString vidName)
{
    return _projectManager->getAllRegionsYcoords(projName, vidName);
}

std::vector<int>* BvSystem::getAllRegionsWidths(QString projName, QString vidName)
{
    return _projectManager->getAllRegionsWidths(projName, vidName);
}

std::vector<int>* BvSystem::getAllRegionsHeights(QString projName, QString vidName)
{
    return _projectManager->getAllRegionsHeights(projName, vidName);
}

std::vector<int>* BvSystem::getAllRegionsThresholds(QString projName, QString vidName)
{
    return _projectManager->getAllRegionsThresholds(projName, vidName);
}

int BvSystem::getVideoWidth(QString projName, QString vidName)
{
    return _projectManager->getVideoWidth(projName,vidName);
}

int BvSystem::getVideoHeight(QString projName, QString vidName)
{
    return _projectManager->getVideoHeight(projName,vidName);

}

/*!
 * Handles a request to begin analyzing a video.
 *
 * Creates an Analyzer object (subclass of BvThread Worker) with the needed region data and then calls
 * the startThread method on ThreadManager and passes it that object.  The manager handles the rest.
 * Parameters are all settings for the analysis.
 *
 * \param projName The name of the project that contains the video to be analyzed.
 * \param vidName The name of the video that will be analyzed.
 * \param startSec
 * \param stopSec
 * \param videoEditTimes
 * \param defaultThreshold
 * \param isPreviewSelected
 * \param previewSpeed
 * \param previewSize
 * \param imageOutputSize
 * \param isOutputImages
 *
 * \return an error string if it fails (thread is already running) empty string otherwise
 */
QString BvSystem::sendAnalyzeRequest(QString projName, QString vidName, int startSec, int stopSec, std::deque<int> videoEditTimesInSeconds, int motionSensitivity, bool isPreviewSelected,
                                  int previewSpeed, int previewSize, int imageOutputSize, bool isOutputImages, bool isFullFrameAnalysis)
{
    // get the region data that we will need to pass to Analyzer.
    std::vector<int>* xCoords = _projectManager->getAllRegionsXcoords(projName, vidName);
    std::vector<int>* yCoords = _projectManager->getAllRegionsYcoords(projName, vidName);
    std::vector<int>* widths = _projectManager->getAllRegionsWidths(projName, vidName);
    std::vector<int>* heights = _projectManager->getAllRegionsHeights(projName, vidName);
    std::vector<int>* thresholds = _projectManager->getAllRegionsThresholds(projName, vidName);
    std::vector<QString>* regionNames = _projectManager->getAllRegionNames(projName, vidName);


    QString filePath = _projectManager->getVideoPath(projName, vidName);

    // Check to make sure the video has not been moved or deleted.
    if(!QFile::exists(filePath))
    {
        return "Video cannot be opened.  Was the video file at location '" + filePath + "' moved or deleted?";
    }


    // If preview is not selected, run a standard analyze by creating an instance of the analyzer class.
    if(isPreviewSelected == false)
    {
        BvThreadWorker *analyzer = new Analyzer(xCoords, yCoords, widths, heights, thresholds, filePath, startSec, stopSec, videoEditTimesInSeconds, motionSensitivity, regionNames,
                                                imageOutputSize, isOutputImages, isFullFrameAnalysis);
        //Commented out for final release
        //qDebug() << "starting thread from bvSystem.";

        if(!_threadManager->startThread(analyzer))
        {
            return _threadManager->getCurrentTaskMessage();
        }
        else
            return "";
    }
    // Otherwise, create an instance of the preview analyze class.
    else
    {
        BvThreadWorker *previewAnalyze = new DetailAnalyzer(xCoords, yCoords, widths, heights, thresholds, filePath, startSec, stopSec, videoEditTimesInSeconds, motionSensitivity,
                                                            previewSpeed, previewSize);

        if(!_threadManager->startThread(previewAnalyze))
        {
            return _threadManager->getCurrentTaskMessage();
        }
        else
            return "";
    }
}

/*!
 * \brief cancelTask sets static BvThreadWorker variable _isCancelled to true, signaling any running analysis to stop.
 */
void BvSystem::cancelTask()
{
    BvThreadWorker::_isCancelled = true;
}

/*!
 * \brief clearCarouselSlot calls clearCarousel function in WindowManager to clear the frame carousel of any remaining frames.
 */
void BvSystem::clearCarouselSlot()
{
    //Commented out for final release
    //qDebug() << "BvSystem clearCarousel call";

    _windowManager->clearCarousel();
}


/*!
 *  BvSystem::progressUpdateSlot waits for the thread to send back progress updates.
 *
 *  When the system receives a progress update it passes that to the windowManager to update the progress.
 *
 * \param progress a measure of the progress of an analyze task, currently expressed as an int.
 */
void BvSystem::progressUpdateSlot(int progress)
{
    _windowManager->updateProgress(progress);
}

/*!
 * \brief BvSystem::updateCarouselSlot calls window manager to update the carousel with the newly written image at this
 * filename.
 *
 * \param imageName the name of the image to pass.
 */
void BvSystem::updateCarouselSlot(QString imageName, QString index)
{
    _windowManager->updateCarousel(imageName, index);
}

/*!
 * \brief BvSystem::displayErrorWindow() calls window manager to display an error window when an error is thrown during an analysis
 *
 */
void BvSystem::displayErrorWindowSlot()
{
    _windowManager->displayErrorWindow();
}

/*!
 * BvSystem::handleResultSlot is called when the thread is done.  It is passed a pointer to a Result object
 * that contains the data gathered from the threading call.
 *
 * \param result the result object.  \see Result
 */
void BvSystem::handleResultSlot(Result* result)
{   
    // If the data field is set, then it was a video copy request.
    if(result->getData()!="")
    {
        // Check if the request passed or failed.
        if(result->getData()=="error")
        {
            // display an error message to the user.
            _windowManager->displayVidCopyError();
        }
        // otherwise update the path of the video to the new path.
        else
        {
            QString path = result->getData();
            QString vidName = path.mid(path.lastIndexOf('/')+1, path.size() - path.lastIndexOf('/') - 1);

            if(!_projectManager->setVideoPath(result->getProject(), vidName, path))
            {
                _windowManager->displayVidCopyError();
            }
            else//do nothing
            {
                //Commented out for final release
                //qDebug() << "congrats you copied the video!";
            }
        }
    }
    // this is an Analyze finished request.  Prompt the user on further actions.
    else
    {
        // get the approximate size of the images that were generated by this run.
        QString size = _projectManager->getSizeOfImages();

        // Prompt to save the images from this run.  If so, pass true for that boolean value.
        if(_windowManager->launchAnalyzeFinishedDialog(size))
        {
            _projectManager->outputResults(_analyzeProjectName, _analyzeVideoName, _analyzeExperimentName, _overwrite, true);
        }
        // otherwise, pass false.
        else
        {
            _projectManager->outputResults(_analyzeProjectName, _analyzeVideoName, _analyzeExperimentName, _overwrite, false);
        }
    }
}

/*!
 * \brief BvSystem::setOverwriteOldExperiment If an experiment name already exists, then we prompt the user if they want
 * to overwrite that experiment data.  This function sets the result of that prompt, and then this value is passed to the
 * projectManager->outputResults function after an analyze has finished.
 *
 * \param overwrite true if old data exists and we want to overwrite it, false otherwise.
 */
void BvSystem::setOverwriteOldExperiment(bool overwrite)
{
    _overwrite = overwrite;
}

/*!
 * \brief BvSystem::checkExperimentName checks an experiment name to see if it exist or not.
 *
 * IMPORTANT NOTE: It also saves the parameters projName, vidName, and runName to class variables in BvSystem so
 * that later BvSystem has access to them to output the results of analyze to the right directory.
 *
 * \param projName the project name that holds the video to be analyzed.
 * \param vidName The name of the video to be analyzed.
 * \param runName The run name the user has just input that is going to be validated.
 *
 * \return True if the run name is good (does not exist) false if it does exist.
 */
bool BvSystem::checkExperimentName(QString projName, QString vidName, QString runName)
{
    // Save the data to be used later.
    _analyzeProjectName = projName;
    _analyzeVideoName = vidName;
    _analyzeExperimentName = runName;

    return (_projectManager->checkForRun(projName, vidName, runName));
}

/*!
 * \brief BvSystem::getAllMetaData gets all of the metadata from a video and saves it in class level variables.
 *
 * \param videoFilePath
 */
void BvSystem::getAllMetaData(QString videoFilePath)
{
    OpenCV openCV = OpenCV();
    openCV.openVideoFile(videoFilePath.toStdString());
    _numberOfFrames = openCV.OpenCV::getNumberOfVideoFrames();
    _frameRate = openCV.getVideoFrameRate();
    _frameWidth = openCV.getVideoFrameWidth();
    _frameHeight = openCV.getVideoFrameHeight();
    openCV.closeVideoFile();
}


/*!
 * Saves an image from the current video when a region is created, draws the newly created region onto it,
 * and passes the path of that image back to the GUI so it can be displayed in a RegionWindow
 *
 * \param videoFilePath: The path to the video file we are saving an image from
 * \param videoTimeInMilliseconds: The current time of the video, obtained from the video player in the GUI
 * \param frameX1: The x1 coordinate of the region to draw
 * \param frameY1: The y1 coordinate of the region to draw
 * \param frameWidth: The width of the region to draw
 * \param frameHeight: The height of the region to draw
 * \param regionNumber:  The number of the region to draw, used to determine the color for the drawn region
 * \return Returns the file path of the newly created image
 */
std::string BvSystem::saveFrameWhenRegionCreated(QString videoFilePath, int videoTimeInMilliseconds, int frameX1, int frameY1, int frameWidth, int frameHeight, int regionNumber)
{
    OpenCV openCV = OpenCV();

    //open the video file, and set all data for drawing onto a frame
    openCV.openVideoFile(videoFilePath.toStdString());
    openCV.initializeFrameSizeSensitivityAndDrawSize(0.0);

    openCV.setCurrentVideoTime((double)videoTimeInMilliseconds);
    cv::Mat frameToSave;
    openCV.getFrameForAnalysis(frameToSave);

    std::string savedImageFilePath;
    openCV.drawRegionRectangle(frameX1, frameY1, (frameWidth + frameX1), (frameHeight + frameY1), regionNumber, frameToSave);


    #if defined WIN32
    savedImageFilePath = "tmp\\tempRegionCreationFrame.jpg";
    if( cv::imwrite(savedImageFilePath, frameToSave) )
    {
        //Commented out for final release
        //qDebug() << "Writing Image worked on first try.";
    }
    else
    {
        savedImageFilePath = "/tmp/tempRegionCreationFrame.jpg";
        if(cv::imwrite(savedImageFilePath,frameToSave) )
        {
            //Commented out for final release
            //qDebug() << "Writing Image worked on second try.";
        }
        else
        {
            //Commented out for final release
            //qDebug () << "Writing Image didn't work.";

        }
    }

    #else
    savedImageFilePath = "tmp/tempRegionCreationFrame.jpg";
    cv::imwrite(savedImageFilePath, frameToSave);
    #endif

    return savedImageFilePath;
}

/*!
 * Sets the start and end time for a video analysis in ProjectManager when they are changed by the user in the GUI
 *
 * \param projName: The name of the project that contains the current video
 * \param vidName: The name of the current video
 * \param currentStartTime: The start time currently selected by the user
 * \param currentEndTime: The end time currently selected by the user
 */
void BvSystem::setVideoStartAndStopTimes(QString projName, QString vidName, QTime currentStartTime, QTime currentEndTime)
{
    _projectManager->setVideoStartAndStopTimes(projName, vidName, currentStartTime, currentEndTime);
}

/*!
 * Adds a new video edit point in ProjectManager when an edit point is added be the user in the GUI
 *
 * \param projName: The name of the project that contains the current video
 * \param vidName: The name of the current video
 * \param newEditTime: The edit time added by the user
 */
void BvSystem::addVideoEditTime(QString projName, QString vidName, QTime newEditTime)
{
    _projectManager->addVideoEditTime(projName, vidName, newEditTime);
}

/*!
 * Tells ProjectManager to remove the last video edit point in ProjectManager that was added to the current video by the user
 *
 * \param projName: The name of the project that contains the current video
 * \param vidName: The name of the current video
 */
void BvSystem::removeLastVideoEditTime(QString projName, QString vidName)
{
    _projectManager->removeLastVideoEditTime(projName, vidName);
}

std::vector<QTime> BvSystem::getVideoTimes(QString projName, QString vidName)
{
    return _projectManager->getAllVideoTimes(projName, vidName);
}

int BvSystem::getNumberOfRegionsInVideo(QString projName, QString vidName)
{
    return _projectManager->getNumberOfRegionsInVideo(projName, vidName);
}

std::vector <int> BvSystem::getRegionDataForRegionWindow(QString projName, QString vidName, QString regionName)
{
    return _projectManager->getRegionDataForRegionWindow(projName, vidName, regionName);
}

