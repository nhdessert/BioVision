/*!
 * \class EnlargedFrameWindow displays an image from the carousel when the user double-clicks it.
 */

#ifndef ENLARGEDFRAMEWINDOW_H
#define ENLARGEDFRAMEWINDOW_H

#include <QWidget>
#include <QDialog>
#include <QGraphicsScene>
#include <QDesktopWidget>

namespace Ui {
class EnlargedFrameWindow;
}

class EnlargedFrameWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit EnlargedFrameWindow(QWidget *parent = 0);
    ~EnlargedFrameWindow();
    void displayFrame(QString fileName);
    
private:
    Ui::EnlargedFrameWindow *ui;
    QPixmap *pix;

    /*! QGraphicsScene to display the image */
    QGraphicsScene *_scene;
};

#endif // ENLARGEDFRAMEWINDOW_H
