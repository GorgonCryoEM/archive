/*
 * VolumeRenderer.h
 *
 *  Created on: Feb 18, 2016
 *      Author: shadow_walker
 */

#ifndef SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_
#define SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_

#include "DisplayBase.h"

namespace Visualization {

    class Display : public DisplayBase {
        public:
            Display();

            bool calculateDisplay();

        private:
            vector<DisplayBase *> displays;
    };
}




#endif /* SRC_TOOLKIT_VISUALIZATION_DISPLAY_H_ */
