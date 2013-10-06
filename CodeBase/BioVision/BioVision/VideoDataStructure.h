#ifndef VIDEODATASTRUCTURE_H
#define VIDEODATASTRUCTURE_H

#include "BvRegion.h"
#include <QString>
#include <QtGui/qfiledialog.h>
#include <phonon>
#include <vector>

class VideoDS
{
	public:
        //Constructors
		VideoDS();
		~VideoDS();

        //Properties
        QString filepath;
        QString name;
        Phonon::MediaSource source;
        std::vector<Region*> ListOfRegions;

        //Methods
        void load();
		void addRegion(Region);
	private:
};

#endif
