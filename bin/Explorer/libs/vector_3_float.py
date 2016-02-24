from libpytoolkit import Vector3Float as Vec3F


class Vec3(Vec3F):
#     pass

#     def __init__(self):
#         print "I am in : ", __name__
#         super(Vec3, self).__init__(0.0,0.0,0.0)
        
    def __init__(self, *args, **kwargs):
#         print "args: ", args
#         print "kwargs: ", kwargs
#
#         if args:
#             print "args"
#         if not args:
#             print "args not"
#
#         if kwargs:
#             print "kwargs"
#         if not kwargs:
#             print "kwargs not"
        
        if kwargs:
            raise Exception("Wrong named-arguments")
        if len(args) == 1 and isinstance(args[0], list):
            [x, y, z] = args[0]
        elif len(args) == 3:
            [x, y, z] = [args[i] for i in range(3)]
        else:
            raise Exception("Wrong arguments")
#         if len(args) == 1 and not isinstance(args[1], list):
#             raise Exception("Not a list")

#         [x, y, z] = [args[i] for i in range(3)]
        super(Vec3, self).__init__(x, y, z)

#     def __init__(self, x, y, z):
#         super(Vec3, self).__init__(x, y, z)
