/*
 * VolumeRenderer.h
 *
 *  Created on: Feb 18, 2016
 *      Author: shadow_walker
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_
#define SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_

#include "DisplayBase.h"
#include "RendererBase.h"
#include <Core/volume.h>

namespace Visualization {

    typedef vector<DisplayBase *> DisplayListType;

    class Display : public Volume, public RendererBase {
        public:
            Display();

            bool calculateDisplay();
            void setViewingType(const int type);

            void draw(int subSceneIndex, bool selectEnabled);
            void setSampleInterval(const int size);
            void setSurfaceValue(const float value);
            void setMaxSurfaceValue(const float value);
            bool setCuttingPlane(float position, float vecX, float vecY, float vecZ);

            void enableDraw(bool enable);
            void setDisplayRadius(const int radius);
            void setDisplayRadiusOrigin(float radiusOriginX,
                                        float radiusOriginY,
                                        float radiusOriginZ);
            void load(string fileName);
            void initializeOctree();
            void useDisplayRadius(bool useRadius);

        private:
            DisplayListType displays;
            DisplayBase * cur;

            bool drawEnabled;
            int displayRadius;
            Vec3F radiusOrigin;
            bool _useDisplayRadius;

    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_ */
