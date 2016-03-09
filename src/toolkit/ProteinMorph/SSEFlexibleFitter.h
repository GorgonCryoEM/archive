/*
 * SSEFlexibleFitter.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_PROTEINMORPH_SSEFLEXIBLEFITTER_H_
#define SRC_TOOLKIT_PROTEINMORPH_SSEFLEXIBLEFITTER_H_

#include "Visualization/NonManifoldMesh.h"
#include <Core/volume.h>
//#include <MathTools/Vector3D.h>
#include <GraphMatch/Shape.h>

//using namespace GraphMatch;
//using namespace MathTools;
using namespace Visualization;
using namespace SkeletonMaker;

namespace Protein_Morph {
    struct SheetIdsAndSelect {
        unsigned char id;
        bool selected;
    };

    typedef NonManifoldMesh<bool, bool, SheetIdsAndSelect> NonManifoldMesh_SheetIds;

    class SSEFlexibleFitter {
        public:
            SSEFlexibleFitter(Volume * vol);
            ~SSEFlexibleFitter();
            void FitHelix(Shape * helix, double translationStepSize, double rotationStepSize, double discretizationStepSize, int maxStepCount);
            void FitSheet(int sheetId, NonManifoldMesh_SheetIds * sheetMesh, double translationStepSize, double rotationStepSize, double discretizationStepSize, int maxStepCount);

        private:
            Vec3F GetForceOnPoint(Vec3F pt);
            Vec3F GetForceOnHelix(Vec3F p, Vec3F q, double discretizationStepSize);
            Vec3F GetForceOnSheet(vector<Vec3F> pts);
            Vec3F GetTorqueOnPoint(Vec3F pt, Vec3F center);
            Vec3F GetTorqueOnHelix(Vec3F p, Vec3F q, double discretizationStepSize);
            Vec3F GetTorqueOnSheet(vector<Vec3F> pts);
            Vec3F WorldPointToVolume(Vec3F pt);
            Vec3F VolumePointToWorld(Vec3F pt);
            Vec3F VolumeVectorToWorld(Vec3F pt);
            void MoveHelix(Shape * helix, double translationStepSize, double rotationStepSize, double discretizationStepSize);
            void MoveSheet(int sheetId, NonManifoldMesh_SheetIds * sheetMesh, double translationStepSize, double rotationStepSize, double discretizationStepSize);

        private:
            Vec3F * forceField;
            Volume * volume;
    };

} /* namespace Protein_Morph */

#endif /* SRC_TOOLKIT_PROTEINMORPH_SSEFLEXIBLEFITTER_H_ */
