#include "BvRegion.h"

/*!
 * \brief BvRegion::BvRegion constructs a region, allocates the given parameters.
 *
 * \param x
 * \param y
 * \param width
 * \param height
 * \param name
 */
BvRegion::BvRegion(int x, int y, int width, int height, QString name)
{
    _x = x;
    _y = y;
    _width = width;
    _height = height;
    _name = name;
    _notes = "";
}

/*!
 * Default constructor
 */
BvRegion::BvRegion()
{
}

/*!
 * Default destructor
 */
BvRegion::~BvRegion()
{
}

/*!
 * \brief BvRegion::getQTreeWidgetItem updates the name of the widget item and returns it.
 *
 * \return the item to display in the project browser.
 */
QTreeWidgetItem* BvRegion::getQTreeWidgetItem()
{
    QTreeWidgetItem* output = new QTreeWidgetItem(2);
    output->setText(0, _name);
    return output;
}
