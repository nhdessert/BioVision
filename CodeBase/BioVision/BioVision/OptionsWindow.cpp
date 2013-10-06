#include "optionswindow.h"
#include "ui_OptionsWindow.h"

/*!
 * \brief OptionsWindow::OptionsWindow sets up the UI, gets the old workspace, sets the window title, and connects the slots
 * needed to save the workspace.
 *
 * \param windowManager A reference to the controller window manager for saving the data.
 *
 * \param parent The QWidget parent.
 */
OptionsWindow::OptionsWindow(WindowManager *windowManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsWindow)
{
    ui->setupUi(this);

    _windowManager = windowManager;

    // set the title of the window.
    this->setWindowTitle("BioVision Settings");

    _oldWorkspace = _windowManager->getWorkspace();

    ui->workspace->setText(_oldWorkspace);

    // Save the data.
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSlot()));

    // Open a QFile Dialog to change the workspace directory.
    connect(ui->setWorkspaceButton, SIGNAL(clicked()), this, SLOT(setWorkspaceSlot()));
}

/*!
 * \brief OptionsWindow::~OptionsWindow delete the ui file on destruction.
 */
OptionsWindow::~OptionsWindow()
{
    delete ui;
}

/*!
 * \brief OptionsWindow::saveSlot called when the user hits the 'ok' button.  Handles prompts and data validation also.
 *
 * Checks to make sure it is not an empty string, then checks to be sure that the directory the user has input exists,
 * and then does a final prompt to make sure the user wants to change directories (this will prevent the old projects
 * from auto-loading).
 */
void OptionsWindow::saveSlot()
{
    QMessageBox errorMsg;
    errorMsg.setStandardButtons(QMessageBox::Ok);
    // validate data- make sure that the workspace is not an empty string.
    if(ui->workspace->text()=="")
    {
        errorMsg.setText("If you do not select a workspace you will be unable to create new projects.");
        errorMsg.setInformativeText("Please set a workspace.");
        errorMsg.exec();
    }
    else
    {
        QDir dir(ui->workspace->text());
        if(dir.exists())
        {
            QMessageBox msg;
            msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msg.setText("Are you sure you want to change your workspace?");
            msg.setInformativeText("This will stop all projects in the current workspace from auto-loading.");
            int ret = msg.exec();

            switch (ret)
            {
                case QMessageBox::Ok:
                    // We want to overwrite the workspace.
                    _windowManager->saveOptions(ui->workspace->text());
                    this->accept();
                break;

                case QMessageBox::Cancel:
                    // Reload the old text.
                    ui->workspace->setText(_oldWorkspace);
                break;

                default:
                    // should never be reached
                break;
            }
        }
        else
        {
            errorMsg.setText("This is not a valid file path in the system.");
            errorMsg.setInformativeText("Please set a valid filepath for the workspace.");
            errorMsg.exec();
        }
    }
}

/*!
 * \brief OptionsWindow::setWorkspaceSlot called when the user clicks the tool button to select a new directory.
 *
 * Gets the directory they select and loads it into the value of the workspace text box.
 */
void OptionsWindow::setWorkspaceSlot()
{
    QString workspacePath = QFileDialog::getExistingDirectory(this, "Please specify a workspace directory.", ui->workspace->text());

    if(workspacePath!="")
        ui->workspace->setText(workspacePath);
}
