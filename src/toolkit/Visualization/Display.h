/*
 * VolumeRenderer.h
 *
 *  Created on: Feb 18, 2016
 *      Author: shadow_walker
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_
#define SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_

#include "IsoSurface.h"

namespace Visualization {

    class Display : public IsoSurface {
        public:
            Display();

        private:
            vector<DisplayBase *> displays;
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_ */
