/*!
 * VideoCopier.h copies a video (or file) from one location to another.  It is a subclass of BvThreadWorker, so that it can
 * run on another thread via the thread manager.
 *
 * Note that although this class has the potential to support progress, QFile::copy (the library call we are using) does not
 * support progress emitting for that operation.  Therefore the only notification of a video being copied is if the user
 * tries to analyze or preview analyze while a video copy operation is still in progress, this class' message will be
 * displayed, alerting the user to the fact that a video is still copying.
 */

#ifndef VIDEOCOPIER_H
#define VIDEOCOPIER_H

#include "BvThreadWorker.h"
#include <QString>
#include "QFile"

class VideoCopier : public BvThreadWorker
{

public:
    VideoCopier(QString projName, QString, QString);
	virtual ~VideoCopier();

    void copyVideo();

public Q_SLOTS:
    void startSlot();

private:

    /*! The file that we want to copy */
    QFile _fromFile;

    /*! The file that we want to copy it to (this location is always in the workspace) */
    QFile _newFile;

    /*! The name of the project associated with this video */
    QString _projName;

};
#endif // !defined(EA_B17885EF_D04F_4e4e_A2BD_1A3F244C39DA__INCLUDED_)
