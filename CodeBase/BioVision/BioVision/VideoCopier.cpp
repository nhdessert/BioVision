#include "VideoCopier.h"

/*!
 * \brief VideoCopier::VideoCopier The constructor takes a project name, and then a path to the video and a path to copy the
 * video too.  It also sets the busy message for this task (video is being copied).
 *
 * \param videoPath The current location of the video (file) to be copied.
 * \param projectPath The location (with filename) of where we will copy the file to.
 */
VideoCopier::VideoCopier(QString projName, QString videoPath, QString newPath)
{
    _fromFile.setFileName(videoPath);
    _newFile.setFileName(newPath);

    _projName = projName;

    // set the message that a video copier is running.
    setMessage("A video is currently being copied, please wait until this process finishes to run another request.");
}

/*!
 * \brief VideoCopier::~VideoCopier default destructor.
 */
VideoCopier::~VideoCopier()
{
    // everything here is statically allocated.
}

/*!
 * \brief VideoCopier::startSlot is run on the other thread.  Calls the copyVideo function to perform the actual copying,
 * sends the result signal after that is finished, and then sends the finished signal to end the thread.
 */
void VideoCopier::startSlot()
{
    copyVideo();

    emit sendResultSignal(_result);

    emit finished();
}

/*!
 * \brief VideoCopier::copyVideo performs the actual video copying logic.  Creates the result object and sets the data field
 * in the result based on whether or not the copying operation was successful.  If it is successful, sets the data field
 * to the new file name, if not, sets it to 'error'.  If the videopaths are the same (the video is already in the workspace)
 * then do not display an error, just update the file path like normal.  Also, if the video already exists in the workspace
 * but is being added from somewhere else, also display no error message, just update the file path.
 */
void VideoCopier::copyVideo()
{
    _result = new Result();

    // If the video selected is not the workspace video, and if the new file does not exist, then copy.
    if(_fromFile.fileName()!=_newFile.fileName() && (!QFile::exists(_newFile.fileName())))
    { 
        // Create the result object- pass it the copy call.  It will set the success of the result based on the copy call.
        bool success = _fromFile.copy(_newFile.fileName());

        if(success)
        {
            _result->setData(_newFile.fileName());
            _result->setProject(_projName);
        }
        else
            _result->setData("error");
    }

    // Otherwise, just update the filepath and return.
    else
    {
        _result->setData(_newFile.fileName());
        _result->setProject(_projName);
    }
}
