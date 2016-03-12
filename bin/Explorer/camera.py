from PyQt4 import QtOpenGL, QtCore, QtGui

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from libpytoolkit import *
from cmath import *
from .libs import Vec3


class Camera(QtOpenGL.QGLWidget):

    def __init__(self, scene, main):
        QtOpenGL.QGLWidget.__init__(self, main)
        
        self.app = main
        self.sceneID = -1

        self.near = 0
        self.cuttingPlane = 0.0
        self.scene = scene
        self.aspectRatio   = 1.0
        self.selectedScene = -1
        self.lightsEnabled = [True, False]
        self.lightsPosition = [Vec3(1000,1000,1000),
							   Vec3(-1000,-1000,-1000)
							   ]
        self.lightsUseEyePosition = [True, False]
        
        self.mouseMovePoint = QtCore.QPoint(0,0)
        self.mouseDownPoint = QtCore.QPoint(0,0)
        self.mouseLeftPressed  = False
        self.mouseMidPressed   = False
        self.mouseRightPressed = False
        
        self.fogDensity = 0.01
        self.fogEnabled = False
        
        self.center = Vec3(0.0,  0.0, 0.0)
        self.eye    = Vec3(0.0, -4.1, 0.0)
        self.look   = Vec3(0.0, 1.1, 0.0)
        self.right  = Vec3(1.1, 0.0, 0.0)
        self.up     = Vec3(0.0, 0.0, 1.1)
        self.near    = 0.11
        self.far     = 1000.01
        self.eyeZoom = 0.26
        
        self.setEyeRotation(0.0, 0.0, 0.0)
        self.lastPos = QtCore.QPoint()
        
        for i in range(len(self.scene)):
            self.scene[i].sceneIndex = i;

        for s in self.scene:
            self.connect(s, QtCore.SIGNAL("viewerSetCenter(float, float, float, float)"), self.sceneSetCenter)
    
    def setEye(self, v):
        if(self.eye != v):
            self.eye = v
            try:
                self.look  = (self.center - self.eye).normalize()
                self.right = (self.look^self.up).normalize()            #print("Eye: right :", self.right)
                self.up    = (self.right^self.look).normalize()
            except:
                self.look  = Vec3(0,1,0)
                self.right = Vec3(1,0,0)
                self.up    = Vec3(0,0,1)
    
    def setCenter(self, v):
        if(self.center != v):
            self.center = v
            try:
                self.look  = (self.center - self.eye).normalize()
                self.right = (self.look^self.up).normalize()
            except:
                self.look  = Vec3(0,1,0)
                self.right = Vec3(1,0,0)
        
    def setUp(self, v):
        if(self.up != v.normalize()):
            self.up = v.normalize()
            try:
                self.right = (self.look^self.up   ).normalize()
                self.up    = (self.right^self.look).normalize()
            except:
                self.right = Vec3(1,0,0)
            self.setRendererCuttingPlanes()
            self.emitCameraChanged()
        
    def setEyeRotation(self, yaw, pitch, roll):
        newLook = (self.eye + self.right*yaw - self.center).normalize()
        d = (self.eye - self.center).length()
        newEye = self.center + newLook*d
        
        newLook = (newEye+self.up*pitch - self.center).normalize()
        d = (newEye - self.center).length()
        newEye = self.center + newLook*d
        
        self.setEye(newEye)
        
        newUp = (self.right*roll*0.01 + self.up).normalize()
        self.setUp(newUp)
            
    def setNearFarZoom(self, near, far, zoom):
        if((self.eyeZoom != zoom) or (self.near != near) or (self.far != far)):
            self.eyeZoom = min(max(zoom, 0.0001), 0.9999);
            nearChanged = (self.near != near)
            self.near = max(min(near, far), 0.1)
            self.far = max(self.near + 1.0, far)
            glFogf(GL_FOG_START, self.near)
            glFogf(GL_FOG_END, self.far)
            self.setGlProjection()
            self.emitCameraChanged()
    
    def setRendererCuttingPlanes(self):
        for s in self.scene:
            if(s.renderer.setCuttingPlane(self.cuttingPlane, self.look[0], self.look[1], self.look[2])):
                s.emitModelChanged()

    def sceneSetCenter(self, cX, cY, cZ, d):
        sceneMin = [cX, cY, cZ]
        sceneMax = [cX, cY, cZ]
        for s in self.scene:
            if s.loaded:
                (minPos, maxPos) = s.getMinMax()
                for i in range(3):
                    if minPos[i] < sceneMin[i]:
                        sceneMin[i] = minPos[i]
                    if maxPos[i] > sceneMax[i]:
                        sceneMax[i] = maxPos[i]
        
        sceneMin = Vec3(sceneMin)
        sceneMax = Vec3(sceneMax)
        
        distance = (sceneMin - sceneMax).length()
        center   = (sceneMin + sceneMax)*0.5
        [centerX, centerY, centerZ] = [center.x(), center.y(), center.z()]
                     
        self.setCenter(center)
        self.setEye(Vec3(self.center[0], self.center[1], self.center[2] - distance))
        self.setUp(Vec3(0, -1, 0))
        centerDistance = (self.eye - self.center).length()
         
        self.updateGL()
    
    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)
       
    def initializeGL(self):
        if((sys.platform != 'darwin') and (sys.platform != 'win32')):
            glutInit(sys.argv)      #This must be here to get it to work with Freeglut.
            #otherwise you get: "freeglut  ERROR:  Function <glutWireCube> called without first calling 'glutInit'."
       
        bkgCol = QtGui.QColor(0, 0, 0, 255)
        glClearColor(bkgCol.redF(), bkgCol.greenF(), bkgCol.blueF(), bkgCol.alphaF())
        glClearDepth( 1.0 )
        
        self.setLights()
        self.setNearFarZoom(0.1, 1000, 0.25)

        if(self.fogEnabled):
            fogColor = QtGui.QColor(0, 0, 0, 255)
            glFogi(GL_FOG_MODE, GL_LINEAR)
            glFogfv(GL_FOG_COLOR, [fogColor.redF(), fogColor.greenF(), fogColor.blueF(), fogColor.alphaF()])
            glFogf(GL_FOG_DENSITY, self.fogDensity)
            glHint(GL_FOG_HINT, GL_DONT_CARE)
            glFogf(GL_FOG_START, self.near)
            glFogf(GL_FOG_END, self.far)
            glEnable(GL_FOG)
        else:
            glDisable(GL_FOG)

        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

    def setLights(self):
        glLight = [GL_LIGHT0, GL_LIGHT1]
        light0Col = QtGui.QColor(255, 255, 255, 255)
        light1Col = QtGui.QColor(255, 255, 255, 255)

        lightsCol = [[light0Col.redF(), light0Col.greenF(), light0Col.blueF(), 1.0],
                       [light1Col.redF(), light1Col.greenF(), light1Col.blueF(), 1.0]
                       ]
        for i in range(2):
            if(self.lightsEnabled[i]):
                afPropertiesAmbient = [lightsCol[i][0]*0.3, lightsCol[i][1]*0.3, lightsCol[i][2]*0.3, 1.00]
                afPropertiesDiffuse = lightsCol[i]
                afPropertiesSpecular = [lightsCol[i][0]*0.1, lightsCol[i][0]*0.1, lightsCol[i][0]*0.1, 1.00]
                if(self.lightsUseEyePosition[i]):
                    afLightPosition = [self.eye[0], self.eye[1], self.eye[2], 1.0]
                else:
                    afLightPosition = [self.lightsPosition[i][0], self.lightsPosition[i][1], self.lightsPosition[i][2], 1.0]
                glLightfv(glLight[i], GL_AMBIENT,  afPropertiesAmbient)
                glLightfv(glLight[i], GL_DIFFUSE,  afPropertiesDiffuse)
                glLightfv(glLight[i], GL_SPECULAR, afPropertiesSpecular)
                glLightfv(glLight[i], GL_POSITION, afLightPosition)
                glEnable(glLight[i])
            else:
                glDisable(glLight[i])
       
    def setGluLookAt(self):
        gluLookAt(self.eye[0], self.eye[1], self.eye[2],
                  self.center[0], self.center[1], self.center[2],
                  self.up[0], self.up[1], self.up[2])
        
    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glEnable(GL_DEPTH_TEST)
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        self.setGluLookAt()
        self.setLights()
        for i in range(len(self.scene)):
            glPushName(i)
            self.scene[i].draw()
            glPopName()
        glPopMatrix()
        
    def processMouseWheel(self, direction, event):
        for s in self.scene:
            try:
                s.processMouseWheel(direction, event)
            except:
                pass
     
    def resizeGL(self, width, height):
        if(height > 0):
            self.aspectRatio = width/(1.0*height)
            glViewport(0,0, width, height)
            self.setGlProjection()

    def setGlProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(180 * self.eyeZoom, self.aspectRatio, self.near, self.far)
        #glOrtho(-200 * self.eyeZoom, 200 * self.eyeZoom, -200 * self.eyeZoom, 200 * self.eyeZoom, self.near, self.far)
        glMatrixMode(GL_MODELVIEW)
    
    def mousePressEvent(self, e):
#         x = self.mouseMovePoint.x
#         y = self.mouseMovePoint.y
        
        x = e.x()
        y = e.y()
        
        self.hits = self.pickObject(x, y)
#         print [k for i,j,k in self.hits]
        print [s[2] for s in self.hits]
        
#         self.mouseMovePoint.x = x
#         self.mouseMovePoint.y = y
#         for s in self.scene:
#             s.mousePressEvent(e)

    def mouseReleaseEvent(self, event):
        self.mouseUpPoint = QtCore.QPoint(event.pos())
        #Enter selection mode only if we didnt move the mouse much.. (If the mouse was moved, then we assume a camera motion instead of a selection
        dx = self.mouseUpPoint.x() - self.mouseDownPoint.x()
        dy = self.mouseUpPoint.y() - self.mouseDownPoint.y()
             
        if (pow(self.mouseDownPoint.x() - self.mouseUpPoint.x(), 2) + pow(self.mouseDownPoint.y() - self.mouseUpPoint.y(), 2) <= 2):
            self.processMouseClick(self.pickObject(self.mouseUpPoint.x(), self.mouseUpPoint.y()), event, self.mouseLeftPressed, self.mouseMidPressed, self.mouseRightPressed)
        
        # auto rotate if ctrl + alt pressed
        if(self.mouseLeftPressed) and (event.modifiers() & QtCore.Qt.CTRL) and (event.modifiers() & QtCore.Qt.ALT):
            for i in range(100):
                self.setEyeRotation(-dx/10.0, dy/10.0, 0)
                self.updateGL()
#                 time.sleep(0.01)
            
    def mouseDoubleClickEvent(self, e):
        for s in self.scene:
            s.mouseDoubleClickEvent(e)
    
    def rotateSelectedScene(self, dx, dy):
        print "In: rotateSelectedScene"
        newDx = (self.eye - self.center).length() * abs(tan(pi * self.eyeZoom)) * dx / float(self.width())
        newDy = (self.eye - self.center).length() * abs(tan(pi * self.eyeZoom)) * dy / float(self.height())

        moveLength    = self.up*(-newDy) + self.right*newDx
        moveDirection = moveLength.normalize()
        rotationAxis  = moveDirection^self.look
        
        rotationAxis3D = Vec3(rotationAxis)
        centerOfMass   = Vec3(0,0,0)
        
        for s in self.scene:
            selectionCOM  = s.worldToObjectCoordinates(centerOfMass)
            selectionAxis = s.worldToObjectCoordinates(rotationAxis3D)
            s.selectionRotate(selectionCOM, selectionAxis, moveLength.length())
            s.emitModelChanged()
            
        self.updateGL()
                     
    def moveSelectedScene(self, dx, dy):
        newDx = (self.eye - self.center).length() * abs(tan(pi * self.eyeZoom)) * dx / float(self.width())
        newDy = (self.eye - self.center).length() * abs(tan(pi * self.eyeZoom)) * dy / float(self.height())
        moveDirection = self.up*(-newDy) + self.right*newDx
        dirVec = Vec3(moveDirection)
        
#         print self.hits[0][3]
        
        for s in self.scene:
#             s.selectionMove(dirVec)
#             s.emitModelChanged()
            try:
                s.selectionMove(dirVec)
                s.emitModelChanged()
            except:
                pass

    def pickObject(self, x, y):
        viewport = list(glGetIntegerv(GL_VIEWPORT))
        glSelectBuffer(10000)
        glRenderMode(GL_SELECT)

        glInitNames()
        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        gluPickMatrix(x, viewport[3]-y, 5, 5, viewport)
        gluPerspective(180 * self.eyeZoom, self.aspectRatio, self.near, self.far)
        for s in self.scene:
            s.draw()
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()
        glFlush()

        mouseHits = glRenderMode(GL_RENDER)
        return mouseHits

    def mouseMoveEvent(self, e):
        dx = e.x() - self.mouseMovePoint.x()
        dy = e.y() - self.mouseMovePoint.y()
                        
        if (e.buttons() & QtCore.Qt.LeftButton):
            if (e.buttons() & QtCore.Qt.RightButton):           # Rolling the scene
                self.setEyeRotation(0, 0, dx)
            else:
                if e.modifiers() & QtCore.Qt.CTRL:           # Rotating the selection
                    print "e.modifiers() & QtCore.Qt.CTRL"
#                     self.selectedScene = 3
                    self.rotateSelectedScene(dx, dy)
                else:                                               # Rotating the scene
                    self.setEyeRotation(-dx, dy, 0)
            
        elif (e.buttons() & QtCore.Qt.RightButton):
            print "e.modifiers() & QtCore.Qt.RightButton"
            if e.modifiers() & QtCore.Qt.CTRL:                 # Translating the selection
                print "   e.modifiers() & QtCore.Qt.CTRL"
                self.moveSelectedScene(dx, dy)
            else:                                                   # Translating the scene
                newDx = (self.eye - self.center).length() * abs(tan(pi * self.eyeZoom)) * dx / float(self.width())
                newDy = (self.eye - self.center).length() * abs(tan(pi * self.eyeZoom)) * dy / float(self.height())
                translation = self.up*newDy + self.right*(-newDx);
                newEye = self.eye + translation;
                newCenter = self.center + translation;
                self.setEye(newEye)
                self.setCenter(newCenter)
                
        self.mouseMovePoint = QtCore.QPoint(e.pos())

        self.updateGL()
    
    def wheelEvent(self, event):
        if(event.delta() != 0):
            direction = event.delta()/abs(event.delta())
            self.processMouseWheel(direction, event)
            if(event.modifiers() & QtCore.Qt.ALT):                 # Setting the cutting plane
                self.setCuttingPlane(self.cuttingPlane + direction * 0.01)
            elif (not (event.modifiers() & QtCore.Qt.ALT) and not (event.modifiers() & QtCore.Qt.CTRL)):     # Zoom in / out
                self.setNearFarZoom(self.near, self.far, self.eyeZoom + direction * 10.0/360.0)
            self.updateGL()
        
    def emitCameraChanged(self):
        self.emit(QtCore.SIGNAL("cameraChanged()"))
