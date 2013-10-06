/*!
 * \class Video
 *
 * Represents a video and the data a video contains for BioVision.
 *
 * This includes the video name, filepath, meta data (frame rate, etc), list of regions, and time data that has been stored
 * for this video (start analysis, end, etc).
 */

#ifndef VIDEO_H
#define VIDEO_H

#include "BvRegion.h"
#include <QString>
#include <QTreeWidgetItem>
#include "QtCore"

class Video
{

public:
	Video();
    ~Video();

    QTreeWidgetItem* getQTreeWidgetItem();

    /*!
     * \brief File path to the selected video.
	 */
    QString _filePath;

    /*!
     * \brief _listOfRegions holds all of the regions of a given video that have been defined as a user.
     */
    std::vector<BvRegion*> _listOfRegions;

    /*!
     * \brief Name of the video. Default name is the name of the file after the last slash in
	 * the filepath (including the extension).
	 */
	QString _name;

    /*!
     *\brief the QTreeWidget item for this video, to display in the project browser.
     */
    QTreeWidgetItem* _videoItem;

    // I changed this to a public element instead of protected because I need to access it in the save funtion.
    //-Professor X
	int _threshold;

    //Molly - Metadata Variables
    int _numberOfFramesInVideo;
    int _frameRate;
    int _frameWidth;
    int _frameHeight;

    // Start/end time variables that are stored for the user.
    std::vector<QTime> _times;
    QTime _currentEnd;
    QTime _currentStart;
    //QTime _lastEnd;
    //QTime _lastStart;
};
#endif // !defined(EA_CD73B2D3_446D_4a0a_8B36_E58788B74B9B__INCLUDED_)
