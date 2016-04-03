from sphere import Sphere


class Dot(Sphere):
    
    def __init__(self):
        super(Dot, self).__init__()
        
        self.title = "Dot"
        
        self.setColor(120, 0, 0, 150)
        
        self.setLoc(0., 0., 0.)
