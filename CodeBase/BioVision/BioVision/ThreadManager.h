/*!
 *  \class ThreadManager
 *
 *  ThreadManager is responsible for starting new threads when the bvSystem object asks for them and for making sure those
 * threads are finished properly.  It does so through signals and slots.
 *
 * All threads are implemented with the QThread class, which is a cross-platform thread wrapping class.
 */

#ifndef THREADMANAGER_H
#define THREADMANAGER_H

class BvSystem;

#include "BvSystem.h"
#include "BvThreadWorker.h"
#include "QObject"

class ThreadManager : public QObject
{

    Q_OBJECT

public:
    ThreadManager(BvSystem* system);
    virtual ~ThreadManager();

    /*! A reference to the system so that we can connect signals back to it. */
    BvSystem* _bvSystem;

    bool startThread(BvThreadWorker *worker);

    QString getCurrentTaskMessage();

public Q_SLOTS:
    void taskFinished();

private:
    /*! The currently running task's busy message. */
    QString _currentMessage;

    /*! Whether a task is running right now or not. */
    bool _taskRunning;

};
#endif // !defined(EA_3EDCA5F3_8ADE_465a_A0B1_0125E8E320BE__INCLUDED_)
