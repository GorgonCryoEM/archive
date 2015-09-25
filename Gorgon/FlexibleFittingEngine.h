// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   An engine for flexible fitting of calpha atoms to volumes


#ifndef FLEXIBLEFITTINGENGINE_H
#define FLEXIBLEFITTINGENGINE_H

#include <set>
#include <map>
#include <queue>
#include <vector>
#include <utility>
#include <math.h>
#include <tnt/tnt.h>
#include <tnt/jama_lu.h>
#include <tnt/jama_qr.h>
#include <tnt/jama_cholesky.h>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <MathTools/Matrix.h>
#include <MathTools/Vector3D.h>
#include <MathTools/LinearSolver.h>
#include <ProteinMorph/SSECorrespondenceNode.h>
#include <ProteinMorph/SSECorrespondenceFeature.h>
#include <ProteinMorph/SSECorrespondenceFinder.h>

#if !defined(_WIN32) && !defined(_WIN64)
    #include <umfpack.h>
#endif

using namespace TNT;
using namespace JAMA;
using namespace wustl_mm::MathTools;

class FlexibleFittingEngine {
    public:
        // Constructors
        FlexibleFittingEngine();

        // Reset
        void resetCorrespondence();
        void resetDeformation();
        void resetDeformationVertices();
        void resetDeformationEdges();
        void resetDeformationHandles();
        void resetDeformationOptions();

        // Setters
        void addCorrespondenceSourceFeature(const Vector3DFloat&, const Vector3DFloat&);
        void addCorrespondenceTargetFeature(const Vector3DFloat&, const Vector3DFloat&);
        void addCorrespondenceManual(const int&, const int&, const int&, const bool&);
        void addDeformationOriginalVertex(const Vector3DFloat&);
        void addDeformationVertex(const Vector3DFloat&);
        void addDeformationEdge(const int&, const int&);
        void addDeformationHandle(const int&, const Vector3DFloat&);
        void addDeformationRigidInitializationTransformation(const MatrixFloat&);
        void setCorrespondenceOptions(const int&, const float&, const float&, const float&, const float&);
        void setCorrespondenceDirection(const int&, const int&, const int&, const bool&);
        void setDeformationRigidInitialization(const bool&);
        void setDeformationNeighborhoodSizes(const int&, const int&, const int&);
        void setDeformationWeights(const double&, const double&);
        void setDeformationVerticesToOriginalVertices();
        void setDeformationVerticesToDeformedVertices();
        void mergeCorrespondenceClusters(const int&, const int&, const int&);

        // Getters
        int getCorrespondenceAlignmentCount();
        int getCorrespondenceClusterCount(const int&);
        int getCorrespondenceFeatureCount(const int&, const int&);
        SSECorrespondenceNode getCorrespondence(const int&, const int&, const int&);
        MatrixFloat getCorrespondenceAlignmentTransformation(const int&);
        MatrixFloat getCorrespondenceClusterTransformation(const int&, const int&);
        MatrixFloat getCorrespondenceFeatureTransformation(const int&, const int&, const int&);
        vector<Vector3DFloat> getDeformedVertices();
        MatrixFloat getDeformationTransformation(const int &);

        // Methods
        void calculateCorrespondences();
        void calculateNeighborVertexIndicesCaches(const int&);
        void calculateDeformationTransformations(const int&);
        void deformLaplacian();

    private:
        // Helpers
        Array2D<double> transformationForm(const Vector3DFloat&, const int&, const bool&);
        Array2D<double> transformationForm(const vector<Vector3DFloat>&, const int&, const bool&);
        
        // Matrix
        Array2D<double> inverse(const Array2D<double>&);
        Array2D<double> transpose(const Array2D<double>&);
        Array2D<double> linearSolve(const Array2D<double>&, const Array2D<double>&);
        double* sparseLinearSolve(const int&, const int&, const vector<int>&, const vector<int>&, const vector<double>&, const vector<double>&);

        // Correspondence Fields
        int maxCorrespondenceSolutions;

        float helixLengthDifference,
              helixCentroidDifference,
              jointAngleDifference,
              dihedralAngleDifference;

        SSECorrespondenceFinder sseCorrespondenceFinder;
        
        vector<SSECorrespondenceFeature> sourceFeatures,
                                         targetFeatures;

        vector< vector< vector<SSECorrespondenceNode> > > correspondences;

        // Deformation Fields
        bool rigidInitialization;

        int laplacianNeighborhoodSize,
            transformationNeighborhoodSize,
            deformationNeighborhoodSize;
       
        double fittingWeight,
              distortionWeight;

        vector<int> handleIndices;
        
        vector<Vector3DFloat> originalVertices,
                              vertices,
                              targetVertices,
                              deformedVertices;

        vector<MatrixFloat> rigidInitializationTransformations,
                            deformationTransformations;

        vector< vector<int> > laplacianNeighborVertexIndicesCache,
                              transformationNeighborVertexIndicesCache,
                              deformationNeighborVertexIndicesCache;

        vector< pair<int, int> > edges;
        
        LinearSolver linearSolver;
};

FlexibleFittingEngine::FlexibleFittingEngine() {
    resetCorrespondence();
    resetDeformation();
}

void FlexibleFittingEngine::resetCorrespondence() {
    maxCorrespondenceSolutions = 1;

    jointAngleDifference    = 0.0;
    dihedralAngleDifference = 0.0;
    helixLengthDifference   = 0.0;
    helixCentroidDifference = 0.0;
    
    sourceFeatures.clear();
    targetFeatures.clear();

    correspondences.clear();
}

void FlexibleFittingEngine::resetDeformation() {
    resetDeformationVertices();
    resetDeformationEdges();
    resetDeformationHandles();
    resetDeformationOptions();
}

void FlexibleFittingEngine::resetDeformationVertices() {
    originalVertices.clear();
    vertices.clear();
    deformedVertices.clear();
    
    rigidInitializationTransformations.clear();
    deformationTransformations.clear();
}

void FlexibleFittingEngine::resetDeformationEdges() {
    edges.clear();
    
    laplacianNeighborVertexIndicesCache.clear();
    transformationNeighborVertexIndicesCache.clear();
    deformationNeighborVertexIndicesCache.clear();
}

void FlexibleFittingEngine::resetDeformationHandles() {
    handleIndices.clear();
    targetVertices.clear();
}

void FlexibleFittingEngine::resetDeformationOptions() {
    rigidInitialization = false;

    laplacianNeighborhoodSize      = 1;
    transformationNeighborhoodSize = 1;
    deformationNeighborhoodSize    = 1;

    fittingWeight    = 1.0;
    distortionWeight = 1.0;
}

void FlexibleFittingEngine::addCorrespondenceSourceFeature(const Vector3DFloat& startVertex, const Vector3DFloat& endVertex) {
    sourceFeatures.push_back(SSECorrespondenceFeature(startVertex, endVertex));
}

void FlexibleFittingEngine::addCorrespondenceTargetFeature(const Vector3DFloat& startVertex, const Vector3DFloat& endVertex) {
    targetFeatures.push_back(SSECorrespondenceFeature(startVertex, endVertex));
}

void FlexibleFittingEngine::addCorrespondenceManual(const int& alignmentIndex, const int& pIndex, const int& qIndex, const bool& isForward) {
    vector< vector<SSECorrespondenceNode> > newAlignment;
    for (int clusterIndex = 0; clusterIndex < getCorrespondenceClusterCount(alignmentIndex); clusterIndex++) {
        vector<SSECorrespondenceNode> newCluster;
        for (int featureIndex = 0; featureIndex < getCorrespondenceFeatureCount(alignmentIndex, clusterIndex); featureIndex++) {
            SSECorrespondenceNode correspondence = getCorrespondence(alignmentIndex, clusterIndex, featureIndex);
            if (correspondence.GetPIndex() != pIndex && correspondence.GetQIndex() != qIndex) {
                newCluster.push_back(correspondence);
            }
        }
        if (!newCluster.empty()) {
            newAlignment.push_back(newCluster);
        }
    }
    newAlignment.push_back(vector<SSECorrespondenceNode>(1, SSECorrespondenceNode(pIndex, qIndex, isForward))); 
    
    correspondences[alignmentIndex] = newAlignment;
}

void FlexibleFittingEngine::addDeformationOriginalVertex(const Vector3DFloat& vertex) {
    originalVertices.push_back(vertex);
}

void FlexibleFittingEngine::addDeformationVertex(const Vector3DFloat& vertex) {
    vertices.push_back(vertex);
}

void FlexibleFittingEngine::addDeformationEdge(const int& firstVertexIndex, const int& secondVertexIndex) {
    edges.push_back(make_pair(firstVertexIndex, secondVertexIndex));
}

void FlexibleFittingEngine::addDeformationHandle(const int& vertexIndex, const Vector3DFloat& targetVertex) {
    handleIndices.push_back(vertexIndex);
    targetVertices.push_back(targetVertex);
}

void FlexibleFittingEngine::addDeformationRigidInitializationTransformation(const MatrixFloat& rigidInitializationTransformation) {
    rigidInitializationTransformations.push_back(rigidInitializationTransformation);
}

void FlexibleFittingEngine::setCorrespondenceOptions(const int& newMaxCorrespondenceSolutions, const float& newHelixLengthDifference, const float& newHelixCentroidDifference, const float& newJointAngleDifference, const float& newDihedralAngleDifference) {
    maxCorrespondenceSolutions = newMaxCorrespondenceSolutions;
    
    helixLengthDifference      = newHelixLengthDifference;
    helixCentroidDifference    = newHelixCentroidDifference;

    jointAngleDifference       = (newJointAngleDifference    * PI) / 180.0;
    dihedralAngleDifference    = (newDihedralAngleDifference * PI) / 180.0;
}

void FlexibleFittingEngine::setCorrespondenceDirection(const int& alignmentIndex, const int& clusterIndex, const int& featureIndex, const bool& isForward) {
    correspondences[alignmentIndex][clusterIndex][featureIndex].SetForward(isForward);
}

void FlexibleFittingEngine::setDeformationRigidInitialization(const bool& newRigidInitialization) {
    rigidInitialization = newRigidInitialization;
}

void FlexibleFittingEngine::setDeformationNeighborhoodSizes(const int& newLaplacianNeighborhoodSize, const int& newTransformationNeighborhoodSize, const int& newDeformationNeighborhoodSize) {
    laplacianNeighborhoodSize      = newLaplacianNeighborhoodSize;
    transformationNeighborhoodSize = newTransformationNeighborhoodSize;
    deformationNeighborhoodSize    = newDeformationNeighborhoodSize;
}

void FlexibleFittingEngine::setDeformationWeights(const double& newFittingWeight, const double& newDistortionWeight) {
    fittingWeight    = newFittingWeight;
    distortionWeight = newDistortionWeight;
}

void FlexibleFittingEngine::setDeformationVerticesToOriginalVertices() {
    vertices = originalVertices;
}

void FlexibleFittingEngine::setDeformationVerticesToDeformedVertices() {
    vertices = deformedVertices;
}

void FlexibleFittingEngine::mergeCorrespondenceClusters(const int& alignmentIndex, const int& firstClusterIndex, const int& secondClusterIndex) {
    // Add Second Cluster Features to First Cluster
    correspondences[alignmentIndex][firstClusterIndex].insert(correspondences[alignmentIndex][firstClusterIndex].end(), correspondences[alignmentIndex][secondClusterIndex].begin(), correspondences[alignmentIndex][secondClusterIndex].end());

    // Delete Second Cluster
    correspondences[alignmentIndex].erase(correspondences[alignmentIndex].begin() + secondClusterIndex);
}

int FlexibleFittingEngine::getCorrespondenceAlignmentCount() {
    return correspondences.size();
}

int FlexibleFittingEngine::getCorrespondenceClusterCount(const int& alignmentIndex) {
    return correspondences[alignmentIndex].size();
}

int FlexibleFittingEngine::getCorrespondenceFeatureCount(const int& alignmentIndex, const int& clusterIndex) {
    return correspondences[alignmentIndex][clusterIndex].size();
}

SSECorrespondenceNode FlexibleFittingEngine::getCorrespondence(const int& alignmentIndex, const int& clusterIndex, const int& featureIndex) {
    return correspondences[alignmentIndex][clusterIndex][featureIndex];
}

MatrixFloat FlexibleFittingEngine::getCorrespondenceAlignmentTransformation(const int& alignmentIndex) {
    vector<SSECorrespondenceNode> alignmentCorrespondences;
    for (int clusterIndex = 0; clusterIndex < getCorrespondenceClusterCount(alignmentIndex); clusterIndex++) {
        for (int featureIndex = 0; featureIndex < getCorrespondenceFeatureCount(alignmentIndex, clusterIndex); featureIndex++) {
            alignmentCorrespondences.push_back(getCorrespondence(alignmentIndex, clusterIndex, featureIndex));
        }
    }

    MatrixFloat alignmentTransformation = sseCorrespondenceFinder.GetTransform(alignmentCorrespondences, 10);

    return alignmentTransformation;
}

MatrixFloat FlexibleFittingEngine::getCorrespondenceClusterTransformation(const int& alignmentIndex, const int& clusterIndex) {
    vector<SSECorrespondenceNode> clusterCorrespondences;
    if (getCorrespondenceFeatureCount(alignmentIndex, clusterIndex) == 1) {
        // Get Correspondence
        SSECorrespondenceNode correspondence = getCorrespondence(alignmentIndex, clusterIndex, 0);

        int sourceFeatureIndex = correspondence.GetPIndex(),
            targetFeatureIndex = correspondence.GetQIndex();
 
        // Get Source Feature Centroid
        Vector3DFloat sourceFeatureCentroid = sourceFeatures[sourceFeatureIndex].GetCentroid();

        // Calculate Distances to Other Source Features
		vector< boost::tuple<double, int, int> > closestSourceFeatures;
        for (int otherClusterIndex = 0; otherClusterIndex < getCorrespondenceClusterCount(alignmentIndex); otherClusterIndex++) {
            if (otherClusterIndex == clusterIndex) {
                continue;
            }

            for (int otherFeatureIndex = 0; otherFeatureIndex < getCorrespondenceFeatureCount(alignmentIndex, otherClusterIndex); otherFeatureIndex++) {
                // Get Other Correspondence
                SSECorrespondenceNode otherCorrespondence = getCorrespondence(alignmentIndex, otherClusterIndex, otherFeatureIndex);

                // Get Other Source Feature Index
                int otherSourceFeatureIndex = otherCorrespondence.GetPIndex();
                
                // Get Other Source Feature Centroid
                Vector3DFloat otherSourceFeatureCentroid = sourceFeatures[otherSourceFeatureIndex].GetCentroid();

                // Calculate Distance
                double distance = (otherSourceFeatureCentroid - sourceFeatureCentroid).Length();

                // Store Distance and Correspondence
				closestSourceFeatures.push_back(boost::tuple<double, int, int>(distance, otherClusterIndex, otherFeatureIndex));
            }
        }

        // Sort Distances 
        sort(closestSourceFeatures.begin(), closestSourceFeatures.end());

        // Build Temporary Cluster
        int clusterCorrespondencesSize = 3;
        for (int clusterCorrespondenceIndex = 0; clusterCorrespondenceIndex < clusterCorrespondencesSize; clusterCorrespondenceIndex++) {
            double distance       = closestSourceFeatures[clusterCorrespondenceIndex].get<0>();

            int otherClusterIndex = closestSourceFeatures[clusterCorrespondenceIndex].get<1>(),
                otherFeatureIndex = closestSourceFeatures[clusterCorrespondenceIndex].get<2>();

            clusterCorrespondences.push_back(getCorrespondence(alignmentIndex, otherClusterIndex, otherFeatureIndex));
        }
    }
    else {
        clusterCorrespondences = correspondences[alignmentIndex][clusterIndex];
    }

    MatrixFloat clusterTransformation = sseCorrespondenceFinder.GetTransform(clusterCorrespondences, 10);

    return clusterTransformation;
}

MatrixFloat FlexibleFittingEngine::getCorrespondenceFeatureTransformation(const int& alignmentIndex, const int& clusterIndex, const int& featureIndex) {
    // Get Correspondence
    SSECorrespondenceNode correspondence = getCorrespondence(alignmentIndex, clusterIndex, featureIndex);

    int sourceFeatureIndex = correspondence.GetPIndex(),
        targetFeatureIndex = correspondence.GetQIndex();
    
    bool flip = !correspondence.IsForward();

    // Cluster Transformation
    MatrixFloat clusterTransformation = getCorrespondenceClusterTransformation(alignmentIndex, clusterIndex);

    // Translation
    Vector3DFloat sourceCentroid    = sourceFeatures[sourceFeatureIndex].GetCentroid().Transform(clusterTransformation),
                  targetCentroid    = targetFeatures[targetFeatureIndex].GetCentroid(),
                  translationVector = targetCentroid - sourceCentroid;

    MatrixFloat translationTransformation = MatrixFloat::Identity(4);
    translationTransformation.SetValue(translationVector[0], 0, 3);
    translationTransformation.SetValue(translationVector[1], 1, 3);
    translationTransformation.SetValue(translationVector[2], 2, 3);

    // Rotation
    Vector3DFloat sourceStartPosition = sourceFeatures[sourceFeatureIndex].GetEndPoint(0).Transform(translationTransformation * clusterTransformation),
                  sourceEndPosition   = sourceFeatures[sourceFeatureIndex].GetEndPoint(1).Transform(translationTransformation * clusterTransformation),
                  targetStartPosition = targetFeatures[targetFeatureIndex].GetEndPoint(0),
                  targetEndPosition   = targetFeatures[targetFeatureIndex].GetEndPoint(1),
                  sourceVector        = sourceEndPosition - sourceStartPosition,
                  targetVector        = targetEndPosition - targetStartPosition;

    sourceVector.Normalize();
    targetVector.Normalize();
    
    Vector3DFloat normal = sourceVector ^ targetVector;
    normal.Normalize();

    double angle = acos(sourceVector * targetVector);
    if (angle > (PI / 2)) {
        angle -= PI;
    }
    if (flip) {
        angle *= -1;
    }

    Vector3DFloat row0(1.0, 0.0, 0.0),
                  row1(0.0, 1.0, 0.0),
                  row2(0.0, 0.0, 1.0);

    row0.Rotate(normal, angle);
    row1.Rotate(normal, angle);
    row2.Rotate(normal, angle);

    MatrixFloat rotationTransformation = MatrixFloat::Identity(4);
    rotationTransformation.SetValue(row0[0], 0, 0); rotationTransformation.SetValue(row0[1], 0, 1); rotationTransformation.SetValue(row0[2], 0, 2);
    rotationTransformation.SetValue(row1[0], 1, 0); rotationTransformation.SetValue(row1[1], 1, 1); rotationTransformation.SetValue(row1[2], 1, 2);
    rotationTransformation.SetValue(row2[0], 2, 0); rotationTransformation.SetValue(row2[1], 2, 1); rotationTransformation.SetValue(row2[2], 2, 2);

    // Rotation Origin Offset
    sourceCentroid = sourceFeatures[sourceFeatureIndex].GetCentroid().Transform(translationTransformation * clusterTransformation);
    
    MatrixFloat translationToOriginTransformation = MatrixFloat::Identity(4);
    translationToOriginTransformation.SetValue(-sourceCentroid[0], 0, 3);
    translationToOriginTransformation.SetValue(-sourceCentroid[1], 1, 3);
    translationToOriginTransformation.SetValue(-sourceCentroid[2], 2, 3);

    MatrixFloat translationFromOriginTransformation = MatrixFloat::Identity(4);
    translationFromOriginTransformation.SetValue(sourceCentroid[0], 0, 3);
    translationFromOriginTransformation.SetValue(sourceCentroid[1], 1, 3);
    translationFromOriginTransformation.SetValue(sourceCentroid[2], 2, 3);

    return translationFromOriginTransformation * rotationTransformation * translationToOriginTransformation * translationTransformation * clusterTransformation;
}

vector<Vector3DFloat> FlexibleFittingEngine::getDeformedVertices() {
    return deformedVertices;
}

MatrixFloat FlexibleFittingEngine::getDeformationTransformation(const int& vertexIndex) {
    return deformationTransformations[vertexIndex];
}

void FlexibleFittingEngine::deformLaplacian() {
    // Initialization
    int numVertices = vertices.size(),
        numHandles  = handleIndices.size(),
        dimensions  = 3; 

    #if defined(_WIN32) || defined(_WIN64)
        Array2D<double> AtA(numVertices * dimensions, numVertices * dimensions, 0.0),
                        AtB(numVertices * dimensions, 1, 0.0);
    #else
        vector<int>    AtARowIndices,
                       AtAColumnIndices;
        vector<double> AtAValues,
                       AtB(numVertices * dimensions, 0.0);
    #endif

    //cout << "Done initializing matrices!" << endl;
   
    // Fitting Terms
    for (int handleIndex = 0; handleIndex < numHandles; handleIndex++) {
        int vertexIndex = handleIndices[handleIndex];
        Vector3DFloat targetVertex = targetVertices[handleIndex];

        for (int dimension = 0; dimension < dimensions; dimension++) {
            #if defined(_WIN32) || defined(_WIN64)
                AtA[vertexIndex + (dimension * numVertices)][vertexIndex + (dimension * numVertices)] += fittingWeight * fittingWeight;
                AtB[vertexIndex + (dimension * numVertices)][0] += fittingWeight * fittingWeight * targetVertex[dimension];
            #else
                AtARowIndices.push_back(vertexIndex + (dimension * numVertices));
                AtAColumnIndices.push_back(vertexIndex + (dimension * numVertices));
                AtAValues.push_back(fittingWeight * fittingWeight);
                AtB[vertexIndex + (dimension * numVertices)] += fittingWeight * fittingWeight * targetVertex[dimension];
            #endif
        };
    }
    
    //cout << "Done adding fitting terms!" << endl;
    
    // Distortion Terms
    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
        // Lookup Rigid Initialization Transformation
        MatrixFloat rigidInitializationTransformation(4, 4);
        if (rigidInitialization) {
            rigidInitializationTransformation = rigidInitializationTransformations[vertexIndex];
        }

        // Lookup Vertex
        Vector3DFloat vertex = vertices[vertexIndex]; 
        if (rigidInitialization) {
            vertex = vertex.Transform(rigidInitializationTransformation);
        }

        // Calculate Laplacian
        vector<int> laplacianNeighborVertexIndices = laplacianNeighborVertexIndicesCache[vertexIndex];
        int laplacianNumNeighbors = laplacianNeighborVertexIndices.size();
        if (laplacianNumNeighbors == 0) {
            cerr << "Unable to calculate laplacian, vertex with 0 neighbors!" << endl;
            exit(-1);
        }

        Vector3DFloat neighborSum(0.0, 0.0, 0.0);
        for (vector<int>::iterator neighborVertexIndexIterator = laplacianNeighborVertexIndices.begin(); neighborVertexIndexIterator != laplacianNeighborVertexIndices.end(); neighborVertexIndexIterator++) {
            Vector3DFloat neighborVertex = vertices[*neighborVertexIndexIterator];
            if (rigidInitialization) {
                neighborVertex = neighborVertex.Transform(rigidInitializationTransformation);
            }
            neighborSum += neighborVertex;
        }
        Vector3DFloat laplacian = vertex - (neighborSum * (1.0 / laplacianNumNeighbors));
  
        // Transformation Compensation
        vector<int> transformationNeighborVertexIndices = transformationNeighborVertexIndicesCache[vertexIndex];
        vector<Vector3DFloat> transformationNeighborhoodVertices(1, vertex);
        for (vector<int>::iterator neighborVertexIndexIterator = transformationNeighborVertexIndices.begin(); neighborVertexIndexIterator != transformationNeighborVertexIndices.end(); neighborVertexIndexIterator++) {
            Vector3DFloat neighborVertex = vertices[*neighborVertexIndexIterator];
            if (rigidInitialization) {
                neighborVertex = neighborVertex.Transform(rigidInitializationTransformation);
            }
            transformationNeighborhoodVertices.push_back(neighborVertex);
        }
        
        Array2D<double> C  = transformationForm(transformationNeighborhoodVertices, dimensions, true),
                        D  = transformationForm(laplacian, dimensions, false),
                        Ct = transpose(C),
                        T  = matmult(matmult(D, inverse(matmult(Ct, C))), Ct);

        // Construct Row
        for (int dimensionOuter = 0; dimensionOuter < dimensions; dimensionOuter++) {
            map<int, double> a;
            
            a[vertexIndex + (dimensionOuter * numVertices)] += distortionWeight;
            for (int dimensionInner = 0; dimensionInner < dimensions; dimensionInner++) {
                a[vertexIndex + (dimensionInner * numVertices)] -= distortionWeight * T[dimensionOuter][dimensionInner];
            }
           
            for (vector<int>::iterator neighborVertexIndexIterator = laplacianNeighborVertexIndices.begin(); neighborVertexIndexIterator != laplacianNeighborVertexIndices.end(); neighborVertexIndexIterator++) {
                int neighborVertexIndex = *neighborVertexIndexIterator;
                
                a[neighborVertexIndex + (dimensionOuter * numVertices)] -= distortionWeight / laplacianNumNeighbors;
            }

            int neighborIndex = 1;
            for (vector<int>::iterator neighborVertexIndexIterator = transformationNeighborVertexIndices.begin(); neighborVertexIndexIterator != transformationNeighborVertexIndices.end(); neighborVertexIndexIterator++) {
                int neighborVertexIndex = *neighborVertexIndexIterator;

                for (int dimensionInner = 0; dimensionInner < dimensions; dimensionInner++) {
                    a[neighborVertexIndex + (dimensionInner * numVertices)] -= distortionWeight * T[dimensionOuter][(neighborIndex * dimensions) + dimensionInner];
                }
                
                neighborIndex++;
            }

            for (map<int, double>::iterator atIterator = a.begin(); atIterator != a.end(); atIterator++) {
                for (map<int, double>::iterator aIterator = a.begin(); aIterator != a.end(); aIterator++) {
                    int atIndex = (*atIterator).first,
                        aIndex  = (*aIterator).first;
            
                    double atValue = (*atIterator).second,
                           aValue  = (*aIterator).second;
           
                    #if defined(_WIN32) || defined(_WIN64)
                        AtA[atIndex][aIndex] += atValue * aValue;
                    #else
                        AtARowIndices.push_back(atIndex);
                        AtAColumnIndices.push_back(aIndex);
                        AtAValues.push_back(atValue * aValue);
                    #endif
                }
            }
        }
    }

    //cout << "Done adding distortion terms!" << endl;

    // Solution
    #if defined(_WIN32) || defined(_WIN64)
        Array2D<double> X = linearSolve(AtA, AtB);
    #else
        double* X = sparseLinearSolve(numVertices, dimensions, AtARowIndices, AtAColumnIndices, AtAValues, AtB);
    #endif

    //cout << "Done solving linear system!" << endl;
    
    // Convert to Vector
    deformedVertices.clear();
    deformedVertices.reserve(numVertices);
    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
        #if defined(_WIN32) || defined(_WIN64)
            deformedVertices.push_back(Vector3DFloat(
                X[(vertexIndex + (0 * numVertices))][0], 
                X[(vertexIndex + (1 * numVertices))][0], 
                X[(vertexIndex + (2 * numVertices))][0]
            ));
        #else
            deformedVertices.push_back(Vector3DFloat(
                X[(vertexIndex + (0 * numVertices))], 
                X[(vertexIndex + (1 * numVertices))], 
                X[(vertexIndex + (2 * numVertices))]
            ));
        #endif
    }

    //cout << "Done converting deformed vertices!" << endl;
    
    // Cleanup
    #if !defined(_WIN32) && !defined(_WIN64)
        delete[] X;
    #endif
}

void FlexibleFittingEngine::calculateCorrespondences() {
    sseCorrespondenceFinder.InitializeFeatures(sourceFeatures, targetFeatures); 
    sseCorrespondenceFinder.InitializeConstants(0, helixLengthDifference, 0, 0, 0, 0, 0, jointAngleDifference, dihedralAngleDifference, helixCentroidDifference, maxCorrespondenceSolutions, 10);
    
    correspondences = sseCorrespondenceFinder.GetAStarTriangleBasedCliqueDistanceFeatureCorrespondence(false, true, 4);
}

void FlexibleFittingEngine::calculateNeighborVertexIndicesCaches(const int& numVertices) {
    // Initialize Immediate Neighbor Cache
    vector< vector<int> > neighborVertexIndicesCache(numVertices, vector<int>());

    // Calculate Immediate Neighbor Cache
    for (vector< pair<int, int> >::iterator edgeIterator = edges.begin(); edgeIterator != edges.end(); edgeIterator++) {
        pair<int, int> edge = *edgeIterator;
    
        neighborVertexIndicesCache[edge.first].push_back(edge.second);
        neighborVertexIndicesCache[edge.second].push_back(edge.first);
    }

    // Calculate Extended Neighbor Caches
    laplacianNeighborVertexIndicesCache      = vector< vector<int> >(numVertices, vector<int>());
    transformationNeighborVertexIndicesCache = vector< vector<int> >(numVertices, vector<int>());
    deformationNeighborVertexIndicesCache    = vector< vector<int> >(numVertices, vector<int>());
    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
        queue< pair<int, int> > frontier;
        set<int> explored;

        frontier.push(make_pair(vertexIndex, 0));
        explored.insert(vertexIndex);
        while (!frontier.empty()) {
            pair<int, int> exploration = frontier.front();

            int explorationVertexIndex = exploration.first,
                explorationDistance    = exploration.second;

            if (explorationVertexIndex != vertexIndex) {
                if (explorationDistance <= laplacianNeighborhoodSize) {
                    laplacianNeighborVertexIndicesCache[vertexIndex].push_back(explorationVertexIndex);
                }

                if (explorationDistance <= transformationNeighborhoodSize) {
                    transformationNeighborVertexIndicesCache[vertexIndex].push_back(explorationVertexIndex);
                }

                if (explorationDistance <= deformationNeighborhoodSize) {
                    deformationNeighborVertexIndicesCache[vertexIndex].push_back(explorationVertexIndex);
                }

                if (explorationDistance > laplacianNeighborhoodSize && explorationDistance > transformationNeighborhoodSize && explorationDistance > deformationNeighborhoodSize) {
                    break;
                }
            }
            frontier.pop();

            vector<int> neighborVertexIndices = neighborVertexIndicesCache[explorationVertexIndex];
            for (vector<int>::iterator neighborVertexIndexIterator = neighborVertexIndices.begin(); neighborVertexIndexIterator != neighborVertexIndices.end(); neighborVertexIndexIterator++) {
                int neighborVertexIndex = *neighborVertexIndexIterator;

                if (explored.count(neighborVertexIndex) == 0) {
                    frontier.push(make_pair(neighborVertexIndex, explorationDistance + 1));
                    explored.insert(neighborVertexIndex);
                }
            }
        }
        
        sort(laplacianNeighborVertexIndicesCache[vertexIndex].begin(), laplacianNeighborVertexIndicesCache[vertexIndex].end());
        sort(transformationNeighborVertexIndicesCache[vertexIndex].begin(), transformationNeighborVertexIndicesCache[vertexIndex].end());
        sort(deformationNeighborVertexIndicesCache[vertexIndex].begin(), deformationNeighborVertexIndicesCache[vertexIndex].end());
    }

    //cout << "Done calculating neighbor vertex indices caches!" << endl;
}

void FlexibleFittingEngine::calculateDeformationTransformations(const int& numVertices) {
    deformationTransformations.reserve(numVertices);
    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
        vector<Vector3DFloat> sourceNeighborhood,
                              targetNeighborhood;

        sourceNeighborhood.push_back(originalVertices[vertexIndex]); 
        targetNeighborhood.push_back(deformedVertices[vertexIndex]); 

        vector<int> neighborVertexIndices = deformationNeighborVertexIndicesCache[vertexIndex];
        for (vector<int>::iterator neighborVertexIndexIterator = neighborVertexIndices.begin(); neighborVertexIndexIterator != neighborVertexIndices.end(); neighborVertexIndexIterator++) {
            sourceNeighborhood.push_back(originalVertices[*neighborVertexIndexIterator]); 
            targetNeighborhood.push_back(deformedVertices[*neighborVertexIndexIterator]); 
        }

        deformationTransformations.push_back(linearSolver.FindRotationTranslation(sourceNeighborhood, targetNeighborhood));
    }

    //cout << "Done calculating deformation transformations!" << endl;
}

Array2D<double> FlexibleFittingEngine::transformationForm(const Vector3DFloat& vertex, const int& dimensions, const bool& oneFlag) {
    vector<Vector3DFloat> vertices;
    vertices.push_back(vertex);

    return transformationForm(vertices, dimensions, oneFlag);
}

Array2D<double> FlexibleFittingEngine::transformationForm(const vector<Vector3DFloat>& workVertices, const int& dimensions, const bool& oneFlag) {
    int numVertices = workVertices.size();
    
    Array2D<double> form(numVertices * dimensions, (2 * dimensions) + 1, 0.0);
    for (int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
        // X  0  Z -Y  1  0  0
        // Y -Z  0  X  0  1  0
        // Z  Y -X  0  0  0  1

        Vector3DFloat vertex = workVertices[vertexIndex];

        form[vertexIndex * dimensions + 0][0] =  vertex.X();
        form[vertexIndex * dimensions + 0][2] =  vertex.Z();
        form[vertexIndex * dimensions + 0][3] = -vertex.Y();
       
        form[vertexIndex * dimensions + 1][0] =  vertex.Y();
        form[vertexIndex * dimensions + 1][1] = -vertex.Z();
        form[vertexIndex * dimensions + 1][3] =  vertex.X();
        
        form[vertexIndex * dimensions + 2][0] =  vertex.Z();
        form[vertexIndex * dimensions + 2][1] =  vertex.Y();
        form[vertexIndex * dimensions + 2][2] = -vertex.X();

        if (oneFlag) {
            form[vertexIndex * dimensions + 0][4] = 1.0;
            form[vertexIndex * dimensions + 1][5] = 1.0;
            form[vertexIndex * dimensions + 2][6] = 1.0;
        }
    }

    return form;
}

Array2D<double> FlexibleFittingEngine::inverse(const Array2D<double>& array) {
    if (array.dim1() != array.dim2()) {
        cerr << "Unable to invert matrix, not square!" << endl;
        exit(-1);
    }

    Array2D<double> identity(array.dim1(), array.dim1(), 0.0);
    for (int i = 0; i < array.dim1(); i++) {
        identity[i][i] = 1.0;
    }

    return linearSolve(array, identity);
}

Array2D<double> FlexibleFittingEngine::transpose(const Array2D<double>& array) {
    Array2D<double> arrayTranspose(array.dim2(), array.dim1());

    for (int row = 0; row < array.dim1(); row++) {
        for (int col = 0; col < array.dim2(); col++) {
            arrayTranspose[col][row] = array[row][col];
        }
    }

    return arrayTranspose;
}

Array2D<double> FlexibleFittingEngine::linearSolve(const Array2D<double>& A, const Array2D<double>& B) {
    Array2D<double> solution;
    
    LU<double> lu(A);
    if (lu.isNonsingular()) {
        solution = lu.solve(B);
    }
    else {
        cerr << "Unable to solve linear system of equations using LU!" << endl;
        exit(-1);
    }

    //QR<double> qr(A);
    //if (qr.isFullRank()) {
    //    solution = qr.solve(B);
    //}
    //else {
    //    cerr << "Unable to solve linear system of equations using QR!" << endl;
    //    exit(-1);
    //}
            
    //Cholesky<double> ch(A);
    //if (ch.is_spd()) {
    //    solution = ch.solve(B);
    //}
    //else {
    //    cerr << "Unable to solve linear system of equations using Cholesky!" << endl;
    //    exit(-1);
    //}

    return solution;
}

#if !defined(_WIN32) && !defined(_WIN64)
    double* FlexibleFittingEngine::sparseLinearSolve(const int& numVertices, const int& dimensions, const vector<int>& ATripletRowIndices, const vector<int>& ATripletColumnIndices, const vector<double>& ATripletValues, const vector<double>& B) {
        // Convert to UMFPACK Format
        int *AColumnCount      = new int [(numVertices * dimensions) + 1],
            *AColumnRowIndices = new int [ATripletValues.size()];

        double *AColumnValues = new double [ATripletValues.size()];

        umfpack_di_triplet_to_col(numVertices * dimensions, numVertices * dimensions, ATripletValues.size(), &ATripletRowIndices[0], &ATripletColumnIndices[0], &ATripletValues[0], AColumnCount, AColumnRowIndices, AColumnValues, NULL);

        // Solve
        void *symbolic,
             *numeric;
        
        double *X = new double [numVertices * dimensions];

        umfpack_di_symbolic(numVertices * dimensions, numVertices * dimensions, AColumnCount, AColumnRowIndices, AColumnValues, &symbolic, NULL, NULL);
        umfpack_di_numeric(AColumnCount, AColumnRowIndices, AColumnValues, symbolic, &numeric, NULL, NULL);
        umfpack_di_solve(UMFPACK_A, AColumnCount, AColumnRowIndices, AColumnValues, X, &B[0], numeric, NULL, NULL);

        // Cleanup
        delete [] AColumnCount;
        delete [] AColumnRowIndices;
        delete [] AColumnValues;
        umfpack_di_free_symbolic(&symbolic);
        umfpack_di_free_numeric(&numeric);

        return X;
    }
#endif

#endif
