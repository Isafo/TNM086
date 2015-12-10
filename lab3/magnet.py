from H3D import *
from H3DInterface import *

import math

class forceClass(TypedField(SFVec3f,(SFVec3f, SFVec3f, SFRotation))):
	def update(self, event):
		routes_in = self.getRoutesIn()
		devicePos = routes_in[0].getValue()
		linePos = routes_in[1].getValue()
		rotAxis = routes_in[2].getValue()
		
		axis = Vec3f(rotAxis.x, rotAxis.y, rotAxis.z)
		theta = rotAxis.a
		
		axisL = math.sqrt(math.pow(rotAxis.x, 2) + math.pow(rotAxis.y, 2) + math.pow(rotAxis.z, 2))
		axisNorm = Vec3f(axis.x / axisL, axis.y / axisL, axis.z / axisL)
		
		probePos = Vec3f(devicePos.x + axisNorm.x * -0.01, devicePos.y + axisNorm.y * -0.01, devicePos.z + axisNorm.z * -0.01)
		vec = Vec3f(linePos - probePos )
	
		length0 = math.pow(vec.x, 2)
		length1 = math.pow(vec.y, 2)
		length2 = math.pow(vec.z, 2)
		
		limit = 0.1
		length = math.sqrt(length0 + length1 + length2)
	
		print length
	
		if length > limit:
			return Vec3f(0.0,0.0,0.0)
		else:
			vec = vec/length
			return vec * ( (1 - length / limit) * 0.8)

forceM = forceClass()