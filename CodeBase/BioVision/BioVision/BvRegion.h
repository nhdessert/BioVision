/*!
 * \class BvRegion
 *
 * Class that describes and holds data for a region of a video.
 *
 * Has an x, y, width, and height, as well as other associated data.  Also includes a function to get a
 * QTreeWidgetItem for display in the project browser.
 *
 */

#ifndef BVREGION_H
#define BVREGION_H

#include <QTreeWidgetItem>
#include <QString>

class BvRegion
{
	public:

        /*! Creates a region, storing the x, y, width, and height, and name. */
        BvRegion(int,int,int,int,QString);

        BvRegion();
        ~BvRegion();

        //Properties
        /*! The region's x , expressed in pixels.*/
        int _x;

        /*! The region's y , expressed in pixels.*/
        int _y;

        /*! The region's width , expressed in pixels.*/
        int _width;

        /*! The region's height , expressed in pixels.*/
        int _height;

        /*! The region's name.*/
        QString _name;

        /*! The region's individual threshold.*/
        int _threshold;

        /*! The region's notes.*/
        QString _notes;

        // methods
        QTreeWidgetItem* getQTreeWidgetItem();
};

#endif
