from libpytoolkit import Vector3Float as Vec3F


class Vec3(Vec3F):
#     pass

#     def __init__(self):
#         print "I am in : ", __name__
#         super(Vec3, self).__init__(0.0,0.0,0.0)
        
    def __init__(self, l):
        [x, y, z] = [l[i] for i in range(3)]
        super(Vec3, self).__init__(x, y, z)

#     def __init__(self, x, y, z):
#         super(Vec3, self).__init__(x, y, z)
