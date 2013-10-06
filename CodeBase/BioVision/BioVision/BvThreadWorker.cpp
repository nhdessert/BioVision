#include "BvThreadWorker.h"

//static variable used to stop an analysis when it is cancled from the GUI
bool BvThreadWorker::_isCancelled = false;
/*!
 * Instantiates the boolean cancelTask to false.
 */
BvThreadWorker::BvThreadWorker()
{
    _message = "A task is currently running.";
}

/*!
 *  Deletes the result object.
 */
BvThreadWorker::~BvThreadWorker()
{
    //delete _result;
}

/*!
 * Sets a message for subclasses to send for communication when a thread is already running
 */
void BvThreadWorker::setMessage(QString message)
{
    _message = message;
}

/*!
 * Returns the message for subclasses to send for communication when threading
 */
QString BvThreadWorker::getMessage()
{
    return _message;
}

/*!
 * \brief BvThreadWorker::sendImageInfoSlot is implemented by subclasses.
 *
 * \param imageName the file name of the image to load.
 */
void BvThreadWorker::sendImageInfoSlot(QString imageName, QString index)
{
    // do nothing- subclasses can specify unique behaviors.
}

/*!
 * \brief BvThreadWorker::clearCarouselSlot is implemented by subclasses.
 */
void BvThreadWorker::clearCarouselSlot()
{
    //do nothing- subclasses can specify unique behaviors.
}

/*!
 * \brief BvThreadWorker::progressConvert is implemented by subclasses.
 *
 * \param progress is a measure of the progress of a given task.
 */
void BvThreadWorker::progressConvert(qint64 progress)
{
    // do nothing- subclasses can specify unique behaviors.
}
