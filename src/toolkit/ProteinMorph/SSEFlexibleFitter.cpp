/*
 * SSEFlexibleFitter.cpp
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#include "SSEFlexibleFitter.h"

namespace Protein_Morph {


    SSEFlexibleFitter::SSEFlexibleFitter(Volume * vol) {
        this->volume = new Volume(vol->getSizeX(), vol->getSizeY(), vol->getSizeZ(), 0, 0, 0, vol);
        this->volume->normalize(0.0, 1.0);
        int sizex, sizey, sizez, ix;
        sizex = volume->getSizeX();
        sizey = volume->getSizeY();
        sizez = volume->getSizeZ();

        forceField = new Vec3F[sizex * sizey * sizez];
        for(int x = 0; x < sizex; x++) {
            for(int y = 0; y < sizey; y++) {
                for(int z = 0; z < sizez; z++) {
                    ix = volume->getIndex(x, y, z);
                    forceField[ix] = Vec3F(0, 0, 0);

                    if((x != 0) && (y != 0) && (z != 0) && (x != sizex-1) && (y != sizey-1) && (z != sizez-1)) {

                        for(int xx = -1; xx <= 1; xx++) {
                            for(int yy = -1; yy <= 1; yy++) {
                                for(int zz = -1; zz <= 1; zz++) {
                                    forceField[ix] += Vec3F::normalize(Vec3F(xx, yy, zz)) * (volume->getDataAt(x+xx, y+yy, z+zz) - volume->getDataAt(ix));
                                }
                            }
                        }
                    }

                }
            }
        }

    }

    SSEFlexibleFitter::~SSEFlexibleFitter() {
        delete [] forceField;
        delete volume;
    }

    Vec3F SSEFlexibleFitter::WorldPointToVolume(Vec3F pt) {
        return Vec3F(
            (pt.X() - volume->getOriginX()) / volume->getSpacingX(),
            (pt.Y() - volume->getOriginY()) / volume->getSpacingY(),
            (pt.Z() - volume->getOriginZ()) / volume->getSpacingZ());
    }

    Vec3F SSEFlexibleFitter::VolumePointToWorld(Vec3F pt) {
        return Vec3F (
            volume->getOriginX() + pt.X() * volume->getSpacingX(),
            volume->getOriginY() + pt.Y() * volume->getSpacingY(),
            volume->getOriginZ() + pt.Z() * volume->getSpacingZ());
    }

    Vec3F SSEFlexibleFitter::VolumeVectorToWorld(Vec3F pt) {
        return Vec3F (
            pt.X() * volume->getSpacingX(),
            pt.Y() * volume->getSpacingY(),
            pt.Z() * volume->getSpacingZ());
    }

    Vec3F SSEFlexibleFitter::GetForceOnPoint(Vec3F pt) {
        int baseX = (int)floorf(pt.X());
        int baseY = (int)floorf(pt.Y());
        int baseZ = (int)floorf(pt.Z());
        if((baseX >= 0) && (baseY >= 0) && (baseZ >= 0) && (baseX < volume->getSizeX() - 1) && (baseY < volume->getSizeY() - 1) && (baseZ < volume->getSizeZ() - 1)) {
            Vec3F dist = pt - Vec3F(baseX, baseY, baseZ);
            Vec3F a = forceField[volume->getIndex(baseX    , baseY    , baseZ    )];
            Vec3F b = forceField[volume->getIndex(baseX + 1, baseY    , baseZ    )];
            Vec3F c = forceField[volume->getIndex(baseX    , baseY + 1, baseZ    )];
            Vec3F d = forceField[volume->getIndex(baseX + 1, baseY + 1, baseZ    )];
            Vec3F e = forceField[volume->getIndex(baseX    , baseY    , baseZ + 1)];
            Vec3F f = forceField[volume->getIndex(baseX + 1, baseY    , baseZ + 1)];
            Vec3F g = forceField[volume->getIndex(baseX    , baseY + 1, baseZ + 1)];
            Vec3F h = forceField[volume->getIndex(baseX + 1, baseY + 1, baseZ + 1)];

            Vec3F f1 = a * (1.0-dist.X()) + b * dist.X();
            Vec3F f2 = c * (1.0-dist.X()) + d * dist.X();
            Vec3F f3 = e * (1.0-dist.X()) + f * dist.X();
            Vec3F f4 = g * (1.0-dist.X()) + h * dist.X();

            Vec3F f5 = f1 * (1.0-dist.Y()) + f2 * dist.Y();
            Vec3F f6 = f3 * (1.0-dist.Y()) + f4 * dist.Y();

            return f5 * (1.0-dist.Z()) + f6 * dist.Z();
        } else {
            return Vec3F(0,0,0);
        }

    }

    Vec3F SSEFlexibleFitter::GetForceOnHelix(Vec3F p, Vec3F q, double discretizationStepSize) {
        Vec3F force = Vec3F(0,0,0);
        for(double i = 0.0; i <= 1.0; i+= discretizationStepSize) {
            force = force + GetForceOnPoint(p * (1.0 - i) + q * i);
        }
        return force;
    }

    Vec3F SSEFlexibleFitter::GetForceOnSheet(vector<Vec3F> pts) {
        Vec3F force = Vec3F(0,0,0);
        for(unsigned int i = 0; i < pts.size(); i++) {
            force = force + GetForceOnPoint(pts[i]);
        }
        return force;
    }

    Vec3F SSEFlexibleFitter::GetTorqueOnPoint(Vec3F pt, Vec3F center) {
        return ((pt - center) ^ GetForceOnPoint(pt));
    }

    Vec3F SSEFlexibleFitter::GetTorqueOnHelix(Vec3F p, Vec3F q, double discretizationStepSize) {
        Vec3F torque = Vec3F(0,0,0);
        Vec3F center = (p + q) * 0.5;
        for(double i = 0.0; i <= 1.0; i+= discretizationStepSize) {
            torque = torque + GetTorqueOnPoint(p * (1.0 - i) + q * i, center);
        }
        return torque;
    }

    Vec3F SSEFlexibleFitter::GetTorqueOnSheet(vector<Vec3F> pts) {
        Vec3F torque = Vec3F(0,0,0);
        Vec3F center = Vec3F(0,0,0);

        for(unsigned int i = 0; i < pts.size(); i++) {
            center = center + pts[i] * (1.0f/(float)pts.size());
        }

        for(unsigned int i = 0; i < pts.size(); i++) {
            torque = torque + GetTorqueOnPoint(pts[i], center);
        }
        return torque;
    }


    void SSEFlexibleFitter::MoveHelix(Shape * helix, double translationStepSize, double rotationStepSize, double discretizationStepSize) {
        Vec3F p = WorldPointToVolume(helix->GetCornerCell3(1));
        Vec3F q = WorldPointToVolume(helix->GetCornerCell3(2));

        Vec3F torque = VolumeVectorToWorld(GetTorqueOnHelix(p, q, discretizationStepSize));
        if(!isZero(torque.length(), 0.0001)) {
            torque.normalize();
            helix->Rotate(Vec3D(torque.X(), torque.Y(), torque.Z()), rotationStepSize * 2 * PI);
        }

        Vec3F force = VolumeVectorToWorld(GetForceOnHelix(p, q, discretizationStepSize));
        if(!isZero(force.length(), 0.0001)) {
            force = Vec3F::normalize(force) * translationStepSize;
            helix->Translate(Vec3D(force.X(), force.Y(), force.Z()));
        }


    }


    void SSEFlexibleFitter::MoveSheet(int sheetId, NonManifoldMesh_SheetIds * sheetMesh, double translationStepSize, double rotationStepSize, double discretizationStepSize) {
        vector<Vec3F> sheetPoints, trianglePoints;
        for(unsigned int j = 0; j < sheetMesh->faces.size(); j++) {
            if(sheetMesh->faces[j].tag.id == sheetId) {
                trianglePoints = sheetMesh->SampleTriangle(j, discretizationStepSize);
                for(unsigned int k = 0; k < trianglePoints.size(); k++) {
                    sheetPoints.push_back(WorldPointToVolume(trianglePoints[k]));
                }
            }
        }
        trianglePoints.clear();

        Vec3F center = Vec3F(0,0,0);

        for(unsigned int i = 0; i < sheetPoints.size(); i++) {
            center = center + sheetPoints[i] * (1.0f/(float)sheetPoints.size());
        }

        Vec3F torque = VolumeVectorToWorld(GetTorqueOnSheet(sheetPoints));
        if(!isZero(torque.length(), 0.0001)) {
            torque.normalize();

            for(unsigned int i=0; i < sheetMesh->vertices.size(); i++) {
                sheetMesh->vertices[i].tag = false;
            }

            unsigned int vertexIx;
            for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
                if(sheetMesh->faces[i].tag.id == sheetId) {
                    for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
                        vertexIx = sheetMesh->GetVertexIndex(sheetMesh->faces[i].vertexIds[j]);
                        NonManifoldMeshVertex<bool> * v = &(sheetMesh->vertices[vertexIx]);
                        if(!v->tag) {
                            v->tag = true;
                            v->position = v->position - center;
                            v->position = v->position.Rotate(torque, rotationStepSize * 2 * PI);
                            v->position = v->position + center;
                        }
                    }
                }
            }
        }


        Vec3F force = VolumeVectorToWorld(GetForceOnSheet(sheetPoints));
        if(!isZero(force.length(), 0.0001)) {
            force = Vec3F::normalize(force) * translationStepSize;

            for(unsigned int i=0; i < sheetMesh->vertices.size(); i++) {
                sheetMesh->vertices[i].tag = false;
            }

            unsigned int vertexIx;
            for(unsigned int i = 0; i < sheetMesh->faces.size(); i++) {
                if(sheetMesh->faces[i].tag.id == sheetId) {
                    for(unsigned int j = 0; j < sheetMesh->faces[i].vertexIds.size(); j++) {
                        vertexIx = sheetMesh->GetVertexIndex(sheetMesh->faces[i].vertexIds[j]);
                        NonManifoldMeshVertex<bool> * v = &(sheetMesh->vertices[vertexIx]);
                        if(!v->tag) {
                            v->tag = true;
                            sheetMesh->TranslateVertex(vertexIx, force);
                        }
                    }
                }
            }
        }

        sheetPoints.clear();
    }

    void SSEFlexibleFitter::FitHelix(Shape * helix, double translationStepSize, double rotationStepSize, double discretizationStepSize, int maxStepCount) {
        double t;
        for(int i = 0; i < maxStepCount; i++) {
            t = (double)(maxStepCount - i) / (double)maxStepCount;
            MoveHelix(helix, translationStepSize * t, rotationStepSize * t, discretizationStepSize);
        }
    }

    void SSEFlexibleFitter::FitSheet(int sheetId, NonManifoldMesh_SheetIds * sheetMesh, double translationStepSize, double rotationStepSize, double discretizationStepSize, int maxStepCount) {
        double t;
        for(int i = 0; i < maxStepCount; i++) {
            t = (double)(maxStepCount - i) / (double)maxStepCount;
            MoveSheet(sheetId, sheetMesh, translationStepSize * t, rotationStepSize * t, discretizationStepSize);
        }
    }
}

} /* namespace Protein_Morph */
