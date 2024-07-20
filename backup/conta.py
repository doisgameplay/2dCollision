import math


def somar(x1,y1,x2,y2):
    return(x1+x2, y1+y2)

def sub(x1,y1,x2,y2, L):
    L = (180*L)/math.pi
    return (y2*math.cos(L)-y1*math.sin(L), x2*math.cos(L)-x1*math.sin(L))



print(sub(-1,0,0,1,45))



