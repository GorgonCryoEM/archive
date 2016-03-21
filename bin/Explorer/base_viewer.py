from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from libpytoolkit import Display
from libpytoolkit import *
from .libs import Vec3


class BaseViewer(QtOpenGL.QGLWidget):
    DisplayStyleWireframe, DisplayStyleFlat, DisplayStyleSmooth = range(3)
    
    def __init__(self, main):
        super(BaseViewer, self).__init__(main)
        
        self.app = main

        self.sceneIndex = -1;
        self.loaded = False
        self.selectEnabled = True
        self.mouseMoveEnabled = True
        self.isClosedMesh = True
#         self.displayStyle = self.DisplayStyleSmooth
        self.displayStyle = self.DisplayStyleWireframe
        
        self.rotation = self.identityMatrix()
        self.angle = 0.0
        self.axis = Vec3(1,1,1)
        
        self.connect(self, QtCore.SIGNAL("modelLoaded()"), self.modelChanged)

        self.glLists = []
        self.showBox = False
        self.twoWayLighting = False
        
        self.multipleSelection = True
        self.modelColor = QtGui.QColor(180, 180, 180, 150)
        
    def identityMatrix(self):
        return [[1.0, 0.0, 0.0, 0.0],
				[0.0, 1.0, 0.0, 0.0],
				[0.0, 0.0, 1.0, 0.0],
				[0.0, 0.0, 0.0, 1.0]
				]
    
    def setScale(self, x, y, z):
        self.setScaleNoEmit(x, y, z)
        self.app.mainCamera.updateGL()

    def setScaleNoEmit(self, x, y, z):
        self.renderer.setSpacing(x, y, z)
        
    def objectToWorldCoordinates(self, objectCoords):
        #Need to apply rotations
        origin = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]

        return Vec3([objectCoords[i] * scale[i] + origin[i] for i in range(3)])
    
    def worldToObjectCoordinates(self, worldCoords):
        origin = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        
        return Vec3([(worldCoords[i] - origin[i]) / scale[i] for i in range(3)])

    def objectVectorToWorldCoordinates(self, objectCoords):
        #Need to apply rotations
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        return Vec3([objectCoords[i] * scale[i] for i in range(3)])
    
    def worldVectorToObjectCoordinates(self, worldCoords):
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        return Vec3([worldCoords[i] / scale[i] for i in range(3)])
        
    def setDisplayStyle(self, style):
        self.displayStyle = style
        self.emitModelVisualizationChanged()

    def setMaterials(self, color):
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF())
        diffuseMaterial = [color.redF(), color.greenF(), color.blueF(), color.alphaF()]
        ambientMaterial = [color.redF()*0.2, color.greenF()*0.2, color.blueF()*0.2, color.alphaF()]
        specularMaterial = [1.0, 1.0, 1.0, 1.0]
        glMaterialfv(GL_BACK,  GL_AMBIENT,   ambientMaterial)
        glMaterialfv(GL_BACK,  GL_DIFFUSE,   diffuseMaterial)
        glMaterialfv(GL_BACK,  GL_SPECULAR,  specularMaterial)
        glMaterialf (GL_BACK,  GL_SHININESS, 0.1)
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ambientMaterial)
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuseMaterial)
        glMaterialfv(GL_FRONT, GL_SPECULAR,  specularMaterial)
        glMaterialf (GL_FRONT, GL_SHININESS, 0.1)

    def processMouseWheel(self, amount, event):
        #Override this method to enable mouse wheel functionality
        pass
                                
    def getBoundingBox(self):
        scale    = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        minPos = Vec3([(self.renderer.getMin(i)*scale[i] + location[i]) for i in range(3)])
        maxPos = Vec3([(self.renderer.getMax(i)*scale[i] + location[i]) for i in range(3)])
        return (minPos, maxPos)
        
    def getCenterAndDistance(self):
        scale    = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        loc = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        min = Vec3([(self.renderer.getMin(i)*scale[i] + loc[i]) for i in range(3)])
        max = Vec3([(self.renderer.getMax(i)*scale[i] + loc[i]) for i in range(3)])
        dist = (min - max).length()

        center = (min + max)*0.5

        return (center, dist)

    def initializeGLDisplayType(self):
        glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT)
        if(self.isClosedMesh):
            glEnable(GL_CULL_FACE)
        else:
            glDisable(GL_CULL_FACE)
            
        if(self.twoWayLighting):
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        else:
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
            
        #glDisable(GL_CULL_FACE)
        glEnable(GL_LIGHTING)
        
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if self.displayStyle == self.DisplayStyleWireframe:
            glPolygonMode(GL_FRONT, GL_LINE)
            glPolygonMode(GL_BACK, GL_LINE)
            
        elif self.displayStyle == self.DisplayStyleFlat:
            glPolygonMode(GL_FRONT, GL_FILL)
            glPolygonMode(GL_BACK, GL_FILL)
            glShadeModel(GL_FLAT)
            
        elif self.displayStyle == self.DisplayStyleSmooth:
            glPolygonMode(GL_FRONT, GL_FILL)
            glPolygonMode(GL_BACK, GL_FILL)
            glShadeModel(GL_SMOOTH)
            
        else:
            self.displayStyle = self.DisplayStyleSmooth;
            self.setDisplayType()
    
    def unInitializeGLDisplayType(self):
        glPopAttrib()

    def paintGL(self):
        glPushMatrix()
        ax = self.axis
        glRotatef(self.angle, ax[0], ax[1], ax[2])
        loc = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        glTranslated(loc[0], loc[1], loc[2])
#         glMultMatrixf(self.rotation)
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        glScaled(scale[0], scale[1], scale[2])
        
        glLineWidth(1.)
                
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT)
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        
        if(self.loaded and self.showBox):
            self.setMaterials(self.getBoundingBoxColor())
            self.renderer.drawBoundingBox()
        
        self.emitDrawingModel()
        
        for i in range(len(self.glLists)):
            if(self.loaded):
                self.setMaterials(self.modelColor)
                self.initializeGLDisplayType()
                glCallList(self.glLists[i])
                self.unInitializeGLDisplayType();

        glPopAttrib()
        glPopMatrix()

    def load(self, fileName):
        try:
            self.renderer.loadFile(str(fileName))
            print self.renderer.getSize()
            self.setScaleNoEmit(self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ())
            self.loaded = True
            self.dirty = False
            self.emitModelLoadedPreDraw()
            self.emitModelLoaded()
            self.emitViewerSetCenter()
        except:
            QtGui.QMessageBox.critical(self, "Unable to load data file", "The file might be corrupt, or the format may not be supported.", "Ok")

            self.loaded = False
        
    def modelChanged(self):
        for list in self.glLists:
            glDeleteLists(list,1)
        self.glLists = []
            
        glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT)
                         
        if(self.loaded):
            list = glGenLists(1)
            glNewList(list, GL_COMPILE)
            self.glLists.append(list)

            if(self.modelColor.alpha() < 255):
                glDepthFunc(GL_LESS)
                glColorMask(False, False, False, False)
                self.renderer.draw(0, False)
                glDepthFunc(GL_LEQUAL)
                glColorMask(True, True, True, True)
                self.renderer.draw(0, self.selectEnabled or self.mouseMoveEnabled)
            else:
                self.renderer.draw(0, self.selectEnabled or self.mouseMoveEnabled)
            glEndList()
                                    
        glPopAttrib()

    def setCenter(self, center):
        return False
    
    def emitModelLoadedPreDraw(self):
        self.emit(QtCore.SIGNAL("modelLoadedPreDraw()"))
        
    def emitModelLoaded(self):
        self.emit(QtCore.SIGNAL("modelLoaded()"))
        
    def emitModelChanged(self):
        self.emit(QtCore.SIGNAL("modelChanged()"))
        
    def emitDrawingModel(self):
        self.emit(QtCore.SIGNAL("modelDrawing()"))

    def emitViewerSetCenter(self):
        (center, distance) = self.getCenterAndDistance()
        self.emit(QtCore.SIGNAL("viewerSetCenter(float, float, float, float)"), center[0], center[1], center[2], distance)
