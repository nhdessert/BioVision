/*!
 * \class BvThreadWorker
 *
 * Abstract superclass for objects that can be moved to threads and do background work for BioVision.
 *
 * Implemented by Analyzer, DetailAnalyzer, and VideoCopier.  These classes override the virtual startSlot() function and allow
 * customized behavior when the thread is started.  This way thread manager does not need to know what kind of
 * task it has to perform, it just takes a task, connects the right signals and slots (signals defined here) and
 * starts the thread.
 */

#ifndef BVTHREADWORKER_H
#define BVTHREADWORKER_H

#include <QObject>
#include "Result.h"

class BvThreadWorker : public QObject
{
    Q_OBJECT

    public:
        BvThreadWorker();
        virtual ~BvThreadWorker();

        /*!
         * \brief _result pointer to the result object that the thread will modify and use.
         */
        Result *_result;//!< Can hold any number of different data- subclass of result will be instantiated */

        /*!
         * \brief _message holds a message for if the current task is running.  Customized by subclasses.
         */
        QString _message;

        void setMessage(QString message);
        QString getMessage();

        static bool _isCancelled;

    public Q_SLOTS:
        virtual void startSlot()=0;
        virtual void sendImageInfoSlot(QString imageName, QString index);
        virtual void clearCarouselSlot();
        virtual void progressConvert(qint64 progress);

    Q_SIGNALS:
        void progressSignal(int progress);
        void sendImageInfoSignal(QString name, QString index);
        void clearCarouselSignal();
        void sendResultSignal(Result* result);
        void displayErrorMessageSignal();
        void finished();
};


#endif // !defined(EA_06A1980D_A6EA_48c4_9415_D654F4B0BA9F__INCLUDED_)
