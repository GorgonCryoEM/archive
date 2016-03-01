/*
 * Display.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "Display.h"
#include "IsoSurface.h"
#include "CrossSection.h"
#include "Solid.h"

using namespace Visualization;

Display::Display()
{
    displays.push_back(new IsoSurface  (static_cast<Volume &>(*this)));
    displays.push_back(new CrossSection(static_cast<Volume &>(*this)));
    displays.push_back(new Solid       (static_cast<Volume &>(*this)));

    cur = displays[0];
//    #ifdef GORGON_DEBUG
          cout<<"\033[32mDEBUG: File:   Display.cpp"<<endl;
          cout<<"DEBUG: Method: Display::Display()\033[0m"<<endl;
          cout<<getSize()<<endl;
//    #endif

}

#ifdef BASE

bool Display::calculateDisplay() {
    return DisplayBase::calculateDisplay();
}

void Display::draw(int subSceneIndex, bool selectEnabled) {
    DisplayBase::draw(subSceneIndex, selectEnabled);
}

void Display::setSampleInterval(const int size) {
    DisplayBase::setSampleInterval(size);
}

void Display::setSurfaceValue(const float value) {
    DisplayBase::setSurfaceValue(value);
}

void Display::setMaxSurfaceValue(const float value) {
    DisplayBase::setMaxSurfaceValue(value);
}

bool Display::setCuttingPlane(float position, float vecX,
                                             float vecY, float vecZ)
{
    return DisplayBase::setCuttingPlane(position, vecX, vecY, vecZ);
}
#else

void Display::setViewingType(const int type) {
    cur = displays[type];

    cur->load3DTexture();
    cur->calculateDisplay();
}

bool Display::calculateDisplay() {
    return cur->calculateDisplay();
}

void Display::draw(int subSceneIndex, bool selectEnabled) {
    cur->draw(subSceneIndex, selectEnabled);
}

void Display::setSampleInterval(const int size) {
    cur->setSampleInterval(size);
}

void Display::setSurfaceValue(const float value) {
    cur->setSurfaceValue(value);
}

void Display::setMaxSurfaceValue(const float value) {
    cur->setMaxSurfaceValue(value);
}

bool Display::setCuttingPlane(float position, float vecX,
                                             float vecY, float vecZ)
{
    return cur->setCuttingPlane(position, vecX, vecY, vecZ);
}

void Display::enableDraw(bool enable) {
    if(drawEnabled != enable) {
        drawEnabled = enable;
        if(drawEnabled) {
            calculateDisplay();
        }
    }
}

void Display::setDisplayRadius(const int radius) {
        displayRadius = radius;
}

void Display::setDisplayRadiusOrigin(float radOrigX, float radOrigY,
                                     float radOrigZ)
{
    radiusOrigin = Vec3F(radOrigX, radOrigY, radOrigZ);
}

void Display::load(string fileName) {
//    for(DisplayListType::iterator it=displays.begin();
//            it!=displays.end();
//            ++it)
//    {
//        (*it)->load(fileName);
//    }

    Volume::load(fileName);
//    #ifdef GORGON_DEBUG
          cout<<"\033[32mDEBUG: File:   Display.cpp"<<endl;
          cout<<"DEBUG: Method: Display::load(string)\033[0m"<<endl;
          cout<<"this: "<<this<<endl;
          cout<<getSize()<<endl;
//    #endif

    initializeOctree();
    updateBoundingBox();

    #ifdef _WIN32
        glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
    #endif

    setDisplayRadiusOrigin(volData->getSizeX()/2, volData->getSizeY()/2, volData->getSizeZ()/2);
}

void Display::initializeOctree() {
#ifdef USE_OCTREE_OPTIMIZATION
    if(octree != NULL) {
        delete octree;
    }
    unsigned int sizeX = dataVolume->getSizeX();
    unsigned int sizeY = dataVolume->getSizeY();
    unsigned int sizeZ = dataVolume->getSizeZ();
    octree  = new VolumeRendererOctreeType(sizeX, sizeY, sizeZ);
    for(unsigned int x = 0; x < sizeX-1; x++) {
        for(unsigned int y = 0; y < sizeY-1; y++) {
            for(unsigned int z = 0; z < sizeZ-1; z++) {
                octree->AddNewLeaf(x, y, z, 1);
            }
        }
    }
    initializeOctreeTag(octree->GetRoot());
    float minVal, maxVal, val;
    VolumeRendererOctreeNodeType * node;

    for(unsigned int x = 0; x < sizeX-1; x++) {
        for(unsigned int y = 0; y < sizeY-1; y++) {
            for(unsigned int z = 0; z < sizeZ-1; z++) {
                node = octree->GetLeaf(x, y, z);
                minVal = MAX_FLOAT;
                maxVal = MIN_FLOAT;
                for(unsigned int xx = 0; xx < 2; xx++) {
                    for(unsigned int yy = 0; yy < 2; yy++) {
                        for(unsigned int zz = 0; zz < 2; zz++) {
                            val = dataVolume->(*this)(x+xx, y+yy, z+zz);
                            minVal = min(minVal, val);
                            maxVal = max(maxVal, val);
                        }
                    }
                }

                while(node != NULL) {
                    node->tag.maxVal = max(node->tag.maxVal, maxVal);
                    node->tag.minVal = min(node->tag.minVal, minVal);
                    node = node->parent;
                }
            }
        }
    }
#endif
}

void Display::useDisplayRadius(bool useRadius) {
    _useDisplayRadius = useRadius;
}

#endif

