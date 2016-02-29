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
    displays.push_back(new IsoSurface  (*this));
    displays.push_back(new CrossSection(*this));
    displays.push_back(new Solid       (*this));

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

void Visualization::Display::draw(int subSceneIndex, bool selectEnabled) {
    DisplayBase::draw(subSceneIndex, selectEnabled);
}

void Visualization::Display::setSampleInterval(const int size) {
    DisplayBase::setSampleInterval(size);
}

void Visualization::Display::setSurfaceValue(const float value) {
    DisplayBase::setSurfaceValue(value);
}

void Visualization::Display::setMaxSurfaceValue(const float value) {
    DisplayBase::setMaxSurfaceValue(value);
}

bool Visualization::Display::setCuttingPlane(float position, float vecX,
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

void Visualization::Display::draw(int subSceneIndex, bool selectEnabled) {
    cur->draw(subSceneIndex, selectEnabled);
}

void Visualization::Display::setSampleInterval(const int size) {
    cur->setSampleInterval(size);
}

void Visualization::Display::setSurfaceValue(const float value) {
    cur->setSurfaceValue(value);
}

void Visualization::Display::setMaxSurfaceValue(const float value) {
    cur->setMaxSurfaceValue(value);
}

bool Visualization::Display::setCuttingPlane(float position, float vecX,
                                             float vecY, float vecZ)
{
    return cur->setCuttingPlane(position, vecX, vecY, vecZ);
}
//    #ifdef GORGON_DEBUG
          cout<<"\033[32mDEBUG: File:   Display.cpp"<<endl;
          cout<<"DEBUG: Method: Display::load(string)\033[0m"<<endl;
          cout<<"this: "<<this<<endl;
          cout<<getSize()<<endl;
//    #endif

    bool calc = calculateDisplay();
//    #ifdef GORGON_DEBUG
          cout<<"\033[33mDEBUG: File:   Display.cpp"<<endl;
          cout<<"DEBUG: Method: Display::load(string)\033[0m"<<endl;
          cout<<"calculateDisplay(): "<<calc<<endl;
          cout<<"getSize(): "<<getSize()<<endl;
//    #endif

#endif

