/*!
 * \class OpenCV
 *
 * Contains all the of our openCV functionality
 * This class is responsible for all openCV code, and all actual analysis of video files.
 *
 * Its responsibilities include:
 *
 * Gathering Metadata from video source.
 * Gathering Frame data from video source.
 * Comparing data.
 * Constructing results.
 * Returning results.
 * Outputing Image Data
 */

#ifndef OPENCV_H
#define OPENCV_H


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "QString"

class OpenCV
{

public:
    OpenCV();
    ~OpenCV();

    //all of the general video data that will be passed to result to be parsed and stored
    struct generalVideoData
    {
        std::string videoName;
        int frameWidthResult;
        int frameHeightResult;
        int totalNumberOfFramesResult;
        double frameRateResult;
        int hoursOfRunTimeResult;
        int minutesOfRunTimeResult;
        int secondsOfRunTimeResult;
        int totalVideoRunTimeInSeconds;
        int frameAnalysisStart;
        int frameAnalysisEnd;
        int totalFramesPastThreshHold;
    };

    //holds all data about a flagged frame in a given region
    struct frameData
    {
        int frameNumber;
        int hourFrameAppears;
        int minuteFrameAppears;
        int secondFrameAppears;
        int totalDifferentPixels;
    };

    //a vector of 10 regionsData structres will be used to store our analysis data and
    //pass it back to region results.
    struct regionData
    {
        int regionStartPointX;
        int regionStartPointY;
        int regionEndPointX;
        int regionEndPointY;
        std::string regionRectangleColor;
        float regionThreshHold;

        int totalFramesOverThreshHold;
        std::vector <frameData> framesOverThreshHold;
    };

    //holds and RGB color, and that color's name
    struct regionColors
    {
        int r;
        int g;
        int b;
        std::string colorName;
    };

    //array of 11 different region colors
    regionColors colorList[11];

    bool openVideoFile(std::string videoFilePath);

    bool closeVideoFile();

    void collectVideoMetaData();

    void getFormattedVideoTime(int &hours, int &minutes, int &seconds, double currentVideoFrame, double frameRate);

    double getCurrentVideoFrame();

    void setCurrentVideoFrame(double nextFrameToAnalyze);

    double getCurrentVideoTime();

    void setCurrentVideoTime(double newVideoTime);

    //simple get functions
    double getNumberOfVideoFrames();
    double getVideoFrameRate();
    double getVideoFrameWidth();
    double getVideoFrameHeight();

    void getFrameForAnalysis(cv::Mat& frameToAnalize);

    void setPreviewWindowOptions(int sizeSelected, int speedSelected);

    cv::Mat resizePreviewImage(cv::Mat previewImage);

    void setAnalyzeOptions(bool isOutputImage, int imageSizeSelected);

    void setFrameAnalysisSize(std::vector < std::vector<int> > regionCoordinates, bool isFullFrameAnalysis);

    cv::Mat resizeOutputImage(cv::Mat outputImage);

    void openPreviewWindow();

    void closePreviewWindow();

    void shrinkPreviewWindow();

    QString saveFrameAsJPG(cv::Mat imageToSave, int frameNumber, std::string outputFilePath);

    void drawRegionRectangle(int startPointX, int startPointY, int endPointX, int endPointY, int regionNumber, cv::Mat &differenceImage);

    void drawMotionRegionRectangle(int startPointX, int startPointY, int endPointX, int endPointY, int regionNumber, cv::Mat &differenceImage);

    void evaluateRegionalChanges(int xPosition, int yPosition, std::vector <int> &regionPixleChanges, std::vector < std::vector<int> > &regionCoordinates);

    void drawDifferencePixelOnFrame(int startPointX, int endPointX, int yAxisPoint, cv::Mat &currentImageCopy);

    void drawTimeOnToImage(cv::Mat &currentImage);

    void initializeStartFrameAndFileName(std::string outFilePath, std::string vidFileName, double startFrame);

    void initializePixelChangeVariables(int numberOfRegions, std::vector <OpenCV::regionData> &indexedRegionOutput, float percentOfImageChange[10], std::vector<int>* regionWidths, std::vector<int>* regionHeights);

    void initializeFrameSizeSensitivityAndDrawSize(float userMotionSensitivity);

    void initializeMovingAverageFrame();

    void deallocateFramesOnError();

    QString analyzeCurrentFrame(int &currentFrameNumber, std::vector < std::vector<int> > &regionCoordinates, OpenCV::generalVideoData &videoInfo,
                             std::vector <OpenCV::regionData> &indexedRegionOutput, bool isEditFrame);

    void deallocateMovingAverageFrame();

    void previewAnalysis(int &currentFrameNumber, std::vector < std::vector<int> > &regionCoordinates, std::vector <regionData> &indexedRegionOutput, bool isEditFrame);

private:

    //VideoCapture openCV object, used to call all openCV video analysis functions
    cv::VideoCapture vidStream;

    //unchanging video attributes
    double _numberOfFramesInVideo;
    double _frameRate;
    double _frameWidth;
    double _frameHeight;

    int _currentFrameNumber;

    float _motionSensitivity;

    //Dustin Variable Declarations
    std::string _outputFilePath;
    std::string _videoFileName;
    double _analysisStartFrame;
    std::vector <int> _regionPixelChanges;
    int _previousFramePixelChanges[10];
    CvSize _imgSize;
    int _pixelsThatMustChangePerRegion[10];
    IplImage* _movingAverage;

    int _previewPlaybackSpeed;
    int _previewSizeX;
    int _previewSizeY;

    bool _isOutputingImages;
    int _outputImageSizeX;
    int _outputImageSizeY;

    int _xStartOfFrameAnalysisArea;
    int _yStartOfFrameAnalysisArea;
    int _xEndOfFrameAnalysisArea;
    int _yEndOfFrameAnalysisArea;

    //variables for drawing video times onto frame
    int _x1Time;
    int _y1Time;
    int _x2Time;
    int _y2Time;
    int _xTimeText;
    int _yTimeText;
    float _timeFontSize;

    //variables that hold size data for drawing region/motion rectangles and chenged pixels onto images
    int _lineSizeRegionRect;
    int _lineSizeMotionRect;
    int _offsetMotionRect;
    int _pixelSize;

    //variables that will hold frame data during an analysis
    IplImage* _differenceIpl;
    IplImage* _differenceIplLessThan;
    IplImage* _greyDiffImage;
    IplImage* _greyDiffImageLessThan;
    IplImage* _currentColorImage;
    IplImage* _tempIpl;

    cv::Mat _differenceBetweenFrames;
    cv::Mat _differenceBetweenFramesLessThan;

    std::string _randomImageNameAddition;
};
#endif
