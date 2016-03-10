from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from libpytoolkit import Display
from libpytoolkit import *
from .libs import Vec3


class BaseViewer(QtOpenGL.QGLWidget):
    DisplayStyleWireframe, DisplayStyleFlat, DisplayStyleSmooth = range(3)
    
    def __init__(self, main, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)
        self.app = main
        self.title = "Untitled"
        self.shortTitle = "UNT"
        self.fileName = "";
        self.sceneIndex = -1;
        self.loaded = False
        self.selectEnabled = True
        self.mouseMoveEnabled = True
        self.mouseMoveEnabledRay = True
        self.isClosedMesh = True
        self.viewerAutonomous = True
        self.displayStyle = self.DisplayStyleSmooth;
        self.modelVisible = True
        self.model2Visible = True
        self.model3Visible = True
        self.rotation = self.identityMatrix()
        self.connect(self, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.connect(self, QtCore.SIGNAL("modelLoaded()"), self.modelChanged)
        self.connect(self, QtCore.SIGNAL("modelUnloaded()"), self.modelChanged)

        self.glLists = []
        self.showBox = False
        self.twoWayLighting = False
        
        self.multipleSelection = True
        self.modelColor = QtGui.QColor(180, 180, 180, 150)
        
    def initVisualizationOptions(self, visualizationForm):
        self.visualizationOptions = visualizationForm
    
    def setModelColor(self, color):
        oldColor = self.getModelColor()
        self.setModelColor0(color)
        self.repaintCamera()

    def setModelColor0(self, color):
        self.modelColor = color

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
        
    def setLocation(self, x, y, z):
        self.setLocationNoEmit(x, y, z)
        self.app.mainCamera.updateGL()
        
    def setLocationNoEmit(self, x, y, z):
        self.renderer.setOrigin(x, y, z)
        self.visualizationOptions.ui.doubleSpinBoxLocationX.setValue(x)
        self.visualizationOptions.ui.doubleSpinBoxLocationY.setValue(y)
        self.visualizationOptions.ui.doubleSpinBoxLocationZ.setValue(z)
                        
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
        
    def repaintCamera(self):
        if(hasattr(self.app, "mainCamera")):
            self.app.mainCamera.updateGL()
        
    def setDisplayStyle(self, style):
        self.displayStyle = style
        self.emitModelVisualizationChanged()

    def getModelColor(self):
        return self.modelColor

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

    def getBoundingBox(self):
        scale    = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        minPos = Vec3([(self.renderer.getMin(i)*scale[i] + location[i]) for i in range(3)])
        maxPos = Vec3([(self.renderer.getMax(i)*scale[i] + location[i]) for i in range(3)])
        return (minPos, maxPos)
        
    def getCenterAndDistance(self):
        scale    = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        minPos = Vec3([(self.renderer.getMin(i)*scale[i] + location[i]) for i in range(3)])
        maxPos = Vec3([(self.renderer.getMax(i)*scale[i] + location[i]) for i in range(3)])
        distance = (minPos - maxPos).length()

        center = (minPos + maxPos)*0.5

        return (center, distance)

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

    def draw(self):
        glPushMatrix()
        location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        glTranslated(location[0], location[1], location[2])
        glMultMatrixf(self.rotation)
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        glScaled(scale[0], scale[1], scale[2])
                
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT)
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        
        if(self.loaded and self.showBox):
            self.setMaterials(self.getBoundingBoxColor())
            self.renderer.drawBoundingBox()
        
        self.emitDrawingModel()
        
        visibility = self.getDrawVisibility()
        colors     = self.getModelColor()
                
        for i in range(len(self.glLists)):
            if(self.loaded and visibility[i]):
                self.setMaterials(colors)
                self.initializeGLDisplayType()
                glCallList(self.glLists[i])
                self.unInitializeGLDisplayType();

        glPopAttrib()
        glPopMatrix()
        
    def load(self, fileName):
        self.setCursor(QtCore.Qt.WaitCursor)
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
        
        self.setCursor(QtCore.Qt.ArrowCursor)
        
    def getDrawVisibility(self):
        return [self.modelVisible, self.model2Visible, self.model3Visible]
        
    def modelChanged(self):
        for list in self.glLists:
            glDeleteLists(list,1)
        self.glLists = []
            
        visibility = self.getDrawVisibility()
        colors = self.getModelColor()
        
        glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT)
                         
        if(self.loaded):
            list = glGenLists(1)
            glNewList(list, GL_COMPILE)
            self.glLists.append(list)

            if(colors.alpha() < 255):
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

    def emitModelUnloaded(self):
        self.emit(QtCore.SIGNAL("modelUnloaded()"))
        
    def emitModelChanged(self):
        self.emit(QtCore.SIGNAL("modelChanged()"))
        
    def emitModelVisualizationChanged(self):
        self.emit(QtCore.SIGNAL("modelVisualizationChanged()"))
    
    def emitDrawingModel(self):
        self.emit(QtCore.SIGNAL("modelDrawing()"))

    def emitViewerSetCenterLocal(self):
        (center, distance) = self.getCenterAndDistance()
        self.emit(QtCore.SIGNAL("viewerSetCenterLocal(float, float, float, float)"), center[0], center[1], center[2], distance)
    
    def emitViewerSetCenter(self):
        (center, distance) = self.getCenterAndDistance()
        self.emit(QtCore.SIGNAL("viewerSetCenter(float, float, float, float)"), center[0], center[1], center[2], distance)
