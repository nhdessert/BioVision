#include "Video.h"

/*!
 * \brief Video::Video default constructor.
 */
Video::Video()
{
}

/*!
 * \brief Video::~Video default destructor.
 */
Video::~Video()
{
}

/*!
 * \brief Video::getQTreeWidgetItem updates the QTreeWidgetItem for this video, and then adds the regions to it.
 *
 * \return the item to display.
 */
QTreeWidgetItem* Video::getQTreeWidgetItem()
{
    QTreeWidgetItem* output = new QTreeWidgetItem(1);
    output->setText(0, _name);
    for(unsigned int i=0; i<_listOfRegions.size(); i++)
    {
        output->addChild( _listOfRegions[i]->getQTreeWidgetItem() );
    }
    return output;
}
