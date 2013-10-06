#include "OpenCV.h"
#include <sstream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <QDir>


using namespace std;
using namespace cv;

//Constructor
OpenCV::OpenCV()
{
    //set all metadata to default values
    this->_numberOfFramesInVideo = 0;
    this->_frameRate = 0;
    this->_frameWidth = 0;
    this->_frameHeight = 0;
    this->_currentFrameNumber = 0;

    //list of colors for each region in a project

    //pink
    colorList[0].r = 255;
    colorList[0].g = 0;
    colorList[0].b = 255;
    colorList[0].colorName = "Pink";

    //yellow
    colorList[1].r = 255;
    colorList[1].g = 255;
    colorList[1].b = 0;
    colorList[1].colorName = "Yellow";

    //teal
    colorList[2].r = 0;
    colorList[2].g = 255;
    colorList[2].b = 255;
    colorList[2].colorName = "Teal";

    //light/lime green
    colorList[3].r = 125;
    colorList[3].g = 255;
    colorList[3].b = 0;
    colorList[3].colorName = "Lime";

    //purple
    colorList[4].r = 120;
    colorList[4].g = 0;
    colorList[4].b = 180;
    colorList[4].colorName = "Purple";

    //orange
    colorList[5].r = 255;
    colorList[5].g = 171;
    colorList[5].b = 61;
    colorList[5].colorName = "Orange";

    //standard blue
    colorList[6].r = 5;
    colorList[6].g = 65;
    colorList[6].b = 230;
    colorList[6].colorName = "Blue";

    //standard green
    colorList[7].r = 75;
    colorList[7].g = 165;
    colorList[7].b = 75;
    colorList[7].colorName = "Green";

    //gray
    colorList[8].r = 160;
    colorList[8].g = 160;
    colorList[8].b = 160;
    colorList[8].colorName = "Gray";

    //lavender
    colorList[9].r = 175;
    colorList[9].g = 150;
    colorList[9].b = 228;
    colorList[9].colorName = "Lavender";

    //red
    //used only for RegionWindow when the user tries to create a region and there are already 10 regions in their current video
    colorList[10].r = 255;
    colorList[10].g = 0;
    colorList[10].b = 0;
    colorList[10].colorName = "Red";

    //sets random number to be appended to saved image files. should insure
    //that image file names for are unique for the frame carousel each run.
    srand(time(NULL));
    std::stringstream converter;
    int randomNumber = rand() % 1000000;
    converter << randomNumber;
    _randomImageNameAddition = converter.str();
}


//Destructor
OpenCV::~OpenCV()
{

}

/*!
 * Opens a video file stream, and collects the meta data for the opened file
 *
 * \param videoFilePath : the directory path to the fideo file you wish to open
 *
 * \return Returns a bool value, true if the file stream was successfully opened, and false otherwise
 *
 * \see collectVideoMetaData() for meta data acquisition
 */
bool OpenCV::openVideoFile(string videoFilePath)
{
    //Commented out for final release
    //cout << "Opening the video file." << endl;

    vidStream.open(videoFilePath);

    //Commented out for final release
    //cout<<videoFilePath << endl;

    if(vidStream.isOpened())
    {
        collectVideoMetaData();
        return true;
    }
    else
    {
        return false;
    }
}

/*!
 * Closes a currently open video file stream
 *
 * \return Returns a bool value, true is the stream was successfully closed, and false otherwise
 */
bool OpenCV::closeVideoFile()
{
    vidStream.release();

    if(!vidStream.isOpened())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*!
 * Collects meta data from the current video stream, and stores it in the current openCV object
 */
void OpenCV::collectVideoMetaData()
{
    this->_numberOfFramesInVideo = vidStream.get(CV_CAP_PROP_FRAME_COUNT);
    this->_frameRate = vidStream.get(CV_CAP_PROP_FPS);
    this->_frameWidth = vidStream.get(CV_CAP_PROP_FRAME_WIDTH);
    this->_frameHeight = vidStream.get(CV_CAP_PROP_FRAME_HEIGHT);
}

/*!
 * Gets a video time in hours, minutes and seconds, based on its frame rate ann the current video frame passed in
 *
 * \param hours: Passed by reference, returns how many hours of run time there are
 * \param minutes: Passed by reference, returns how many minutes of run time there are
 * \param seconds: Passed by reference, returns how many seconds of run time there are
 * \param currentVideoFrame: The video frame that is currently active
 * \param frameRate: Phe frame rate of the video being analyzed
 *
 * \return Peturns nothing, run time is passed back by reference
 */
void OpenCV::getFormattedVideoTime(int &hours, int &minutes, int &seconds, double currentVideoFrame, double frameRate)
{
    hours = (currentVideoFrame / frameRate) / 3600;
    minutes = (currentVideoFrame / frameRate) / 60;
    seconds = (currentVideoFrame / frameRate);

    if(minutes >= 60)
    {
        minutes = minutes - (hours * 60);
    }

    seconds = currentVideoFrame / frameRate;

    if(seconds >= 60)
    {
        seconds = seconds - ( (hours * 3600) + (minutes * 60) );
    }
}

/*!
 * Sets the next frame of the video that will be analyzed
 *
 * \param nextFrameToAnalyze: The frame number of the next frame that will be analyzed
 */
void OpenCV::setCurrentVideoFrame(double nextFrameToAnalyze)
{
    vidStream.set(CV_CAP_PROP_POS_FRAMES, nextFrameToAnalyze);
}

/*!
 * Get function for current video frame
 *
 * \return Returns the number of the next frame that will be analyzed
 */
double OpenCV::getCurrentVideoFrame()
{
    return  vidStream.get(CV_CAP_PROP_POS_FRAMES);
}

/*!
 * Get the current video time in milliseconds
 *
 * \return Returns video time in milliseconds
 */
double OpenCV::getCurrentVideoTime()
{
    return  vidStream.get(CV_CAP_PROP_POS_MSEC);
}

/*!
 * Set the time of the video to continue an analysis from
 *
 * \param newVideoTime: The new time to set the analysis to in milliseconds
 */
void OpenCV::setCurrentVideoTime(double newVideoTime)
{
    vidStream.set(CV_CAP_PROP_POS_MSEC, newVideoTime);
}

/*!
 * Get function for video frame count
 *
 * \return Returns the number of frames in the current video
 */
double OpenCV::getNumberOfVideoFrames()
{
    return this->_numberOfFramesInVideo;
}

/*!
 * Get function for video frame rate
 *
 * \return Returns the frame rate of the video
 */
double OpenCV::getVideoFrameRate()
{
    return this->_frameRate;
}

/*!
 * Get function for video frame width
 *
 * \return Returns the horizontal resolution of each frame of the video
 */
double OpenCV::getVideoFrameWidth()
{
    return this->_frameWidth;
}

/*!
 * Get function for video frame height
 *
 * \return Returns the vertical resolution of each frame of the video
 */
double OpenCV::getVideoFrameHeight()
{
  return this->_frameHeight;
}

/*!
 * Gets the next frame available from the video stream, and sets it to a Matrix variable for analysis
 *
 * \param frameToAnalyze: The Matrix that the frame will be stored in
 *
 * \return Returns nothing, passes back the the Matrix that contains the current frame by reference
 */
void OpenCV::getFrameForAnalysis(Mat& frameToAnalize)
{
    vidStream.read(frameToAnalize);
}

/*!
 * Set the preview window output size if the video is low enough resolution,
 * and set the speed of the preview playback. Based on user input
 *
 * Note: Only videos with standard asspect rations (16:9 or 4:3) will be resized,
 * all non-standard rations will play back at native resolution
 *
 * \param sizeSelected: An int from 1 to 3 set by user selected GUI options, 1 == nativeSize, 2 == mediumSizes, 3 == largeSize
 * \param speedSelected: An int from 1 to 3 set by user selected GUI options, 1 == slow, 2 == medium, 3 == fast
 */
void OpenCV::setPreviewWindowOptions(int sizeSelected, int speedSelected)
{
    if(sizeSelected == 1)
    {
        _previewSizeX = 0;
        _previewSizeY = 0;
    }
    else if(sizeSelected == 2)
    {

        //image is widescreen (16x9)
        if(_frameWidth/_frameHeight > 1.77f && _frameWidth/_frameHeight < 1.8f)
        {
            if(_frameWidth < 600)
            {
                _previewSizeX = 640;
                _previewSizeY = 360;
            }
            else
            {
                _previewSizeX = 0;
                _previewSizeY = 0;
            }
        }
        //image is fullscreen (4x3)
        else if(_frameWidth/_frameHeight > 1.33f && _frameWidth/_frameHeight < 1.4f)
        {
            if(_frameWidth < 600)
            {
                _previewSizeX = 640;
                _previewSizeY = 480;
            }
            else
            {
                _previewSizeX = 0;
                _previewSizeY = 0;
            }
        }
        else //non-standard aspect ratio, stay at native resolution
        {
            _previewSizeX = 0;
            _previewSizeY = 0;
        }
    }
    else
    {
        //image is widescreen (16x9)
        if(_frameWidth/_frameHeight > 1.77f && _frameWidth/_frameHeight < 1.8f)
        {
            if(_frameWidth < 800)
            {
                _previewSizeX = 960;
                _previewSizeY = 540;
            }
            else
            {
                _previewSizeX = 0;
                _previewSizeY = 0;
            }
        }
        //image is fullscreen (4x3)
        else if(_frameWidth/_frameHeight > 1.33f && _frameWidth/_frameHeight < 1.4f)
        {
            if(_frameWidth < 800)
            {
                _previewSizeX = 800;
                _previewSizeY = 600;
            }
            else
            {
                _previewSizeX = 0;
                _previewSizeY = 0;
            }
        }
        else //non-standard aspect ratio, stay at native resolution
        {
            _previewSizeX = 0;
            _previewSizeY = 0;
        }
    }


    if(speedSelected == 1)
    {
        _previewPlaybackSpeed = 150;
    }
    else if(speedSelected == 2)
    {
        _previewPlaybackSpeed = 75;
    }
    else
    {
        _previewPlaybackSpeed = 2;
    }
}

/*!
 * Resize preview window image frames based on settings chosen by the user
 *
 * \param previewImage: The image to be resized
 *
 * \return Returns new resized version of previewImage
 * \see setPreviewWindowOptions() for assingment of resizing values
 */
Mat OpenCV::resizePreviewImage(Mat previewImage)
{
    Mat largerImage = Mat (_previewSizeY, _previewSizeX, CV_8U);
    resize(previewImage, largerImage, largerImage.size(), 0, 0, CV_INTER_LINEAR);

    return largerImage;
}

/*!
 * Set the values for certain analysis output functionality, based on settings passeed by the user
 *
 * \param isOutputImage: User set value from the GUI that determines if we will be saving image output for this analysis
 * \param imageSizeSelected: User set value from the GUI that determine if we will be resizing saved images for this analysis
 */
void OpenCV::setAnalyzeOptions(bool isOutputImage, int imageSizeSelected)
{
    _isOutputingImages = isOutputImage;

    if(imageSizeSelected == 1)
    {
        _outputImageSizeX = 0;
        _outputImageSizeY = 0;
    }
    else if(imageSizeSelected == 2)//SMALL: make image much smaller if it is above a certain size
    {
        if(_frameWidth < 600)
        {
            _outputImageSizeX = 0;
            _outputImageSizeY = 0;
        }
        else
        {
            //save smaller widescreen (16x9) image
            if(_frameWidth/_frameHeight > 1.77f && _frameWidth/_frameHeight < 1.8f)
            {
                _outputImageSizeX = 384;
                _outputImageSizeY = 216;
            }
            //save smaller fullscreen (4x3) image
            else if(_frameWidth/_frameHeight > 1.33f && _frameWidth/_frameHeight < 1.4f)
            {
                _outputImageSizeX = 480;
                _outputImageSizeY = 360;
            }
            else//non-standard aspect ratio, save at native resolution
            {
                _outputImageSizeX = 0;
                _outputImageSizeY = 0;
            }
        }
    }
    else//MEDIUM: make image a bit smaller if it is above a certain size
    {
        if(_frameWidth < 700)
        {
            _outputImageSizeX = 0;
            _outputImageSizeY = 0;
        }
        else
        {
            //save smaller widescreen (16x9) image
            if(_frameWidth/_frameHeight > 1.77f && _frameWidth/_frameHeight < 1.8f)
            {
                _outputImageSizeX = 640;
                _outputImageSizeY = 360;
            }
            //save smaller fullscreen (4x3) image
            else if(_frameWidth/_frameHeight > 1.33f && _frameWidth/_frameHeight < 1.4f)
            {
                _outputImageSizeX = 640;
                _outputImageSizeY = 480;
            }
            else//non-standard aspect ratio, save at native resolution
            {
                _outputImageSizeX = 0;
                _outputImageSizeY = 0;
            }
        }
    }
}

/*!
 * Set the area of the frame to analyze if Full Frame Analysis is disabled. Based on all region selected by the user
 *
 * \param regionCoordinates: Coordinates of all regions selected by the user
 * \param isFullFrameAnalysis: Does the user want to analyze the whole frame, or just a subsection containing all selected regions
 */
void OpenCV::setFrameAnalysisSize(std::vector < std::vector<int> > regionCoordinates, bool isFullFrameAnalysis)
{
    if(isFullFrameAnalysis == false)
    {
        int smallestStartingXCoordinate = regionCoordinates[0][0];
        int smallestStartingYCoordinate = regionCoordinates[0][1];
        int largestStartingXCoordinate = regionCoordinates[0][2];
        int largestStartingYCoordinate = regionCoordinates[0][3];

        if(regionCoordinates.size() > 1)
        {
            for(unsigned int i = 1; i < regionCoordinates.size(); i++)
            {
                if(smallestStartingXCoordinate > regionCoordinates[i][0])
                {
                    smallestStartingXCoordinate = regionCoordinates[i][0];
                }

                if(smallestStartingYCoordinate > regionCoordinates[i][1])
                {
                    smallestStartingYCoordinate = regionCoordinates[i][1];
                }

                if(largestStartingXCoordinate < regionCoordinates[i][2])
                {
                    largestStartingXCoordinate = regionCoordinates[i][2];
                }

                if(largestStartingYCoordinate < regionCoordinates[i][3])
                {
                    largestStartingYCoordinate = regionCoordinates[i][3];
                }
            }//end for
        }//end if regionCoordinates.size < 1

        _xStartOfFrameAnalysisArea = smallestStartingXCoordinate;
        _yStartOfFrameAnalysisArea = smallestStartingYCoordinate;
        _xEndOfFrameAnalysisArea = largestStartingXCoordinate;
        _yEndOfFrameAnalysisArea = largestStartingYCoordinate;
    }
    else
    {
        _xStartOfFrameAnalysisArea = 0;
        _yStartOfFrameAnalysisArea = 0;
        _xEndOfFrameAnalysisArea = _frameWidth;
        _yEndOfFrameAnalysisArea = _frameHeight;
    }
}



/*!
 * Resize output image frames based on settings chosen by the user
 *
 * \param outputImage: The image to be resized
 *
 * \return Returns new resized version of outputImage
 * \see setAnalyzeOptions() for assingment of resizing values
 */
Mat OpenCV::resizeOutputImage(cv::Mat outputImage)
{
    Mat smallerImage = Mat (_outputImageSizeY, _outputImageSizeX, CV_8U);
    resize(outputImage, smallerImage, smallerImage.size(), 0, 0, CV_INTER_AREA);

    return smallerImage;
}

/*!
 * Launch the preview window for preview playback
 */
void OpenCV::openPreviewWindow()
{
    cvNamedWindow("Preview Analyze", CV_WINDOW_AUTOSIZE);
}

/*!
 * Close the preview window
 */
void OpenCV::closePreviewWindow()
{
    destroyWindow("Preview Analyze");
}

/*!
 * Shrink the preview window when unable to close it
 */
void OpenCV::shrinkPreviewWindow()
{
    imshow("Preview Analyze", Mat(1, 1, CV_8U));
}

/*!
 * Takes an image represented in the matrix "Mat" format from openCV, and saves it as a .JPG image to a directory specified
 * by the user.
 *
 * \param imageToSave : The image you wish to save to a .JPG file, passed as a matrix type, which is defined by openCV.
 *
 * \param frameNumber : The frame of the video that the passed image is derived form. This number is added to the file
 *                      name of the generated .JPG file, and is used to to identifiy which frame the image came from to
 *                      user, and groups the "current", "previous", and "different" images derived from the same frame
 *                      together in the file browser on the users OS
 *
 * \param outoutFilePath : The full path to the directory the .JPG will be output to, including the trailing  "\" or
 *                         "/" after the directory name, for Windows and Mac respactively.
 *
 * \return Returns a QString containing the image path, which is sent through the system so the frame carosel can be updated.
 */
QString OpenCV::saveFrameAsJPG(Mat imageToSave, int frameNumber, string outputFilePath)
{
    string frameNumberAsString;
    stringstream converter;
    converter << frameNumber;
    frameNumberAsString = converter.str();

    string filePath = outputFilePath;
    QString qFileName;

    //QString to hold path that will be sent to the carousel
    qFileName = QString::fromStdString(_randomImageNameAddition) + QString::fromStdString(_videoFileName) + "frame-" + QString::fromStdString(frameNumberAsString) + "-" + ".jpg";

    //output native resolution image
    if(_outputImageSizeX == 0)
    {  
        imwrite(filePath + _randomImageNameAddition + _videoFileName + "frame-" + frameNumberAsString + "-" + ".jpg", imageToSave);
    }
    else//else output a smaller resized image
    {
        Mat smallerImage = resizeOutputImage(imageToSave);
        imwrite(filePath + _randomImageNameAddition + _videoFileName + "frame-" + frameNumberAsString + "-" + ".jpg", smallerImage);
    }

    //return path for carousel
    return qFileName;
}

/*!
 * Draws a region rectangle onto an image based on the start and end points passed as argumanets
 *
 * \param startPointX: The X1 coordinate
 * \param startPointY: The Y1 coordinate
 * \param endPointX: The X2 coordinate
 * \param endPointY: The Y2 coordinate
 * \param regionNumber: The number of the region we are drawing, used to determine the color of the drawn rectangle
 * \param differenceImage: The matrix image we will be drawing on, passed by reference
 *
 * \return Returns nothing, the new image containing the drawn rectangle is passed back by reference
 */
void OpenCV::drawRegionRectangle(int startPointX, int startPointY, int endPointX, int endPointY, int regionNumber, cv::Mat &differenceImage)
{
    //draw top of region
    //if region is the full size of the frame
    if(startPointY == 0)
    {
        line(differenceImage, cvPoint(startPointX, startPointY + (_lineSizeRegionRect/2)), cvPoint(endPointX, startPointY + (_lineSizeRegionRect/2)), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
    }
    else
    {
        line(differenceImage, cvPoint(startPointX, startPointY), cvPoint(endPointX, startPointY), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
    }

    //draw bottom of region
    //if region is the full size of the frame
    if(endPointY == differenceImage.rows)
    {
        if(_lineSizeRegionRect == 4)
        {
            line(differenceImage, cvPoint(startPointX, endPointY - (_lineSizeRegionRect - 1)), cvPoint(endPointX, endPointY - (_lineSizeRegionRect - 1)), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
        }
        else
        {
            line(differenceImage, cvPoint(startPointX, endPointY - (_lineSizeRegionRect/2)), cvPoint(endPointX, endPointY - (_lineSizeRegionRect/2)), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
        }
    }
    else
    {
        line(differenceImage, cvPoint(startPointX, endPointY), cvPoint(endPointX, endPointY), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
    }

    //draw left side of region
    //if region is the full size of the frame
    if(startPointX == 0)
    {
        line(differenceImage, cvPoint(startPointX + (_lineSizeRegionRect/2), startPointY), cvPoint(startPointX + (_lineSizeRegionRect/2), endPointY), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
    }
    else
    {
        line(differenceImage, cvPoint(startPointX, startPointY), cvPoint(startPointX, endPointY), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
    }


    //draw right side of region
    //if region is the full size of the frame
    if(endPointX == differenceImage.cols)
    {
        if(_lineSizeRegionRect == 4)
        {
            line(differenceImage, cvPoint(endPointX - (_lineSizeRegionRect - 1), startPointY), cvPoint(endPointX - (_lineSizeRegionRect- 1), endPointY), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
        }
        else
        {
            line(differenceImage, cvPoint(endPointX - (_lineSizeRegionRect/2), startPointY), cvPoint(endPointX - (_lineSizeRegionRect/2), endPointY), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
        }
    }
    else
    {
        line(differenceImage, cvPoint(endPointX, startPointY), cvPoint(endPointX, endPointY), CV_RGB(colorList[regionNumber].r, colorList[regionNumber].g, colorList[regionNumber].b), _lineSizeRegionRect, 8, 0);
    }

}

/*!
 * Draws a slightly smaller rectangle. Used to draw another rectangle inside an existing region rectangle
 * whenever a region contains motion greater than its user specified threshol value
 *
 * \param startPointX: The X1 coordinate
 * \param startPointY: The Y1 coordinate
 * \param endPointX: The X2 coordinate
 * \param endPointY: The Y2 coordinate
 * \param regionNumber: The number of the region we are drawing, used to determine the color of the drawn rectangle
 * \param differenceImage: The Matrix image we will be drawing on, passed by reference
 *
 * \return Returns nothing, the new image containing the drawn rectangle is passed back by reference
 */
void OpenCV::drawMotionRegionRectangle(int startPointX, int startPointY, int endPointX, int endPointY, int regionNumber, cv::Mat &differenceImage)
{
    //draw black motion rectangles onto the colored region rectangles
    int color = 0;

    //draw top of region
    //if region is the full size of the frame
    if(startPointY == 0)
    {
        line(differenceImage, cvPoint(startPointX + _offsetMotionRect, startPointY + _offsetMotionRect), cvPoint(endPointX - (_offsetMotionRect + 1), startPointY + _offsetMotionRect), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
    }
    else
    {
        line(differenceImage, cvPoint(startPointX, startPointY), cvPoint(endPointX, startPointY), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
    }

    //draw bottom of region
    //if region is the full size of the frame
    if(endPointY == differenceImage.rows)
    {
        if(_offsetMotionRect == 2)
        {
            line(differenceImage, cvPoint(startPointX + _offsetMotionRect, endPointY - (_offsetMotionRect + 1)), cvPoint(endPointX - (_offsetMotionRect + 1), endPointY - (_offsetMotionRect + 1)), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
        }
        else
        {
            line(differenceImage, cvPoint(startPointX + _offsetMotionRect, endPointY - _offsetMotionRect), cvPoint(endPointX - _offsetMotionRect, endPointY - _offsetMotionRect), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
        }
    }
    else
    {
        line(differenceImage, cvPoint(startPointX, endPointY), cvPoint(endPointX, endPointY), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
    }

    //draw left side of region
    //if region is the full size of the frame
    if(startPointX == 0)
    {
        line(differenceImage, cvPoint(startPointX + _offsetMotionRect, startPointY + _offsetMotionRect), cvPoint(startPointX + _offsetMotionRect, endPointY - (_offsetMotionRect + 1)), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
    }
    else
    {
        line(differenceImage, cvPoint(startPointX, startPointY), cvPoint(startPointX, endPointY), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
    }

    //draw right side of region
    //if region is the full size of the frame
    if(endPointX == differenceImage.cols)
    {
        if(_offsetMotionRect == 2)
        {
            line(differenceImage, cvPoint(endPointX - (_offsetMotionRect + 1), startPointY + _offsetMotionRect), cvPoint(endPointX - (_offsetMotionRect + 1), endPointY - (_offsetMotionRect + 1)), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
        }
        else
        {
            line(differenceImage, cvPoint(endPointX - (_offsetMotionRect), startPointY + _offsetMotionRect), cvPoint(endPointX - (_offsetMotionRect), endPointY - _offsetMotionRect), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
        }
    }
    else
    {
        line(differenceImage, cvPoint(endPointX, startPointY), cvPoint(endPointX, endPointY), CV_RGB(color, color, color), _lineSizeMotionRect, 8, 0);
    }

}

/*!
 * Takes the coordinates of a single changed pixel, and determines which regions contain that pixel
 *
 * \param xPosition: The X coordinate of the pixel we are checking
 * \param yPosition: The Y coordinate of the pixel we are checking
 * \param regionPixelChanges: A vector that holds the number of pixels that have changed in each region for this frame
 * \param regionCoordinates: A vector that holds the start and end points of each region
 *
 * \return Returns nothing, increments the values held in regionPixelChanges to count the number of pixels changed in this frame per region
 */
void OpenCV::evaluateRegionalChanges(int xPosition, int yPosition, std::vector <int> &regionPixleChanges, std::vector < std::vector<int> > &regionCoordinates)
{
    for(unsigned int regionNum = 0; regionNum < regionCoordinates.size(); regionNum++)
    {
        //if this pixel falls within this region's x axis area
        if(xPosition  >= regionCoordinates[regionNum][0] && xPosition <= regionCoordinates[regionNum][2])
        {
            //and if this pixel falls within this region's y axis area
            if(yPosition  >= regionCoordinates[regionNum][1] && yPosition <= regionCoordinates[regionNum][3])
            {
                regionPixleChanges[regionNum] ++;
            }

        }

    }

}

/*!
 * Takes the coordinates of a pixel that changed this frame, and draws it in red to the frame, scaled based on frame resolution
 *
 * \param startPointX: Start point to draw the pixel to
 * \param endPointX: End point to draw the pixel to
 *
 * \return Returns nothing, passes back the current frame with the changed pixel drawn to it by reference
 */
void OpenCV::drawDifferencePixelOnFrame(int startPointX, int endPointX, int yAxisPoint, cv::Mat &currentImageCopy)
{
    line(currentImageCopy, cvPoint(startPointX, yAxisPoint), cvPoint(endPointX, yAxisPoint), CV_RGB(255, 0, 0), _pixelSize, 8, 0);
}

/*!
 * Draws the current video time string onto the passed in Matrix image
 *
 * \param currentImage: Image we are drawing the text to, passed by reference
 *
 * \return Returns nothing, passes back the current frame with the text drawn to it by reference
 */
void OpenCV::drawTimeOnToImage(cv::Mat &currentImage)
{
    int hours;
    int minutes;
    int seconds;

    string currentVideoTime;
    stringstream hourConverter;
    stringstream minuteConverter;
    stringstream secondConverter;

    //get video time
    getFormattedVideoTime(hours, minutes, seconds, getCurrentVideoFrame(), _frameRate);

    if(hours < 10)
    {
        hourConverter << hours;
        currentVideoTime = "0" + hourConverter.str() + ":";
    }
    else
    {
        hourConverter << hours;
        currentVideoTime = hourConverter.str() + ":";
    }

    if(minutes < 10)
    {
        minuteConverter << minutes;
        currentVideoTime += ("0" + minuteConverter.str() + ":");
    }
    else
    {
        minuteConverter << minutes;
        currentVideoTime += (minuteConverter.str() + ":");
    }

    if(seconds < 10)
    {
        secondConverter << seconds;
        currentVideoTime += ("0" + secondConverter.str());
    }
    else
    {
        secondConverter << seconds;
        currentVideoTime += secondConverter.str();
    }

    rectangle(currentImage, cvPoint(_x1Time, _y1Time), cvPoint(_x2Time, _y2Time), CV_RGB(0, 0, 0), CV_FILLED, 8, 0);
    putText(currentImage, currentVideoTime.c_str(), cvPoint(_xTimeText, _yTimeText), FONT_HERSHEY_SIMPLEX, _timeFontSize, CV_RGB(255, 255, 255), 1, CV_AA, false);
}

/*!
 * Stores the file output path, file name, and first frame to analyze in openCV object at the start of an analysis
 *
 * \param outFilePath: The output path for all output files as a string
 * \param vidFileName: The name of the current video file as a string
 * \param startFrame: The first frame of the video that will be analyzed
 *
 * \return Returns nothing
 */
void OpenCV::initializeStartFrameAndFileName(std::string outFilePath, std::string vidFileName, double startFrame)
{
    _outputFilePath = outFilePath;
    _videoFileName = vidFileName;
    _analysisStartFrame = startFrame;
}

/*!
 * Sets initial values in openCV object for variables responsible for tracking pixel changes during an analysis
 *
 * \param numberOfRegions: The number of regions selected by the user for this analysis
 * \param indexedRegionOutput: Stores the region data collected over the course of an analysis, used to generate output files later
 * \param percentOfImageChange: An array that holds thresholds set by the user for each region
 * \param regionWidths: Width of each region, used for calculating minimum number of chandged pixels that will flag a regions threshold
 *\param regionHeights: Height of each region, used for calculating minimum number of chandged pixels that will flag a regions threshold
 *
 * \return Returns nothing, passes back indexedRegionOutput by reference
 */
void OpenCV::initializePixelChangeVariables(int numberOfRegions, std::vector <regionData> &indexedRegionOutput, float percentOfImageChange[10], std::vector<int>* regionWidths, std::vector<int>* regionHeights)
{
    for(int i = 0; i < numberOfRegions; i++)
    {
        _regionPixelChanges.push_back(0);
        _previousFramePixelChanges[i] = 0;

        //if a threshold larger than 0 was selected by the user, set that threshold as the % of pixels that
        //must change between frames for a frame to be flagged
        if(percentOfImageChange[i] > 0)
        {
            indexedRegionOutput[i].regionThreshHold = percentOfImageChange[i];
            _pixelsThatMustChangePerRegion[i] = percentOfImageChange[i] * (*regionHeights)[i] * (*regionWidths)[i];//* _frameHeight * _frameWidth;
        }
        else//if the threshold is 0, than flag any pixel changes that occure between frames
        {
            indexedRegionOutput[i].regionThreshHold = percentOfImageChange[i];
            _pixelsThatMustChangePerRegion[i] = 1;
        }
    }
}

/*!
 * Initializes frame size used for generating images that will store openCV change data, motion sensitivity for this analysis,
 * and all sizing data for elements that will be drawn to video frames based on the video's resolution
 *
 * \param userSelectedSensitivity: Motion sensetivity selected by the user on the slider
 *
 * \return Returns nothing
 */
void OpenCV::initializeFrameSizeSensitivityAndDrawSize(float userSelectedSensitivity)
{
    _imgSize.width = _frameWidth;
    _imgSize.height = _frameHeight;

    if (_frameWidth < 600)
    {
        //data for drawing the time onto video frames
        _x1Time = 6;
        _y1Time = 6;
        _x2Time = 76;
        _y2Time = 20;
        _xTimeText = 6;
        _yTimeText = 18;
        _timeFontSize = 0.5f;

        //data dor drawing rectangles onto video frames
        _lineSizeRegionRect = 4;
        _lineSizeMotionRect = 2;
        _offsetMotionRect = 2;

        //draw size of pixels flagged as motion
        _pixelSize = 2;
    }
    else if(_frameWidth < 1200)
    {
        //data for drawing the time onto video frames
        _x1Time = 6 * 2;
        _y1Time = 6 * 2;
        _x2Time = 76 * 2;
        _y2Time = 20 * 2;
        _xTimeText = 6 * 2;
        _yTimeText = 18 * 2;
        _timeFontSize = 0.5f * 2;

        //data dor drawing rectangles onto video frames
        _lineSizeRegionRect = 6;
        _lineSizeMotionRect = 2;
        _offsetMotionRect = 3;

        //draw size of pixels flagged as motion
        _pixelSize = 4;
    }
    else
    {
        //data for drawing the time onto video frames
        _x1Time = 6 * 3;
        _y1Time = 6 * 3;
        _x2Time = 76 * 3;
        _y2Time = 20 * 3;
        _xTimeText = 6 * 3;
        _yTimeText = 18 * 3;
        _timeFontSize = 0.5f * 3;

        //data for drawing rectangles onto video frames
        _lineSizeRegionRect = 8;
        _lineSizeMotionRect = 3;
        _offsetMotionRect = 4;

        //draw size of pixels flagged as motion
        _pixelSize = 6;
    }

    //set motion sensitivity based on user selected value
    _motionSensitivity = (0.90F + (userSelectedSensitivity / 1000));
}

/*!
 * Initializes an image variable that stores the average of all frame values analyzed so far
 *
 * \return Returns nothing
 */
void OpenCV::initializeMovingAverageFrame()
{
    _movingAverage = cvCreateImage( _imgSize, IPL_DEPTH_32F, 3);
}

/*!
 * Deallocates frame data stored in memory when an openCV error is thrown
 *
 * \return Returns nothing
 */
void OpenCV::deallocateFramesOnError()
{
    cvReleaseImage(&_differenceIpl);
    cvReleaseImage(&_differenceIplLessThan);
    cvReleaseImage(&_greyDiffImage);
    cvReleaseImage(&_greyDiffImageLessThan);
    cvReleaseImage(&_currentColorImage);
    cvReleaseImage(&_tempIpl);
}

/*!
 * Analyzes a single frame from the video stream, and outputs image files if that frame passes any regions threshold
 *
 * \param currentFrameNumber: The frame number we will be analyzing on this function call
 * \param fileStream: The stream object from Analyze.cpp, used for printing to the output files
 * \param regionCoordinates: A vector containing one integer vector for every region. Each internal vector hold X1, Y1, X2 and Y2 coordinates of a region
 * \param videoInfo: Holds general video data and non region specific analysis data that will be output to a file later
 * \param indexedRegionOutput: Holds analysis output data for each region selected by the user
 * \param isEditFrame: Determines if the running frame average should be reset if is is eitehr the beginning of the video, or a new edit point
 *
 * \return Returns nothing, currentFrameNumber is incremented and passed back by reference
 *
 * \see Analyzer for loop structure that calls this function
 */
QString OpenCV::analyzeCurrentFrame(int &currentFrameNumber, std::vector < std::vector<int> > &regionCoordinates, generalVideoData &videoInfo, std::vector <regionData> &indexedRegionOutput, bool isEditFrame)
{
    _greyDiffImage = cvCreateImage(cvSize(_frameWidth,_frameHeight), IPL_DEPTH_8U, 1);
    _greyDiffImageLessThan = cvCreateImage(cvSize(_frameWidth,_frameHeight), IPL_DEPTH_8U, 1);

    Mat currentVideoFrame;
    Mat currentFrameWithDifference;


    //if this is the first frame to analyze, or first frame after an edit point,
    //get the next video frame, and set our current frame average to it
    if(isEditFrame == true)
    {
        getFrameForAnalysis(currentVideoFrame);

        //copy the current frame to this difference frame, which will have all pixels that change between frames drawn onto it
        currentVideoFrame.copyTo(currentFrameWithDifference);

        //convert first image to iplImage for analysis
        _currentColorImage = cvCloneImage(&(IplImage)currentVideoFrame);//ignore this compiler warning
        _differenceIpl = cvCloneImage(_currentColorImage);
        _differenceIplLessThan = cvCloneImage(_currentColorImage);
        _tempIpl = cvCloneImage(_currentColorImage);
        cvConvertScale(_currentColorImage, _movingAverage, 1.0, 0.0);
    }
    else //else get the next frame we are analyzing in the video, and update the average frame motion
    {
        getFrameForAnalysis(currentVideoFrame);
        currentVideoFrame.copyTo(currentFrameWithDifference);
        _currentColorImage = cvCloneImage(&(IplImage)currentVideoFrame);//ignore this compiler warning
        _differenceIpl = cvCloneImage(_currentColorImage);
        _differenceIplLessThan = cvCloneImage(_currentColorImage);
        _tempIpl = cvCloneImage(_currentColorImage);

        cvRunningAvg(_currentColorImage, _movingAverage, _motionSensitivity, NULL);
    }

    //Convert the scale of the moving average.
    cvConvertScale(_movingAverage, _tempIpl, 1.0, 0.0);

    //Get high and low end pixel differences between running average and current frame
    compare((Mat)_currentColorImage, ((Mat)_tempIpl + 100), (Mat)_differenceIpl, CMP_GT);
    compare((Mat)_currentColorImage, ((Mat)_tempIpl - 100), (Mat)_differenceIplLessThan, CMP_LT);

    //Convert the difference image to grayscale.
    cvCvtColor(_differenceIpl, _greyDiffImage, CV_RGB2GRAY);
    cvCvtColor(_differenceIplLessThan, _greyDiffImageLessThan, CV_RGB2GRAY);

    //Convert the grayscale difference image to black and white.
    cvThreshold(_greyDiffImage, _greyDiffImage, 70, 255, CV_THRESH_BINARY);
    cvThreshold(_greyDiffImageLessThan, _greyDiffImageLessThan, 70, 255, CV_THRESH_BINARY);

    //copy iplImage difference to a matrix format image for editing and collecting data
    _differenceBetweenFrames = cvCloneImage(_greyDiffImage);
    _differenceBetweenFramesLessThan = cvCloneImage(_greyDiffImageLessThan);

    //check every pixel in the current frame for changes
    for(int i = _yStartOfFrameAnalysisArea; i < _yEndOfFrameAnalysisArea; i++)
    {
        const unsigned char* currentRow = _differenceBetweenFrames.ptr<unsigned char>(i);
        const unsigned char* currentRowLessThan = _differenceBetweenFrames.ptr<unsigned char>(i);

        for(int j = _xStartOfFrameAnalysisArea; j < _xEndOfFrameAnalysisArea; j++)
        {
            //when a difference is detected
            if(currentRow[j] != 0 && currentRowLessThan != 0)
            {
                //draw the pixel changed detected to a copy of the current image
                drawDifferencePixelOnFrame(j, j, i, currentFrameWithDifference);

                //find out which regions the change occured in
                evaluateRegionalChanges(j, i, _regionPixelChanges, regionCoordinates);
            }
        }
    }

    //flag showing that at least one region had activity higher than its threshHold
    bool atLeastOneThreshHoldPassed = false;

    //loop through data for each region after a frame has been analyzed
    for(unsigned int regionNum = 0; regionNum < regionCoordinates.size(); regionNum++)
    {
        //draw region rectangles onto image
        drawRegionRectangle(regionCoordinates[regionNum][0], regionCoordinates[regionNum][1], regionCoordinates[regionNum][2], regionCoordinates[regionNum][3], regionNum, currentFrameWithDifference);

        //check each region to see if it has passed its threshHold on this frame, if it has,
        //collect its data for results to use later, and draw its region rectangle onto the output difference image
        if( (_pixelsThatMustChangePerRegion[regionNum] <= _regionPixelChanges[regionNum]) && (_regionPixelChanges[regionNum] != _previousFramePixelChanges[regionNum]) && (_regionPixelChanges[regionNum] != 0) )
        {
            //draw over other rectangle when motion past threshold in region detected
            drawMotionRegionRectangle(regionCoordinates[regionNum][0], regionCoordinates[regionNum][1], regionCoordinates[regionNum][2], regionCoordinates[regionNum][3], regionNum, currentFrameWithDifference);

            atLeastOneThreshHoldPassed = true;

            indexedRegionOutput[regionNum].totalFramesOverThreshHold++;

            frameData tempFrameData;
            tempFrameData.frameNumber = currentFrameNumber;
            tempFrameData.totalDifferentPixels = _regionPixelChanges[regionNum];
            getFormattedVideoTime(tempFrameData.hourFrameAppears, tempFrameData.minuteFrameAppears, tempFrameData.secondFrameAppears, currentFrameNumber, _frameRate);

            indexedRegionOutput[regionNum].framesOverThreshHold.push_back(tempFrameData);
        }

    }

    ///draw current video time onto this frame
    drawTimeOnToImage(currentFrameWithDifference);

    QString imageFilePath = "";

    //if at least one region passed its threshold, output the current frame and a copy of the
    //current frame containing its  regions/difference pixels as a .JPG
    if(atLeastOneThreshHoldPassed == true)
    {
        videoInfo.totalFramesPastThreshHold++;

        //don't output .JPGs unless the user specifies it
        if(_isOutputingImages == true)
        {
            imageFilePath = saveFrameAsJPG(currentFrameWithDifference, currentFrameNumber, _outputFilePath);
        }
    }

    //reset pixel differences found in each region, and set previous pixel differences found for next frame analysis
    for(unsigned int regionNumber = 0; regionNumber < _regionPixelChanges.size(); regionNumber++)
    {
        _previousFramePixelChanges[regionNumber] = _regionPixelChanges[regionNumber];
        _regionPixelChanges[regionNumber] = 0;
    }

    //increment current frame number
    currentFrameNumber++;

    //deallocate all currently allocated analysis frames in memory
    cvReleaseImage(&_differenceIpl);
    cvReleaseImage(&_differenceIplLessThan);
    cvReleaseImage(&_greyDiffImage);
    cvReleaseImage(&_greyDiffImageLessThan);
    cvReleaseImage(&_currentColorImage);
    cvReleaseImage(&_tempIpl);
    _differenceBetweenFrames.deallocate();
    _differenceBetweenFramesLessThan.deallocate();

    return imageFilePath;
}

/*!
 * Removes our avereage frame variable from memory, called from our analyze function when analysis is complete
 */
void OpenCV::deallocateMovingAverageFrame()
{
    cvReleaseImage(&_movingAverage);
}

/*!
 * Preview analysis function. Runs an analysis without storing any data, just shows analyzed video playback to the user
 *
 * \param currentFrameNumber: The frame number we will be analyzing on this function call
 * \param regionCoordinates: A vector containing one integer vector for every region. Each internal vector hold X1, Y1, X2 and Y2 coordinates of a region
 * \param indexedRegionOutput: Holds analysis output data for each region selected by the user
 * \param isEditFrame: Determines if the running frame average should be reset if is is eitehr the beginning of the video, or a new edit point
 *
 * \return Returns nothing, currentFrameNumber is incremented and passed back by reference
 *
 * \see DetailAnalyzer for loop structure that calls this function
 */
void OpenCV::previewAnalysis(int &currentFrameNumber, std::vector < std::vector<int> > &regionCoordinates, std::vector <regionData> &indexedRegionOutput, bool isEditFrame)
{
    _greyDiffImage = cvCreateImage(cvSize(_frameWidth,_frameHeight), IPL_DEPTH_8U, 1);
    _greyDiffImageLessThan = cvCreateImage(cvSize(_frameWidth,_frameHeight), IPL_DEPTH_8U, 1);

    //the matrix variables that will be used to store, parse through, and
    //output analysis difference data
    Mat currentVideoFrame;
    Mat currentFrameWithDifference;


    //if this is the first frame to analyze, get a our first video frame, and set our current frame average to it
    if(isEditFrame == true)
    {
        //get the first frame from video, or hte first after an edit point
        getFrameForAnalysis(currentVideoFrame);

        //copy the current frame to this difference frame, which will have all pixels that change between frames drawn onto it
        currentVideoFrame.copyTo(currentFrameWithDifference);

        //convert first image to iplImage for analysis
        _currentColorImage = cvCloneImage(&(IplImage)currentVideoFrame);//ignore this compiler warning
        _differenceIpl = cvCloneImage(_currentColorImage);
        _differenceIplLessThan = cvCloneImage(_currentColorImage);
        _tempIpl = cvCloneImage(_currentColorImage);
        cvConvertScale(_currentColorImage, _movingAverage, 1.0, 0.0);
    }
    else //else get the next frame we are analyzing in the video, and update the average frame motion
    {
        getFrameForAnalysis(currentVideoFrame);
        currentVideoFrame.copyTo(currentFrameWithDifference);
        _currentColorImage = cvCloneImage(&(IplImage)currentVideoFrame);//ignore this compiler warning
        _differenceIpl = cvCloneImage(_currentColorImage);
        _differenceIplLessThan = cvCloneImage(_currentColorImage);
        _tempIpl = cvCloneImage(_currentColorImage);

        cvRunningAvg(_currentColorImage, _movingAverage, _motionSensitivity, NULL);
    }

    //Convert the scale of the moving average.
    cvConvertScale(_movingAverage, _tempIpl, 1.0, 0.0);

    //Minus the current frame from the moving average, leaving only the difference pixels
    compare((Mat)_currentColorImage, ((Mat)_tempIpl + 100), (Mat)_differenceIpl, CMP_GT);
    compare((Mat)_currentColorImage, ((Mat)_tempIpl - 100), (Mat)_differenceIplLessThan, CMP_LT);

    //Convert the difference image to grayscale.
    cvCvtColor(_differenceIpl, _greyDiffImage, CV_RGB2GRAY);
    cvCvtColor(_differenceIplLessThan, _greyDiffImageLessThan, CV_RGB2GRAY);

    //Convert the grayscale difference image to black and white.
    cvThreshold(_greyDiffImage, _greyDiffImage, 70, 255, CV_THRESH_BINARY);
    cvThreshold(_greyDiffImageLessThan, _greyDiffImageLessThan, 70, 255, CV_THRESH_BINARY);

    //copy iplImage difference to a matrix format image for editing and collecting data
    _differenceBetweenFrames = cvCloneImage(_greyDiffImage);
    _differenceBetweenFramesLessThan = cvCloneImage(_greyDiffImageLessThan);

    //check every pixel in the current frame for changes
    for(int i = 0; i < _frameHeight; i++)
    {
        const unsigned char* currentRow = _differenceBetweenFrames.ptr<unsigned char>(i);
        const unsigned char* currentRowLessThan = _differenceBetweenFrames.ptr<unsigned char>(i);

        for(int j = 0; j < _frameWidth; j++)
        {
            //when a difference is detected
            if(currentRow[j] != 0 && currentRowLessThan != 0)
            {
                //draw the pixel changed detected to a copy of the current image
                drawDifferencePixelOnFrame(j, j, i, currentFrameWithDifference);

                //find out which regions the change occured in
                evaluateRegionalChanges(j, i, _regionPixelChanges, regionCoordinates);
            }

        }

    }

    //flag showing that at least one region had activity higher than its threshHold
    bool atLeastOneThreshHoldPassed = false;

    //loop through data for each region after a frame has been analyzed
    for(unsigned int regionNum = 0; regionNum < regionCoordinates.size(); regionNum++)
    {
        //draw region rectangles onto image
        drawRegionRectangle(regionCoordinates[regionNum][0], regionCoordinates[regionNum][1], regionCoordinates[regionNum][2], regionCoordinates[regionNum][3], regionNum, currentFrameWithDifference);

        //check each region to see if it has passed its threshHold on this frame, if it has,
        //collect its data for results to use later, and draw its region rectangle onto the output difference image
        if( (_pixelsThatMustChangePerRegion[regionNum] <= _regionPixelChanges[regionNum]) && (_regionPixelChanges[regionNum] != _previousFramePixelChanges[regionNum]) && (_regionPixelChanges[regionNum] != 0) )
        {
            //draw over other rectangle when motion past threshold in region detected
            drawMotionRegionRectangle(regionCoordinates[regionNum][0], regionCoordinates[regionNum][1], regionCoordinates[regionNum][2], regionCoordinates[regionNum][3], regionNum, currentFrameWithDifference);

            atLeastOneThreshHoldPassed = true;

            indexedRegionOutput[regionNum].totalFramesOverThreshHold++;

            frameData tempFrameData;
            tempFrameData.frameNumber = currentFrameNumber;
            tempFrameData.totalDifferentPixels = _regionPixelChanges[regionNum];
            getFormattedVideoTime(tempFrameData.hourFrameAppears, tempFrameData.minuteFrameAppears, tempFrameData.secondFrameAppears, currentFrameNumber, _frameRate);

            indexedRegionOutput[regionNum].framesOverThreshHold.push_back(tempFrameData);
        }
    }

    //reset pixel differences found in each region, and set previous pixel differences found for next frame analysis
    for(unsigned int regionNumber = 0; regionNumber < _regionPixelChanges.size(); regionNumber++)
    {
        _previousFramePixelChanges[regionNumber] = _regionPixelChanges[regionNumber];
        _regionPixelChanges[regionNumber] = 0;
    }

    //the current video time onto the frame
    drawTimeOnToImage(currentFrameWithDifference);

    //resize preview frame if applicable
    if(_previewSizeX != 0)
    {
        Mat resizedFrame = resizePreviewImage(currentFrameWithDifference);

        //show image in preview window
        imshow("Preview Analyze", resizedFrame);
    }
    else
    {
        //show image in preview window
        imshow("Preview Analyze", currentFrameWithDifference);
    }

    //update image, and show that image for a number of milliseconds equal to the _previewPlaybackSpeed
    waitKey(_previewPlaybackSpeed);

    //increment frame number, reset threshHold check(, on second look, threshHold reset looks unneeded syntactically)
    currentFrameNumber++;
    atLeastOneThreshHoldPassed = false;

    //deallocate all currently allocated analysis frames in memory
    cvReleaseImage(&_differenceIpl);
    cvReleaseImage(&_differenceIplLessThan);
    cvReleaseImage(&_greyDiffImage);
    cvReleaseImage(&_greyDiffImageLessThan);
    cvReleaseImage(&_currentColorImage);
    cvReleaseImage(&_tempIpl);
    _differenceBetweenFrames.deallocate();
    _differenceBetweenFramesLessThan.deallocate();
}
