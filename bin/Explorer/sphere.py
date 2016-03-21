from shape import Shape


class Sphere(Shape):
    
    def __init__(self, main):
        super(Sphere, self).__init__(main)
        
        self.title = "Sphere"
        
        self.setColor(120, 18, 80, 150)
        
        self.setLoc(30., 10., 10.)
        
    def paintGL(self):
        self.setMaterials(self.color)
        self.s.drawSphere(self.loc, 10.)
