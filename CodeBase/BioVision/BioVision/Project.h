/*!
 * \class BVProject
 *
 * Data structure class representing and holding data for a project in BioVision.
 *
 * Holds the name of the project, the widget item, and a vector of videos, which each hold the specific region data.
 */

#ifndef PROJECT_H
#define PROJECT_H

#include <QTreeWidgetItem>
#include <vector>
#include <QString>
#include "Video.h"

class Project
{

    public:
        Project();
        ~Project();

        /*! The list of videos in this project */
        std::vector<Video*> _listOfVideos;

        /*! The name of the project, must be unique in the workspace */
        QString _projectName;

        /*! The path to this project (filepath) */
        QString _path;

        QTreeWidgetItem* getQTreeWidgetItem();

        void setDataChanged(bool dataChanged);
        /**
         * True if the data has changed. This will be used to determine if saving
         * functionality should be envoked.
         */
        bool _dataChanged;
};
#endif // !defined(EA_2B4F1EA4_72E8_4c04_AEB1_235680C03567__INCLUDED_)
