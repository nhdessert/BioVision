/*!
 * \class AboutWindow
 *
 * AboutWindow displays text referencing information BioVision.  This text is stored in the .cpp file.
 *
 * Information includes:  Developers, sponsor, library information, and version number.
 */

#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#define VERSION "1.0.1" //TODO: Get actual version number

#include <QWidget>

namespace Ui {
class AboutWindow;
}

class AboutWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit AboutWindow(QWidget *parent = 0);
    ~AboutWindow();
    
private:
    Ui::AboutWindow *ui;
};

#endif // ABOUTWINDOW_H
