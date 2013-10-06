///////////////////////////////////////////////////////////
//  DetailAnalyzer.cpp
//  Implementation of the Class DetailAnalyzer
//  Created on:      15-Jan-2013 5:49:27 PM
///////////////////////////////////////////////////////////

#include "DetailAnalyzer.h"
#include <iostream>
#include <fstream>

/*!
 * Default constructor.
 */
DetailAnalyzer::DetailAnalyzer()
{

}

/*!
 * Constuctor that initializes all of the data needed for a run through DetailAnalyzer.
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
 * \param previewSpeed: Determines the playback speed of the preview
 * \param previewSize: Determines the size of the preview window, used to enlarge low resolution videos for easier viewing
 */
DetailAnalyzer::DetailAnalyzer(std::vector<int>* xCoords, std::vector<int>* yCoords, std::vector<int>* widths, std::vector<int>* heights,
                   std::vector<int>* thresholds, QString videoFilePath, int startSecond, int stopSecond, std::deque<int> videoEditTimesInSeconds,
                   int motionSensitivity, int previewSpeed, int previewSize)
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
    _previewSpeed = previewSpeed;
    _previewSize = previewSize;

    _isCancelled = false;

    // Set the message for detailed Analyzer.
    setMessage("Preview Analyze is currently running.");
}

/*!
 * Destructor.  Free all region data vectors.
 */
DetailAnalyzer::~DetailAnalyzer()
{
    delete (_regionXCoords);
    delete (_regionYCoords);
    delete (_regionWidths);
    delete (_regionHeights);
    delete (_regionThresholds);
}

/*!
 * The slot that is run on the QThread object.  It contains the actual analyze algorithm
 * definition.
 *
 * Uses the CV object to call all of the open-cv related methods.  Updates progress via the progressSignal,
 * and calls the finish signal when it is done.  **Important** also instantiate any objects that the thread will use
 * in this function, otherwise it will be a conflict (different thread affinities, as this is the only method run on
 * the new thread).
 */
void DetailAnalyzer::startSlot()
{
    previewAnalyze();

    emit finished();
}

/*!
 * Parses and adapts data passed by the GUI into format used by preview analysis functions, and runs loop to complete an analysis of a video
 *
 * Takes no parameters, all data needed is referenced form class level variables that store data Passed from GUI
 *
 * \return Returns nothing, a video analysis is complete once this function terminates
 *
 * \see DetailAnalyzer() constructor for data and values that are passed from the GUI
 */
void DetailAnalyzer::previewAnalyze()
{
    std::vector <OpenCV::regionData> regionData;

    std::vector < std::vector<int> > regionCoordinates;

    std::vector <int> regionTemp;

    float percentChangeInRegion[10];

    //set 2 region coordinates using start point passed from system. and region height and width passed from system
    if(_regionHeights->size() != 0)
    {
        for(unsigned int i = 0; i < _regionHeights->size();  i++)
        {
            regionCoordinates.push_back(regionTemp);

            regionCoordinates[i].push_back( (*_regionXCoords)[i] );
            regionCoordinates[i].push_back( (*_regionYCoords)[i] );
            regionCoordinates[i].push_back( (*_regionXCoords)[i] + (*_regionWidths)[i] );
            regionCoordinates[i].push_back( (*_regionYCoords)[i] + (*_regionHeights)[i] );

            //convert from int to decimal
            percentChangeInRegion[i] = ( (*_regionThresholds)[i] );

            percentChangeInRegion[i] = ( percentChangeInRegion[i] / 100 );
        }
    }
    else
    {
        percentChangeInRegion[0] = 0;
    }

    //copy the video file path as a standard string
    std::string videoFilePath = _videoFilePath.toStdString();
    //Commented out for final release
    //std::cout << videoFilePath.c_str() << " Copied Video File Path\n";


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

        //ignore this warning, this comparison is working properly
        if(videoFileName.find_last_of('/') != -1)
        {
            videoFileName = videoFileName.substr(videoFileName.find_last_of('/') + 1, videoFileName.size() );
            videoFileName = videoFileName.substr(0, (videoFileName.size() - 4) );
        }
        else
        {
            videoFileName = videoFileName.substr(videoFileName.find_last_of('\\') + 1, videoFileName.size() );
            videoFileName = videoFileName.substr(0, (videoFileName.size() - 4) );
        }

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
            regionCoordinates[0][2] = _cvObject.getVideoFrameWidth();;
            regionCoordinates[0][3] = _cvObject.getVideoFrameHeight();

            tempData.regionStartPointX = 0;
            tempData.regionStartPointY = 0;
            tempData.regionEndPointX = _cvObject.getVideoFrameWidth();;
            tempData.regionEndPointY = _cvObject.getVideoFrameHeight();

            regionData.push_back(tempData);
        }

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


        //starts analysis at frame specified by user, otherwise, start at first frame
        if(_startSecond > 0)
        {
            _cvObject.setCurrentVideoTime(_startSecond * 1000);
            currentFrameNumber = _cvObject.getCurrentVideoFrame();
            analysisStartFrame = currentFrameNumber;
        }
        else
        {
            analysisStartFrame = 0;
            currentFrameNumber = 0;
        }

        //set analysis end time based on time chosen by the user
        analysisEndFrame = _stopSecond * _cvObject.getVideoFrameRate();
        _stopSecond = _stopSecond * 1000;

        //set start frame, output path, and video file name in open CV class NOTE: output path is empty for DetailAnalyzer, as it has no output
        _cvObject.initializeStartFrameAndFileName("", videoFileName, analysisStartFrame);

        //set current frame size of video, used for creating image variables for analysis
        _cvObject.initializeFrameSizeSensitivityAndDrawSize(_motionSensitivity);

        //initialize average frame motion image variable
        _cvObject.initializeMovingAverageFrame();

        //set preview options passed by the user
        _cvObject.setPreviewWindowOptions(_previewSize, _previewSpeed);

        //is the current frame to be analyzed a new edit frame chosen by the user
        bool isEditFrame = true;

        //open preview window
        _cvObject.openPreviewWindow();

        //emit a starting signal to show that analysis has begun for very long jobs
        emit progressSignal(1);

        //has an openCV error occured on this run
        bool isErrorThrown = false;

        while(true)
        {
            try
            {
                _cvObject.previewAnalysis(currentFrameNumber, regionCoordinates, regionData, isEditFrame);
            }
            catch(cv::Exception& e)
            {
                //Commented out for final release
                //const char* err_msg = e.what();
                //std::cout << err_msg;

                isErrorThrown = true;
                _isCancelled = true;
                _cvObject.shrinkPreviewWindow();
                _cvObject.deallocateFramesOnError();
                break;
            }

            //if the cancel button is clicked, close the preview window and reset status bar
            if(_isCancelled == true)
            {
                emit progressSignal(0);
                _cvObject.closePreviewWindow();
                break;
            }

            if(_cvObject.getCurrentVideoTime() >= _stopSecond)
            {
                _cvObject.shrinkPreviewWindow();
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

                    //set the video to the user selected time to resume analysis
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
        }

        //if we reach the end of the video, shrink the preview window and reset status bar
        if(_isCancelled == false)
        {
            emit progressSignal(0);
            _cvObject.shrinkPreviewWindow();
        }

        //release average frame data
        _cvObject.deallocateMovingAverageFrame();
        //after the entire video has been analyzed, close video filestream
        _cvObject.closeVideoFile();

        if(isErrorThrown == true)
        {
            emit progressSignal(0);
            //display error window letting user know something is wrong while analyzing this video
            displayErrorMessageSignal();
        }
    }
}
