#include "AnalyzeCheckDialog.h"
#include "ui_AnalyzeCheckDialog.h"
#include "QMessageBox"

/*!
 * \brief AnalyzeCheckDialog::AnalyzeCheckDialog constructs the dialog.  Sets the 'Yes' button to disabled so that
 * the user has to enter an experiment name before they can begin the analysis.  Also connects signals and slots,
 * and sets the focus on the text box, allowing the user to type.
 *
 * \param windowManager a reference to WindowManager that allows the dialog to save data.
 * \param parent The QWidget parent of the dialog.
 */
AnalyzeCheckDialog::AnalyzeCheckDialog(WindowManager* windowManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnalyzeCheckDialog)
{
    ui->setupUi(this);

    _windowManager = windowManager;

    // Initialize close window, which stops events from closing the dialog if it is false.
    _closeWindow = false;

    ui->buttonBox->button(QDialogButtonBox::Yes)->setEnabled(false);

    ui->experimentName->setFocus();

    // set window modality so that the user can't interact with the main app.
    this->setWindowModality(Qt::ApplicationModal);

    // Save the data.
    connect(ui->buttonBox->button(QDialogButtonBox::Yes), SIGNAL(clicked()), this, SLOT(saveSlot()));
    connect(ui->experimentName, SIGNAL(textChanged(QString)), this, SLOT(checkInputSlot(QString)));
}

/*!
 * \brief AnalyzeCheckDialog::~AnalyzeCheckDialog deletes the UI file.  No other memory is dynamically allocated in
 * this class.
 */
AnalyzeCheckDialog::~AnalyzeCheckDialog()
{
    delete ui;
}

/*!
 * \brief AnalyzeCheckDialog::saveSlot validates the user input of Experiment name. If a name has been entered, it
 * attempts to save this experiment by calling windowManager->saveExperimentData.  If this returns false, the experiment
 * already exists, and so we notify the user of that and allow them to decide whether or not to overwrite it.
 * If all goes well, close the dialog.
 *
 */
void AnalyzeCheckDialog::saveSlot()
{
    QRegExp regExp("^[a-zA-Z0-9 ]{1,27}$");

    if(!regExp.exactMatch(ui->experimentName->text()))
    {
        // launch a message box- we had an error- The experiment can only contain alpha numeric characters.
        //   with that name.
        QMessageBox errorMsg;
        errorMsg.setText("Experiment name cantains bad characters.");
        errorMsg.setInformativeText("The experiment name can only contain alpha numeric characters and be 27 characters long.");
        errorMsg.exec();

        return;
    }

    if(!_windowManager->saveExperimentData(_projName, _vidName, ui->experimentName->text()))
    {
        QMessageBox runExistsPrompt;
        runExistsPrompt.setText("This trial data already exists.  Overwrite?");
        runExistsPrompt.setInformativeText("This will remove old data.");
        runExistsPrompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        runExistsPrompt.setDefaultButton(QMessageBox::No);

        int ret = runExistsPrompt.exec();
        switch (ret)
        {
            case QMessageBox::Yes:
                // We want to overwrite
                _closeWindow = true;
                _windowManager->setOverwriteOldExperiment(true);
                this->accept();
            break;

            case QMessageBox::No:
                // Don't do anything then, just focus on the text box so they can change the name.
                ui->experimentName->setFocus();
            break;

            default:
                // should never be reached
            break;
        }
    }
    else
    {
        _closeWindow = true;
        this->accept();
    }
}

/*!
 * \brief AnalyzeCheckDialog::setAnalyzeInfo called from WindowManager, sets the information of the analysis
 * to display to the user for review.
 *
 * \param projName The project name that the video belongs to.
 * \param videoName The video that is going to be analyzed.
 * \param detailedText The specific settings that the user requested for this analysis.
 */
void AnalyzeCheckDialog::setAnalyzeInfo(QString projName, QString videoName, QString detailedText)
{
    _projName = projName;
    _vidName = videoName;
    ui->videoLabel->setText("Preparing to analyze " + videoName + ":");
    ui->detailedText->setText(detailedText);
}

/*!
 * \brief AnalyzeCheckDialog::checkInputSlot Validates the input on the text box.  If it is empty, disable the 'Yes' button.
 *
 * \param text The text currently in the text box.
 */
void AnalyzeCheckDialog::checkInputSlot(QString text)
{
    if(text!="")
        ui->buttonBox->button(QDialogButtonBox::Yes)->setEnabled(true);
    else
        ui->buttonBox->button(QDialogButtonBox::Yes)->setEnabled(false);
}

/*!
 * \brief AnalyzeCheckDialog::accept override accept so that the dialog only closes when data has been validated.
 */
void AnalyzeCheckDialog::accept()
{
    if(_closeWindow)
        QDialog::accept();
}
