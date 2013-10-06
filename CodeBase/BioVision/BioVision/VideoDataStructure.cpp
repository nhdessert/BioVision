#include "VideoDataStructure.h"

VideoDS::VideoDS()
{
    filepath = "";
    name = "";
    ListOfRegions.reserve(2);
}

VideoDS::~VideoDS()
{
}

void VideoDS::load()
{
    filepath = QFileDialog::getOpenFileName();
    name = filepath.mid( filepath.lastIndexOf('/') + 1 );
    source = Phonon::MediaSource(filepath);
}
