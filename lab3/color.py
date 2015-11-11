from H3D import *
from H3DInterface import *


class colorClass(TypedField(SFColor,MFBool)):
    def update(self, event):
      if event.getValue() is False:
	return RGB(0.8,0.0,0.3)
      else:
	return RGB(0.0,0.8,0.2)

color = colorClass()