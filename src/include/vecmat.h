//Extremely simple vector and matrix classes by Janusz Malinowski. 
#include <stdio.h>
#pragma once
class Vector4
{
 public:
	 Vector4();
	 void Set(double x,double y,double z);
	 double GetX();
	 double GetY();
	 double GetZ();

	 void SetColor(unsigned char r, unsigned char g, unsigned char b);
	 double GetRed();
	 double GetGreen();
	 double GetBlue();

	 Vector4 operator-(const Vector4 &);
	 friend Vector4 operator*(const Vector4 &,double);
	 double data[4];

	 unsigned char color[3];
};

class Matrix4
{
 public:
	 double data[4][4];
	 Matrix4();
	 Matrix4 operator*(const Matrix4);
	 friend Vector4 operator*(const Matrix4,const Vector4);
};


