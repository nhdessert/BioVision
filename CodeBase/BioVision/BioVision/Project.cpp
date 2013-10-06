#include "Project.h"

/*!
 * Default Constructor
 */
Project::Project()
{
    _dataChanged = false;
}

/*!
 * Default Destructor
 */
Project::~Project()
{

}

/*!
 * \brief Project::setProjectDataChanged notifies the project that data has changed within the videos or a video's region.
 *
 * \param dataChanged true if data has changed, false otherwise.
 */
void Project::setDataChanged(bool dataChanged)
{
    _dataChanged = dataChanged;
}

/*!
 * Get the data for the project tree which holds all project information in the GUI
 */
QTreeWidgetItem* Project::getQTreeWidgetItem()
{
    QTreeWidgetItem *output = new QTreeWidgetItem(0);
    output->setText(0, _projectName);
    for(int i=0; i<_listOfVideos.size(); i++)
    {
        output->addChild( _listOfVideos[i]->getQTreeWidgetItem() );
    }
    return output;
}
