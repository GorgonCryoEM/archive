from PyQt4 import QtOpenGL, QtCore, QtGui

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from libpytoolkit import *
from cmath import *
from .libs import Vec3
from line import Line
from dot import Dot


class Camera(QtOpenGL.QGLWidget):

    def __init__(self, shapes, main):
        super(Camera, self).__init__()
        
        self.shapes = shapes
        self.app = main
        self.shapeID = -1

        self.linecom = Line(self.app, Vec3(100,100,100))
        self.linecom.color = QtGui.QColor(40, 70, 50, 150)
        self.lineaxis = Line(self.app, Vec3(100,100,100))
        self.lineaxis.color = QtGui.QColor(80, 40, 50, 150)
        self.lineaxis.depthEnabled = True
        self.dotcenter = Dot(self.app)
        self.dotcom = Dot(self.app)
        self.dotcom.color = QtGui.QColor(0, 130, 0, 150)
        self.shapes.append(self.lineaxis)
        self.shapes.append(self.dotcenter)
        self.shapes.append(self.dotcom)
        
        self.near = 0
        self.cuttingPlane = 0.0
        
        self.mouseTrackingEnabled    = False
        self.mouseTrackingEnabledRay = False
        self.aspectRatio   = 1.0
        self.selectedShape = -1
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
        
        self.center = Vec3(0.0,  0.0, 0.0)
        self.eye    = Vec3(0.0, -4.1, 0.0)
        self.look   = Vec3(0.0, 1.1, 0.0)
        self.right  = Vec3(1.1, 0.0, 0.0)
        self.up     = Vec3(0.0, 0.0, 1.1)
        self.near    = 0.11
        self.far     = 1000.01
        self.eyeZoom = 0.26
        
        self.setEyeRotation(0.0, 0.0, 0.0)
        self.setCenter     (self.center)
        self.setEye        (self.eye)
        self.setUp         (self.up)
        self.lastPos = QtCore.QPoint()
        
        for i in range(len(self.shapes)):
            self.shapes[i].shapeIndex = i;

    def setEye(self, v):
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
        self.center = v
        try:
            self.look  = (self.center - self.eye).normalize()
            self.right = (self.look^self.up).normalize()
        except:
            self.look  = Vec3(0,1,0)
            self.right = Vec3(1,0,0)

    def setUp(self, v):
        self.up = v.normalize()
        try:
            self.right = (self.look^self.up   ).normalize()
            self.up    = (self.right^self.look).normalize()
        except:
            self.right = Vec3(1,0,0)
        
    def setEyeRotation(self, yaw, pitch, roll):
        newLook = (self.eye + self.up*pitch + self.right*yaw - self.center).normalize()
        d = (self.eye - self.center).length()
        newEye = self.center + newLook*d
        
        self.setEye(newEye)
        
        newUp = (self.right*roll*0.01 + self.up).normalize()
        self.setUp(newUp)
            
    def setNearFarZoom(self, near, far, zoom):
        self.eyeZoom = min(max(zoom, 0.0001), 0.9999);
        self.near = max(min(near, far), 0.1)
        self.far = max(self.near + 1.0, far)
        self.setGlProjection()
    
    def setCuttingPlane(self, cuttingPlane):
        self.cuttingPlane = min(max(cuttingPlane, -1.0), 1.0)
        
        for s in self.shapes:
            s.renderer.setCuttingPlane(self.cuttingPlane, self.look[0], self.look[1], self.look[2])
                
    def sceneSetCenter(self):
        minmax=[Range(), Range(), Range()]
        for s in self.shapes:
            if s.loaded:
                minPos, maxPos = s.getMinMax()
                for i in range(3):
                    minmax[i].setMin(minPos[i])
                    minmax[i].setMax(maxPos[i])
        
        sceneMin = Vec3([minmax[i].getMin() for i in range(3)])
        sceneMax = Vec3([minmax[i].getMax() for i in range(3)])
        
        distance = (sceneMin - sceneMax).length()
        center   = (sceneMin + sceneMax)*0.5
        
        self.sceneSetCenterLocal(center, distance)
    
    def sceneSetCenterLocal(self, center, distance):
        self.setCenter(center)
        self.setEye(Vec3(self.center[0], self.center[1], self.center[2] - distance))
        self.setUp(Vec3(0, -1, 0))
        self.setCuttingPlane(0.0)
        self.modelChanged()
         
        self.updateGL()
     
    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)
       
    def initializeGL(self):
        if((sys.platform != 'darwin') and (sys.platform != 'win32')):
            glutInit(sys.argv)      #This must be here to get it to work with Freeglut.
            #otherwise you get: "freeglut  ERROR:  Function <glutWireCube> called without first calling 'glutInit'."
       
        backgroundColor = QtGui.QColor(0, 0, 0, 255)
        glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), backgroundColor.alphaF())
        glClearDepth( 1.0 )
        
        self.setLights()
        self.setNearFarZoom(0.1, 1000, 0.25)

        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

    def setLights(self):
        glLight = [GL_LIGHT0, GL_LIGHT1]
        light0Color = QtGui.QColor(255, 255, 255, 255)
        light1Color = QtGui.QColor(255, 255, 255, 255)

        lightsColor = [[light0Color.redF(), light0Color.greenF(), light0Color.blueF(), 1.0],[light1Color.redF(), light1Color.greenF(), light1Color.blueF(), 1.0]]
        for i in range(2):
            if(self.lightsEnabled[i]):
                afPropertiesAmbient = [lightsColor[i][0]*0.3, lightsColor[i][1]*0.3, lightsColor[i][2]*0.3, 1.00]
                afPropertiesDiffuse = lightsColor[i]
                afPropertiesSpecular = [lightsColor[i][0]*0.1, lightsColor[i][0]*0.1, lightsColor[i][0]*0.1, 1.00]
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
        for i in range(len(self.shapes)):
            glPushName(i)
            self.shapes[i].draw()
            glPopName()
        self.additionalDraw()
        glPopMatrix()
        
    def legend(self):
#         glMatrixMode(GL_MODELVIEW)
#         w=40
#         h=40
        w=self.width()/4
        h=self.height()/4
        
        glViewport(0,0,w,h)
        
#         glMatrixMode(GL_PROJECTION)
#         glPushMatrix()
#         glLoadIdentity()
#         glMatrixMode(GL_MODELVIEW)
#         glPushMatrix()
#         glLoadIdentity()
        
        glColor(.5,.2,.8, .5)
        
        glBegin(GL_LINE_LOOP)
        glVertex(-1,-1,0)
        glVertex(-1,1,0)
        glVertex(1,1,0)
        glVertex(1,-1,0)
        glEnd()
        
#         glMatrixMode(GL_PROJECTION)
#         glPopMatrix()
#         glMatrixMode(GL_MODELVIEW)
#         glPopMatrix()

    def axes(self):
        w = self.width()/4
        
        sc=.8
        sh=0
        L=sc*w/2
        L=L-sh
        
        glPushMatrix()
        
#         glTranslate(-w*0.9/2,h*0.9/2,self.eye[2])
#         glTranslate(-w*0.9/2,0,0)
#         glRotate(90,0,1,0)
        
        glBegin(GL_LINES)
        glColor(1,0,0)
        glVertex(0,0,0)
        glVertex(L,0,0)
        glEnd()
        
        glBegin(GL_LINES)
        glColor(0,1,0)
        glVertex(0,0,0)
        glVertex(0,L,0)
        glEnd()

        glBegin(GL_LINES)
        glColor(0,0,1)
        glVertex(0,0,0)
        glVertex(0,0,L)
        glEnd()
        
        glPopMatrix()

        glViewport(0,0,self.width(),self.height())

    def additionalDraw(self):
        self.legend()
        self.axes()

    def processMouseDown(self, mouseHits, event):
        globalMinDepth = self.far + 1
        minNames = list()
        shapeId = -1
        for hit_record in mouseHits:
            minDepth, maxDepth, names = hit_record
            names = list(names)
            if(globalMinDepth > minDepth):
                globalMinDepth = minDepth
                minNames = names
        if(minNames != list()):
            shapeId = minNames[0];
            minNames.pop(0)
        self.selectedShape = shapeId;
            
    def setGluPerspective(self):
        gluPerspective(180 * self.eyeZoom, self.aspectRatio, self.near, self.far)
        #glOrtho(-200 * self.eyeZoom, 200 * self.eyeZoom, -200 * self.eyeZoom, 200 * self.eyeZoom, self.near, self.far)

    def pickObject(self, x, y):
        viewport = list(glGetIntegerv(GL_VIEWPORT))
        glSelectBuffer(10000)
        glRenderMode(GL_SELECT)

        glInitNames()
        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        gluPickMatrix(x, viewport[3]-y, 5, 5, viewport)
        self.setGluPerspective()
        self.updateGL()
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()
        glFlush()

        mouseHits = glRenderMode(GL_RENDER)
        return mouseHits

    def getMouseRay(self, x, y):
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        self.setGluLookAt()
        viewport = glGetIntegerv(GL_VIEWPORT)
        modelview = glGetDoublev(GL_MODELVIEW_MATRIX)
        projection = glGetDoublev(GL_PROJECTION_MATRIX)
        oglX = x
        oglY = viewport[3] - y
        oglZ = glReadPixels(oglX, oglY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT)[0][0]
        
        p2 = gluUnProject(oglX, oglY, oglZ, modelview, projection, viewport)
        glPopMatrix()
        return Vec3(p2) - self.eye
                
    def resizeGL(self, width, height):
        if(height > 0):
            self.aspectRatio = width/(1.0*height)
            glViewport(0,0, width, height)
            self.setGlProjection()

    def setGlProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        self.setGluPerspective()
        glMatrixMode(GL_MODELVIEW)
    
    def refreshMouseTracking(self):
        self.mouseTrackingEnabled    = False
        self.mouseTrackingEnabledRay = False
        for s in self.shapes:
            self.mouseTrackingEnabled    = self.mouseTrackingEnabled    or s.mouseMoveEnabled
            self.mouseTrackingEnabledRay = self.mouseTrackingEnabledRay or s.mouseMoveEnabledRay
        self.setMouseTracking(self.mouseTrackingEnabled or self.mouseTrackingEnabledRay)
        self.updateGL()
     
    def moveConstant(self):
        return (self.eye - self.center).length() #* abs(tan(pi * self.eyeZoom))

    def moveSelectedShape(self, dx, dy):
        print "In: moveSelectedShape"
        print "selectedShape: %d" % self.selectedShape
        
        if self.selectedShape > -1:
            dirVec = self.mouseVec(dx, dy)
            
            s = self.shapes[self.selectedShape]
            s.selectionMove(dirVec)

    def rotateSelectedShape(self, dx, dy):
        print "In: rotateSelectedShape"
        print "selectedShape: %d" % self.selectedShape
        
        if self.selectedShape > -1:
            dirVec = self.mouseVec(dx, dy).normalize()
            rotationAxis3D  = dirVec^self.look
            
            s = self.shapes[self.selectedShape]
            centerOfMass   = s.getCOM()
            print "  COM: ", s, centerOfMass
            centerOfMass.Print()
            
            self.lineaxis.redraw(centerOfMass, rotationAxis3D*1000.)
            self.dotcom.loc = centerOfMass
            s.selectionRotate(centerOfMass, rotationAxis3D, 5.)

    def mousePressEvent(self, event):
        self.mouseDownPoint    = QtCore.QPoint(event.pos())
        self.mouseMovePoint    = QtCore.QPoint(event.pos())
        self.mouseLeftPressed  = (event.buttons() & QtCore.Qt.LeftButton)
        self.mouseMidPressed   = (event.buttons() & QtCore.Qt.MidButton)
        self.mouseRightPressed = (event.buttons() & QtCore.Qt.RightButton)
        self.processMouseDown(self.pickObject(self.mouseDownPoint.x(), self.mouseDownPoint.y()), event)
        
    def mouseVec(self, dx, dy):
        newDx = self.moveConstant() * dx / float(self.width())
        newDy = self.moveConstant() * dy / float(self.height())
        return self.up*(-newDy) + self.right*newDx;

    def mouseMoveEvent(self, event):
        if(self.mouseTrackingEnabledRay):
            ray = self.getMouseRay(event.x(), event.y())
            for s in self.shapes:
                if(s.mouseMoveEnabledRay):
                    s.processMouseMoveRay(ray, 0.1, self.eye, event)
                       
        if(self.mouseTrackingEnabled):
            self.processMouseMove(self.pickObject(event.x(), event.y()), event)

        dx = event.x() - self.mouseMovePoint.x()
        dy = event.y() - self.mouseMovePoint.y()
                        
        if (self.mouseLeftPressed):
            print "mouseLeftPressed"
            if event.modifiers() & QtCore.Qt.CTRL:           # Rotating the selection
                print "event.modifiers() & QtCore.Qt.CTRL"
                self.rotateSelectedShape(dx, dy)
            else:                                               # Rotating the shapes
                self.setEyeRotation(-dx, dy, 0)
            
        elif (self.mouseRightPressed):
            print "mouseRightPressed"
            if event.modifiers() & QtCore.Qt.CTRL:                 # Translating the selection
                print "event.modifiers() & QtCore.Qt.CTRL"
                self.moveSelectedShape(dx, dy)
            else:                                                   # Translating the shapes
                translation = self.mouseVec(-dx, -dy)
                newEye = self.eye + translation;
                newCenter = self.center + translation;
                self.setEye(newEye)
                self.setCenter(newCenter)
                
        self.mouseMovePoint = QtCore.QPoint(event.pos())

        self.updateGL()
    
    def wheelEvent(self, event):
        direction = event.delta()/abs(event.delta())
        
        if(event.modifiers() & QtCore.Qt.ALT):                 # Setting the cutting plane
            self.setCuttingPlane(self.cuttingPlane + direction * 0.01)
        elif (not (event.modifiers() & QtCore.Qt.ALT) and not (event.modifiers() & QtCore.Qt.CTRL)):     # Zoom in / out
            self.setNearFarZoom(self.near, self.far, self.eyeZoom + direction * 10.0/360.0)
        self.updateGL()
        
    def modelChanged(self):
        minDist = 1000000000000.0
        maxDist = 0.0
        eyeDist = (self.eye - self.center).length()
        for s in self.shapes:
            if(s.loaded):
                center = s.getCenter()
                dist   = s.getDistance()
                modelDist = (self.center - center).length()
                minDist = min(minDist, eyeDist - modelDist - dist/2.0)
                maxDist = max(maxDist, eyeDist + modelDist + dist/2.0)
        self.setNearFarZoom(minDist, maxDist, self.eyeZoom)
        self.updateGL()

    def mouseDoubleClickEvent(self, event):
        pass
#         self.updateGL()
