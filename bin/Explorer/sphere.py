from shape import Shape


class Sphere(Shape):
    
    def __init__(self, main, R=1.0):
        super(Sphere, self).__init__(main)
        
        self.title = "Sphere"
        self.R = R
        
        self.setColor(120, 18, 80, 150)
        
        self.setLoc(30., 10., 10.)
        
    def draw(self):
        self.setMaterials(self.color)
        self.s.drawSphere(self.loc, self.R)
