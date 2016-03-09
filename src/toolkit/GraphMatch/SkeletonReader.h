/*
 * SkeletonReader.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_GRAPHMATCH_SKELETONREADER_H_
#define SRC_TOOLKIT_GRAPHMATCH_SKELETONREADER_H_

//#include <SkeletonMaker/reader.h>
//#include <SkeletonMaker/volume.h>
//#include <MathTools/BasicDefines.h>
#include "Shape.h"
//#include <vector>
//#include <queue>
//#include <list>
//#include "GlobalConstants.h"
//
//using namespace std;
//using namespace SkeletonMaker;

namespace GraphMatch {
    class SkeletonReader {
        public:
            static int GetGraphIndex(vector<Shape*> & helixes, int helixNum, int cornerNum);
            static int GetGraphIndex(vector<Shape*> & helixes, int helixNum, Point3Pair * point);
            static StandardGraph * ReadFile(char * volumeFile, char * helixFile, char * sseFile, char * sheetFile);
            static Volume* getSheetsNoThreshold( Volume * vol, int minSize );
            static void ReadSheetFile(char * sheetFile, vector<Shape*> & helixes);
            static void ReadHelixFile(char * helixFile, char * sseFile, vector<Shape*> & helixes);
            static void FindSizes(int startHelix, int startCell, vector<Shape*> & helixList, Volume * vol, Volume * coloredVol, StandardGraph * graph);
            static void FindPaths(StandardGraph * graph);
            static void FindPath(int startIx, int endIx, vector<vector<Vec3I> > nodes, Volume * maskVol, StandardGraph * graph, bool eraseMask);
            static void FindCornerCellsInSheet(Volume * vol, Volume * paintedVol, vector<Shape*> & helixes, int sheetId);
            static int isSkeletonSheet(Volume * vol, int ox, int oy, int oz );
    };

} /* namespace GraphMatch */

#endif /* SRC_TOOLKIT_GRAPHMATCH_SKELETONREADER_H_ */
