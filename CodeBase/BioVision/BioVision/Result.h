/*!
 * Result is a class that is used to pass messages back when a task has finished.  Any instance of BvThreadWorker has
 * a reference to this class and uses the sendResultSignal to send the result back when a task has completed.
 *
 * Result also contains the functionality to export the data from an analysis to a text file.
 */

#ifndef RESULT_H
#define RESULT_H

#include <vector>
#include <QString>
#include "OpenCV.h"
#include "QDebug"

class Result
{

public:
	Result();
	virtual ~Result();

    /*! Data field for setting flags, msgs, etc */
    QString _data;

    /*! Project associated with a given result. */
    QString _project;

    void exportToText(std::string videoName, std::string outputPath, OpenCV::generalVideoData &videoData, std::vector < OpenCV::regionData > &indexedRegionData, std::vector<QString>* regionNames);

    virtual QString getData();
    virtual void setData(QString data);
    virtual void setProject(QString projName);
    virtual QString getProject();
};
#endif // !defined(EA_4A788D8A_16F7_496d_83BF_B3616936D5F5__INCLUDED_)
