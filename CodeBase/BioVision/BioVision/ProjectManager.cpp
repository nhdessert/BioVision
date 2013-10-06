#include "ProjectManager.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

/*!
 * Default Constructor
 */
ProjectManager::ProjectManager()
{
}

/*!
 * Default Destructor
 */
ProjectManager::~ProjectManager()
{
}

/*!
 * ProjectManager::getProject retrieves a specified project from the project vector.
 *
 * ProjectManager::getProject uses the parameter projName to iterate through the project vector to find the Project object corresponding to the specified project name.
 *
 * \param projName QString that contains the specified project name.
 * \return A pointer to the Project object for the specified project.
 */
Project* ProjectManager::getProject(QString projName)
{

    _projectIt = _projects.begin();

    while(_projectIt != _projects.end())
    {
        if((*_projectIt)->_projectName == projName)
               return *_projectIt;
        _projectIt++;
    }
    return *_projects.begin();
}

/*!
 * ProjectManager::getAllProjects retrieves the entire project vector.
 *
 * ProjectManager::getAllProjects retrieves a pointer to the entire project vector when called.
 *
 * \return A pointer to the Project vector.
 */
std::vector<Project*>* ProjectManager::getAllProjects()
{
    return &_projects;
}

/*!
 * ProjectManager::getVideo retrieves a specified video from the video vector for a specific project.
 *
 * ProjectManager::getVideo uses the parameter projName to iterate through the project vector to find the Project object corresponding to the specified project name.  It then uses parameter vidName to iterate through the video vector for that project to find the Video object corresponding to the specified video name.
 *
 * \param projName QString that contains the specified project name.
 * \param vidName QString that contains the specified video name in a specific project.
 * \return A pointer to the Video object for the specified video in a specific project.
 */
Video* ProjectManager::getVideo(QString projName, QString vidName)
{
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
            return *videoIt;
        videoIt++;
    }
    return *currentProject->_listOfVideos.begin();
}

/*!
 * ProjectManager::getAllVideos retrieves the entire video vector for a specific project.
 *
 * ProjectManager::getAllVideos retrieves a pointer to the entire video vector for the specified project when called.
 *
 * \param projName QString that contains the specified project name.
 * \return A pointer to the Video vector for a specific project.
 */
std::vector<Video*>* ProjectManager::getAllVideos(QString projName)
{
    Project* currentProject = getProject(projName);
    return &(currentProject->_listOfVideos);
}

/*!
 * ProjectManager::getRegion retrieves a specified region from the region vector for a specific video in a specific project.
 *
 * ProjectManager::getRegion uses the parameter projName to iterate through the project vector to find the Project object corresponding to the specified project name.  It then uses parameter vidName to iterate through the video vector for that project to find the Video object corresponding to the specified video name.  Finally, it uses the parameter regionName to iterate through the region vector for that video to find the region object corresponding to the specified region name.
 *
 * \param projName QString that contains the specified project name.
 * \param vidName QString that contains the specified video name in a specific project.
 * \param regionName QString that contains the specified region name for a specific video in a specific project.
 * \return A pointer to the Region object for the specified region for a specific video in a specific project.
 */
BvRegion* ProjectManager::getRegion(QString projName, QString vidName, QString regionName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();

    while(regionIt != currentVideo->_listOfRegions.end())
    {
        if((*regionIt)->_name == regionName)
            return *regionIt;
        regionIt++;
    }
    return *currentVideo->_listOfRegions.begin();
}

/*!
 * ProjectManager::getAllRegions retrieves the entire region vector for a specific video in a specific project.
 *
 * ProjectManager::getAllRegions retrieves a pointer to the entire region vector for the specified video in a specific project when called.
 *
 * \param projName QString that contains the specified project name.
 * \param vidName QString that contains the specified video name for a specific project.
 * \return A pointer to the Video vector for a specific project.
 */
std::vector<BvRegion*>* ProjectManager::getAllRegions(QString projName, QString vidName)
{
    Project* currentProject = getProject(projName);
    Video* currentVideo = getVideo(currentProject->_projectName, vidName);
    return &(currentVideo->_listOfRegions);
}

/*!
 * \brief ProjectManager::checkRegionSize Checks to see if the size of a list of regions is less than or equal to 10.
 *
 * \param projName The name of a project.
 * \param vidName The name of a video.
 * \return  True if the size is less than or equal to 10, false otherwise.
 */
bool ProjectManager::checkRegionSize(QString projName, QString vidName)
{
    vector<BvRegion*>* regions = getAllRegions(projName, vidName);

    if(regions->size()<10)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*!
 * ProjectManager::addProject adds a new project to the projects vector, if that project has a unique name.
 *
 * Checks to be sure the parameter projName does not match any other project names in the vector, if so, adds the project
 * to the vector.
 *
 * \param projName the name of the project to add.
 *
 * \return True if successful and unique name, false if name already existed.
 *
 * \see makeNewProject(QString projName)
 */
bool ProjectManager::addProject(QString projName)
{
    bool uniqueName = true;
    _projectIt = _projects.begin();

    while(_projectIt != _projects.end())
    {
        if((*_projectIt)->_projectName == projName)
               uniqueName = false;
        _projectIt++;
    }
    if(uniqueName)
    {
        Project* newProject = new Project();
        newProject->_projectName = projName;
        _projects.push_back(newProject);
        projectToDirectory(projName, 1);
    }
    return uniqueName;
}

/*!
 * \brief ProjectManager::makeNewProject Creates a new project and adds it to the list of projects.  Also creates a
 * a directory for that project in the workspace.
 *
 * Implements two checks- checks if a project name already exists, and checks if the directory for that project name
 * already exists.  If the directory exists, it does not add the project and returns false.  If the project name exists,
 * it also returns false- notifying the gui that a problem occurred.
 *
 * \param projName The name of the new project and of the new directory.
 * \return True if operation was successful, false otherwise.
 */
bool ProjectManager::makeNewProject(QString projName)
{
    bool uniqueDir = true;
    bool uniqueName = true;

    // Creating the Directory Structure
    QDir Directory;
    Directory.setPath(_workspace);
    uniqueDir = Directory.mkdir(projName);

    if(uniqueDir)
    {
        // The directory is unique if this is true, so let's go ahead and add the project.
        uniqueName = addProject(projName);

        if(uniqueName)
            saveProject(projName);
    }
     return (uniqueDir && uniqueName);
}

/*!
 * ProjectManager::removeVideoFromProject removes a specified video from a specific project.
 *
 * ProjectManager::removeVideoFromProject uses parameter projName to access the correct Project object, and then uses parameter vidName to find the correct Video object.  It then erases that Video object from the Video vector.
 *
 * \param projName QString containing the name of the project.
 * \param vidName QString containing the name of the video to be deleted from the project.
 */
void ProjectManager::removeVideoFromProject(QString projName, QString vidName)
{
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
        {
            (currentProject->_listOfVideos).erase(videoIt);
            return;
        }
        videoIt++;
    }
}

/*!
 * ProjectManager::setVideoStartAndStopTimes sets the start and stop time variables for a specified video in a specific project.
 *
 * ProjectManager::setVideoStartAndStopTimes sets the start and stop time variables (for when a user wishes to analyze only a portion of a video) by finding the correct video in the specified project.  It sets the start time property and end time property for the video using parameters currentStartTime and currentEndTime.
 *
 * \param projName QString containing the name of the project.
 * \param vidName QString containing the name of the video to be deleted from the project.
 * \param currentStartTime QTime variable containing the user's specified start time for the video.
 * \param currentEndTime QTime variable containing the user's specified end time for the video.
 *
 * \see otherMethod()
 */
void ProjectManager::setVideoStartAndStopTimes(QString projName, QString vidName, QTime currentStartTime, QTime currentEndTime)
{
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
        {
            (*videoIt)->_currentStart = currentStartTime;
            (*videoIt)->_currentEnd = currentEndTime;
        }
        videoIt++;
    }
}

/*!
 * ProjectManager::addVideoEditTime adds a new edit time for a specified video in a specific project.
 *
 * ProjectManager::addVideoEditTime adds a new edit time (for when a user wishes to skip over a portion of the video for analysis) by finding the correct video in the specified project.  It pushes the new edit time (parameter newEditTime) back onto the times vector for the specified video.
 *
 * \param projName QString containing the name of the project.
 * \param vidName QString containing the name of the video to be deleted from the project.
 * \param newEditTime QTime variable containing the new edit time the user wishes to add to the video.
 */

void ProjectManager::addVideoEditTime(QString projName, QString vidName, QTime newEditTime)
{
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
        {
            (*videoIt)->_times.push_back(newEditTime);
        }
        videoIt++;
    }
}

/*!
 * ProjectManager::removeLastVideoEditTime removes the last edit time for a specified video in a specific project.
 *
 * ProjectManager::removeLastVideoEditTime removes the last edit time added (for when a user wishes to skip over a portion of the video for analysis) by finding the correct video in the specified project.  It deletes the last added edit time from the times vector for the specified video.
 *
 * \param projName QString containing the name of the project.
 * \param vidName QString containing the name of the video to be deleted from the project.
 */

void ProjectManager::removeLastVideoEditTime(QString projName, QString vidName)
{
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
        {
            (*videoIt)->_times.pop_back();
        }
        videoIt++;
    }
}

/*!
 * ProjectManager::getAllVideoTimes retrieves the start and stop times times for a specified video in a specific project.
 *
 * ProjectManager::getAllVideoTimes returns a vector containing the start and stop times for a specified video by finding the correct video in the specified project.  It finds the start and stop times and then returns these times in a QTime vector.
 *
 * \param projName QString containing the name of the project.
 * \param vidName QString containing the name of the video in a specific project.
 */
std::vector <QTime> ProjectManager::getAllVideoTimes(QString projName, QString vidName)
{
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
        {
            std::vector <QTime> tempTimes = (*videoIt)->_times;

            QTime tempStartTime = (*videoIt)->_currentStart;
            QTime tempEndTime = (*videoIt)->_currentEnd;

            QString tempStartS = tempStartTime.toString("hh:mm:ss");
            QString tempStartE = tempEndTime.toString("hh:mm:ss");

            if(tempStartS == "")//QTime::fromString("00:00:00", "hh:mm:ss"))
            {
                (*videoIt)->_currentStart = QTime::fromString("00:00:00", "hh:mm:ss");
            }
            if(tempStartE  == "")
            {
                (*videoIt)->_currentEnd = QTime::fromString("00:00:00", "hh:mm:ss");
            }

            tempTimes.push_back((*videoIt)->_currentStart);
            tempTimes.push_back((*videoIt)->_currentEnd);

            tempStartTime = (*videoIt)->_currentStart;
            tempEndTime = (*videoIt)->_currentEnd;

            return tempTimes;
        }
        videoIt++;
    }
}

/*!
 * \brief ProjectManager::addVideoToProject adds a new video to a project.
 *
 * \param projName The name of the project to add the video to.
 * \param pathToVideoFile The file path to the video file.
 * \param vidName The name of the video, parsed from the file path.
 * \param numberOfFrames The number of frames in the video.
 * \param frameRate The frame rate of the video.
 * \param frameHeight The height of the video, expressed in pixels.
 * \param frameWidth The width of the video, expressed in pixels.
 * \param threshold The threshold of the video
 *
 * \return true if successful and the video has a uniquename, false otherwise.
 */
bool ProjectManager::addVideoToProject(QString projName, QString pathToVideoFile, QString vidName, int numberOfFrames, int frameRate, int frameHeight, int frameWidth, int threshold)
{
    bool uniqueName = true;
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
            uniqueName = false;
        videoIt++;
    }
    if(uniqueName)
    {
        Video* newVideo = new Video();
        newVideo->_filePath = pathToVideoFile;
        newVideo->_name = vidName;
        //Begin metadata
        newVideo->_numberOfFramesInVideo = numberOfFrames;
        newVideo->_frameRate = frameRate;
        newVideo->_frameHeight = frameHeight;
        newVideo->_frameWidth = frameWidth;
        newVideo->_threshold = threshold;
        (currentProject->_listOfVideos).push_back(newVideo);

        // Creating the Directory Structure
        QDir Directory;

        if(_isWindows == true)
        {
            Directory.setPath(_workspace + "\\" + projName);
        }

        else
        {
            Directory.setPath(_workspace + "/" + projName);
        }

        Directory.mkdir(vidName);

    }
     return uniqueName;
}


/*!
 * \brief ProjectManager::setRegion constructs a region or updates a region given by parameter oldName.
 *
 * It is called from BvSystem, which is called from WindowManager.  RegionWindow's saveSlot initiates the communication
 * by calling WindowManager and passing the related data.
 *
 * \param projName The project to add the region to.
 * \param vidName The video to add the region to.
 * \param oldName The old name of the region.  This will be "" if a region is added for the first time.
 * \param newName The name the user entered for the region.
 * \param threshold The threshold the user set for the region.
 * \param notes The notes the user set for the region.
 * \param x X coordinate of the region.
 * \param y Y coordinate of the region.
 * \param width Width of the region.
 * \param height Height of the region.
 *
 * \return True if it successfully added or updated the region, false if it was adding a new region and found a duplicate name.
 */
bool ProjectManager::setRegion(QString projName, QString vidName, QString oldName, QString newName, int threshold, QString notes, int x, int y, int width, int height)
{
    notes.replace(QRegExp("\n"), " ");

    // Search for duplicate names.
    bool uniqueName = true;
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();

    while(regionIt != currentVideo->_listOfRegions.end())
    {
        if((*regionIt)->_name == newName)
            uniqueName = false;
        regionIt++;
    }

    // If this is a new region and the name of the region is unique, then add it.
    if((oldName == "") && (uniqueName == true))
    {
        newName.replace("(", "");
        newName.replace(")", " ");
        newName.replace("-", "_");
        newName.replace("/", " ");
        newName.replace("\\", " ");
        BvRegion* newRegion = new BvRegion();
        newRegion-> _name = newName;
        newRegion-> _threshold = threshold;
        newRegion-> _notes = notes;
        newRegion->_x = x;
        newRegion->_y = y;
        newRegion->_width = width;
        newRegion->_height = height;
        (currentVideo->_listOfRegions).push_back(newRegion);
    }
    // Otherwise, if this is a region update, then do it and just return true.
    else if(oldName != "")
    {
        Video* currentVideo = getVideo(projName, vidName);
        std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();

        oldName.replace("(", "");
        oldName.replace(")", "");
        oldName.replace("-", "");
        oldName.replace("/", "");
        oldName.replace("\\", "");

        while(regionIt != currentVideo->_listOfRegions.end())
        {
            if((*regionIt)->_name == oldName)
            {
                (*regionIt)->_name = newName;
                (*regionIt)->_threshold = threshold;
                (*regionIt)->_notes = notes;
            }
            regionIt++;
        }

        return true;
    }

    // Return uniqueName- if it was a new region added successfully, this will be true.  Otherwise it will be false.
    return uniqueName;
}

/*!
 * Brief Description
 *
 * Detailed Description
 *
 * \param x Description on parameter x.
 * \param y Description on parameter y.
 * \return Description on what is being returned.
 *
 * \see otherMethod()
 */
void ProjectManager::removeRegion(QString projName, QString vidName, QString regionName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();

    while(regionIt != currentVideo->_listOfRegions.end())
    {
        if((*regionIt)->_name == regionName)
        {
            (currentVideo->_listOfRegions).erase(regionIt);
            return;
        }
        regionIt++;
    }
}

/*!
 * Brief Description
 *
 * Detailed Description
 *
 * \param x Description on parameter x.
 * \param y Description on parameter y.
 * \return Description on what is being returned.
 *
 * \see otherMethod()
 */
QString ProjectManager::getVideoPath(QString projName, QString vidName)
{
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
            return (*videoIt)->_filePath;
        videoIt++;
    }
    return QString();
}

/*!
 * \brief ProjectManager::setVideoPath is called from BvSystem when a copy video operation has finished.
 * It sets the path of the video to the updated path in the system.  It then saves the project to ensure that
 * the changes will persist- otherwise the user may forget and the copy operation will have been wasted.
 *
 * \param projName The name of the project the video is associated with.
 * \param vidName the name of the video that was copied.
 * \param newPath the path to the new location of the copied video.
 *
 * \return bool whether or not the new path was successfully set and saved.
 */
bool ProjectManager::setVideoPath(QString projName, QString vidName, QString newPath)
{
    bool isReset = false;

    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
        {
            (*videoIt)->_filePath = newPath;
            isReset = true;
        }
        videoIt++;
    }

    // Make sure the changes persist.  Otherwise we copied it for nothing.
    if(isReset)
        saveProject(projName);

    return isReset;
}

/*!
 * Gets the width of a video in a project, and returns it.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video who's width we want to return
 *
 * \return Returns the width of the specified video
 */
int ProjectManager::getVideoWidth(QString projName, QString vidName)
{
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
            return (*videoIt)->_frameWidth;
        videoIt++;
    }
    return 0;
}

/*!
 * Gets the height of a video in a project, and returns it.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video who's height we want to return
 *
 * \return Returns the height of the specified video
 */
int ProjectManager::getVideoHeight(QString projName, QString vidName)
{
    Project* currentProject = getProject(projName);
    std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

    while(videoIt != (currentProject->_listOfVideos).end())
    {
        if((*videoIt)->_name == vidName)
            return (*videoIt)->_frameHeight;
        videoIt++;
    }
    return 0;
}

/*!
 * Gets the threshold of a region in a video and returns it.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video who's region we want to access
 * \param regionName: The name of the region in the video who's threshold we want to return
 *
 * \return Returns the threshold of the specified region
 */
int ProjectManager::getRegionThreshold(QString projName, QString vidName, QString regionName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();

    while(regionIt != currentVideo->_listOfRegions.end())
    {
        if((*regionIt)->_name == regionName)
            return (*regionIt)->_threshold;
        regionIt++;
    }
    return -1;
}

/*!
 * Gets the notes of a region in a video and returns it.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video who's region we want to access
 * \param regionName: The name of the region in the video who's notes we want to return
 *
 * \return Returns the notes of the specified region
 */
QString ProjectManager::getRegionNotes(QString projName, QString vidName, QString regionName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();

    while(regionIt != currentVideo->_listOfRegions.end())
    {
        if((*regionIt)->_name == regionName)
            return (*regionIt)->_notes;
        regionIt++;
    }
    return "";
}

/*!
 * Sets the threshold of a region in a video.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video who's region we want to access
 * \param regionName: The name of the region who's threshold we want to set
 * \param thereshold: The value to set our threshold to
 */
void ProjectManager::setRegionThreshold(QString projName, QString vidName, QString regionName, int threshold)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();

    while(regionIt != currentVideo->_listOfRegions.end())
    {
        if((*regionIt)->_name == regionName)
            (*regionIt)->_threshold = threshold;
        regionIt++;
    }
}

/*!
 * Sets the notes of a region in a video.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video who's region we want to access
 * \param regionName: The name of the region who's notes we want to set
 * \param thereshold: The QString value to set our notes to
 */
void ProjectManager::setRegionNotes(QString projName, QString vidName, QString regionName, QString notes)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();

    while(regionIt != currentVideo->_listOfRegions.end())
    {
        if((*regionIt)->_name == regionName)
            (*regionIt)->_notes = notes;
        regionIt++;
    }
}


/*!
 * Gets all X coordinates of all regions in a video, and return them.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video we want to access
 *
 * \return Returns a pointer to the vector containing all region X coordinates
 */
std::vector<int>* ProjectManager::getAllRegionsXcoords(QString projName, QString vidName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();
    vector<int>* temp = new vector<int>;
    while (regionIt  != (currentVideo->_listOfRegions).end())
    {
        (*temp).push_back((*regionIt)->_x);
        regionIt++;
    }
    return temp;
}

/*!
 * Gets all Y coordinates of all regions in a video, and return them.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video we want to access
 *
 * \return Returns a pointer to the vector containing all region Y coordinates
 */
std::vector<int>* ProjectManager::getAllRegionsYcoords(QString projName, QString vidName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();
    vector<int>* temp = new vector<int>;
    while (regionIt  != (currentVideo->_listOfRegions).end())
    {
        (*temp).push_back((*regionIt)->_y);
        regionIt++;
    }
    return temp;
}

/*!
 * Gets the widths of all regions in a video, and return them.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video we want to access
 *
 * \return Returns a pointer to the vector containing all region widths
 */
std::vector<int>* ProjectManager::getAllRegionsWidths(QString projName, QString vidName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();
    vector<int>* temp = new vector<int>;
    while (regionIt  != (currentVideo->_listOfRegions).end())
    {
        (*temp).push_back((*regionIt)->_width);
        regionIt++;
    }
    return temp;
}

/*!
 * Gets the heights of all regions in a video, and return them.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video we want to access
 *
 * \return Returns a pointer to the vector containing all region heights
 */
std::vector<int>* ProjectManager::getAllRegionsHeights(QString projName, QString vidName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();
    vector<int>* temp = new vector<int>;
    while (regionIt  != (currentVideo->_listOfRegions).end())
    {
        (*temp).push_back((*regionIt)->_height);
        regionIt++;
    }
    return temp;
}

/*!
 * Gets the thresholds of all regions in a video, and return them.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video we want to access
 *
 * \return Returns a pointer to the vector containing all region thresholds
 */
std::vector<int>* ProjectManager::getAllRegionsThresholds(QString projName, QString vidName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();
    vector<int>* temp = new vector<int>;
    while (regionIt  != (currentVideo->_listOfRegions).end())
    {
        (*temp).push_back((*regionIt)->_threshold);
        regionIt++;
    }
    return temp;
}

/*!
 * Gets the names of all regions in a video, and return them.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video we want to access
 *
 * \return Returns a pointer to the vector containing all region names
 */
std::vector<QString>* ProjectManager::getAllRegionNames(QString projName, QString vidName)
{
    Video* currentVideo = getVideo(projName, vidName);
    std::vector<BvRegion*>::iterator regionIt = (currentVideo->_listOfRegions).begin();
    vector<QString>* temp = new vector<QString>;
    while (regionIt  != (currentVideo->_listOfRegions).end())
    {
        (*temp).push_back((*regionIt)->_name);
        regionIt++;
    }
    return temp;
}

/*!
 * Gets the number of regions in a video, and return it.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video we want to access
 *
 * \return Returns the number of regions in the specified video
 */
int ProjectManager::getNumberOfRegionsInVideo(QString projName, QString vidName)
{
    Video* currentVideo = getVideo(projName, vidName);

    int numberOfRegions = currentVideo->_listOfRegions.size();

    return numberOfRegions;
}

/*!
 * Gets all size data for a region in a video, and returns it.
 *
 * \param projName: The name of the project that contains the video
 * \param vidName: The name of the video we want to access
 * \param regionName: The name of the regions who's data we want to access
 *
 * \return Returns all size data for the specified region
 */
std::vector <int> ProjectManager::getRegionDataForRegionWindow(QString projName, QString vidName, QString regionName)
{
    std::vector <int> tempRegionData;

    Video* currentVideo = getVideo(projName, vidName);
    for(int i = 0; i < currentVideo->_listOfRegions.size(); i++)
    {
        if((*currentVideo->_listOfRegions[i])._name == regionName)
        {
            //add region number to data vector
            tempRegionData.push_back(i);

            //add region x to data vector
            tempRegionData.push_back(((*currentVideo->_listOfRegions[i])._x));
            //add region y to data vector
            tempRegionData.push_back(((*currentVideo->_listOfRegions[i])._y));
            //add region width to data vector
            tempRegionData.push_back(((*currentVideo->_listOfRegions[i])._width));
            //add region height to data vector
            tempRegionData.push_back(((*currentVideo->_listOfRegions[i])._height));
            break;
        }
    }

    return tempRegionData;
}

/*!
 * Save project data to the same location as previous .bv file.
 * Goes through all the data needed to reload a project and outputs it all into a new file type.
 */
void ProjectManager::saveProject(QString projName)
{
    ofstream out;

    std::string projNam = _workspace.toStdString() + "\\" + projName.toStdString() + "\\" +projName.toStdString() + ".bv";
    if(_isWindows == false)
        projNam = _workspace.toStdString() + "/" + projName.toStdString() + "/" +projName.toStdString() + ".bv";

    Project* currentProject = getProject(projName);
    if(currentProject->_path.size() > 2)
         projNam = currentProject->_path.toStdString() + ".bv";

    out.open(projNam.c_str());

    // Output MetaData, %$#@! signals beginning and end of metadata.
    QString metadata = "This is random metadata, etc, etc.";
    out<<"%$#@!";
    out<<metadata.toStdString();
    out<<"%$#@!"<<endl;

    // Output Project Name
    out<<projName.toStdString()<<endl;

    // Output Project Path
    out<<currentProject->_path.toStdString()<<endl;

    // Output the number of videos in the project.
    out<<(currentProject->_listOfVideos.size())<<endl;

    // Begin loop to output all of the video data. @@@ signals beginning of video, and $$$ the end
    for(int a = 0; a < (currentProject->_listOfVideos.size()); a++)
    {
        // Pointer to the current video
        Video* tempV = currentProject->_listOfVideos[a];

        // Output video name
        out<<tempV->_name.toStdString()<<endl;

        // Output video path
        out<<tempV->_filePath.toStdString()<<endl;

        // Output video threshold
        out<<tempV->_threshold<<endl;

        // Output video number of frames
        out<<tempV->_numberOfFramesInVideo<<endl;

        // Output video frame rate
        out<<tempV->_frameRate<<endl;

        // Output video frame height
        out<<tempV->_frameHeight<<endl;

        // Output video frame width
        out<<tempV->_frameWidth<<endl;

        //Output Current Start time
        out<<tempV->_currentStart.toString().toStdString()<<endl;

        //Output Current End time
        out<<tempV->_currentEnd.toString().toStdString()<<endl;

        //Output number of start and end times in vector
        out<<tempV->_times.size()<<endl;

        //Output each element in the QTime vector
        for(int count = 0; count < tempV->_times.size(); count++ )
            out<<tempV->_times[count].toString().toStdString()<<endl;

        // Output the number of regions
        out<<(tempV->_listOfRegions.size())<<endl;

        // Begin loop for output of region data
        for(int b=0; b < (tempV->_listOfRegions.size());b++)
        {
            // Pointer tot he current region
            BvRegion* tempR = tempV->_listOfRegions[b];

            // Output region name
            out<<tempR->_name.toStdString()<<endl;

            // Output region threshold
            out<<tempR->_threshold<<endl;

            // Output region notes
            out<<tempR->_notes.toStdString()<<endl;

            // Output region height
            out<<tempR->_height<<endl;

            // Output region width
            out<<tempR->_width<<endl;

            // Output region x
            out<<tempR->_x<<endl;

            // Output region y
            out<<tempR->_y<<endl;
        }
    }
    projectToDirectory(projName, 1);
    out.close();
}

/*!
 * Saves a project with a unique name and path
 * This allowes the user to pick the name and location of the project file.
 */
void ProjectManager::saveProjectAs(QString projName, QString projPath)
{
    ofstream out;

    //Commented out for final release
    //cout<<projPath.toStdString();

    std::string projNam = projPath.toStdString() + ".bv";
    out.open(projNam.c_str());

    Project* currentProject = getProject(projName);
    currentProject->_path = projPath;

    // Output MetaData, %$#@! signals beginning and end of metadata.
    QString metadata = "This is random metadata, etc, etc.";
    out<<"%$#@!";
    out<<metadata.toStdString();
    out<<"%$#@!"<<endl;

    // Output Project Name
    out<<projName.toStdString()<<endl;

    // Output Project Path
    out<<currentProject->_path.toStdString()<<endl;

    // Output the number of videos in the project.
    out<<(currentProject->_listOfVideos.size())<<endl;

    // Begin loop to output all of the video data. @@@ signals beginning of video, and $$$ the end
    for(int a = 0; a < (currentProject->_listOfVideos.size()); a++)
    {
        // Pointer to the current video
        Video* tempV = currentProject->_listOfVideos[a];

        // Output video name
        out<<tempV->_name.toStdString()<<endl;

        // Output video path
        out<<tempV->_filePath.toStdString()<<endl;

        // Output video threshold
        out<<tempV->_threshold<<endl;

        // Output video number of frames
        out<<tempV->_numberOfFramesInVideo<<endl;

        // Output video frame rate
        out<<tempV->_frameRate<<endl;

        // Output video frame height
        out<<tempV->_frameHeight<<endl;

        // Output video frame width
        out<<tempV->_frameWidth<<endl;

        //Output Current Start time
        out<<tempV->_currentStart.toString().toStdString()<<endl;

        //Output Current End time
        out<<tempV->_currentEnd.toString().toStdString()<<endl;

        //Output number of start and end times in vector
        out<<tempV->_times.size()<<endl;

        //Output each element in the QTime vector
        for(int count = 0; count < tempV->_times.size(); count++ )
            out<<tempV->_times[count].toString().toStdString()<<endl;

        // Output the number of regions
        out<<(tempV->_listOfRegions.size())<<endl;

        // Begin loop for output of region data
        for(int b=0; b < (tempV->_listOfRegions.size());b++)
        {
            // Pointer tot he current region
            BvRegion* tempR = tempV->_listOfRegions[b];

            // Output region name
            out<<tempR->_name.toStdString()<<endl;

            // Output region threshold
            out<<tempR->_threshold<<endl;

            // Output region notes
            out<<tempR->_notes.toStdString()<<endl;

            // Output region height
            out<<tempR->_height<<endl;

            // Output region width
            out<<tempR->_width<<endl;

            // Output region x
            out<<tempR->_x<<endl;

            // Output region y
            out<<tempR->_y<<endl;
        }
    }
    out.close();
}

/*!
 * Saves all the currently loaded projects.
 *
 * \param currentProject is a pointer to the vector of Projects.
 * \param _projects[i] cycles through every project and calls saveProjec() for each one.
 * \return void
 *
 * \see saveProject()
 */
void ProjectManager::saveAllProjects()
{
    Project* currentProject = *_projects.begin();
    for(int i = 0; i <(_projects.size()); i++)
    {
        currentProject = _projects[i];
        saveProject(currentProject->_projectName);
    }
}

/*!
 * Takes in a file path to a .bv file and pulls all the project data stored inside to reload those projects.
 *
 * \param vName Is a pointer to an element in a vector of videos.
 * \param rName Is a pointer to an element in a vector of regions inside an element in a vector of videos.
 * \return void
 *
 * \see addProject()
 * \see addVideoToProject()
 * \see addRegion()
 */
void ProjectManager::openProject(QString path)
{
    QFile temp(path);
    bool fileOpen = true;
    fileOpen = temp.open(QIODevice::ReadOnly);
    if(fileOpen)
        temp.close();

    if(fileOpen)
    {
        ifstream in;

        std::string thePath = path.toStdString();
        in.open(thePath.c_str());

        // Get the metadata.
        char meta[255];
        in.getline(meta,255);
        QString metadata(meta);

        // Read in the project name
        char pName[50];
        in.getline(pName,50);
        QString projectName(pName);

        addProject(projectName);

        // Read in the project path
        char pPath[255];
        in.getline(pPath,255);
        QString projectPath(pPath);

        Project* currentProject = getProject(projectName);
        currentProject->_path = projectPath;

        // Get the number of Videos in the project.
        char temp[50];
        in.getline(temp,2);
        int numberOfVideos = atoi(temp);

        // Begin loop to input all of the video data.
        for(int a = 0; a < numberOfVideos; a++)
        {
            // Get video name
            char vName[255];
            in.getline(vName, 255);
            QString videoName(vName);

            // Get video path
            char vPath[255];
            in.getline(vPath, 255);
            QString videoPath(vPath);

            // Get the video threshold
            char vThreshold[50];
            in.getline(vThreshold,50);
            int videoThreshold = atoi(vThreshold);

            // Get the number of frames
            char vFrameN[50];
            in.getline(vFrameN,50);
            int videoNumberOfFrames = atoi(vFrameN);

            // Get the video frame rate
            char vFrameR[50];
            in.getline(vFrameR,50);
            int videoFrameRate = atoi(vFrameR);

            // Get the video height
            char vHeight[50];
            in.getline(vHeight,50);
            int videoHeight = atoi(vHeight);

            // Get the video width
            char vWidth[50];
            in.getline(vWidth,50);
            int videoWidth = atoi(vWidth);

            addVideoToProject(projectName, videoPath, videoName, videoNumberOfFrames, videoFrameRate, videoHeight, videoWidth, videoThreshold);

            Project* currentProject = getProject(projectName);
            std::vector<Video*>::iterator videoIt = (currentProject->_listOfVideos).begin();

            while(videoIt != (currentProject->_listOfVideos).end())
            {
                if((*videoIt)->_name == videoName)
                    (*videoIt)->_threshold = videoThreshold;
                videoIt++;
            }

            Video* currentVideo = getVideo(projectName, videoName);

            // Read in the current Start time
            char vCstart[255];
            in.getline(vCstart,255);
            QString videoCurrentStart(vCstart);
            QTime tempCS = QTime::fromString(videoCurrentStart);
            currentVideo->_currentStart = tempCS;

            // Read in the current end time
            char vCend[255];
            in.getline(vCend,255);
            QString videoCurrentEnd(vCend);
            QTime vCE = QTime::fromString(videoCurrentEnd);
            currentVideo->_currentEnd = vCE;

            // Get the number of times for QTime vector
            char vTimes[50];
            in.getline(vTimes,50);
            int videoTimes = atoi(vTimes);

            for(int temp = 0; temp < videoTimes; temp++)
            {
                // Read in times for the vector
                char vtemp[255];
                in.getline(vtemp,255);
                QString videoTemp(vtemp);

                QTime tempTime = QTime::fromString(videoTemp);
                currentVideo->_times.push_back(tempTime);
            }


            // Get the number of Regions in the Video.
            char regions[50];
            in.getline(regions,10);
            int numberOfRegions = atoi(regions);

            // Begin loop for input of region data
            for(int b=0; b < numberOfRegions;b++)
            {
                // Get region name
                char rName[255];
                in.getline(rName, 255);
                QString regionName(rName);

                // Get the region threshold
                char rThreshold[50];
                in.getline(rThreshold,50);
                int regionThreshold = atoi(rThreshold);

                //Get the region notes.
                char rNotes[4096];
                in.getline(rNotes,4096);
                QString regionNotes(rNotes);

                // Get the region height
                char height[50];
                in.getline(height,50);
                int regionHeight = atoi(height);

                // Get the region width
                char width[50];
                in.getline(width,50);
                int regionWidth = atoi(width);

                // Get the region x
                char x[50];
                in.getline(x,50);
                int regionX = atoi(x);

                // Get the region y
                char y[50];
                in.getline(y,50);
                int regionY = atoi(y);

                setRegion(projectName, videoName,"", regionName, regionThreshold, regionNotes, regionX, regionY, regionWidth, regionHeight);
                setRegionThreshold(projectName, videoName, regionName, regionThreshold);

                ////Dustin Added, New loading code to save edit and start/stop times
                ////Dustin Added, New loading code to save edit and start/stop times
            }
        }
        projectToDirectory(projectName, 1);
        in.close();
    }
}

/*!
 * \brief ProjectManager::removeProject removes a project by doing 3 things:
 * 1) Remove the project from the list of projects to auto-load (call projectToDirectory with param decision=0)
 * 2) Remove all of the project data from the workspace.
 * 3) Delete the project object from the vector.
 *
 * \param projName the name of the project to remove.
 *
 * \return true if successfully removed, false otherwise.
 */
bool ProjectManager::removeProject(QString dirName)
{
    bool result;
    QDir dir(dirName);

    if (dir.exists(dirName))
    {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (info.isDir())
                result = removeProject(info.absoluteFilePath());

            else
                result = QFile::remove(info.absoluteFilePath());

            if (!result)
                return result;
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

/*!
 * \brief ProjectManager::getWorkspaceDirectory gets the workspace directory.  It first checks to see if the workspace
 * is currently stored in project manager- if not, it retrieves it from file (note that this value may also be null
 * on the condition that this is the user's first run of the program).
 *
 * \return The workspace directory path as a QString.
 */
QString ProjectManager::getWorkspaceDirectory()
{
    if(_workspace=="")
    {
        ifstream in;
        in.open("settings.txt");

        // Read the workspace directory in
        std::string workspace;
        std::getline(in, workspace);

        in.close();

        // Set the class workspace variable
        _workspace = QString::fromStdString(workspace);

        // Set whether or not we are on mac or windows.
        int lastSlashInPath = workspace.find_last_of('/');

        if(lastSlashInPath != -1)
            _isWindows = false;
        else
            _isWindows = true;

        return QString::fromStdString(workspace);
    }

    else
        return _workspace;
}

/*!
 * \brief ProjectManager::autoLoadProjects auto loads each project that is in the workspace directory and that has not
 * been hidden by the user.  These are stored in the settings.txt file, so this function accesses that and then calls
 * openProject for each project.
 */
void ProjectManager::autoLoadProjects()
{
    ifstream in;
    in.open("settings.txt");

    // Read the workspace directory in
    std::string workspace;
    std::getline(in, workspace);

    // Get the number of projects in the file.
    char number[50];
    in.getline(number,50);
    int numberOfProjects = atoi(number);

    for(int counter = 0; counter < numberOfProjects; counter++)
    {
        char path[255];
        in.getline(path, 255);
        QString projectPath(path);
        openProject(projectPath);
    }
    in.close();
}

/*!
 * \brief ProjectManager::setWorkspaceDirectory Sets a new workspace directory path by writing it to the file, then updates
 * the class variable _workspace with the new workspace path.
 *
 * \param workspacePath the new path to set.
 */
void ProjectManager::setWorkspaceDirectory(QString workspacePath)
{
    ofstream out;
    out.open("settings.txt");

    // Save the new workspace in the class variable for easy access.
    _workspace = workspacePath;

    // Set whether or not we are on mac or windows.
    int lastSlashInPath = workspacePath.toStdString().find_last_of('/');

    if(lastSlashInPath != -1)
        _isWindows = false;
    else
        _isWindows = true;

    _ProjectNames.clear();
    int numberOfProjects = _ProjectNames.size();

    // Output the workspace path and the number of proejcts
    out<<_workspace.toStdString()<<endl;
    out<<numberOfProjects<<endl;

    out.close();
}

/*!
 * \brief ProjectManager::projectToDirectory, if the decision is 1 it takes the project name and adds it to the vector ProjectNames
 * which is used to output file paths to the settings.txt file. If the decision is anything else it removes said project from
 * the vector and overwrites the settings file so the project is no longer auto loaded
 *
 * \param projectName The name of the project we wish to access
 * \param decision The value which determines the action projectToDirectory should take
 */
void ProjectManager::projectToDirectory(QString projectName, int decision)
{
    QString name = "";
    // This will deal with adding a project to the list of ones to open
    if (decision == 1)
    {
        int numberOfProjects = _ProjectNames.size();

        // Make sure the file path is not already there, and then push back the file path.
        name = _workspace + "\\" + projectName + "\\" + projectName + ".bv";
        if(_isWindows == false)
            name = _workspace + "/" + projectName+ "/" + projectName + ".bv";;

        for(int count = 0; count < numberOfProjects; count++)
            if(_ProjectNames[count] == name) name = "";

        if(name != "")
            _ProjectNames.push_back(name);

        // Open the file for writing
        ofstream out;
        out.open("settings.txt");

        numberOfProjects = _ProjectNames.size();

        // Output the workspace path and the number of proejcts
        out<<_workspace.toStdString()<<endl;
        out<<numberOfProjects<<endl;

        for(int count = 0; count < numberOfProjects; count++)
            out<<_ProjectNames[count].toStdString()<<endl;

        out.close();
    }

    else
    {
        // Make sure the file path is not already there, and then push back the file path.
        name = _workspace + "\\" + projectName + "\\" + projectName + ".bv";
        if(_isWindows == false)
            name = _workspace + "/" + projectName+ "/" + projectName + ".bv";;

        for(vector<QString>::iterator it = _ProjectNames.begin(); it<_ProjectNames.end(); it++)
        {
            if( *it == name)
                _ProjectNames.erase(it);
        }

        ofstream out;
        out.open("settings.txt");

        int numberOfProjects = _ProjectNames.size();
        out<<_workspace.toStdString()<<endl;
        out<<numberOfProjects<<endl;

        for(int count = 0; count < numberOfProjects; count++)
            out<<_ProjectNames[count].toStdString()<<endl;

        out.close();

        _projectIt = _projects.begin();

        while(_projectIt != _projects.end())
        {
            if((*_projectIt)->_projectName == projectName)
            {
                _projects.erase(_projectIt);
                return;
            }
            _projectIt++;
        }
    }
}



/*!
 * \brief ProjectManager::checkForRun checks the folder structure for a folder named after the current run,
 * if one exists it returns false telling it the run already exists, otherwise it creates the folder for the images and results files
 *
 * \param projName The name of the project that contains the video
 * \param vidName The name of the video we want to access
 * \param runName The folder name of the current run
 *
 * \return Returns bool value telling the calling function if the directory already exists
 */
bool ProjectManager::checkForRun(QString projName, QString vidName, QString runName)
{
    runName.replace(" ", "");
    runName.replace(")", "");
    runName.replace("(", "");
    runName.replace("_", "");
    runName.replace("-", "");
    runName.replace("1", "one");
    runName.replace("2", "two");
    runName.replace("3", "three");
    runName.replace("4", "four");
    runName.replace("5", "five");
    runName.replace("6", "six");
    runName.replace("7", "seven");
    runName.replace("8", "eight");
    runName.replace("9", "nine");
    runName.replace("0", "zero");
    QString path = _workspace + "\\" + projName + "\\" +vidName;
    if(_isWindows == false)
        path = _workspace + "/" + projName + "/" +vidName;

    bool nameExists = false;
    Project* currentProject = getProject(projName);

    if(currentProject->_path.size() > 2)
    {
         path = currentProject->_path + "\\" + vidName;
         if(_isWindows == false)
             path = currentProject->_path + "/" + vidName;
    }

    QDir Directory1;
    Directory1.setPath(path);
    nameExists = Directory1.mkdir(runName);

    return nameExists;
}

/*!
 * \brief outPutResults has two paramaters that affect what happens, overwrite tells the function whether or not
 * it needs to delete files in an already existing folder. images tells whether or not images need to be output to the folder
 * based on these there are 4 cases that copy the results and excel document from the tmp folder into the results folder
 * with the appropiate names.
 *
 * \param projName The name of the project that contains the video
 * \param vidName The name of the video we want to access
 * \param runName The folder name of the current run
 * \param overRight Determines if function should overwrite existing data
 * \param images Determines if function should ocopy image data
 *
 * \return Returns bool value telling the calling function if the directory already exists
 */
bool ProjectManager::outputResults(QString projName, QString vidName, QString runName, bool overwrite, bool images)
{
    QString path = _workspace + "\\" + projName + "\\" +vidName;
    if(_isWindows == false)
    {
        path = _workspace + "/" + projName + "/" +vidName;
    }

    bool nameExists = false;
    Project* currentProject = getProject(projName);

    if(currentProject->_path.size() > 2)
    {
         path = currentProject->_path + "\\" + vidName;
         if(_isWindows == false)
         {
             path = currentProject->_path + "/" + vidName;
         }
    }

    runName.replace(" ", "");
    runName.replace(")", "");
    runName.replace("(", "");
    runName.replace("_", "");
    runName.replace("-", "");
    runName.replace("1", "one");
    runName.replace("2", "two");
    runName.replace("3", "three");
    runName.replace("4", "four");
    runName.replace("5", "five");
    runName.replace("6", "six");
    runName.replace("7", "seven");
    runName.replace("8", "eight");
    runName.replace("9", "nine");
    runName.replace("0", "zero");

    QDir Directory;
    Directory.setPath(path);
    nameExists = Directory.mkdir(runName);

    if((nameExists == false) && (overwrite == true))
    {
        QString newPath = _workspace + "\\" + projName + "\\" +vidName + "\\" + runName;
        if(_isWindows == false)
        {
            newPath = _workspace + "/" + projName + "/" +vidName +"/" +runName;
        }

        removeProject(newPath);

        Directory.setPath(path);
        Directory.mkdir(runName);

        // Copy the excel file into the right place with the right name.
        path = _workspace + "\\" + projName + "\\" +vidName + "\\" + runName;
        if(_isWindows == false)
        {
             path = _workspace + "/" + projName + "/" +vidName + "/" + runName;
        }

        QDir dir(path);


        // Copy the excel file into the right place with the right name.
        path = _workspace + "\\" + projName + "\\" +vidName + "\\" + runName + "\\" + runName + ".xlsm";
        if(_isWindows == false)
             path = _workspace + "/" + projName + "/" +vidName + "/" + runName + "/" + runName + ".xlsm";

        QFile _resultsFile;
        _resultsFile.copy("macros.xlsm", path);

        // Output the result object here.
        QString destination = _workspace + "\\" + projName + "\\" +vidName + "\\" + runName + "\\" + runName +".txt";
        if(_isWindows == false)
            destination = _workspace + "/" + projName + "/" +vidName + "/" + runName + "/" + runName +".txt";

        _resultsFile.copy("tmp/tmp.txt", destination);

        if(images == true)
        {
            QDir dir("tmp");
            dir.setNameFilters(QStringList() << "*.jpg");
            dir.setFilter(QDir::Files);

            QStringList files = dir.entryList();

            QFile _temp;
            while(files.size() > 0)
            {
                QString copyTo = _workspace + "\\" + projName + "\\" +vidName + "\\" + runName + "\\" + files.first();
                if(_isWindows == false)
                {
                    copyTo = _workspace + "/" + projName + "/" +vidName + "/" + runName + "/" + files.first();
                }

                _temp.copy("tmp/"+files.first(),copyTo);
                files.removeFirst();
            }
        }
        return nameExists;
    }

    else
    {
       // Copy the excel file into the right place with the right name.
       path = _workspace + "\\" + projName + "\\" +vidName + "\\" + runName + "\\" + runName + ".xlsm";
       if(_isWindows == false)
       {
           path = _workspace + "/" + projName + "/" +vidName + "/" + runName + "/" + runName + ".xlsm";
       }

       QFile _resultsFile;
       _resultsFile.copy("macros.xlsm", path);

       // Output the result object here.
       QString destination = _workspace + "\\" + projName + "\\" +vidName + "\\" + runName + "\\" + runName +".txt";
       if(_isWindows == false)
       {
           destination = _workspace + "/" + projName + "/" +vidName + "/" + runName + "/" + runName +".txt";
       }

       _resultsFile.copy("tmp/tmp.txt", destination);

       if(images == true)
       {
           QDir dir("tmp");

           dir.setNameFilters(QStringList() << "*.jpg");
           dir.setFilter(QDir::Files);

           QStringList files = dir.entryList();

           QFile _temp;
           while(files.size() > 0)
           {
               QString copyTo = _workspace + "\\" + projName + "\\" +vidName + "\\" + runName + "\\" + files.first();
               if(_isWindows == false)
                   copyTo = _workspace + "/" + projName + "/" +vidName + "/" + runName + "/" + files.first();

               _temp.copy("tmp/"+files.first(),copyTo);
               files.removeFirst();
           }
       }
    }
    return nameExists;
}

/*!
 * \brief ProjectManager::getSizeOfImages gets the total size of the images that will be copied (all of the images in
 * the tmp folder).
 *
 * \return a QString containing the number of the total size of all of the .jpg files in tmp.
 */
QString ProjectManager::getSizeOfImages()
{
    QDir dir("tmp/");
    qint64 totalSize;
    QFileInfo image;

    dir.setNameFilters(QStringList() << "*.jpg");
    dir.setFilter(QDir::Files);

    QFileInfoList files = dir.entryInfoList();

    //Commented out for final release
    //qDebug() << "num of image files: " + QString::number(files.size());

    if(!files.isEmpty())
        image = files.at(0);

    //Commented out for final release
    //qDebug() << "size of first image file: " + QString::number(image.size());

    if(image.exists())
        totalSize = image.size()*files.size();
    else
        totalSize = 0;

    return convertToReadableSize(totalSize);
}

/*!
 * \brief ProjectManager::convertToReadableSize converts a qint64 value (in bytes) into the equivalent value in kilobytes,
 * megabytes, or gigabytes, and formats it as a QString.
 *
 * \param bytes the total number of bytes of all of the images currently stored in the tmp folder.
 *
 * \return a QString containing a readable value of the size of the images converted to kilobytes, megabytes, or gigabytes.
 */
QString ProjectManager::convertToReadableSize(qint64 bytes)
{
    QString size;

    if(bytes<1024)
    {
        size = QString::number(bytes) + " bytes";
    }
    else if(bytes<1048576)
    {
        qint64 kbytes = bytes/1024;
        qint64 remainder = bytes%1024;

        double kbytesDecimal = remainder/(double)1024;
        kbytesDecimal = kbytesDecimal + kbytes;

        size = QString::number(kbytesDecimal)+ " kilobytes";
    }
    else if(bytes<1073741824)
    {
        qint64 mbytes = bytes/1048576;
        qint64 remainder = bytes%1048576;

        double mbytesDecimal = remainder/(double)1048576;
        mbytesDecimal = mbytesDecimal + mbytes;

        size = QString::number(mbytesDecimal) + " megabytes";
    }
    else
    {
        qint64 gbytes = bytes/1073741824;
        qint64 remainder = bytes%1073741824;

        double gbytesDecimal = remainder/(double)1073741824;
        gbytesDecimal = gbytesDecimal + gbytes;

        size = QString::number(gbytesDecimal) + " gigabytes";
    }

    return size;
}
