#include "ThreadManager.h"

/*!
 * \brief ThreadManager::ThreadManager initializes the system to the param, _taskRunning to false, and _currentMessage to "".
 *
 * \param system a reference to BvSystem for connecting some of system's slots to signals emitted by the thread.
 */
ThreadManager::ThreadManager(BvSystem* system)
{
    _bvSystem = system;

    // initialize task running to false.
    _taskRunning = false;

    // initialize current message
    _currentMessage = "";
}

/*!
 * \brief ThreadManager::~ThreadManager default destructor.
 */
ThreadManager::~ThreadManager()
{
}

/*!
 * \brief ThreadManager::taskFinished is a slot that sets the _taskRunning variable to false.  It is called when the
 * finished signal is emitted by the thread, indicating the thread has ended.
 */
void ThreadManager::taskFinished()
{
    _taskRunning = false;
}

/*!
 * \brief ThreadManager::getCurrentTaskMessage returns the message of the currently running task.
 *
 * \return a message that the currently running task has set.
 */
QString ThreadManager::getCurrentTaskMessage()
{
    return _currentMessage;
}

/*!
 *  ThreadManager::startThread checks to see if a task is currently running, and if not, creates a QThread,
 *  moves the worker to that new thread, and then connects the start, progress, result, and cleanup signals.
 *
 * \param worker The BvThreadWorker object that contains the slot we want to run on another thread.
 *
 * \return true if the thread was started successfully- there are no other tasks running.  False otherwise.
 */
bool ThreadManager::startThread(BvThreadWorker *worker)
{
    // If there is a task running, then return false- we can't run another one yet.
    if(_taskRunning)
    {
        return false;
    }
    // Otherwise, start the thread, and change _taskRunning to true, and return true.
    else
    {
        _taskRunning = true;

        // Update the current worker's task message so we can pass it back to BvSystem.
        _currentMessage = worker->getMessage();

        QThread* thread = new QThread;

        worker->moveToThread(thread);

        // Start the thread by connecting the started signal to our start slot.
        connect(thread, SIGNAL(started()), worker, SLOT(startSlot()));

        //Signal/slot for clearing the carousel before analyze begins.
        connect(worker, SIGNAL(clearCarouselSignal()), _bvSystem, SLOT(clearCarouselSlot()));

        // Signal/slot for sending progress bar updates.  Will get called often.
        connect(worker, SIGNAL(progressSignal(int)), _bvSystem, SLOT(progressUpdateSlot(int)));

        // Notifies the UI that an image has been written, and its filename.
        connect(worker, SIGNAL(sendImageInfoSignal(QString, QString)), _bvSystem, SLOT(updateCarouselSlot(QString, QString)));

        //Dustin added: sends a signal from an analysis when an error is thrown to display error window in the MainWindow class
        connect(worker, SIGNAL(displayErrorMessageSignal()), _bvSystem, SLOT(displayErrorWindowSlot()));

        // Allow the thread to send updates via the updateSignal- connect it to a test thread slot on the main window.
        connect(worker, SIGNAL(sendResultSignal(Result*)), _bvSystem, SLOT(handleResultSlot(Result*)));

        // Tell the thread to quit when the finished signal is emitted.
        connect(worker, SIGNAL(finished()), thread, SLOT(quit()));

        // Mark the worker instance for deletion.
        connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));

        // Causes the thread to be deleted only after it has fully shut down.
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        // Updates ThreadManager's _taskRunning value to false when the thread finishes.
        connect(thread, SIGNAL(finished()), this, SLOT(taskFinished()));

        // If the terminate signal is emitted (from the destructor, before finished has a chance to emit) then we still delete the thread.
        connect(thread, SIGNAL(terminated()), thread, SLOT(deleteLater()));

        thread->start(/*QThread::HighPriority*/);

        return _taskRunning;
    }
}
