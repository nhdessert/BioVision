/*!
 * \class DetailAnalyzer
 * DetailAnalyzer is a subclass of BvThreadWorker, therefore it is designed to be run on another thread and fulfill the specific
 * purpose of analyzing the video when the user requests with the given set of options.  It contains a reference to the OpenCV
 * class for this purpose.
 *
 * DetailAnalyzer overrides the startSlot (which is run on another thread) and performs the main logical loop of the program,
 * analyzing the video for differences.  At the end it emits a signal to send the results and a signal to finish the
 * thread.  During it emits progress signals to notify the user of the progress.
 *
 * Unlike Analyzer, DetailAnalyzer saves no image files and collects no other analysis data. Instead, it plays the video back in
 * real time for the user, using the analysis setting they selected in order to allow them to preview their full analysis run,
 * and tweak and analysis data they have set before they run a full analysis to collect data and save images.
 */

#ifndef DETAILANALYZER_H
#define DETAILANALYZER_H


#include "OpenCV.h"
#include "Result.h"
#include "BvThreadWorker.h"

class DetailAnalyzer : public BvThreadWorker
{

public:
    DetailAnalyzer();
    DetailAnalyzer(std::vector<int>* xCoords, std::vector<int>* yCoords, std::vector<int>* widths, std::vector<int>* heights,
             std::vector<int>* thresholds, QString videoFilePath, int startSecond, int stopSecond, std::deque<int> videoEditTimesInSeconds,
             int motionSensitivity, int previewSpeed, int previewSize);
    ~DetailAnalyzer();
    void previewAnalyze();

public Q_SLOTS:
    void startSlot();

private:
    OpenCV _cvObject;
    std::vector<int>* _regionXCoords;
    std::vector<int>* _regionYCoords;
    std::vector<int>* _regionWidths;
    std::vector<int>* _regionHeights;
    std::vector<int>* _regionThresholds;
    QString _videoFilePath;
    int _startSecond;
    int _stopSecond;
    std::deque<int> _editPoints;
    float _motionSensitivity;
    int _previewSpeed;
    int _previewSize;

};
#endif
