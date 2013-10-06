/*!
 * \class AnalyzeCheckDialog
 *
 * AnalyzeCheckDialog is launched when the user attempts to start an Analyze run.  It prompts the user to enter an experiment
 * name, which is the folder that the Analysis results will be saved under.  It also confirms that the user wants to begin
 * an analysis operation (this can be time consuming).
 *
 * The dialog is shown through a method on WindowManager called launchAnalyzeCheckDialog.  This method is called by
 * MainWindow whenever the analyze button is pressed and it is not a preview operation (previews don't need to be confirmed)
 * When the user hits ok, this dialog then saves the experiment data by calling a WindowManager method called
 * saveExperimentData.  This creates the folder in the right directory within the workspace.
 */

#ifndef AnalyzeCheckDialog_H
#define AnalyzeCheckDialog_H

class WindowManager;

#include <QDialog>
#include "WindowManager.h"

namespace Ui {
class AnalyzeCheckDialog;
}

class AnalyzeCheckDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AnalyzeCheckDialog(WindowManager* windowManager, QWidget *parent = 0);
    ~AnalyzeCheckDialog();

    void accept();
    void setAnalyzeInfo(QString projName, QString videoName, QString detailedText);

public slots:
    void saveSlot();
    void checkInputSlot(QString text);
    
private:
    /*! The UI file from the designer. */
    Ui::AnalyzeCheckDialog *ui;

    /*! A reference to WindowManager to save the experiment data when the user hits 'Ok'. */
    WindowManager* _windowManager;

    /*! a variable to hold whether or not the window should close (if data is valid or not) */
    bool _closeWindow;

    /*! the project name- needed to save the experiment folder. */
    QString _projName;

    /*! the video name- also needed to save the experiment folder. */
    QString _vidName;

};

#endif // AnalyzeCheckDialog_H
