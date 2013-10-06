/*!
 * \class OptionsWindow displays a dialog for the user that allows them to change BioVision settings.
 *
 * The only setting that can be changed right now is the location of the user's workspace.
 */

#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

class WindowManager;

#include <QDialog>
#include "WindowManager.h"

namespace Ui {
class OptionsWindow;
}

class OptionsWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptionsWindow(WindowManager *windowManager, QWidget *parent = 0);
    ~OptionsWindow();
    
public slots:
    void saveSlot();
    void setWorkspaceSlot();

private:
    Ui::OptionsWindow *ui;

    /*! Reference to window manager to pass back requests to save the options. */
    WindowManager* _windowManager;

    /*! A string holding the old workspace, to refresh the data if the user cancels a request. */
    QString _oldWorkspace;
};

#endif // OPTIONSWINDOW_H
