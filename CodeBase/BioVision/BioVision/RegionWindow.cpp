#include "RegionWindow.h"
#include "ui_RegionWindow.h"

RegionWindow::RegionWindow(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::RegionWindow)
{
    ui->setupUi(this);
}

RegionWindow:: RegionWindow(QWidget* parent, WindowManager* windowManager, QString projName,
                            QString vidName, QString regionName, int threshold, QString notes, int x,
                            int y, int width, int height, QString pathToPicture) :
    QDialog(parent),
    ui(new Ui::RegionWindow)
{
    //Setup properties
    _windowManager = windowManager;
    _projName = projName;
    _vidName = vidName;
    _regionName = regionName;
    _notes = notes;
    _x = x;
    _y = y;
    _width = width;
    _height = height;
    _pathToPicture = pathToPicture;


    // Set default threshold to 0 unless the user has saved one already.
    if((threshold > 100) || (threshold < 0))
    {
        _threshold = 0;
    }
    else
    {
        _threshold = threshold;
    }

    // Set up this window.
    ui->setupUi(this);

    //Set GUI elements from properties
    ui->RegionNameText->setText(_regionName);
    ui->NotesText->setPlainText(_notes);
    ui->ThresholdSlider->setValue(_threshold);
    ui->regionWidthLabelNum->setText(QString::number(_width));
    ui->regionHeightLabelNum->setText(QString::number(_height));
    ui->regionXLabelNum->setText(QString::number(_x));
    ui->regionYLabelNum->setText(QString::number(_y));
    setGraphicsDisplay(_pathToPicture);


    QString s;
    s = s.number(_threshold);
    ui->ThresholdNum->setText(s);

    // Set the focus to the text box.
    ui->RegionNameText->setFocus();

    // When the user clicks enter it should save the dialog.
    ui->regionButtons->button(QDialogButtonBox::Save)->setDefault(true);
    ui->regionButtons->button(QDialogButtonBox::Save)->setAutoDefault(true);

    // Disable the save button if the passed in text is empty or null.
    // It is re-enabled if user types in the region name box.
    if( _regionName=="" || _regionName == QString() )
        ui->regionButtons->button(QDialogButtonBox::Save)->setEnabled(false);

    //Connect the threshold slider- change the value in text box as the user moves the slider.
    connect(ui->ThresholdSlider,SIGNAL(valueChanged(int)), this, SLOT(sliderChangedSlot(int)));

    // and the reverse- change the slider if the value in the text box changes.
    connect(ui->ThresholdNum, SIGNAL(textChanged(QString)), this, SLOT(moveSliderSlot(QString)));

    // Connect the save button- save the data when it is clicked.
    connect(ui->regionButtons, SIGNAL(accepted()), this, SLOT(saveSlot()));

    // Enable the save button if the user types a region name.
    connect(ui->RegionNameText, SIGNAL(textChanged(QString)), this, SLOT(checkInputSlot(QString)));
}

RegionWindow::~RegionWindow()
{
    delete _image;
    //delete _pen;
    delete _scene;
    delete ui;
}

/*!
 * \brief RegionWindow::saveSlot Saves a region.  If the x and y parameters are 0, that signals an update, this region already exists.
 * therefore we don't pass those values into setRegion, just let the default parameters handle it.  If we do have values
 * for x and y and width and height that are non 0, then this is a region creation, so pass those values in as well.
 */
void RegionWindow::saveSlot()
{
    QRegExp regExp("^[a-zA-Z0-9 ]{1,27}$");

    if(!regExp.exactMatch(ui->RegionNameText->text()))
    {
        // launch a message box- we had an error- The region name can only contain alpha numeric characters.
        //   with that name.
        QMessageBox errorMsg;
        errorMsg.setText("Region name cantains bad characters.");
        errorMsg.setInformativeText("The region name can only contain alpha numeric characters and be 27 characters long.");
        errorMsg.exec();

        return;
    }

    if(_x==0 && _y==0 && _height==0 && _width==0)
        _windowManager->setRegion(_projName, _vidName, _regionName, ui->RegionNameText->text(), _threshold, ui->NotesText->toPlainText());
    else
        _windowManager->setRegion(_projName, _vidName, _regionName, ui->RegionNameText->text(), _threshold, ui->NotesText->toPlainText(), _x, _y, _width, _height);
}

/*!
 * \brief RegionWindow::checkInputSlot Called every time the text in the region name text box is edited.  If the text is
 * "", then invalidate the save button, so the user cannot save a region with an empty name.
 *
 * \param text the text that has changed (from the region name text box)
 */
void RegionWindow::checkInputSlot(QString text)
{
    if(text!="")
        ui->regionButtons->button(QDialogButtonBox::Save)->setEnabled(true);
    else
        ui->regionButtons->button(QDialogButtonBox::Save)->setEnabled(false);
}

/*!
 * \brief RegionWindow::sliderChangedSlot sets the value of the slider to the box next to it.
 * \param value the value to set in the box.
 */
void RegionWindow::sliderChangedSlot(int value)
{
    _threshold = value;
    QString s;
    s = s.number(_threshold);
    ui->ThresholdNum->setText(s);
}

/*!
 * \brief MainWindow::moveSliderSlot moves the slider if the user edits the value in the text box.
 *
 * \param value the value to set in the box.
 */
void RegionWindow::moveSliderSlot(QString value)
{
    QRegExp regExp("^[0-9]{0,2}$");

    int val;

    if(!regExp.exactMatch(value))
    {
        val = _threshold;
        ui->ThresholdNum->setText(QString::number(_threshold));
    }
    else
    {
        val = value.toInt();

        if(val<=0)
            ui->ThresholdSlider->setValue(0);
        else if((val>0) && (val<100))
            ui->ThresholdSlider->setValue(val);
        else
            ui->ThresholdSlider->setValue(100);
    }
}


void RegionWindow::setGraphicsDisplay(QString path)
{
    _image = new QPixmap();
    if(_image->load(path))
    {
        //Commented out for final release
        //qDebug() << "Everything went better than expected.";
    }
    else
    {
        //Commented out for final release
        //qDebug() << "Could not load " << path;
    }
    _xResolution = _image->width();
    _yResolution = _image->height();
    setupResolutionConverters();

    _scene = new QGraphicsScene();
    _scene->setSceneRect( 0,0, ui->graphicsDisplay->width(), ui->graphicsDisplay->height() );

    if( !_image->isNull() )
    {
        *_image = _image->scaled( ui->graphicsDisplay->width() - _xBlackSpace, ui->graphicsDisplay->height() - _yBlackSpace);
        _imageItem = _scene->addPixmap(*_image);
        _imageItem->setOffset( _xBlackSpace / (double)2, _yBlackSpace / (double)2 );
    }
    ui->graphicsDisplay->setScene(_scene);
}


int RegionWindow::frameXToUiX(int frameX)
{
    int out = roundToNearest( ((double)frameX + _xBlackSpace/ (double)2) / _xConverter );
    return out;
}


int RegionWindow::frameYToUiY(int frameY)
{
    int out = roundToNearest( ((double)frameY + _yBlackSpace/ (double)2) / _yConverter );
    return out;
}


void RegionWindow::setupResolutionConverters()
{
    _xScaling = (double)ui->graphicsDisplay->width() / _xResolution;

    //Commented out for final release
    //qDebug() << _xScaling;

    _yScaling = (double)ui->graphicsDisplay->height() / _yResolution;

    //Commented out for final release
    //qDebug() << _yScaling;

    _xBlackSpace = max(0, (double)ui->graphicsDisplay->width() - _xResolution * _yScaling);

    //Commented out for final release
    //qDebug() << _xBlackSpace;

    _yBlackSpace = max(0, (double)ui->graphicsDisplay->height() - _yResolution * _xScaling);

    //Commented out for final release
    //qDebug() << _yBlackSpace;

    _xConverter = _xResolution / (double)(ui->graphicsDisplay->width() - _xBlackSpace);

    //Commented out for final release
    //qDebug() << _xConverter;

    _yConverter = _yResolution / (double)(ui->graphicsDisplay->height() - _yBlackSpace);

    //Commented out for final release
    //qDebug() << _yConverter;
    //qDebug() << "Converters for RegionWindowsetup.";
}
inline double RegionWindow::min(double x, double y)
{
    return (x <= y) ? x : y;
}

inline double RegionWindow::max(double x, double y)
{
    return (x >= y) ? x : y;
}

inline int RegionWindow::roundToNearest(double x)
{
    return (x-(int)x >= 0.5) ? ((int)x)+1 :  (int)x;
}
