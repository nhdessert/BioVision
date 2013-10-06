/*!
 * \class Analyzer
 *
 * Analyzer is a subclass of BvThreadWorker, therefore it is designed to be run on another thread and fulfill the specific
 * purpose of analyzing the video when the user requests with the given set of options.  It contains a reference to the OpenCV
 * class for this purpose.
 *
 * Analyzer overrides the startSlot (which is run on another thread) and performs the main logical loop of the program,
 * analyzing the video for differences.  At the end it emits a signal to send the results and a signal to finish the
 * thread.  During it emits progress signals to notify the user of the progress, and a signal whenever an image is written
 * to a file (then the carousel can display it.)
 */

#ifndef ANALYZER_H
#define ANALYZER_H

#include "OpenCV.h"
#include "Result.h"
#include "BvThreadWorker.h"
#include "QDir"
#include <QMessageBox>
#include <QStringList>
#include <QDebug>

class Analyzer : public BvThreadWorker
{

public:
    Analyzer();
    Analyzer(std::vector<int>* xCoords, std::vector<int>* yCoords, std::vector<int>* widths, std::vector<int>* heights,
             std::vector<int>* thresholds, QString videoFilePath, int startSecond, int stopSecond, std::deque<int> videoEditTimesInSeconds,
             int motionSensitivity, std::vector<QString>* regionNames, int imageOutputSize, bool isOutputImages, bool isFullFrameAnalysis);
    ~Analyzer();
    void analyze();
    void clearTmpDirectory();

public Q_SLOTS:
    void startSlot();
    void sendImageInfoSlot(QString, QString);
    void clearCarouselSlot();

    Q_SIGNALS:
       void imageWrittenSignal(QString);

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
    std::vector<QString>* _regionNames;

    int _imageOutputSize;
    int _isOutputImages;
    bool _isFullFrameAnalysis;
    QStringList _parseString;
};
#endif
