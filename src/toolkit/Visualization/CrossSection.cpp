/*
 * CrossSection.cpp
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "CrossSection.h"

namespace Visualization {

    CrossSection::CrossSection() {
        // TODO Auto-generated constructor stub

    }

    CrossSection::~CrossSection() {
        // TODO Auto-generated destructor stub
    }

    bool CrossSection::calculateDisplay() {
        cuttingMesh->Clear();

        bool redraw = false;

        if(volData != NULL) {
            redraw = true;

            if((cuttingPlaneCenter.X() >= minPts[0]) && (cuttingPlaneCenter.X() <= maxPts[0]) &&
                (cuttingPlaneCenter.Y() >= minPts[1]) && (cuttingPlaneCenter.Y() <= maxPts[1]) &&
                (cuttingPlaneCenter.Z() >= minPts[2]) && (cuttingPlaneCenter.Z() <= maxPts[2])) {

                for(int i = 0; i < 2; i++) {
                    for(int j = 0; j < 2; j++) {
                        for(int k = 0; k < 2; k++) {
                            cuttingVolume(i, j, k) = (cuttingPlaneCenter - Vec3F(i * getSizeX(), j * getSizeY(), k * getSizeZ()))* cuttingPlaneDirection;
                        }
                    }
                }
                MarchingCube(cuttingVolume, *cuttingMesh, 0.0f, 0, 0, 0, 1);
            }
        }
        return redraw;
    }

    void CrossSection::load3DTexture() {
        if(textureLoaded) {
            glDeleteTextures(1, &textureName);
            textureLoaded = false;
        }

        if(volData != NULL) {
            textureSize[0] = smallest2ndPower(volData->getSizeX());
            textureSize[1] = smallest2ndPower(volData->getSizeY());
            textureSize[2] = smallest2ndPower(volData->getSizeZ());
            double maxVal = maxSurfaceValue;
            double minVal = surfaceValue;
            unsigned char val;

            // Approximations to avoid division by zero
            if(isZero(minVal - maxVal, 0.000000000001)) {
                maxVal = minVal + (Volume::getMax() - Volume::getMin()) / 1000.0;
            }
            if(isZero(minVal - maxVal, 0.000000000001)) {
                maxVal = minVal + 0.0001;
            }

            unsigned char * texels = new unsigned char[textureSize.X() * textureSize.Y() * textureSize.Z()];
            unsigned int pos = 0;
            for(int z = 0; z < textureSize.Z(); z++) {
                for(int y = 0; y < textureSize.Y(); y++) {
                    for(int x = 0; x < textureSize.X(); x++) {
                        if((x < getSizeX()) && (y < getSizeY()) && (z < getSizeZ())) {
                            val = (unsigned char)round((min(max((double)(*this)(x, y, z), minVal), maxVal) - minVal) * 255.0 / (maxVal - minVal));
                        } else {
                            val = 0;
                        }
                        texels[pos] = val;
                        pos++;
                    }
                }
            }
            glGenTextures(1, &textureName);
            glBindTexture(GL_TEXTURE_3D, textureName);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
            try {
                glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, textureSize.X(), textureSize.Y(), textureSize.Z(), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texels);
                textureLoaded = true;
            }   catch (int) {
                textureLoaded = false;
            }
            delete [] texels;

        }
    }

    void CrossSection::draw(int subSceneIndex, bool selectEnabled) {
        if(subSceneIndex == 0) {
            glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);
            glDisable(GL_CULL_FACE);
            Vec3F vertex;
            // The outside box

            glBegin(GL_LINES);
            for(unsigned int i = 0; i < cuttingMesh->edges.size(); i++) {
                if(cuttingMesh->edges[i].faceIds.size() == 1) {

                    for(unsigned int j = 0; j < 2; j++) {
                        vertex = cuttingMesh->vertices[cuttingMesh->GetVertexIndex(cuttingMesh->edges[i].vertexIds[j])].position;
                        glVertex3f(vertex.X() * (float)getSizeX(), vertex.Y() * (float)getSizeY(), vertex.Z() * (float)getSizeZ());
                    }
                }
            }
            glEnd();


            // The cutting surface
            glEnable(GL_TEXTURE_3D);
            //GLboolean resident;
            //glAreTexturesResident(1, &textureName, &resident);
            //if(resident) {
                glBindTexture(GL_TEXTURE_3D, textureName);

                double xRatio = (double)getSizeX() / (double)textureSize.X();
                double yRatio = (double)getSizeY() / (double)textureSize.Y();
                double zRatio = (double)getSizeZ() / (double)textureSize.Z();

                for(unsigned int i = 0; i < cuttingMesh->faces.size(); i++) {
                    glBegin(GL_POLYGON);
                    for(unsigned int j = 0; j < cuttingMesh->faces[i].vertexIds.size(); j++) {
                        vertex = cuttingMesh->vertices[cuttingMesh->GetVertexIndex(cuttingMesh->faces[i].vertexIds[j])].position;
                        glTexCoord3d(vertex.X() * xRatio, vertex.Y()* yRatio, vertex.Z() * zRatio);
                        glVertex3f(vertex.X() * (float)getSizeX(), vertex.Y() * (float)getSizeY(), vertex.Z() * (float)getSizeZ());
                    }
                    glEnd();
                }
            //}

            glPopAttrib();
        }
    }

} /* namespace Visualization */
