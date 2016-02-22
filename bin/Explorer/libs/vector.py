# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A utility module for vector manipulation 


from cmath import *   
from libpytoolkit import *
    
def vectorDistance(v1, v2):
    return abs(sqrt(pow(v1[0]-v2[0], 2) + pow(v1[1]-v2[1], 2) + pow(v1[2]-v2[2], 2)))
        
def vectorSize(v):        
    return Vector3Float(v).length()
        
def vectorNormalize(v):
	v = Vector3Float(v[0], v[1], v[2]).normalize()
	return [v[0], v[1], v[2]]
    
def vectorCrossProduct(v1, v2):
    return [v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]]

def vectorAdd(v1, v2):
	return Vector(v1) + Vector(v2)
    

def vectorSubtract(v1, v2):
    return Vector(v1) - Vector(v2)

def vectorScalarMultiply(s, v):
# 		return s*Vector(v)
	return Vector(v)*float(s)
