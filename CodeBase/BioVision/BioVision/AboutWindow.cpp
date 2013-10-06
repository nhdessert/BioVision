#include "AboutWindow.h"
#include "ui_AboutWindow.h"

/*!
 * \brief AboutWindow::AboutWindow sets up the UI and sets the version text.
 *
 * \param parent the parent of the widget.
 */
AboutWindow::AboutWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
}

/*!
 * \brief AboutWindow::~AboutWindow delete the ui file, that is the only reference.
 */
AboutWindow::~AboutWindow()
{
    delete ui;
}
