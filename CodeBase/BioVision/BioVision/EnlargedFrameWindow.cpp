#include "EnlargedFrameWindow.h"
#include "ui_EnlargedFrameWindow.h"

/*!
 * \brief EnlargedFrameWindow::EnlargedFrameWindow sets up the ui file.
 *
 * \param parent the QWidget parent.
 */
EnlargedFrameWindow::EnlargedFrameWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnlargedFrameWindow)
{
    ui->setupUi(this);
    pix = new QPixmap();
}

/*!
 * \brief EnlargedFrameWindow::~EnlargedFrameWindow deletes the UI file.
 */
EnlargedFrameWindow::~EnlargedFrameWindow()
{
    delete ui;
}

/*!
 * \brief EnlargedFrameWindow::displayFrame displays a frame with the given filename in a pop up window.
 *
 * \param fileName the name of the file (as a precondition the file must be located in the tmp directory).
 */
void EnlargedFrameWindow::displayFrame(QString fileName)
{
    _scene = new QGraphicsScene();
#ifdef WIN32
    pix->load("tmp\\" + fileName);
    if(pix->isNull())
    {
        pix->load("tmp/" + fileName);
    }
#else
    pix->load("tmp/" + fileName);
#endif

    // Set up the image
    _scene->addPixmap(*pix);
    _scene->setSceneRect( pix->rect() );
    ui->frameView->setScene(_scene);
    ui->frameView->setGeometry(0,0, _scene->width(), _scene->height());
    this->setGeometry( this->x(), this->y(), ui->frameView->width(), ui->frameView->height());

    // Move the frame to the center of the screen.
    QDesktopWidget *desktop = QApplication::desktop();
    int x, y;
    x = (desktop->width() - this->width()) / 2;
    y = (desktop->height() - this->height()) / 2;
    y -= 50;
    this->move(x, y);

    // Display the frame.
    ui->frameView->show();
}
