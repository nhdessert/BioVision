/*!
 * \class ProjectManager is responsible for handling the management of the data for BioVision.  This includes handling the
 * Project class, Video class, BvRegion class, and also managing the workspace and file/directory structure for BioVision,
 * such as experiment names and directories and data saving.
 */

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

class BvSystem;

#include "Project.h"
#include <QString>

class ProjectManager
{

public:
	ProjectManager();
	virtual ~ProjectManager();

    /******* Project/Video/Region data getter and setter functions *******/
    // Project
    Project* getProject(QString projName);
    std::vector<Project*>* getAllProjects();

    //Videos
    Video* getVideo(QString projName, QString vidName);
    std::vector<Video*>* getAllVideos(QString projName);
    QString getVideoPath(QString projName, QString vidName);
    bool setVideoPath(QString projName, QString vidName, QString newPath);// called if a video is copied to the workspace.
    int getVideoHeight(QString projName, QString vidName);
    int getVideoWidth(QString projName, QString vidName);
    std::vector <QTime> getAllVideoTimes(QString projName, QString vidName);
    void setVideoStartAndStopTimes(QString projName, QString vidName, QTime currentStartTime, QTime currentEndTime);

    // Regions
    BvRegion* getRegion(QString projName, QString vidName, QString regionName);
    std::vector<BvRegion*>* getAllRegions(QString projName, QString vidName);
    int getRegionThreshold(QString projName, QString vidName, QString regionName);
    QString getRegionNotes(QString projName, QString vidName, QString regionName);
    void setRegionThreshold(QString projName, QString vidName, QString regionName, int threshold);
    void setRegionNotes(QString projName, QString vidName, QString regionName, QString notes);
    std::vector<int>* getAllRegionsXcoords(QString projName, QString vidName);
    std::vector<int>* getAllRegionsYcoords(QString projName, QString vidName);
    std::vector<int>* getAllRegionsWidths(QString projName, QString vidName);
    std::vector<int>* getAllRegionsHeights(QString projName, QString vidName);
    std::vector<QString>* getAllRegionNames(QString projName, QString vidName);
    int getNumberOfRegionsInVideo(QString projName, QString vidName);
    std::vector <int> getRegionDataForRegionWindow(QString projName, QString vidName, QString regionName);
    std::vector<int>* getAllRegionsThresholds(QString projName, QString vidName);

    /******* Project/Video/Region data manipulation- adding, removing, updating. *******/
    // Projects
    bool addProject(QString projName);
    bool makeNewProject(QString projName);
    void saveProject(QString projName);
    void saveProjectAs(QString projName, QString projPath);
    void openProject(QString filePath);
    void saveAllProjects();
    bool removeProject(QString projName);

    // Videos
    void removeVideoFromProject(QString projName, QString vidName);
    bool addVideoToProject(QString projName, QString pathToVideoFile, QString vidName, int numberOfFrames, int frameRate, int frameHeight, int frameWidth, int threshold);
    void addVideoEditTime(QString projName, QString vidName, QTime newEditTime);
    void removeLastVideoEditTime(QString projName, QString vidName);

    // Regions
    bool setRegion(QString projName, QString vidName, QString oldName, QString newName, int threshold, QString notes, int x, int y, int width, int height);
    bool checkRegionSize(QString projName, QString vidName);
    void removeRegion(QString projName, QString vidName, QString regionName);

    /******** Workspace and directory management functions *********/

    // Get and set for the workspace
    QString getWorkspaceDirectory();
    void setWorkspaceDirectory(QString workSpacePath);

    // Hiding/Autoloading projects.
    void projectToDirectory(QString projectName, int decision);
    void autoLoadProjects();

    // Total Image size calculations (to let the user know how much space will be taken up if they copy the images)
    QString getSizeOfImages();
    QString convertToReadableSize(qint64);

    // Outputting Analyze results
    bool outputResults(QString projName, QString vidName, QString runName, bool overwrite, bool images);
    bool checkForRun(QString projName, QString vidName, QString runName);

private:
    /*! The projects within the system bounds */
    std::vector<Project*> _projects;

    /*! An iterator for that vector */
    std::vector<Project*>::iterator _projectIt;

    /*! The names of all of the projects stored separately.  Used for workspace management.*/
    std::vector<QString> _ProjectNames;

    /*! an iterator for the project names vector */
    std::vector<QString>::iterator _ProjectNamesIt;

    /*! The workspace directory. */
    QString _workspace;

    /*! If true, windows OS, if false, Unix */
    bool _isWindows;
};
#endif // !defined(EA_71C866DB_142D_420b_88E1_54DFB25CB2AC__INCLUDED_)
