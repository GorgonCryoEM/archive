from line import Line
from .libs import Vec3


class Line0(Line):
    
    def __init__(self, main, p1):
        super(Line0, self).__init__(main, p1, Vec3( 0, 0, 0))
        
        self.title = "Line0"
        
        self.setColor(40, 70, 100, 150)
