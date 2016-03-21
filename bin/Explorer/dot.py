from shape import Shape


class Dot(Shape):
    
    def __init__(self, main):
        super(Dot, self).__init__(main)
        
        self.title = "Dot"
        
        self.setColor(120, 0, 0, 150)
        
        self.setLoc(0., 0., 0.)
        
    def paintGL(self):
        self.setMaterials(self.color)
        self.s.drawSphere(self.loc, 1.)
