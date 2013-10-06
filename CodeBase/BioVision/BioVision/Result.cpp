#include "Result.h"
#include <iostream>
#include <fstream>
#include "OpenCV.h"

/*!
 * \brief Result::Result default constructor.
 */
Result::Result()
{

}

/*!
 * \brief Result::~Result default destructor.
 */
Result::~Result()
{

}

/*!
 * \brief Result::getData gets the _data member variable.
 * \return the data string.
 */
QString Result::getData()
{
    return _data;
}

/*!
 * \brief Result::setData sets the _data member variable.
 *
 * \param data the string message to set for the data field.
 */
void Result::setData(QString data)
{
    _data = data;
}

/*!
 * \brief Result::getProject
 * \return the _project name.
 */
QString Result::getProject()
{
    return _project;
}

/*!
 * \brief Result::setProject
 * \param project sets a new project name for this result.
 */
void Result::setProject(QString project)
{
    _project = project;
}

/*!
 * \brief Result::exportToText takes the data collected from the results of an OpenCV analysis and store it in a file.
 * for the prototype, this data is simply stored in a plaintext file
 */
void Result::exportToText(std::string videoName, std::string outputPath, OpenCV::generalVideoData &videoData, std::vector < OpenCV::regionData > &indexedRegionData, std::vector<QString>* regionNames)
{
    //ignore this compiler warning, currently working as intended
    if(videoName.find_last_of('/') != -1)
    {
        videoName = videoName.substr(videoName.find_last_of('/') + 1, videoName.size() );
        videoName = videoName.substr(0, (videoName.size() - 4) );
    }
    else
    {
        videoName = videoName.substr(videoName.find_last_of('\\') + 1, videoName.size() );
        videoName = videoName.substr(0, (videoName.size() - 4) );
    }

    std::ofstream fileStream;
    std::string outPathWithFileName = outputPath + "tmp.txt";
    fileStream.open(outPathWithFileName.c_str());

    fileStream << videoName << "Analysis Results:" << ".\n";

    fileStream << "Total Run Time of Video in Seconds: " << videoData.totalVideoRunTimeInSeconds << "\n";

    fileStream << "Video Frame Width: " << videoData.frameWidthResult << ".\n";
    fileStream << "Video Frame Height: " << videoData.frameHeightResult << ".\n";
    fileStream << "Video Frame Rate: " << videoData.frameRateResult << ".\n";
    fileStream << "First Frame Analyzed: " << videoData.frameAnalysisStart << ".\n";
    fileStream << "Last Frame Analyzed: " << videoData.frameAnalysisEnd << ".\n";
    fileStream << "Total Frames that Passed a Threshold: " << videoData.totalFramesPastThreshHold << ".\n\n";

    fileStream << "Region Analysis Results:"<<indexedRegionData.size()<<"\n\n";

    for(unsigned int regionNum = 0; regionNum < indexedRegionData.size(); regionNum++)
    {
        fileStream << "Region" << (*regionNames).at(regionNum).toStdString() << "\n";
        fileStream << "      Start Point: (" << indexedRegionData[regionNum].regionStartPointX << ", " << indexedRegionData[regionNum].regionStartPointY << ").\n";
        fileStream << "      End Point: (" << indexedRegionData[regionNum].regionEndPointX << ", " << indexedRegionData[regionNum].regionEndPointY << ").\n";
        fileStream << "      Color: " << indexedRegionData[regionNum].regionRectangleColor << ".\n";

        if(indexedRegionData[regionNum].regionThreshHold >= 0.01f && indexedRegionData[regionNum].regionThreshHold <= 1.0f)
        {
            fileStream << "      Threshold: " << (indexedRegionData[regionNum].regionThreshHold * 100) << "% Different from Previous Frame.\n\n";
        }
        else
        {
            fileStream << "      Threshold: Less Than 1% Different from Previous Frame.\n\n";
        }

        fileStream << "      Frames That Passed this Region's Threshold:\n";
        for(unsigned int j = 0; j < indexedRegionData[regionNum].framesOverThreshHold.size(); j++)
        {
            fileStream << "Frame Number: " << indexedRegionData[regionNum].framesOverThreshHold[j].frameNumber;
            fileStream << ":Timestamp of Frame: " << ((indexedRegionData[regionNum].framesOverThreshHold[j].hourFrameAppears * 60 * 60) +
                          (indexedRegionData[regionNum].framesOverThreshHold[j].minuteFrameAppears * 60) +
                          (indexedRegionData[regionNum].framesOverThreshHold[j].secondFrameAppears));
            fileStream << ":Total Pixels Changed This Frame: " << indexedRegionData[regionNum].framesOverThreshHold[j].totalDifferentPixels << ".\n";

        }
        fileStream << "\n";
    }

    //close the text file after writing analysis data
    fileStream.close();
}
