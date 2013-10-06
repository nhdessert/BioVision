///////////////////////////////////////////////////////////
//  Analyzer.cpp
//  Implementation of the Class Analyzer
//  Created on:      15-Jan-2013 5:49:26 PM
///////////////////////////////////////////////////////////

#include "Analyzer.h"
#include <iostream>
#include <fstream>


/*!
 * Default constructor.
 */
Analyzer::Analyzer()
{

}

/*!
 * Constuctor that initializes all of the data needed for a run through Analyzer.
 *
 * Note that for the vectors containing region data, each indice of a vector cooresponds
 * to a specific region.  So all of xCoords[0], yCoords[0], etc would be for the first region.
 * Also note that no objects can be instantiated here that will be used on the new thread.
 *
 * \param xCoords: X coordinates of regions, in a vector.
 * \param yCoords: Y coordinates of regions, in a vector.
 * \param widths: Widths of regions, in a vector.
 * \param heights: Heights of regions, in a vector.
 * \param thresholds: Thresholds of regions, in a vector.
 * \param videoFilePath: The file path of the video to analyze.
 * \param startSecond: The video time to start analyzing, passed from the GUI
 * \param stopSecond: The video time to start analyzing, passed from the GUI
 * \param videoEditTimesInSeconds: Any edit times passed by the GUI, used to set multiple start/stop times throughout an analysis
 * \param motionSensitivity: Determines the level of motion detected in a video, passed from GUI slider bar
 * \param regionNames: The names of each region created by the user
 * \param imageOutputSize: Value set by the user, determines how much to reduce the size of large image files output during analysis
 * \param isOutputImages: Determines if we are saving image files for a given analysis or not
 * \param isFullFrameAnalysis: Determines whether we are analyzing the entire video frame, or just a sub-area that contains all user created regions
 */
Analyzer::Analyzer(std::vector<int>* xCoords, std::vector<int>* yCoords, std::vector<int>* widths, std::vector<int>* heights,
                   std::vector<int>* thresholds, QString videoFilePath, int startSecond, int stopSecond, std::deque<int> videoEditTimesInSeconds,
                   int motionSensitivity, std::vector<QString>* regionNames, int imageOutputSize, bool isOutputImages, bool isFullFrameAnalysis)
{
    _regionXCoords = xCoords;
    _regionYCoords = yCoords;
    _regionWidths = widths;
    _regionHeights = heights;
    _regionThresholds = thresholds;

    _videoFilePath = videoFilePath;
    _startSecond = startSecond;
    _stopSecond = stopSecond;
    _editPoints = videoEditTimesInSeconds;
    _motionSensitivity = (float)motionSensitivity;
    _regionNames = regionNames;

    _imageOutputSize = imageOutputSize;
    _isOutputImages = isOutputImages;
    _isFullFrameAnalysis = isFullFrameAnalysis;

    //reset bool for when/if the canceled button is clicked.
    _isCancelled = false;

    // Set the message- means this task is running
    setMessage("Analyzer is currently running.");
}

/*!
 * Destructor.  Free all region data vectors.
 */
Analyzer::~Analyzer()
{
    delete (_regionXCoords);
    delete (_regionYCoords);
    delete (_regionWidths);
    delete (_regionHeights);
    delete (_regionThresholds);
    delete (_regionNames);
}

/*!
 * The slot that is run on the QThread object.  It contains the actual analyze algorithm
 * definition.
 *
 * Uses the CV object to call all of the openCV related methods, and calls the finish signal when
 * it is done.  **Important** Instantiate any objects that the thread will use in this function,
 * otherwise it will be a conflict (different thread affinities, as this is the only method run on
 * the new thread).
 */
void Analyzer::startSlot()
{
    //Commented out for final release
    //qDebug()<<"Calling clearTmpDirectory";
    clearTmpDirectory();

    analyze();

    emit finished();
}

/*!
 * This slot is connected to the OpenCV image written signal.  We then forward the image name to
 * BvSystem by way of a signal that was connected in thread manager to this object.  From there System can tell the
 * carousel to load it.
 *
 * \param fileName The name of a newly written image.
 */
void Analyzer::sendImageInfoSlot(QString fileName, QString index)
{
    emit sendImageInfoSignal(fileName, index);
}

void Analyzer::clearCarouselSlot()
{
    emit clearCarouselSignal();
}

/*!

 * \brief Analyzer::clearTmpDirectory Deletes all of the images in the tmp directory and also clears the frame carousel.
*/

void Analyzer::clearTmpDirectory()
{
    //Commented out for final release
    //Clear the carousel
    //qDebug()<<"Calling clearCarouselSlot";
    clearCarouselSlot();

    // Make sure the tmp directory exists and clean it of old runs.
    if(!QDir("tmp").exists())
    {
        QDir().mkdir("tmp");
    }
    else
    {
        QDir dir("tmp/");

        // sets filters on the files retrieved by entry list- only delete jpg files.
        dir.setNameFilters(QStringList() << "*.jpg");
        dir.setFilter(QDir::Files);

        // get the list of jpg files.
        QStringList files = dir.entryList();

        // iterate through and delete them.
        while(files.size() > 0)
        {
            dir.remove(files.first());
            files.removeFirst();
        }

        dir.setNameFilters(QStringList() << "*.txt");
        dir.setFilter(QDir::Files);
        files = dir.entryList();
        while(files.size() > 0)
        {
            dir.remove(files.first());
            files.removeFirst();
        }
    }
}

/*!
 * Parses and adapts data passed by the GUI into format used by analysis functions, and runs loop to complete an analysis of a video
 * Takes no parameters, all data needed is referenced form class level variables that store data Passed from GUI
 *
 * \return Returns nothing, a video analysis is complete once this function terminates
 *
 * \see Analyzer() constructor for data and values that are passed from the GUI
 */
void Analyzer::analyze()
{
    //holds general video data that will be sent to output
    OpenCV::generalVideoData videoInfo;
    //holds data for individual regions that will be sent to output
    std::vector <OpenCV::regionData> regionData;
    //the region coordinates that will be used for analysis
    std::vector < std::vector<int> > regionCoordinates;
    //temporary container for a single regions coordinates, used to fill regionCoordinates
    std::vector <int> regionTemp;

    //holds all of out possible region threshold values
    float percentChangeInRegion[10];

    //set 2 region coordinates using start point passed from system. and region height and width passed from system
    if(_regionHeights->size() != 0)
    {
        for(unsigned int i = 0; i < _regionHeights->size();  i++)
        {
            //add an empty container to hold one regions coordinates
            regionCoordinates.push_back(regionTemp);

            //fill that container with region data
            regionCoordinates[i].push_back( (*_regionXCoords)[i] );
            regionCoordinates[i].push_back( (*_regionYCoords)[i] );
            regionCoordinates[i].push_back( (*_regionXCoords)[i] + (*_regionWidths)[i] );
            regionCoordinates[i].push_back( (*_regionYCoords)[i] + (*_regionHeights)[i] );

            //convert from int to decimal
            percentChangeInRegion[i] = ( (*_regionThresholds)[i] );

            percentChangeInRegion[i] = ( percentChangeInRegion[i] / 100 );
        }
    }
    else//if no regions were selected by the user, set up a default region
    {
        percentChangeInRegion[0] = 0;

        //add a defult region name for output when no regions are selected
        _regionNames->push_back("default");
    }

    //copy the video file path as a standard string
    std::string videoFilePath = _videoFilePath.toStdString();

    //temporary output is stored in the system's "tmp" folder, final output will be saved in the users working directory
    //if they chose to keep analysis data
    std::string outputFilePath = "tmp";

    int lastSlashInPath = videoFilePath.find_last_of('/');

    //if we are on macOS
    if(lastSlashInPath != -1)
    {
        outputFilePath = "tmp/";
    }
    else//windows
    {
        outputFilePath = "tmp\\";
    }


    //if the video file fails to open, send error, otherwise, continue analysis
    if(!_cvObject.openVideoFile(videoFilePath))
    {
        //Commented out for final release
        //std::cout << "Failed to open video file " << videoFilePath << std::endl;
    }
    else//continue to analyze video
    {
        //percent of analysis that is complete
        int percentComplete = 0;

        //will hold the name of the video file we are analizing, without trailing extention
        std::string videoFileName;

        //testing code, parses throught given file path and gets our video file name without extension
        videoFileName = videoFilePath;

        //parse out video file name
        if(videoFileName.find_last_of('/') != -1)//ignore compiler warning, this comparison is working properly
        {
            videoFileName = videoFileName.substr(videoFileName.find_last_of('/') + 1, videoFileName.size() );
            videoFileName = videoFileName.substr(0, (videoFileName.size() - 4) );
        }
        else
        {
            videoFileName = videoFileName.substr(videoFileName.find_last_of('\\') + 1, videoFileName.size() );
            videoFileName = videoFileName.substr(0, (videoFileName.size() - 4) );
        }

        //store video file name for results later
        videoInfo.videoName = videoFileName;

        //video run time information;
        int hoursOfVideo = 0;
        int minutesOfVideo = 0;
        int secondsOfVideo = 0;

        //get the total run time of the video
        _cvObject.getFormattedVideoTime(hoursOfVideo, minutesOfVideo, secondsOfVideo, _cvObject.getNumberOfVideoFrames(), _cvObject.getVideoFrameRate());

        //store meta data for results later
        videoInfo.frameWidthResult = _cvObject.getVideoFrameWidth();
        videoInfo.frameHeightResult = _cvObject.getVideoFrameHeight();
        videoInfo.totalNumberOfFramesResult = _cvObject.getNumberOfVideoFrames();
        videoInfo.frameRateResult = _cvObject.getVideoFrameRate();
        videoInfo.hoursOfRunTimeResult = hoursOfVideo;
        videoInfo.minutesOfRunTimeResult = minutesOfVideo;
        videoInfo.secondsOfRunTimeResult = secondsOfVideo;
        videoInfo.totalVideoRunTimeInSeconds = ( (hoursOfVideo * 3600) + (minutesOfVideo * 60) + secondsOfVideo );
        //will be accumulated over total analysis
        videoInfo.totalFramesPastThreshHold = 0;


        //if at least one region was selected by the user
        if(regionCoordinates.size() != 0)
        {
            //output and region data, and store it for results later
            for(unsigned int regionNum = 0; regionNum < regionCoordinates.size(); regionNum++)
            {
                //store region data for results later
                OpenCV::regionData tempData;
                tempData.regionStartPointX = regionCoordinates[regionNum][0];
                tempData.regionStartPointY = regionCoordinates[regionNum][1];
                tempData.regionEndPointX = regionCoordinates[regionNum][2];
                tempData.regionEndPointY = regionCoordinates[regionNum][3];
                tempData.regionRectangleColor = _cvObject.colorList[regionNum].colorName;
                regionData.push_back(tempData);
            }

        }
        else //if no regions were selected by the user, set one region selecting the whole frame
        {
            OpenCV::regionData tempData;
            std::vector <int> tempCoordinates(4);

            regionCoordinates.push_back(tempCoordinates);

            regionCoordinates[0][0] = 0;
            regionCoordinates[0][1] = 0;
            regionCoordinates[0][2] = _cvObject.getVideoFrameWidth();
            regionCoordinates[0][3] = _cvObject.getVideoFrameHeight();

            tempData.regionStartPointX = 0;
            tempData.regionStartPointY = 0;
            tempData.regionEndPointX = _cvObject.getVideoFrameWidth();
            tempData.regionEndPointY = _cvObject.getVideoFrameHeight();
            tempData.regionRectangleColor = _cvObject.colorList[0].colorName;

            regionData.push_back(tempData);
        }


        //set amount of frame to analyze based on user input
        _cvObject.setFrameAnalysisSize(regionCoordinates, _isFullFrameAnalysis);

        //setup results for changes
        if(regionCoordinates.size() != 0)
        {
            //initialize variables withing the OpenCV class that track pixel changes per region
            int numberOfRegions = regionCoordinates.size();
            _cvObject.initializePixelChangeVariables(numberOfRegions, regionData, percentChangeInRegion, _regionWidths, _regionHeights);
        }
        else //if no regions were selected by the user, set one region selecting the whole frame
        {
            //initialize pixel change variables for a single region
            _cvObject.initializePixelChangeVariables(1, regionData, percentChangeInRegion, _regionWidths, _regionHeights);
        }


        //currently, the video starts at the beginning and analyzes all the way to the end
        //will be used once start and end are passed by th GUI
        double analysisStartFrame = 0;
        double analysisEndFrame = 0;

        //now declared here since code moved out of OpenCV class
        int currentFrameNumber = 0;

        //if the video start time is greater than the beginning of the video, set video start time,
        //and frame number information
        if(_startSecond > 0)
        {
            _cvObject.setCurrentVideoTime(_startSecond * 1000);
            currentFrameNumber = _cvObject.getCurrentVideoFrame();
            analysisStartFrame = currentFrameNumber;
        }
        else//video is being analyzed form the beginning
        {
            analysisStartFrame = 0;
            currentFrameNumber = 0;
        }

        //set analysis end time based on time chosen by the user
        analysisEndFrame = _stopSecond * _cvObject.getVideoFrameRate();
        _stopSecond = _stopSecond * 1000;

        //output file start and end video data
        videoInfo.frameAnalysisStart = analysisStartFrame;
        videoInfo.frameAnalysisEnd = analysisEndFrame;

        //set start frame, output path, and video file name in open CV class
        _cvObject.initializeStartFrameAndFileName(outputFilePath, videoFileName, analysisStartFrame);

        //set current frame size of video, used for creating image variables for analysis
        _cvObject.initializeFrameSizeSensitivityAndDrawSize(_motionSensitivity);

        //initialize average frame motion image variable
        _cvObject.initializeMovingAverageFrame();

        //set image output options based on GUI options chosen
        _cvObject.setAnalyzeOptions(_isOutputImages, _imageOutputSize);

        //is the current frame to be analyzed the first frame of the video, or a new edit frame chosen by the user
        bool isEditFrame = true;

        //emit a starting signal to show that analysis has begun for very long jobs
        emit progressSignal(1);

        //has an openCV error occured on this run
        bool isErrorThrown = false;

        //Main Analysis Loop//
        while(true)
        {
            //path for image files saved during analysis, send this data through the system to the carosel
            QString savedImagePath;

            //analyze current video frame
            try
            {
                savedImagePath = _cvObject.analyzeCurrentFrame(currentFrameNumber, regionCoordinates, videoInfo, regionData, isEditFrame);
            }
            catch(cv::Exception& e)//if an openCV error is caught, end the loop, deallocate data, and send error message/window
            {
                //Commented out for final release
                //const char* err_msg = e.what();
                //std::cout << err_msg;

                isErrorThrown = true;
                _isCancelled = true;
                _cvObject.deallocateFramesOnError();
                break;
            }

            //check if the user has stopped the analysis by clicking a button on the GUI
            if(_isCancelled == true)
            {
                emit progressSignal(0);
                break;
            }
            //if we are at the end of the video, or at the user selected stopping point, end analysis
            if(_cvObject.getCurrentVideoTime() >= _stopSecond)
            {
                break;
            }

            //output current percentage completion
            float framesAnalyzed = currentFrameNumber;
            float lastFrameToAnylize = analysisEndFrame;

            if(percentComplete < int((framesAnalyzed/lastFrameToAnylize) * 100))
            {
                percentComplete = int((framesAnalyzed/lastFrameToAnylize) * 100);

                //Commented out for final release
                //std::cout << "Analysis is " << percentComplete << "% Complete\n";

                //Emit this data to the GUI to update our progress bar
                emit progressSignal(percentComplete);
            }

            //if we are at an edit point set by the user, skip to the next frame they wish to have analyzed
            if(_editPoints.size() != 0)
            {
                //if the current video time is equal/greater than the next edit point
                if(_cvObject.getCurrentVideoTime() >= (_editPoints.front() * 1000))
                {
                    //pop old edit time
                    _editPoints.pop_front();

                    ///set the video to the user selected time to resume analysis
                    _cvObject.setCurrentVideoTime((double) (_editPoints.front() * 1000));
                    currentFrameNumber = _cvObject.getCurrentVideoFrame();

                    //remove analysis resume time
                    _editPoints.pop_front();

                    //we have skipped time in the video, tells frame analysis algorythim to reset
                    //moving frame average for the next frame
                    isEditFrame = true;
                }
                else
                {
                    isEditFrame = false;
                }
            }
            else//Else no edit had taken place, proceed as normal
            {
                isEditFrame = false;
            }

            //if an image was saved this frame, pass its file path back through the system
            if(savedImagePath != "")
            {
                _parseString = savedImagePath.split("-");
                sendImageInfoSlot(savedImagePath, _parseString.at(1).toLocal8Bit().constData());
            }

        }//End While, Main Analysis Loop

        //release average frame data from memory
        _cvObject.deallocateMovingAverageFrame();

        //after the entire video has been analyzed, close the video filestream
        _cvObject.closeVideoFile();

        //if the user did not stop the analysis while it was in progress, output the analysis result data
        if(_isCancelled == false)
        {
            //reset progress bar
            emit progressSignal(0);

            //process the results from an analysis
            Result getResult;
            getResult.exportToText(videoFilePath, outputFilePath, videoInfo, regionData, _regionNames);

            // Prepare the result object to be emitted.
            _result = new Result();
            emit sendResultSignal(_result);
        }

        //if an openCV error has ended the analysis
        if(isErrorThrown == true)
        {
            emit progressSignal(0);
            //display error window letting user know something went wrong while analyzing this video
            displayErrorMessageSignal();
        }
    }
}


