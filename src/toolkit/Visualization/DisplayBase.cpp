/*
 * DisplayBase.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "DisplayBase.h"
#include "Foundation/StringUtils.h"

namespace Visualization {

    DisplayBase::DisplayBase(Volume & V)
        : RendererBase(),
          cuttingVolume(Volume(2, 2, 2)),
          vol(&V)
    {
        textureLoaded = false;

        surfaceMesh = new VolumeSurfaceMeshType();
        octree = NULL;
        surfaceValue = 1.5;
        sampleInterval = 1;
        cuttingMesh = new NonManifoldMesh();
    }

    DisplayBase::~DisplayBase() {
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }
        delete surfaceMesh;
        if(octree != NULL) {
            delete octree;
        }
        delete cuttingMesh;
    }

    float DisplayBase::getSurfaceValue() const {
        return surfaceValue;
    }

    int DisplayBase::getSampleInterval() const  {
        return sampleInterval;
    }

    string DisplayBase::getSupportedLoadFileFormats() {
        return "All Files (*.mrc *.ccp4 *.map *.raw *.pts);; Volumes (*.mrc *.ccp4 *.map *.raw);;Point Cloud (*.pts)";
    }

    string DisplayBase::getSupportedSaveFileFormats() {
        return "Volumes (*.mrc *.ccp4 *.map *.raw);;Mathematica List (*.nb);;Bitmap Image set (*.bmp);;Structure Tensor Field (*.tns);;Surface Mesh(*.off)";
    }

    bool DisplayBase::setCuttingPlane(float position, float vecX, float vecY, float vecZ) {
        return false;
    }


    void DisplayBase::initializeOctreeTag(VolumeRendererOctreeNodeType * node) {
        if(node != NULL) {
            Range tag;
            node->tag = tag;
            if (!node->isLeaf) {
                for(int i = 0; i < 8; i++) {
                    initializeOctreeTag(node->children[i]);
                }
            }
        }
    }

    void DisplayBase::draw(int subSceneIndex, bool selectEnabled) {
    }


    void DisplayBase::calculateOctreeNode(VolumeRendererOctreeNodeType * node) {
        queue<VolumeRendererOctreeNodeType *> q;
        q.push(node);

        while(!q.empty()) {
            node = q.front();
            q.pop();
            if((node->tag.min <= surfaceValue) && (node->tag.max >= surfaceValue)) {
                if((int)node->cellSize <= sampleInterval + sampleInterval) {
                    for(int i = 0; i < 8; i++) {
                        if(node->children[i] != NULL) {
                            MarchingCube(vol, surfaceMesh, surfaceValue, node->children[i]->pos[0], node->children[i]->pos[1], node->children[i]->pos[2], sampleInterval);
                        }
                    }
                } else {
                    for(int i = 0; i < 8; i++) {
                        if(node->children[i] != NULL) {
                            q.push(node->children[i]);
                        }
                    }
                }
            }
        }
    }

    bool DisplayBase::calculateDisplay() {
        return false;
    }

    void DisplayBase::load3DTexture() {
    }

//        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   DisplayBase.cpp"<<endl;
              cout<<"DEBUG: Method: DisplayBase::load(string)\033[0m"<<endl;
              cout<<(Volume)(*this)<<endl;
//        #endif


//        #ifdef GORGON_DEBUG
              cout<<"\033[32mDEBUG: File:   DisplayBase.cpp"<<endl;
              cout<<"DEBUG: After load()\033[0m"<<endl;
              cout<<(Volume)(*this)<<endl;
//        #endif

    void DisplayBase::save(string fileName) {
        if(vol != NULL) {
            int pos = fileName.rfind(".") + 1;
            string extension = fileName.substr(pos, fileName.length()-pos);

            extension = StringUtils::StringToUpper(extension);

            if(strcmp(extension.c_str(), "MRC") == 0) {
                vol->toMRCFile((char *)fileName.c_str());
            } else {
                printf("Input format %s not supported!\n", extension.c_str());
            }


        }
    }

    void DisplayBase::MarchingCube(Volume * vol, Mesh * mesh, const float iso_level, int iX, int iY, int iZ, int iScale){
//        extern int aiCubeEdgeFlags[256];
//        extern int a2iTriangleConnectionTable[256][16];

        int iVertex, iFlagIndex, iEdgeFlags;
        float fOffset;
        float afCubeValue[8];
        Vec3D asEdgeVertex[12];
        int vertexIds[12];

        //Make a local copy of the values at the cube's corners
        for(int iVertex = 0; iVertex < 8; iVertex++) {
            afCubeValue[iVertex] = vol->getVoxelData(iX + a2iVertexOffset[iVertex][0]*iScale,
                                                iY + a2iVertexOffset[iVertex][1]*iScale,
                                                iZ + a2iVertexOffset[iVertex][2]*iScale);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(int iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(afCubeValue[iVertexTest] <= iso_level)
                        iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                return;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for(int iEdge = 0; iEdge < 12; iEdge++)
        {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge))
                {
                        fOffset = vol->getOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], afCubeValue[ a2iEdgeConnection[iEdge][1] ], iso_level);

                        asEdgeVertex[iEdge][0] = (float)iX + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][0] +  fOffset * (float)a2iEdgeDirection[iEdge][0]) * (float)iScale;
                        asEdgeVertex[iEdge][1] = (float)iY + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][1] +  fOffset * (float)a2iEdgeDirection[iEdge][1]) * (float)iScale;
                        asEdgeVertex[iEdge][2] = (float)iZ + ((float)a2iVertexOffset[ a2iEdgeConnection[iEdge][0] ][2] +  fOffset * (float)a2iEdgeDirection[iEdge][2]) * (float)iScale;

                        vertexIds[iEdge] = mesh->AddMarchingVertex(Vec3F(asEdgeVertex[iEdge][0], asEdgeVertex[iEdge][1], asEdgeVertex[iEdge][2]), vol->getHashKey(iX, iY, iZ, iEdge, iScale));
                }
        }


        //Draw the triangles that were found.  There can be up to five per cube
        for(int iTriangle = 0; iTriangle < 5; iTriangle++)
        {
                if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
                        break;
                int triangleVertices[3];
                for(int iCorner = 0; iCorner < 3; iCorner++)
                {
                    iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];
                    triangleVertices[iCorner] = vertexIds[iVertex];
                }

                mesh->AddMarchingFace(triangleVertices[0], triangleVertices[1], triangleVertices[2]);
        }
    }

    void DisplayBase::setSampleInterval(const int size) {
    }

    void DisplayBase::setSurfaceValue(const float value) {
    }

    void DisplayBase::setMaxSurfaceValue(const float value) {
    }

    void DisplayBase::unload() {
        RendererBase::unload();
        if(octree != NULL) {
            delete octree;
        }
        octree = NULL;
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }
        calculateDisplay();
        updateBoundingBox();
    }

    void DisplayBase::updateBoundingBox() {
        if(vol == NULL) {
            minPts = 0.0;
            maxPts = 1.0;
        } else {
            minPts = 0.0;

            maxPts[0] = vol->getSizeX()-1;
            maxPts[1] = vol->getSizeY()-1;
            maxPts[2] = vol->getSizeZ()-1;
        }
    }
    int smallest2ndPower(int value) {
        int power = 1;
        while (power < value) {
            power = power * 2;
        }
        return power;
    }

}

