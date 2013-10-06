#include "MainWindow.h"
#if defined WIN32
#include "ui_MainWindow_win.h"
#else
#include "ui_MainWindow_mac.h"
#endif

#define PROJECT 0
#define VIDEO 1
#define REGION 2


/*!
 * Constructs MainWindow
 *
 * Sets up the ui including cconnecting signals and slots. Sets default values for certain properties.
 *
 * \param parent A pointer to it's parent QWidget. By default, 0
 * \param windowManager A pointer to WindowManager.
 */
MainWindow::MainWindow(QWidget *parent, WindowManager *windowManager) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    // Initialize values
    _windowManager = windowManager;
    _isRegionBeingDrawn = false;
    _activeProjectName = QString();
    _activeVideoName = QString();
    _activeRegionName = QString();
    _mediaSource = NULL;
    _sensitivity = 50;

    //set initial start and end time  check values
    _previousStartTime = QTime::fromString("00:00:00", "hh:mm:ss");
    _previousEndTime = QTime::fromString("00:00:00", "hh:mm:ss");

    // Setup UI objects
    ui->setupUi(this);

    // Setup VideoPlayer elements
    ui->seekSlider->setMediaObject(ui->videoPlayer->mediaObject());
    ui->seekSlider->setTracking(true);
    ui->volumeSlider->setAudioOutput(ui->videoPlayer->audioOutput());
    ui->volumeSlider->setTracking(true);

    // Set the default sensitivity value
    ui->thresholdSlider->setValue(_sensitivity);
    ui->thresholdValue->setText(QString::number(_sensitivity));

    // Allow right clicks on project browser
    ui->projectBrowser->setContextMenuPolicy(Qt::CustomContextMenu);

    // Load projects
    getAllProjects();   

    //File menu
    connect(ui->actionNew_Project, SIGNAL(triggered()), this, SLOT(newProjectSlot()));
    connect(ui->actionLoad_Project, SIGNAL(triggered()), this, SLOT(loadProjectSlot()));
    connect(ui->actionSave_Project, SIGNAL(triggered()), this, SLOT(saveProjectSlot()));
    connect(ui->actionSave_Project_As, SIGNAL(triggered()),this, SLOT(saveProjectAsSlot()));
    connect(ui->actionSave_All_Projects, SIGNAL(triggered()), this, SLOT(saveAllProjectsSlot()));

    //Edit menu
    connect(ui->actionAdd_Video, SIGNAL(triggered()), this, SLOT(addVideoSlot()));
    connect(ui->actionRemove_Video, SIGNAL(triggered()), this, SLOT(removeVideoSlot()));
    connect(ui->actionDelete_Region_From_Video, SIGNAL(triggered()), this, SLOT(deleteRegionSlot()));

    // Project menu
    connect(ui->actionAnalyze, SIGNAL(triggered()), this, SLOT(analyzeSlot()));
    connect(ui->actionOpen_Excel_Report, SIGNAL(triggered()), this, SLOT(openExcelFileSlot()));
    connect(ui->actionViewWorkspace, SIGNAL(triggered()), this, SLOT(viewWorkspaceSlot()));

    //options
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(launchOptionsSlot()));
    connect(ui->actionAdd_Video_Edit_Point, SIGNAL(triggered()), this, SLOT(addEditPointSlot()));
    connect(ui->actionRemove_Last_Edit_Point, SIGNAL(triggered()), this, SLOT(removeLastEditPointSlot()));

    //Help Menu
    connect(ui->actionHelp, SIGNAL(triggered()),this, SLOT(launchDocumentationSlot()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(launchAboutSlot()));

    //Connect the threshold slider- change the value in text box as the user moves the slider.
    connect(ui->thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(thresholdChangedSlot(int)));
    // and the reverse- change the slider if the value in the text box changes.
    connect(ui->thresholdValue, SIGNAL(textChanged(QString)), this, SLOT(moveSliderSlot(QString)));

    //Analyze- this spawns a new thread.
    connect(ui->Analyze, SIGNAL(clicked()), this, SLOT(analyzeSlot()));
    // Cancel- cancels an operation.
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(cancelSlot()));

    //Project Browser- pressed, double clicked, and right clicked
    connect(ui->projectBrowser, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(projectBrowserPressedSlot(QTreeWidgetItem*,int)));
    connect(ui->projectBrowser, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(projectBrowserDoubleClickedSlot(QTreeWidgetItem*,int)));
    connect(ui->projectBrowser, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(projectBrowserRightClickedSlot(const QPoint&)));

    //Video player slots
    connect(ui->videoPlayer->mediaObject(), SIGNAL(totalTimeChanged(qint64)), this, SLOT(setTotalTimeSlot(qint64)));
    connect(ui->videoPlayer->mediaObject(), SIGNAL(tick(qint64)), this, SLOT(updateCurrentTimeSlot(qint64)));
    connect(ui->PlayButton, SIGNAL(clicked()), this, SLOT(playPauseSlot()));

    //Initialize Result Carousel
    initializeResultCarousel();

    //start time and end time change evaluation
    connect(ui->startTime, SIGNAL(timeChanged(QTime)), this, SLOT(startTimeChangeCheckerSlot()));
    connect(ui->stopTime, SIGNAL(timeChanged(QTime)), this, SLOT(endTimeChangeCheckerSlot()));

    //set preview speed changes
    connect(ui->actionSlowPreviewSpeed, SIGNAL(triggered()), this, SLOT(previewSlowSpeedCheckSlot()));
    connect(ui->actionMediumPreviewSpeed, SIGNAL(triggered()), this, SLOT(previewMediumSpeedCheckSlot()));
    connect(ui->actionFastPreviewSpeed, SIGNAL(triggered()), this, SLOT(previewFastSpeedCheckSlot()));

    //set preview window size changes
    connect(ui->actionNativeWindowSize, SIGNAL(triggered()), this, SLOT(previewWindowSizeNativeSlot()));
    connect(ui->actionMediumWindowSize, SIGNAL(triggered()), this, SLOT(previewWindowSizeMediumSlot()));
    connect(ui->actionLargeWindowSize, SIGNAL(triggered()), this, SLOT(previewWindowSizeLargeSlot()));

    //set image output size chages
    connect(ui->actionNativeSavedImageSize, SIGNAL(triggered()), this, SLOT(analyzeImageSaveNativeSlot()));
    connect(ui->actionSmallSavedImageSize, SIGNAL(triggered()), this, SLOT(analyzeImageSaveSmallSlot()));
    connect(ui->actionMediumSavedImageSize, SIGNAL(triggered()), this, SLOT(analyzeImageSaveMediumSlot()));
}

/*!
 * \brief MainWindow::~MainWindow
 *
 * MainWindow's deconstructor
 */
MainWindow::~MainWindow()
{
    delete _windowManager;
}

/*!
 * A slot that will make a new project.
 *
 * Opens a dialog that asks for the name of the project, and passes that request on.  If the directory already exists
 * or the project name already exists, then display a dialog notifying the user.
 *
 */
void MainWindow::newProjectSlot()
{
    bool ok;

    QString text = QInputDialog::getText(this, "New Project",
                                              "This project will be created by default in your workspace.\n\nProject Name:", QLineEdit::Normal,
                                              "", &ok);

    QRegExp regExp("^[a-zA-Z0-9 ]{1,27}$");

    // First check if the workspace does not exist.  If it does, proceed.  If not, we cannot make the project.
    if(!_windowManager->getWorkspace().isEmpty())
    {
        // If the value is true and the text is not empty, then make a new project.
        if (ok && !text.isEmpty())
        {
            if(!regExp.exactMatch(text))
            {
                // launch a message box- we had an error- The project can only contain alpha numeric characters.
                //   with that name.
                QMessageBox errorMsg;
                errorMsg.setText("Project name cantains bad characters.");
                errorMsg.setInformativeText("The project name can only contain alpha numeric characters and be 27 characters long.");
                errorMsg.exec();

                // If this is the case, show the newProject dialog again.
                newProjectSlot();
            }
            // If the new project returns false, we have a
            else if(!_windowManager->makeNewProject(text))
            {
                // launch a message box- we had an error- either the project name is in use or there is already a directory
                //   with that name.
                QMessageBox errorMsg;
                errorMsg.setText("Project name or directory already exists in workspace.");
                errorMsg.setInformativeText("Select a different name or remove the other project.");
                errorMsg.exec();

                // If this is the case, show the newProject dialog again.
                newProjectSlot();
            }
        }
    }
    // If the workspace does not exist, then notify the user of that.
    else
    {
        QMessageBox errorMsg;
        errorMsg.setText("You have not set an application workspace.");
        errorMsg.setInformativeText("All projects will be stored in the workspace by default.  Go to Tools -> Options to set a workspace.");
        errorMsg.exec();
    }

    refreshProjectBrowser();
}

/*!
 * A slot that will load a project.
 *
 * Opens a \QFileDialog to get the path for loading a project.
 * Then, if the path isn't null, it requests that WindowManager loads that
 * project. Finally, it will refresh the projectBrowser.
 *
 */
void MainWindow::loadProjectSlot()
{
    QString loadProjectPath = QFileDialog::getOpenFileName(this, QString("Open Project"), _windowManager->getWorkspace(), QString("Biovision Project Files (*.bv)") );
    if(loadProjectPath != QString())
    {
        _windowManager->loadProject(loadProjectPath);
        refreshProjectBrowser();
    }
}

/*!
 * A Slot that saves the active Project
 *
 * Checks to make sure _activeProjectName is not a null \QString.
 * Then, requests that WindowManager saves that project.
 *
 */
void MainWindow::saveProjectSlot()
{
    if(_activeProjectName != QString())
    {
        _windowManager->saveProject(_activeProjectName);
    }
}

/*!
 * Saves the active Project with a new name specified by the user.
 *
 * Checks to make sure _activeProjectName is not a null \QString.
 * Then, opens a \QFileDialog to get the new save path.
 * If the save path is not a null \QString, it requests that WindowManager
 * saves the project as.
 *
 */
void MainWindow::saveProjectAsSlot()
{
    if(_activeProjectName != QString())
    {
        QString saveProjectPath = QFileDialog::getSaveFileName(this, QString("Save As"), QString("./"));
        if(saveProjectPath != QString())
        {
            _windowManager->saveAsProject( _activeProjectName , saveProjectPath);
        }
    }
}

/*!
 * Saves all projects.
 *
 * Request WindowManager to save all the projects.
 */
void MainWindow::saveAllProjectsSlot()
{
    _windowManager->saveAllProjects();
}

/*!
 * \brief MainWindow::removeProjectSlot is called when a user indicates through the GUI that they want to remove a project.
 * Launches a confirmation dialog to ensure that the user intended to delete the project, and notifies them
 * that the data in the workspace (including project folder) will not be deleted.
 */
void MainWindow::removeProjectSlot()
{
    QMessageBox removeProjCnfrm;
    removeProjCnfrm.setText("Are you sure you wish to remove " + _activeProjectName + " from BioVision?");
    removeProjCnfrm.setInformativeText("Your data in the workspace WILL be deleted.");
    removeProjCnfrm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    removeProjCnfrm.setDefaultButton(QMessageBox::No);

    int ret = removeProjCnfrm.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
            // We want to delete the project - attempt to do so.
        // if the request returns false, notify the user that something went wrong.
        if(!_windowManager->removeProject(_activeProjectName))
        {
            QMessageBox errorMsg;
            errorMsg.setText("Unable to remove this project's directory data.");
            errorMsg.setInformativeText("Please try again or remove the project directory in your workspace manually.");
            errorMsg.setStandardButtons(QMessageBox::Ok);
            errorMsg.exec();
        }
        else
        {
            inactivateProject();
            refreshProjectBrowser();
        }
        break;

        case QMessageBox::No:
            // Don't do anything then
        break;

        default:
            // should never be reached
        break;
    }
}

/*!
 * \brief MainWindow::hideProjectSlot requests to hide the currently active project name by passing it to Window Manager.
 */
void MainWindow::hideProjectSlot()
{
    if(_activeProjectName != QString())
    {
        _windowManager->hideProject(_activeProjectName);
    }
}

/*!
 * \brief MainWindow::viewWorkspaceSlot opens the workspace folder in the default file explorer for that system.
 */
void MainWindow::viewWorkspaceSlot()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(_windowManager->getWorkspace()));
}

/*!
 * \brief MainWindow::openExcelFileSlot opens an excel file that is selected from a \QFileDialog.
 *
 * Uses the \QDesktopServices class to open the file through the default means available on the system.  If a compatible
 * program is not found to open the file, then it displays an error message notifying the user of that problem.
 */
void MainWindow::openExcelFileSlot()
{
    QString excelFilePath = QFileDialog::getOpenFileName(this, QString("Open Excel Report"), _windowManager->getWorkspace(), QString("Excel files (*.xlsm)") );

    if(!excelFilePath.isEmpty())
    {
        if(!QDesktopServices::openUrl(QUrl("file:///" + excelFilePath)))
        {
            QMessageBox errorMsg;
            errorMsg.setText("Compatible version of Excel could not be found on your machine.");
            errorMsg.setInformativeText("Attempt to open the file manually, or see the BioVision website FAQ.");
            errorMsg.setStandardButtons(QMessageBox::Ok);
            errorMsg.exec();
        }
    }
}

/*!
 * Launches the BioVision documentation in your browser.
 *
 * Attempts to open your browser and visit the documentation page of the BioVision website.
 * Else it displays an error message.
 */
void MainWindow::launchDocumentationSlot()
{
    QString web = "http://www.nhdlite.com/BioVision/documentation.html";
    if(!QDesktopServices::openUrl(QUrl(web)))
    {
        QMessageBox errorMsg;
        errorMsg.setText("Documentation could not be launched.");
        errorMsg.setInformativeText("Visit " + web + " for help.");
        errorMsg.setStandardButtons(QMessageBox::Ok);
        errorMsg.exec();
    }
}

/*!
 * Launches the AboutWindow
 *
 * Requests that WindowManager launches the AboutWindow
 */
void MainWindow::launchAboutSlot()
{
    _windowManager->launchAboutWindow();
}

/*!
 * \brief MainWindow::launchOptionsSlot Launches the options dialog.
 *
 * Allows users to edit workspace.
 */
void MainWindow::launchOptionsSlot()
{
    _windowManager->launchOptionsWindow();
}

/*!
 * Adds a video to the currently active project.
 *
 * Checks to make sure that _activeProjectName is not a null \QString
 * If not, it launches a \QFileDialog that gets the file path to the new video.
 * Then, if that path is not a null \QString it requests that WindowManager adds
 * a video with that path to the actie project.
 * Finally, it refreshes the projectBrowser.
 */
void MainWindow::addVideoSlot()
{
    if(_activeProjectName != QString())
    {
        QString videoPath = QFileDialog::getOpenFileName(this, QString("Add Video"), QString("./"), QString("Video files (*.avi *.mov *.mp4)"));

        if(videoPath != QString())
        {
            // If adding is successful, prompt user if they would like to copy the video.
            if(_windowManager->addVideo(_activeProjectName, videoPath, parseVideoName(videoPath)))
            {
                refreshProjectBrowser();

                QMessageBox copyVideoBox;
                copyVideoBox.setText("Do you wish to copy this video to your project directory?");
                copyVideoBox.setInformativeText("Note that this will create an extra copy of the video and use more hard drive space.");
                copyVideoBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                copyVideoBox.setDefaultButton(QMessageBox::No);
                int ret = copyVideoBox.exec();

                switch (ret) {
                // if yes- send the request to window manager, who will forward it to the system.
                  case QMessageBox::Yes:
                    _windowManager->sendVideoCopyRequest(_activeProjectName, videoPath, parseVideoName(videoPath));
                      break;
                  case QMessageBox::No:
                      // User does not want to move the video.  Do nothing.
                      break;
                  default:
                      // should never be reached
                      break;
                }
            }
            else
            {
                QMessageBox errorMsg;
                errorMsg.setText("Problem adding video.");
                errorMsg.setInformativeText("Did you already add this video?");
                errorMsg.setStandardButtons(QMessageBox::Ok);
                errorMsg.exec();
            }
        }
    } 
}

/*!
 * Removes the currently active video from the currently active project.
 *
 * Checks to make sure that _activeVideoName is not a null \QString (assumes that _activeProjectName is not null). <br>
 * Then it requests that WindowManager removes the video. <br>
 * It inactivates the video from the UI. <br>
 * Finally, it refreshes the projectBrowser. <br>
 * Note the assumption that if there is an active video, there will be an active project.
 */
void MainWindow::removeVideoSlot()
{
    if( _activeVideoName != QString() )
    {
        _windowManager->removeVideo(_activeProjectName, _activeVideoName);
        inactivateVideo();
        refreshProjectBrowser();
    }
}

/*!
 * \brief MainWindow::editRegionSlot
 *
 * Opens a window so that the user can edit an existing region, , and gets region data, such as region number, width,
 * height, x1 and y1 coordinates, and pass it to be displayed to the user on the region window.
 */
void MainWindow::editRegionSlot()
{
    //get region data, such as region number, width, height, x1 and y1 coordinates, and pass them on the the region window
    int currentVideoTimeInMilliseconds = ui->videoPlayer->currentTime();
    std::vector <int> regionData = _windowManager->getRegionDataForRegionWindow(_activeProjectName, _activeVideoName, _activeRegionName);
    int tempRegionNumber = regionData[0];
    int tempX = regionData[1];
    int tempY = regionData[2];
    int tempWidth = regionData[3];
    int tempHeight = regionData[4];

    _windowManager->launchRegionWindow(_activeProjectName, _activeVideoName, _activeRegionName, currentVideoTimeInMilliseconds, tempRegionNumber, tempX, tempY, tempWidth, tempHeight);
}

/*!
 * Deletes the currently active ragion from the currently active video.
 *
 * Checks to make sure _activeRegionName is not a null \QString (assumes that _activeVideoName and _activeProjectName are not null). <br>
 * Then it requests that WindowManager removes that region. <br>
 * It inactivates the region from the UI <br>
 * Finally, it refreshes projectBrowser. <br>
 * Note the assumption that if there is an active region, there will be an active video and active project.
 */
void MainWindow::deleteRegionSlot()
{
    if( _activeRegionName != QString() )
    {
        _windowManager->deleteRegion(_activeProjectName, _activeVideoName, _activeRegionName);
        inactivateRegion();
        refreshProjectBrowser();
    }
}

/*!
 * This method will be called when the analyze button is pressed.  It will pass this request to window manager,
 * who will forward the request to the system for handling.
 *
 * Checks to make sure that _activeVideoName is not a null \QString. <br>
 * Launches a \QMessageBox detailing the settings for the analyze run, and making the user confirm that they want to analyze. <br>
 *    If the stop time is equal to zero, it analyzes the entire video. <br>
 *    If the stop time is greater than the start time and greater than zero, it analyzes the time range berween the two. <br>
 *    If the stop time is less than the start time, this is an error. Do not analyze. <br>
 * It also allows the user to "cancel" and not analyze the video.
 */
void MainWindow::analyzeSlot()
{ 

    //if there are an equal number of edit times, proceed with analysis
    if((_videoEditTimes.size() % 2) == 0)
    {

        if(_activeVideoName != QString())
        {
            int startSec = qTimeToSeconds(ui->startTime->time());
            int stopSec = qTimeToSeconds(ui->stopTime->time());

            QString editTimeString;
            QString defaultEndMessage;

            //if stopTime is set to default, set it to end of video
            if(ui->stopTime->time() == QTime::fromString("00:00:00", "hh:mm:ss"))
            {
                //defaultEndMessage = ui->totalVidTime->text() + " (Defaults To Full Runtime)";
                defaultEndMessage = ui->totalVidTime->text() + " (Default)";
                QString tempStopString = ui->totalVidTime->text();
                QTime tempStopQTime = QTime::fromString(tempStopString, "hh:mm:ss");
                stopSec = qTimeToSeconds(tempStopQTime);
            }
            else
            {
                defaultEndMessage =  ui->stopTime->time().toString();
            }

            if(_videoEditTimes.size() == 0)
            {
                editTimeString = "\n  No Edit Times Selected.\n";
            }
            else
            {
                editTimeString += "\n  Stop Analyzing:   Start Again:";

                for(unsigned int i = 0; i < _videoEditTimes.size(); i++)
                {

                    if( (i % 2) == 0)
                    {
                        editTimeString += ("\n  (" + _videoEditTimes[i].toString() + ")           (");
                    }
                    else
                    {
                        editTimeString += (_videoEditTimes[i].toString() + ")");
                    }

                }

            }
            //add all text describing the current analysis settings, which will be shown on the AnalyzeCheckDialog
            QString detailedText = "Analysis Settings: \n";
            if(ui->actionOutput_Images->isChecked())
            {
                detailedText += "-Output Images: Yes \n";
                switch(getImageOutputSizeSelected())
                {
                    case 1:
                        detailedText += " Saved Image Resolution: Native \n";
                        break;
                    case 2:
                        detailedText += " Saved Image Resolution: Small \n";
                        break;
                    default:
                        detailedText += " Saved Image Resolution: Medium \n";
                        break;
                }
            }
            else
            {
                detailedText += "-Output Images: No \n";
            }
            if(ui->actionFull_Frame_Analysis->isChecked())
            {
                detailedText += "-Full Frame Analysis: Yes \n";
            }
            else
            {
                detailedText += "-Full Frame Analysis: No \n";
            }
            detailedText += "-Start time: " + ui->startTime->time().toString() + "\n"
                                                       "-End time:  " + defaultEndMessage + "\n"
                                "-Edit Time(s):" + editTimeString;

            // Prepare settings for an Analyze run
            int previewSpeed = getPreviewSpeedSelected();
            int previewSize = getPreviewWindowSizeSelected();
            int imageOutputSize = getImageOutputSizeSelected();
            bool isOutputImages = ui->actionOutput_Images->isChecked();
            bool isFullFrameAnalysis = ui->actionFull_Frame_Analysis->isChecked();
            // send a request for analyze through to the system if it has passed.


            //set edit times to a deque to be sent through the system for analysis
            std::deque<int> vidEditTimesInSecondsAsADeque;
            for(unsigned int i = 0; i<_videoEditTimesInSeconds.size(); i++)
            {
                vidEditTimesInSecondsAsADeque.push_back( _videoEditTimesInSeconds[i] );
            }

            //invert the slider value so that the higher the slider value the user sets is,
            //the more sensitive the motion analysis will be
            int sensitivitySliderValue = (99 - ui->thresholdSlider->value());

            // If it is not a preview analyze, show the dialog and ask for experiment name.
            if(!ui->previewCheckbox->isChecked())
            {
                // Prompt the user to be sure about analysis- also input the run name.  Display analyze settings.
                if(_windowManager->launchAnalyzeCheckDialog(_activeProjectName, _activeVideoName, detailedText))
                {
                    _windowManager->sendAnalyzeRequest(_activeProjectName, _activeVideoName, startSec, stopSec, vidEditTimesInSecondsAsADeque, sensitivitySliderValue, ui->previewCheckbox->isChecked(),
                                                        previewSpeed, previewSize, imageOutputSize, isOutputImages, isFullFrameAnalysis);
                }
            }
            // if it is a preview analyze, just send the request- do not show the dialog.
            else
            {
                _windowManager->sendAnalyzeRequest(_activeProjectName, _activeVideoName, startSec, stopSec, vidEditTimesInSecondsAsADeque, sensitivitySliderValue, ui->previewCheckbox->isChecked(),
                                                    previewSpeed, previewSize, imageOutputSize, isOutputImages, isFullFrameAnalysis);
            }
        }
        else
        {
            // Display an error message to the user telling them to select a video.
            QMessageBox noVid;
            noVid.setText("Please select a video.");
            noVid.exec();
        }

    }
    else//else user has not set a resume point for each stop point in an analysis, display error window
    {
        QMessageBox oddNumberOfEditPoints;
        oddNumberOfEditPoints.setText("Each Point Where An Analysis Is Temporarily Stopped Must Be Followed Be Another Point Where The Analysis Resumes. Please Add Or Remove An Edit Point");
        oddNumberOfEditPoints.setStandardButtons(QMessageBox::Ok);
        oddNumberOfEditPoints.setDefaultButton(QMessageBox::Ok);

        oddNumberOfEditPoints.exec();
    }
}

/*!
 * \brief MainWindow::cancelSlot cancels a current background task.  This could be a file copy, but most likely will be
 * an analyze operation.
 */
void MainWindow::cancelSlot()
{
    _windowManager->cancelTask();
}

/*!
 * \brief MainWindow::setProgress Called from window manager to set the progress of a given task.  Could be a video copy,
 * analyze, etc.
 *
 * \param progress the current progress, as an int where 0<=progress<=100
 */
void MainWindow::setProgress(int progress)
{
    ui->progressBar->setValue(progress);
}

/*!
 * \brief MainWindow::thresholdChangedSlot when the slider is changes values, call this function to
 * update the value in the threshold value text box.
 *
 * \param value The value to set in the text box.
 */
void MainWindow::thresholdChangedSlot(int value)
{
    QString s;
    s = s.number(value);
    ui->thresholdValue->setText(s);
    _sensitivity = value;
}

/*!
 * \brief MainWindow::moveSliderSlot moves the slider if the user edits the value in the text box.
 *
 * \param value the value to set in the box.
 */
void MainWindow::moveSliderSlot(QString value)
{
    QRegExp regExp("^[0-9]{0,2}$");

    int val;

    if(!regExp.exactMatch(value)){
        val = _sensitivity;
        ui->thresholdValue->setText(QString::number(_sensitivity));
    }
    else
    {
        val = value.toInt();

        if(val<=0)
            ui->thresholdSlider->setValue(0);
        else if((val>0) && (val<100))
            ui->thresholdSlider->setValue(val);
        else
            ui->thresholdSlider->setValue(100);

        _sensitivity = val;
    }
}

/*!
 * Plays or pauses the videoPlayer.  Related to Phonon video player module.
 *
 * If the playButton is checked, it plays the video. <br>
 * Else it pauses the video.
 */
void MainWindow::playPauseSlot()
{
    if(ui->PlayButton->isChecked())
    {
        ui->videoPlayer->raise();
        ui->videoPlayer->play();
        ui->PlayButton->setIcon(QIcon(":/images/Pause.png"));
        ui->selectRegionButton->setChecked(false);
    }
    else
    {
        ui->videoPlayer->pause();
        ui->PlayButton->setIcon(QIcon(":/images/Play.png"));
    }
}

/*!
 * \brief MainWindow::setTotalTimeSlot sets the total time of the video to the total time label widget.
 *
 * Conversion logic contained in the function convertMilToHourMinSec.
 *
 * \param totalTime The length of the video.
 *
 * \see MainWindow::convertMilToHourMinSec
 */
void MainWindow::setTotalTimeSlot(qint64 totalTime)
{
    QString formattedTime = convertMilToHourMinSec(totalTime);
    ui->totalVidTime->setText(formattedTime);
}

/*!
 * \brief MainWindow::updateCurrentTimeSlot called continuously during video playback.  Updates the current time
 * label widget to show how far the user is in the video.
 *
 * Conversion logic contained in the function convertMilToHourMinSec.
 *
 * \param currentTime The current time of the video.
 *
 * \see MainWindow::convertMilToHourMinSec
 */
void MainWindow::updateCurrentTimeSlot(qint64 currentTime)
{
    QString formattedTime = convertMilToHourMinSec(currentTime);
    ui->curVidTime->setText(formattedTime);
}

/*!
 * \brief MainWindow::convertMilToHourMinSec converts a give qint64 value representing time in milliseconds to formatted
 * time in the format hours:min:sec as such: 00:00:00
 *
 * \param time The time in milliseconds to be converted.
 * \return A \QString that is formatted in the proper format of hours:mins:secs
 */
QString MainWindow::convertMilToHourMinSec(qint64 time)
{
    QString hoursString;
    QString minutesString;
    QString secondsString;

    // Convert the time allotments to readable time.
    int hours = time / (1000*60*60);
    int minutes = (time % (1000*60*60)) / (1000*60);
    int seconds = ((time % (1000*60*60)) % (1000*60)) / 1000;

    // Process into QStrings for readability
    if(hours==0)
        hoursString="00:";
    else
        hoursString="0"+QString::number(hours)+":";

    if(minutes<10)
    {
        if(minutes==0)
            minutesString="00:";
        else
            minutesString="0"+QString::number(minutes)+":";
    }
    else
        minutesString=QString::number(minutes)+":";

    if(seconds<10)
        secondsString="0"+QString::number(seconds);
    else
        secondsString=QString::number(seconds);

    return hoursString+minutesString+secondsString;
}

/*!
 * Handles clicks on the project browser.  If an \QTreeWidgetItem in the tree is pressed, we make it the active item.
 *
 * The method of making an item active differs depending on whether the item is a project, video, or region- so three
 * methods are used to do that.
 */
void MainWindow::projectBrowserPressedSlot(QTreeWidgetItem *item, int counter)
{
    // counter needs to be passed because of Qt framework, but it is not used.
    // This function will flag a compiler warning.
    Qt::MouseButtons buttons = QApplication::mouseButtons();
    if((buttons.testFlag(Qt::LeftButton)) || (buttons.testFlag(Qt::RightButton)))
    {
        //Handle a left click on "item"
        if(item->type() == PROJECT)
        {
            makeProjectActive(item);
        }
        else if(item->type() == VIDEO)
        {
            makeVideoActive(item);
        }
        else if(item->type() == REGION)
        {
            makeRegionActive(item);
        }
    }
}

/*!
 * Handles double clicks on an item in the project browser.  Same as a normal click for all items except regions.
 *
 * If a region is double clicked, it launches the region window to allow editing.
 */
void MainWindow::projectBrowserDoubleClickedSlot(QTreeWidgetItem *item, int counter)
{
    // counter needs to be passed because of Qt framework, but it is not used.
    // This function will flag a compiler warning.

    // Handle double click on "item"
    if(item->type() == PROJECT)
    {
        makeProjectActive(item);
    }
    else if(item->type() == VIDEO)
    {
        makeVideoActive(item);
    }
    else if(item->type() == REGION)
    {
        //Make region active and launch a RegionWindow for it
        makeRegionActive(item);
        editRegionSlot();
    }
}

/*!
 * Handles right clicks on items in the project browser and generates a context menu based on what item type was
 * right clicked.
 *
 * If a project was right clicked, allow the user to: Add a video to it, save it, or remove it. <br>
 * If a video was right clicked, allow the user to: Analyze it, or remove it. <br>
 * If a region was right clicked, allow the user to: Edit it or remove it. <br>
 * Note that each of these actions references a slot.
 */
void MainWindow::projectBrowserRightClickedSlot(const QPoint& pos)
{
     QTreeWidgetItem *item = ui->projectBrowser->itemAt(pos);

     if(!item)
         return;

     QPoint globalPos = ui->projectBrowser->mapToGlobal(pos);
     QMenu myMenu;

     if(item->type() == PROJECT)
     {
         myMenu.addAction("Add Video", this, SLOT(addVideoSlot()));
         myMenu.addAction("Save Project", this, SLOT(saveProjectSlot()));
         myMenu.addAction("Remove Project", this, SLOT(removeProjectSlot()));
         myMenu.addAction("Hide Project", this, SLOT(hideProjectSlot()));
     }
     else if(item->type() == VIDEO)
     {
         myMenu.addAction("Analyze Video", this, SLOT(analyzeSlot()));
         myMenu.addAction("Remove Video", this, SLOT(removeVideoSlot()));
     }
     else if(item->type() == REGION)
     {
         myMenu.addAction("Edit Region", this, SLOT(editRegionSlot()));
         myMenu.addAction("Remove Region", this, SLOT(deleteRegionSlot()));
     }

     myMenu.exec(globalPos);
}

/*!
 * \brief MainWindow::makeProjectActive makes the project represented by item active.
 *
 * Inactivates all other projects then if the item is a project, makes it active. <br>
 * Also enables adding video, saving, and saving as for that project.
 *
 * \param item A \QTreeWidgetItem that was either clicked on by the user or the parent of the video that was clicked on or the parent of the parent of the region that was clicked on.
 */
void MainWindow::makeProjectActive(QTreeWidgetItem *item)
{
    inactivateProject();
    if(item->type() == PROJECT)
    {
        _activeProjectName = item->text(0);
        ui->actionAdd_Video->setEnabled(true);
        ui->actionAdd_Video->setText(QString("Add Video To ") + _activeProjectName);
        ui->actionSave_Project->setEnabled(true);
        ui->actionSave_Project->setText(QString("Save ") + _activeProjectName);
        ui->actionSave_Project_As->setEnabled(true);
        ui->actionSave_Project_As->setText(QString("Save ") + _activeProjectName + QString(" As"));
    }
}

/*!
 * Called when a video is clicked or double clicked.  "Activates" a video- highlights it and loads it into the video player.
 *
 * It begins by inactivating the currently active video.
 * Also sets up the resolution converters for regions and allows the video
 * to be removed through the toolbar menu.  If the video is already activated, does nothing. It also
 * activates the parent project.
 *
 * \param item is the \QTreeWidgetItem that was clicked.
 *
 * \see MainWindow::inactivateVideo
 * \see MainWindow::makeProjectActive
 */
void MainWindow::makeVideoActive(QTreeWidgetItem *item)
{
    if(item->type() == VIDEO)
    {
        inactivateVideo();
        if(item->parent()->text(0) != _activeProjectName)
        {
            makeProjectActive(item->parent());
        }

        // If this video is NOT the active video (or if the active video is empty), then activate the new one.
        if((_activeVideoName=="") || (!(_activeVideoName.contains(item->text(0)))))
        {
            //Dustin Added, reset all time edit information when active video is changed
            QTime resetTime = QTime::fromString("00:00:00", "hh:mm:ss");
            _previousStartTime = resetTime;
            _previousEndTime = resetTime;
            ui->startTime->setTime(resetTime);
            ui->stopTime->setTime(resetTime);
            _videoEditTimes.clear();
            _videoEditTimesInSeconds.clear();
            _editTimeDrawingPositions.clear();
            ui->actionAdd_Video_Edit_Point->setText("Set A Time to Stop Analyzing");
            ui->actionRemove_Last_Edit_Point->setEnabled(false);

            _activeVideoName = item->text(0);
            ui->actionRemove_Video->setEnabled(true);
            ui->actionRemove_Video->setText(QString("Remove ") + _activeVideoName + QString(" from ") + _activeProjectName);

            //Phonon
            _mediaSource = new Phonon::MediaSource( _windowManager->getVideoPath(_activeProjectName, _activeVideoName) );
            _mediaSource->setAutoDelete(true);//Free memory when _mediaSource is deleted.
            ui->videoPlayer->load( *_mediaSource );

            // Video Player will update the elapsed time once a second.
            ui->videoPlayer->mediaObject()->setTickInterval(1000);

            ui->videoPlayer->play(); //Show first frame.
            ui->videoPlayer->pause(); //But don't go farther.

            //Setup resolution-related properties, so regions can be handled.

            //Commented out for final release
            //qDebug() << "About to get video width and height";

            _xResolution = _windowManager->getVideoWidth( _activeProjectName, _activeVideoName);
            _yResolution = _windowManager->getVideoHeight( _activeProjectName, _activeVideoName);

            //Commented out for final release
            //qDebug() << "getVideoWidth Returns " << _xResolution << "\ngetVideoHeight Returns " << _yResolution;
            //qDebug() << "Got video width and height about to setup converters.";

            setupResolutionConverters();


            //Dustin Added, change all active time information for the new video that has been selected
            std::vector <QTime> tempTimes = _windowManager->getAllVideoEditTimes(_activeProjectName, _activeVideoName);

            QTime tempStop = tempTimes.back();
            tempTimes.pop_back();
            QTime tempStart = tempTimes.back();
            tempTimes.pop_back();

            _previousStartTime = tempStart;
            ui->startTime->setTime(tempStart);

            _previousEndTime = tempStop;
            ui->stopTime->setTime(tempStop);


            for(unsigned int i = 0; i < tempTimes.size(); i++)
            {
                _videoEditTimes.push_back(tempTimes[i]);
                int tempTimeInSeconds = qTimeToSeconds(tempTimes[i]);
                _videoEditTimesInSeconds.push_back(tempTimeInSeconds);
                _editTimeDrawingPositions.push_back(calculateEditTimeSliderDrawingPosition(float(tempTimeInSeconds)));
            }

            //set the menu options on the GUI properly when a video in selected
            if((_videoEditTimes.size() % 2) != 0)
            {
                ui->actionAdd_Video_Edit_Point->setText("Set A Time to Start Analyzing Again");
            }
            if(_videoEditTimes.size() > 0)
            {
                ui->actionRemove_Last_Edit_Point->setEnabled(true);
            }
            //update the lines drawn for the edit times
            ui->centralWidget->update();
        }
        // Otherwise we do nothing- the current video will stay.
    }
}

/*!
 * \brief MainWindow::makeRegionActive makes a region active.
 *
 * Fist, it inactivates whatever the current region is then, if the item passed to it is a region,
 * it makes that region the active one and enables actions associated with regions. It will also
 * activate the parent video.
 *
 * \param item is a \QTreeWidgetItem that represents a region.
 *
 * \see MainWindow::inactivateRegion
 * \see MainWindow::makeVideoActive
 */
void MainWindow::makeRegionActive(QTreeWidgetItem *item)
{
    if(item->type() == REGION)
    {
        inactivateRegion();
        if(item->parent()->text(0) != _activeVideoName)
        {
            makeVideoActive(item->parent());
        }
        _activeRegionName = item->text(0);
        ui->actionDelete_Region_From_Video->setEnabled(true);
        ui->actionDelete_Region_From_Video->setText(QString("Remove ") + _activeRegionName + QString(" from ") + _activeVideoName);
    }
}

/*!
 * \brief MainWindow::inactivateProject inactivates the currently active project.
 *
 * Sets MainWindow::_activeProjectName to a null \QString and disables all
 * actions associated with projects. It also inactivates any child video.
 *
 * \see MainWindow::inactivateVideo
 */
void MainWindow::inactivateProject()
{
    _activeProjectName = QString();
    ui->actionAdd_Video->setEnabled(false);
    ui->actionAdd_Video->setText(QString("Add Video To Project"));
    ui->actionSave_Project->setEnabled(false);
    ui->actionSave_Project->setText("Save Project");
    ui->actionSave_Project_As->setEnabled(false);
    ui->actionSave_Project_As->setText("Save Project As");
    inactivateVideo();
}

/*!
 * \brief MainWindow::inactivateVideo inactivates the currently active video.
 *
 * Sets MainWindow::_activeVideoName to a null \QString and disables any actions
 * associated with active videos. Also inactivates any child region.
 *
 * \see MainWindow::inactivateRegion
 */
void MainWindow::inactivateVideo()
{
    _activeVideoName = QString();
    ui->actionRemove_Video->setEnabled(false);
    ui->actionRemove_Video->setText(QString("Remove Video From Project"));
    inactivateRegion();
}

/*!
 * \brief MainWindow::inactivateRegion inactivates the currently active region.
 *
 * Sets MainWindow::_activeRegionName to a null \QString and disables any actions
 * associated with active regions.
 */
void MainWindow::inactivateRegion()
{
    _activeRegionName = QString();
    ui->actionDelete_Region_From_Video->setEnabled(false);
    ui->actionDelete_Region_From_Video->setText(QString("Remove Region From Video"));
}

/*!
 * \brief MainWindow::getAllProjects called in MainWindow's constructor.  Gets all of the projects to be auto-loaded from
 * WindowManager and then adds them to the Project Browser.
 */
void MainWindow::getAllProjects()
{
    std::vector<Project*> projects = _windowManager->getAllProjects();
    for(unsigned int i = 0; i < projects.size(); i++)
    {
        ui->projectBrowser->addTopLevelItem( projects[i]->getQTreeWidgetItem() );
    }
}

/*!
 * MainWindow::refreshProjectBrowser() calls getAllProjects() to refresh the project browser and display additions
 * or changes.
 */
void MainWindow::refreshProjectBrowser()
{
    //Clear out the projectBrowser
    ui->projectBrowser->clear();

    //And recreate it
    getAllProjects();
    QTreeWidgetItem* projectItem;
    for(int i=0; i<ui->projectBrowser->topLevelItemCount(); i++)
    {
        projectItem = ui->projectBrowser->topLevelItem(i);
        if( projectItem->text(0) == _activeProjectName )
        {
            projectItem->setExpanded(true);
            QTreeWidgetItem* videoItem;
            for(int j=0; j<projectItem->childCount(); j++)
            {
                videoItem = projectItem->child(j);
                if(videoItem->text(0) == _activeVideoName)
                {
                    videoItem->setExpanded(true);
                }
            }
        }
    }
}

/*!
 * \brief MainWindow::qTimeToSeconds converts a \QTime into an integer representing that amount of time in seconds.
 * \param time is a \QTime representing some amount of time
 * \return An integer representing the same amount of time in seconds.
 */
int MainWindow::qTimeToSeconds(QTime time)
{
    return 3600 * time.hour() + 60 * time.minute() + time.second();
}

/*!
 * \brief MainWindow::mousePressEvent handles the events raised when a mouse button is pressed.
 */
void MainWindow::mousePressEvent(QMouseEvent *mouseEvent)
{
    if(ui->selectRegionButton->isChecked() && isMouseEventInVideoPlayer(mouseEvent) && _activeVideoName != QString())
    {
        _isRegionBeingDrawn = true;
        _regionBeginX = mouseEvent->x() - (ui->PlayerFrame->x() + ui->videoPlayer->x());
        #if defined WIN32
            //Windows has that menu bar as a part of the window.
            //That leads to calculation errors if you don't account for it.
            _regionBeginY = mouseEvent->y() - (ui->PlayerFrame->y() + ui->videoPlayer->y()) - ui->menuBar->height();
        #else
            _regionBeginY = mouseEvent->y() - (ui->PlayerFrame->y() + ui->videoPlayer->y());
        #endif
    }
}

/*!
 * \brief MainWindow::mouseReleaseEvent handles an event that is raised when the mouse is released.
 *
 * First checks to see if a region should be being created.
 * Then if so, it sets the location of the event to the ending of the region coordinates.
 * It converts the coordinates from on-screen coordinates to their proper location on the video.
 * Lastly, it calls WindowManager::launchRegionWindow to finally create the region.
 *
 * \param mouseEvent is a \QMouseEvent that is raised when the mouse is released.
 *
 * \see MainWindow::uiXToFrameX
 * \see MainWindow::uiYToFrameY
 * \see WindowManager::launchRegionWindow
 */
void MainWindow::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    if( ui->selectRegionButton->isChecked() && _isRegionBeingDrawn)
    {
        //Commented out for final release
        //qDebug() << "Now we should create a region.";

        _regionEndX = mouseEvent->x() - (ui->PlayerFrame->x() + ui->videoPlayer->x());
        #if defined WIN32
            //Windows has that menu bar as a part of the window.
            //That leads to calculation errors if you don't account for it.
            _regionEndY = mouseEvent->y() - (ui->PlayerFrame->y() + ui->videoPlayer->y()) - ui->menuBar->height();
        #else
            _regionEndY = mouseEvent->y() - (ui->PlayerFrame->y() + ui->videoPlayer->y());
        #endif


        int x,y,width,height;
        QString num;


        //Adds a region as a child to a video.
        for(int i=0; i<ui->projectBrowser->topLevelItemCount(); i++)
        {
            if(_activeProjectName == ui->projectBrowser->topLevelItem(i)->text(0))
            {
                for(int j=0; j<ui->projectBrowser->topLevelItem(i)->childCount(); j++)
                {
                    if(ui->projectBrowser->topLevelItem(i)->child(j)->text(0) == _activeVideoName)
                    {
                        num = num.number(ui->projectBrowser->topLevelItem(i)->child(j)->childCount()+1);
                    }
                }
            }
        }

        // Gather region coordinates.
        // Make sure we separate upper left corner and lower right corner.
        x = min(_regionBeginX, _regionEndX);
        y = min(_regionBeginY, _regionEndY);
        width = max(_regionBeginX, _regionEndX) - x;
        height = max(_regionBeginY, _regionEndY) - y;


        // Call window manager to create the region for us and pass it the data.
        int currentVideoTimeInMilliseconds = ui->videoPlayer->currentTime();
        int newRegionNumber = _windowManager->getNumberOfRegionsInVideo(_activeProjectName, _activeVideoName);
        _windowManager->launchRegionWindow(_activeProjectName, _activeVideoName, "", currentVideoTimeInMilliseconds,
                                     newRegionNumber,
                                     uiXToFrameX(x),
                                     uiYToFrameY(y),
                                     uiXToFrameX(x+width) - uiXToFrameX(x),
                                     uiYToFrameY(y+height) - uiYToFrameY(y)
                                     );
    }

    _isRegionBeingDrawn = false;
}

/*!

 * \brief MainWindow::resizeEvent handles an event that is raised when the window is resized.
 *
 * Adjusts the size and location of the frames then calls functions to adjust the size and location of
 * each frame's elements. Finally, sets up the resolution converters.
 *
 * \param resizeEvent is a \QResizeEvent raised when the window is resized.
 *
 * \see MainWindow::moveAndResizeToolbarElements
 * \see MainWindow::moveAndResizePlayerElements
 * \see MainWindow::moveAndResizeStatusElements
 * \see MainWindow::moveAndResizeCarouselElements
 * \see MainWindow::moveAndResizePlayerToolsElements
 * \see MainWindow::moveAndResizeFileElements
 * \see MainWindow::setupResolutionConverters
 */
void MainWindow::resizeEvent(QResizeEvent *resizeEvent)
{
    // Start by moving and resizing Frames
    // All setGeometry calls are of the format
    // functionCall(
    //               x location
    //               y location
    //               width
    //               height
    //              );
    ui->ToolbarFrame->setGeometry(
                                   10,
                                   10,
                                   this->width() - 20,
                                   ui->ToolbarFrame->height()
                                  );
    ui->PlayerFrame->setGeometry(
                                  ui->FileFrame->width() + ui->FileFrame->x() + 10,
                                  ui->ToolbarFrame->y() + ui->ToolbarFrame->height() + 10,
                                  this->width() - ui->FileFrame->width() - 30,
                                  this->height() - ui->ToolbarFrame->height() - ui->CarouselFrame->height() - ui->StatusFrame->height() - ui->PlayerToolsFrame->height() - ui->statusBar->height() - 70
                                 );
    ui->PlayerToolsFrame->setGeometry(
                                       ui->PlayerFrame->x(),
                                       ui->PlayerFrame->y() + ui->PlayerFrame->height() + 10,
                                       ui->PlayerFrame->width(),
                                       ui->PlayerToolsFrame->height()
                                      );
    ui->FileFrame->setGeometry(
                                10,
                                ui->ToolbarFrame->y() + ui->ToolbarFrame->height() + 10,
                                ui->FileFrame->width(),
                                this->height() - ui->ToolbarFrame->height() - ui->CarouselFrame->height() - ui->StatusFrame->height() - ui->statusBar->height() - 60
                               );
    ui->CarouselFrame->setGeometry(
                                    10,
                                    ui->FileFrame->y() + ui->FileFrame->height() + 10,
                                    this->width() - 20,
                                    ui->CarouselFrame->height()
                                  );
    ui->StatusFrame->setGeometry(
                                    10,
                                    ui->CarouselFrame->y() + ui->CarouselFrame->height() + 10,
                                    this->width() - 20,
                                    ui->StatusFrame->height()
                                 );

    //Move elements within frames to their proper locations
    //and resize any elements that need resizing.
    moveAndResizeToolbarElements();
    moveAndResizePlayerElements();
    moveAndResizeStatusElements();
    moveAndResizeCarouselElements();
    moveAndResizePlayerToolsElements();
    moveAndResizeFileElements();

    //And setup the resolution converters, so that, if a video is loaded, regions are still handled correctly.
    setupResolutionConverters();
}


/*!
 * \brief MainWindow::moveAndResizeToolbarElements moves and resizes the elements in the ToolbarFrame.
 * It adjusts the width of the sensitivity slider and moves other elements to fit.
 */
void MainWindow::moveAndResizeToolbarElements()
{
    ui->thresholdSlider->setGeometry(
                                      ui->thresholdSlider->x(),
                                      ui->thresholdSlider->y(),
                                      ui->ToolbarFrame->width() - ui->selectRegionButton->width() - ui->Analyze->width() - ui->previewCheckbox->width() - ui->startTime->width() - ui->startTime->width() - ui->label->width() - ui->thresholdValue->width() - ui->startLabel->width() - ui->stopLabel->width() - 65,
                                      ui->thresholdSlider->height()
                                    );
    if(ui->thresholdSlider->width() > 0)
    {
        ui->thresholdValue->setGeometry(
                                          ui->thresholdSlider->x() + ui->thresholdSlider->width() + 10,
                                          ui->thresholdValue->y(),
                                          ui->thresholdValue->width(),
                                          ui->thresholdValue->height()
                                       );
    }
}


/*!
 * \brief MainWindow::moveAndResizePlayerElements moves and resizes the elements in the PlayerFrame.
 * It makes the videoPlayer take up the entire frame.
 */
void MainWindow::moveAndResizePlayerElements()
{
    ui->videoPlayer->setGeometry( 0,0, ui->PlayerFrame->width(), ui->PlayerFrame->height());
}

/*!
 * \brief MainWindow::moveAndResizeStatusElements moves and resizes elements in the StatusFrame.
 */
void MainWindow::moveAndResizeStatusElements()
{
    #if defined WIN32
        ui->progressBar->setGeometry(0,0, ui->StatusFrame->width() - 100, ui->StatusFrame->height() - 3);
        ui->cancel->setGeometry(ui->progressBar->x() + ui->progressBar->width() + 10, 0, 80, ui->StatusFrame->height() -  2);
    #else
        ui->progressBar->setGeometry(10, 6, ui->StatusFrame->width() - 100, ui->StatusFrame->height());
        ui->cancel->setGeometry(ui->progressBar->x() + ui->progressBar->width() + 10, 0, 80, 30);
    #endif
}

/*!
 * \brief MainWindow::moveAndResizeelElements moves and resizes elements in the CarouselFrame
 */
void MainWindow::moveAndResizeCarouselElements()
{
    ui->declarativeView->setGeometry(0,0, ui->CarouselFrame->width(), ui->CarouselFrame->height());
}

/*!
 * \brief MainWindow::moveAndResizePlayerToolsElements moves and resizes elements in the PlayerToolsFrame
 * It adjusts the width of the seekSlider.
 */
void MainWindow::moveAndResizePlayerToolsElements()
{
    ui->seekSlider->setGeometry(
                                 ui->seekSlider->x(),
                                 ui->seekSlider->y(),
                                 ui->PlayerToolsFrame->width() - ui->curVidTime->width() - ui->PlayButton->width() - ui->totalVidTime->width() - ui->volumeSlider->width() - 60,
                                 ui->seekSlider->height()
                                );
    ui->totalVidTime->setGeometry(
                                    ui->seekSlider->x() + ui->seekSlider->width() + 10,
                                    ui->totalVidTime->y(),
                                    ui->totalVidTime->width(),
                                    ui->totalVidTime->height()
                                  );
    ui->volumeSlider->setGeometry(
                                    ui->totalVidTime->x() + ui->totalVidTime->width() + 10,
                                    ui->volumeSlider->y(),
                                    ui->volumeSlider->width(),
                                    ui->volumeSlider->height()
                                  );
}


/*!
 * \brief MainWindow::moveAndResizeFileElements moves and resizes element sof the FileFrame
 */
void MainWindow::moveAndResizeFileElements()
{
    ui->projectBrowser->setGeometry(0,0, ui->FileFrame->width(), ui->FileFrame->height());
}

/*!
 * \brief MainWindow::isMouseEventInVideoPlayer checks to see if a \QMouseEvent is within the videoPlayer.
 * \param mouseEvent is the \QMouseEvent that will be checked.
 * \return true if the event is within the videoPlayer. False otherwise.
 */
bool MainWindow::isMouseEventInVideoPlayer(QMouseEvent *mouseEvent)
{
    int vidPlayerLeft = ui->videoPlayer->x() + ui->PlayerFrame->x();
    int vidPlayerRight = vidPlayerLeft + ui->videoPlayer->width();
    int vidPlayerTop = ui->videoPlayer->y() + ui->PlayerFrame->y();
    int vidPlayerBottom = vidPlayerTop + ui->videoPlayer->height();
    if(mouseEvent->x() >= vidPlayerLeft &&
       mouseEvent->x() <= vidPlayerRight &&
       mouseEvent->y() >= vidPlayerTop &&
       mouseEvent->y() <= vidPlayerBottom )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*!
 * \brief MainWindow::parseVideoName gets the video name when it is just given a video path.
 *
 * \param videoPath a path to a video on the system.
 *
 * \return a string containing the video name that was parsed from the path.
 */
QString MainWindow::parseVideoName(QString videoPath)
{
    QString s = videoPath.mid(videoPath.lastIndexOf('/')+1, videoPath.size() - videoPath.lastIndexOf('/') - 1);

    //Commented out for final release
    //qDebug() << s;

    return s;
}

/*!
 * Converts an x value on the ui to an x value on the frame of a video.
 *
 * Takes an x value reprsenting the x-location of a mouse click relative to the location of the videoPlayer
 * and converts it to the same x-location on the actual video frame. It uses converters that were setup whenever
 * a new video is made active via the MainWindow::setupResolutionConverters() function.
 *
 * \param uiX An integer representing the x location of a mouse click relative to the location of the videoPlayer.
 * \return An integer representing the x location of the mouse click on the actual frame of the video.
 *
 * \see MainWindow::setupResolutionConverters()
 *
 */
int MainWindow::uiXToFrameX(int uiX)
{
    int out = roundToNearest( _xConverter * ((double)uiX - (_xBlackSpace / (double)2)) );
    out = max(0, out);
    out = min(_xResolution, out);
    return out;
}

/*!
 * Converts an y value on the ui to an y value on the frame of a video.
 *
 * Takes an y value reprsenting the y-location of a mouse click relative to the location of the videoPlayer
 * and converts it to the same y-location on the actual video frame. It uses converters that were setup whenever
 * a new video is made active via the MainWindow::setupResolutionConverters() function.
 *
 * \param uiY An integer representing the y location of a mouse click relative to the location of the videoPlayer.
 * \return An integer representing the y location of the mouse click on the actual frame of the video.
 *
 * \see MainWindow::setupResolutionConverters()
 *
 */
int MainWindow::uiYToFrameY(int uiY)
{
    int out = roundToNearest( _yConverter * ((double)uiY - (_yBlackSpace / (double)2)) );
    out = max(0, out);
    out = min(_yResolution, out);
    return out;
}

/*!
 * Converts an x value on the actual video frame to an x value on the ui.
 *
 * Takes an x value reprsenting the x-location of a point and converts it to the same x-location on
 * the ui relative to the videoPlayer. That is, if a point is 3/4 the way across a video, it will convert
 * it to 3/4 the way across the part of the videoPlayer that is not black space. It uses converters that were setup whenever
 * a new video is made active via the MainWindow::setupResolutionConverters() function.
 *
 * \param FrameX An integer representing the x location of a point on a video.
 * \return An integer representing the x location of the point on the videoPlayer.
 *
 * \see MainWindow::setupResolutionConverters()
 *
 */
int MainWindow::frameXToUiX(int frameX)
{
    int out = roundToNearest( ((double)frameX + _xBlackSpace/ (double)2) / _xConverter );
    return out;
}

/*!
 * Converts an y value on the actual video frame to an y value on the ui.
 *
 * Takes an y value reprsenting the y-location of a point and converts it to the same y-location on
 * the ui relative to the videoPlayer. That is, if a point is 3/4 the way down a video, it will convert
 * it to 3/4 the way down the part of the videoPlayer that is not black space. It uses converters that were setup whenever
 * a new video is made active via the MainWindow::setupResolutionConverters() function.
 *
 * \param FrameY An integer representing the y location of a point on a video.
 * \return An integer representing the y location of the point on the videoPlayer.
 *
 * \see MainWindow::setupResolutionConverters()
 *
 */
int MainWindow::frameYToUiY(int frameY)
{
    int out = roundToNearest( ((double)frameY + _yBlackSpace/ (double)2) / _yConverter );
    return out;
}

/*!
 * \brief MainWindow::min returns the smaller of two numbers.
 * \param x is any number.
 * \param y is any number.
 * \return the value of the smaller of the two numbers, or if they are the same, that value.
 */
inline double MainWindow::min(double x, double y)
{
    return (x <= y) ? x : y;
}

/*!
 * \brief MainWindow::max returns the larger of two numbers.
 * \param x is any number.
 * \param y is any number.
 * \return the value of the larger of the two numbers, or if they are the same, that value.
 */
inline double MainWindow::max(double x, double y)
{
    return (x >= y) ? x : y;
}

/*!
 * \brief MainWindow::roundToNearest rounds a number to the nearest integer.
 * \param x is any number.
 * \return the integer that is closest to that number.
 */
inline int MainWindow::roundToNearest(double x)
{
    return (x-(int)x >= 0.5) ? ((int)x)+1 :  (int)x;
}


/*!
 * \brief MainWindow::setupResolutionConverters sets up converters so that locations can be
 * translated from on-screen coordinates to their matching on-video location.
 */
void MainWindow::setupResolutionConverters()
{
    // Simple scaling between videoPlayer size and actual video sze.
    _xScaling = (double)ui->videoPlayer->width() / _xResolution;
    _yScaling = (double)ui->videoPlayer->height() / _yResolution;

    // Number of pixels of blackspace.
    // In case aspect ratios do not match.
    _xBlackSpace = max(0, ui->videoPlayer->width() - _xResolution * _yScaling);
    _yBlackSpace = max(0, ui->videoPlayer->height() - _yResolution * _xScaling);

    // "Converters"
    // A scaling of of actual video size and videoPlayer size after taking into account black space.
    _xConverter = _xResolution / (double)(ui->videoPlayer->width() - _xBlackSpace);
    _yConverter = _yResolution / (double)(ui->videoPlayer->height() - _yBlackSpace);
}

/*!
 * Initialize the result carousel on the Main GUI Window
 */
//Molly Added
//basic result carousel set up
void MainWindow::initializeResultCarousel()
{
    //hook up qml code

    #ifdef WIN32
      ui->declarativeView->setSource(QUrl::fromLocalFile("tmp\\carouselEx.qml"));
    #else
      ui->declarativeView->setSource(QUrl::fromLocalFile("tmp/carouselEx.qml"));
    #endif

    //facilitate communication from qml to c++
    ui->declarativeView->rootContext()->setContextProperty("MainWindow", this);
    //resize to fit correctly
    //ui->declarativeView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    //hook up carousel object to the GUI
    _resultCarouselObject = ui->declarativeView->rootObject();
}

/*!
 * Slot that catches signals passed through the system during an analysis, and uses the
 * passed information to display images onto the carousel
 *
 * \param name The name of the image file to display onto the carousel
 * \param index The index of the image to be displayed in the carousel
 */
//Molly Added
//called each time the signal is sent that a frame image has been added to the tmp folder
void MainWindow::addImageToCarousel(QString name, QString index)
{
    QVariant returnedValue;
    //Pass QML the image name and display
    QMetaObject::invokeMethod(_resultCarouselObject, "passImageInfo",
            Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, name), Q_ARG(QVariant, index));

    //Commented out for final release
    //qDebug() << "addImageToCarousel returned: " << returnedValue.toString();
}

/*!
 * Called when new analysis is run. Clears the carousel of existing frames
 */
//Molly added
//called when new analysis is run to clear carousel of existing frames
void MainWindow::clearCarousel()
{

    QVariant returnedValue;
       QMetaObject::invokeMethod(_resultCarouselObject, "clearCarousel",
               Q_RETURN_ARG(QVariant, returnedValue));

    //Commented out for final release
    //qDebug() << "clearCarousel returned: " << returnedValue.toString();
}

/*!
 * Called when user double clicks a specific frame in the carousel.
 * Displays a pop-up window with the native resolution version of the clicked carousle frame.
 */
//Molly added
//called when user double clicks a specific frame in the carousel
void MainWindow::expandCarouselFrame(int index, QString url)
{
    //Commented out for final release
    //qDebug() << "MainWindow::expandCarouselFrame successfully called. Index: " << index << " URL: " << url;

    //launch window
    _windowManager->launchEnlargedFrameWindow(url);
}

/*!
 * Called when the user adds an edit point to the current active video.
 * Adds the edit point and passes it to project manager if it is valid,
 * and throws up a warning window to the user if the new selected point is invalid
 */
void MainWindow::addEditPointSlot()
{
    QString tempString = ui->curVidTime->text();
    QTime newEditTime = QTime::fromString(tempString, "hh:mm:ss");
    int newTimeInSeconds = qTimeToSeconds(newEditTime);

    //if user tries to add an edit time when no video is selected
    if(QTime::fromString(ui->totalVidTime->text(), "hh:mm:ss") != QTime::fromString("00:00:00", "hh:mm:ss"))
    {
        //If the user tries to add an edit time at the start of the video, display error window
        if(newEditTime != QTime::fromString("00:00:00", "hh:mm:ss"))
        {
            //if new edit time is greater than current analysis start time
            if(newEditTime > ui->startTime->time())
            {
                //if new edit time is less than current analysis end time
                if(newEditTime < ui->stopTime->time() || ( ui->stopTime->time() == QTime::fromString("00:00:00", "hh:mm:ss") && newEditTime < QTime::fromString(ui->totalVidTime->text(), "hh:mm:ss") ) )
                {
                    //if there are no edit times selected yet, add the new edit time
                    if(_videoEditTimes.size() == 0)
                    {
                        _videoEditTimes.push_back(newEditTime);
                        _videoEditTimesInSeconds.push_back(newTimeInSeconds);
                        ui->actionRemove_Last_Edit_Point->setEnabled(true);

                        //Add a new mark to the Video Timeline indicating where this new edit point has been added
                        int editPointDrawPosition;
                        editPointDrawPosition = calculateEditTimeSliderDrawingPosition(float(newTimeInSeconds));
                        _editTimeDrawingPositions.push_back(editPointDrawPosition);

                        //store this time in projectManager
                        _windowManager->addVideoEditTime(_activeProjectName, _activeVideoName, newEditTime);
                    }
                    else if(newEditTime > _videoEditTimes.back() )//if the new edit time is greater than the last one, add the new edit time
                    {
                        _videoEditTimes.push_back(newEditTime);
                        _videoEditTimesInSeconds.push_back(float(newTimeInSeconds));
                        ui->actionRemove_Last_Edit_Point->setEnabled(true);

                        //Add a new mark to the Video Timeline indicating where this new edit point has been added
                        int editPointDrawPosition;
                        editPointDrawPosition = calculateEditTimeSliderDrawingPosition(newTimeInSeconds);
                        _editTimeDrawingPositions.push_back(editPointDrawPosition);

                        //store this time in projectManager
                        _windowManager->addVideoEditTime(_activeProjectName, _activeVideoName, newEditTime);
                    }
                    else
                    {
                        QMessageBox editLessThanLastEdit;
                        editLessThanLastEdit.setText("Edit Time Must Be Greater Than The Previous Edit Time Selected, Please Select A New Edit Time.");
                        editLessThanLastEdit.setStandardButtons(QMessageBox::Ok);
                        editLessThanLastEdit.setDefaultButton(QMessageBox::Ok);
                        editLessThanLastEdit.exec();
                    }

                }
                else//new edit time is greater than current analysis end time, display error window
                {
                    QMessageBox editGreaterThanEnd;
                    editGreaterThanEnd.setText("Edit Time Must Be Less Than The Video Analysis End Time, Please Select A New Edit Time.");
                    editGreaterThanEnd.setStandardButtons(QMessageBox::Ok);
                    editGreaterThanEnd.setDefaultButton(QMessageBox::Ok);
                    editGreaterThanEnd.exec();
                }
            }
            else//new edit time is less than current analysis start time, display error window
            {
                QMessageBox editLessThanStart;
                editLessThanStart.setText("Edit Time Must Be Greater Than The Start Of The Video Analysis, Please Select A New Edit Time.");
                editLessThanStart.setStandardButtons(QMessageBox::Ok);
                editLessThanStart.setDefaultButton(QMessageBox::Ok);
                editLessThanStart.exec();
            }
        }
        else//new edit time is equal to start of video, display error window
        {
            QMessageBox editCantEqualStart;
            editCantEqualStart.setText("First Edit Time Must Be Greater Than The Start Of The Video, Please Select A New Edit Time.");
            editCantEqualStart.setStandardButtons(QMessageBox::Ok);
            editCantEqualStart.setDefaultButton(QMessageBox::Ok);
            editCantEqualStart.exec();
        }
    }
    else//when no video is currently selected, display error window
    {
        QMessageBox noVideoSelected;
        noVideoSelected.setText("No Video Currently Selected, Please Select A Video To Add Edit Times.");
        noVideoSelected.setStandardButtons(QMessageBox::Ok);
        noVideoSelected.setDefaultButton(QMessageBox::Ok);
        noVideoSelected.exec();
    }


    if((_videoEditTimes.size() % 2) != 0)
    {
        ui->actionAdd_Video_Edit_Point->setText("Set A Time to Start Analyzing Again");
    }
    else
    {
        ui->actionAdd_Video_Edit_Point->setText("Set A Time to Stop Analyzing");
    }
    //update the lines drawn for the edit times
    ui->centralWidget->update();
}

/*!
 * Called when the user removes an edit point from the current active video.
 * Deletes the edit point, calls through project manager to remove that point
 * from the project data
 */
void MainWindow::removeLastEditPointSlot()
{
    _videoEditTimes.pop_back();
    _videoEditTimesInSeconds.pop_back();
    _editTimeDrawingPositions.pop_back();

    //remove this time from projectManager
    _windowManager->removeLastVideoEditTime(_activeProjectName, _activeVideoName);

    if(_videoEditTimes.size() == 0)
    {
        ui->actionRemove_Last_Edit_Point->setEnabled(false);
    }

    if((_videoEditTimes.size() % 2) != 0)
    {
        ui->actionAdd_Video_Edit_Point->setText("Set A Time to Start Analyzing Again");
    }
    else
    {
        ui->actionAdd_Video_Edit_Point->setText("Set A Time to Stop Analyzing");
    }
    //update the lines drawn for the edit times
    ui->centralWidget->update();
}

/*!
 * Called when start time is changed. If new start time input is valid, pass that data to be stored in project manager.
 * If it is invalid, display warning window to the user
 */
void MainWindow::startTimeChangeCheckerSlot()
{
    //if the start time does not equal 00:00:00, or start time equals 00:00:00 and the last start time was also 00:00:00, continue
    if(ui->startTime->time() != QTime::fromString("00:00:00") || ( ui->startTime->time() == QTime::fromString("00:00:00") && _previousStartTime != QTime::fromString("00:00:00")))
    {
        //if no video is currently selected, display an error window
        if(QTime::fromString(ui->totalVidTime->text(), "hh:mm:ss") != QTime::fromString("00:00:00"))
        {

            //if time is less than total video runtime
            if(ui->startTime->time() < QTime::fromString(ui->totalVidTime->text(), "hh:mm:ss"))
            {
                //if start time is less than user chosen end time, which includes the end time of 00:00:00 and default for
                //analyzing the video till the end
                if(ui->startTime->time() < ui->stopTime->time() || ui->stopTime->time() == QTime::fromString("00:00:00", "hh:mm:ss"))
                {

                    if(_videoEditTimes.size() == 0)//if there are no edit times selected, its ok to update the start time
                    {
                        _previousStartTime = ui->startTime->time();

                        //store start and end time data in projectManager
                        _windowManager->setVideoStartAndStopTimes(_activeProjectName, _activeVideoName, ui->startTime->time(), ui->stopTime->time());
                    }
                    else if(ui->startTime->time() < _videoEditTimes[0])//if the start time is still less than the first edit time, its ok to update start time
                    {
                        _previousStartTime = ui->startTime->time();

                        //store start and end time data in projectManager
                        _windowManager->setVideoStartAndStopTimes(_activeProjectName, _activeVideoName, ui->startTime->time(), ui->stopTime->time());
                    }
                    else//start time greater than first edit time, display error window, revert to previously selected time
                    {
                        QMessageBox startGreaterThanFirstEdit;
                        startGreaterThanFirstEdit.setText("Analysis Start Time Must Be Less Than Your First Selected Edit Time, Please Select An Earlier Start Time, Or Increase Your First Edit Time.");
                        startGreaterThanFirstEdit.setStandardButtons(QMessageBox::Ok);
                        startGreaterThanFirstEdit.setDefaultButton(QMessageBox::Ok);

                        startGreaterThanFirstEdit.exec();
                        ui->startTime->setTime(QTime::fromString("00:00:00", "hh:mm:ss"));
                        _previousStartTime = QTime::fromString("00:00:00", "hh:mm:ss");
                    }

                }
                else//start time greater than end time, display error window, revert to previously selected time
                {
                    QMessageBox startGreaterThanEnd;
                    startGreaterThanEnd.setText("Analysis Start Time Must Be Less Than Analysis Endtime, Please Select An Earlier Start Time, Or Increase Your End Time.");
                    startGreaterThanEnd.setStandardButtons(QMessageBox::Ok);
                    startGreaterThanEnd.setDefaultButton(QMessageBox::Ok);
                    startGreaterThanEnd.exec();

                    ui->startTime->setTime(_previousStartTime);
                }

            }
            else//start time greater than video runtime, display error window, revert to previously selected time
            {
                QMessageBox startTimeInvalid;
                startTimeInvalid.setText("Analysis Start Time Must Be Less Than Total Video Runtime, Please Select An Earlier Start Time.");
                startTimeInvalid.setStandardButtons(QMessageBox::Ok);
                startTimeInvalid.setDefaultButton(QMessageBox::Ok);
                startTimeInvalid.exec();

                ui->startTime->setTime(_previousStartTime);
            }
        }
        else//no video is curently selected, display error window, revert to previously selected time
        {
            QMessageBox noVideoSelected;
            noVideoSelected.setText("No Video Selected, Please Select a Video To Set an Analysis Start Time.");
            noVideoSelected.setStandardButtons(QMessageBox::Ok);
            noVideoSelected.setDefaultButton(QMessageBox::Ok);
            noVideoSelected.exec();

            ui->startTime->setTime(QTime::fromString("00:00:00", "hh:mm:ss"));
            _previousStartTime = QTime::fromString("00:00:00", "hh:mm:ss");
        }

    }

}

/*!
 * Called when end time is changed. If new end time input is valid, pass that data to be stored in project manager.
 * If it is invalid, display warning window to the user
 */
void MainWindow::endTimeChangeCheckerSlot()
{
    //if the stop time does not equal 00:00:00, or stop time equals 00:00:00 and the last stop time was also 00:00:00, continue
    if(ui->stopTime->time() != QTime::fromString("00:00:00") || ( ui->stopTime->time() == QTime::fromString("00:00:00") && _previousEndTime != QTime::fromString("00:00:00")))
    {
        //if no video is currently selected, display an error window
        if(QTime::fromString(ui->totalVidTime->text(), "hh:mm:ss") != QTime::fromString("00:00:00"))
        {
            //if end time is less than or equal to total video runtime
            if(ui->stopTime->time() <= QTime::fromString(ui->totalVidTime->text(), "hh:mm:ss"))
            {
                //if end time is greater than currently selected start time
                if(ui->stopTime->time() > ui->startTime->time() || (ui->stopTime->time() == QTime::fromString("00:00:00") && ui->startTime->time() == QTime::fromString("00:00:00")))
                {
                    //if there are no edit times currently selected, change to new end time
                    if(_videoEditTimes.size() == 0)
                    {
                        _previousEndTime = ui->stopTime->time();

                        //store start and end time data in projectManager
                        _windowManager->setVideoStartAndStopTimes(_activeProjectName, _activeVideoName, ui->startTime->time(), ui->stopTime->time());
                    }
                    else if(ui->stopTime->time() > _videoEditTimes.back())//if new end time is greater than last edit time, change to new end time
                    {
                        _previousEndTime = ui->stopTime->time();

                        //store start and end time data in projectManager
                        _windowManager->setVideoStartAndStopTimes(_activeProjectName, _activeVideoName, ui->startTime->time(), ui->stopTime->time());
                    }
                    else//new end time less than or equal to last edit time, display error window, revert to previously selected time
                    {
                        QMessageBox endTimeLessThanEditTime;
                        endTimeLessThanEditTime.setText("End Time For An Analsis Is Less Than The Last Edit Time Currently Selected. Make Sure That All Edit Times Are Always Less Than Your Analyze End Time. \nNOTE: The Default End Time of 00:00:00 Will Be Set To The End Time Of The Video Automatically At The Start Of An Analysis.");
                        endTimeLessThanEditTime.setStandardButtons(QMessageBox::Ok);
                        endTimeLessThanEditTime.setDefaultButton(QMessageBox::Ok);
                        endTimeLessThanEditTime.exec();

                        ui->stopTime->setTime(_previousEndTime);
                    }
                }
                else//end time less than or equal to start time, display error window, revert to previously selected time
                {
                    QMessageBox endTimeLessThanStartTime;
                    endTimeLessThanStartTime.setText("End Time For An Analsis Is Less Than The Currently Selected Analysis Start Time. Decrease Your Analysis Start Time So That It Is Always Less Than Your End Time. \nNOTE: The Default End Time of 00:00:00 Will Be Set To The End Time Of The Video Automatically At The Start Of An Analysis.");
                    endTimeLessThanStartTime.setStandardButtons(QMessageBox::Ok);
                    endTimeLessThanStartTime.setDefaultButton(QMessageBox::Ok);
                    endTimeLessThanStartTime.exec();

                    ui->stopTime->setTime(_previousEndTime);
                }

            }
            else//end time greater than end of video, display error window, set end time to end of video
            {
                QMessageBox endTimeGreaterThanVideoEnd;
                endTimeGreaterThanVideoEnd.setText("End Time For An Analsis Is Greater Than Video End Time, Analysis End Time Now Set To The End Of The Video.");
                endTimeGreaterThanVideoEnd.setStandardButtons(QMessageBox::Ok);
                endTimeGreaterThanVideoEnd.setDefaultButton(QMessageBox::Ok);
                endTimeGreaterThanVideoEnd.exec();

                ui->stopTime->setTime(QTime::fromString(ui->totalVidTime->text(), "hh:mm:ss"));
                _previousEndTime = QTime::fromString(ui->totalVidTime->text(), "hh:mm:ss");
            }

        }
        else//no video is curently selected, display error window, revert to previously selected time
        {
            QMessageBox noVideoSelected;
            noVideoSelected.setText("No Video Selected, Please Select a Video To Set an Analysis End Time.");
            noVideoSelected.setStandardButtons(QMessageBox::Ok);
            noVideoSelected.setDefaultButton(QMessageBox::Ok);
            noVideoSelected.exec();

            ui->stopTime->setTime(QTime::fromString("00:00:00", "hh:mm:ss"));
            _previousEndTime = QTime::fromString("00:00:00", "hh:mm:ss");
        }
    }

}

/*!
 * Paint Event for the Main GUI Window. Used to draw edit points onto the the main window video slider
 */
void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QPen redPen(QColor(255, 0, 0));
    redPen.setWidth(5);
    QPen greenPen(QColor(0, 255, 0));
    greenPen.setWidth(5);

    for(unsigned int i = 0; i < _editTimeDrawingPositions.size(); i++)
    {
        float xPosition = calculateEditTimeSliderDrawingPosition((float)_videoEditTimesInSeconds[i]);
        if(i % 2 == 0)
        {
            painter.setPen(redPen);
            painter.drawLine( QPointF(xPosition, ui->PlayerToolsFrame->y()), QPointF(xPosition, ui->PlayerToolsFrame->y() + ui->PlayerToolsFrame->height()) );

        }
        else
        {
            painter.setPen(greenPen);
            painter.drawLine( QPointF(xPosition, ui->PlayerToolsFrame->y()), QPointF(xPosition, ui->PlayerToolsFrame->y() + ui->PlayerToolsFrame->height()) );
        }
    }
}

/*!
 * \brief MainWindow::closeEvent is called when the MainWindow closes.
 *
 * \param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    _windowManager->promptSaveProjects();
    event->accept();
}

/*!
 * Calculates the position for an edit time when it is added to a video
 *
 * \param currentEditTImeInSeconds The edit time whose position you will be drawing to the GUI
 *
 * \return returns the GUI position to draw this time
 */
float MainWindow::calculateEditTimeSliderDrawingPosition(float currentEditTimeInSeconds)
{
    QString tempString = ui->totalVidTime->text();
    QTime totalRunTime = QTime::fromString(tempString, "hh:mm:ss");
    float totalRunTimeInSeconds = qTimeToSeconds(totalRunTime);

    float percentIntoTotalRunTime = (currentEditTimeInSeconds / totalRunTimeInSeconds);

    #if defined WIN32

        //percent of pixels through the current edit time it through the GUI slider
        //We subtract 14 because the rectangle does not exactly match the slider
        percentIntoTotalRunTime = percentIntoTotalRunTime * (ui->seekSlider->width() - 14);

        //edit point draw position = how far we are on the slider + slider start point
        //We add 6 because the rectangle does not exactly match the slider
        float newEditPointDrawPosition = roundToNearest(percentIntoTotalRunTime + ui->seekSlider->x() + ui->PlayerToolsFrame->x() + 6);
    #else
        //percent of pixels through the current edit time it through the GUI slider
        //We subtract 22 because the rectangle does not exactly match the slider
        percentIntoTotalRunTime = percentIntoTotalRunTime * (ui->seekSlider->width() - 22);

        //edit point draw position = how far we are on the slider + slider start point
        //We add 6 because the rectangle does not exactly match the slider
        float newEditPointDrawPosition = roundToNearest(percentIntoTotalRunTime + ui->seekSlider->x() + ui->PlayerToolsFrame->x() + 6);
    #endif

    return newEditPointDrawPosition;
}

/*!
 * Called when preview speed is changed. Sets preview speed to slow
 */
void MainWindow::previewSlowSpeedCheckSlot()
{
    ui->actionSlowPreviewSpeed->setChecked(true);
    ui->actionMediumPreviewSpeed->setChecked(false);
    ui->actionFastPreviewSpeed->setChecked(false);
}

/*!
 * Called when preview speed is changed. Sets preview speed to medium
 */
void MainWindow::previewMediumSpeedCheckSlot()
{
    ui->actionSlowPreviewSpeed->setChecked(false);
    ui->actionMediumPreviewSpeed->setChecked(true);
    ui->actionFastPreviewSpeed->setChecked(false);
}

/*!
 * Called when preview speed is changed. Sets preview speed to fast
 */
void MainWindow::previewFastSpeedCheckSlot()
{
    ui->actionSlowPreviewSpeed->setChecked(false);
    ui->actionMediumPreviewSpeed->setChecked(false);
    ui->actionFastPreviewSpeed->setChecked(true);
}

/*!
 * Called when preview size is changed. Sets preview size to Native resolution
 */
void MainWindow::previewWindowSizeNativeSlot()
{
    ui->actionNativeWindowSize->setChecked(true);
    ui->actionMediumWindowSize->setChecked(false);
    ui->actionLargeWindowSize->setChecked(false);
}

/*!
 * Called when preview size is changed. Sets preview size to resize to medium
 */
void MainWindow::previewWindowSizeMediumSlot()
{
    ui->actionNativeWindowSize->setChecked(false);
    ui->actionMediumWindowSize->setChecked(true);
    ui->actionLargeWindowSize->setChecked(false);
}

/*!
 * Called when preview size is changed. Sets preview size to resize to large
 */
void MainWindow::previewWindowSizeLargeSlot()
{
    ui->actionNativeWindowSize->setChecked(false);
    ui->actionMediumWindowSize->setChecked(false);
    ui->actionLargeWindowSize->setChecked(true);
}

/*!
 * Called when analyze saved image size is changed. Sets preview size to Native Resolution
 */
void MainWindow::analyzeImageSaveNativeSlot()
{
    ui->actionNativeSavedImageSize->setChecked(true);
    ui->actionSmallSavedImageSize->setChecked(false);
    ui->actionMediumSavedImageSize->setChecked(false);
}

/*!
 * Called when analyze saved image size is changed. Sets preview size to resize to small
 */
void MainWindow::analyzeImageSaveSmallSlot()
{
    ui->actionNativeSavedImageSize->setChecked(false);
    ui->actionSmallSavedImageSize->setChecked(true);
    ui->actionMediumSavedImageSize->setChecked(false);
}

/*!
 * Called when analyze saved image size is changed. Sets preview size to resize to medium
 */
void MainWindow::analyzeImageSaveMediumSlot()
{
    ui->actionNativeSavedImageSize->setChecked(false);
    ui->actionSmallSavedImageSize->setChecked(false);
    ui->actionMediumSavedImageSize->setChecked(true);
}

/*!
 * Passes back preview speed selected by the user in the GUI
 *
 * \return Returns the preview playback speed selected
 */
int MainWindow::getPreviewSpeedSelected()
{
    if(ui->actionSlowPreviewSpeed->isChecked() == true)
    {
        return 1;
    }
    else if(ui->actionMediumPreviewSpeed->isChecked() == true)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

/*!
 * Passes back preview size selected by the user in the GUI
 *
 * \return Returns the preview playback size selected
 */
int MainWindow::getPreviewWindowSizeSelected()
{
    if(ui->actionNativeWindowSize->isChecked() == true)
    {
        return 1;
    }
    else if(ui->actionMediumWindowSize->isChecked() == true)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

/*!
 * Passes back the image output size selected by the user in the GUI
 *
 * \return Returns the image output size selected
 */
int MainWindow::getImageOutputSizeSelected()
{
    if(ui->actionNativeSavedImageSize->isChecked() == true)
    {
        return 1;
    }
    else if(ui->actionSmallSavedImageSize->isChecked() == true)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}
