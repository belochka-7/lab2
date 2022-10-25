#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}
double vec_length(double* vec)
{
	double  result;

	result = sqrt((vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]));
	return (result);
}

double* vec_normalize(double* vec)
{
	double  length = vec_length(vec);
	double* ret = new double[3];
	ret[0] = vec[0] / length;
	ret[1] = vec[1] / length;
	ret[2] = vec[2] / length;
	return ret;
}

double* vec_dot_product(double* vec1, double* vec2)
{
	double* ret = new double[3];
	ret[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
	ret[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
	ret[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
	return (ret);
}

double* get_vec(double* a, double* b) {
	double* ret = new double[3];
	ret[0] = a[0] - b[0];
	ret[1] = a[1] - b[1];
	ret[2] = a[2] - b[2];
	return ret;
}

void draw_prizme()
{
	double a[] = { 0, 4, 0 };
	double b[] = { 2, 0, 0 };
	double c[] = { 6, 0, 0 };
	double d[] = { 8, -6, 0 };
	double e[] = { 3, -8, 0 };
	double f[] = { 0, -4, 0 };
	double g[] = { -6, -5, 0 };
	double h[] = { -8, 0, 0 };

	double hight = 3;

	glColor3d(0, 1, 0);

	glNormal3dv(vec_normalize(vec_dot_product(get_vec(g, a), get_vec(h, a))));

	glBegin(GL_TRIANGLES);
		glVertex3dv(h);
		glVertex3dv(a);
		glVertex3dv(g);
	glEnd();

	glColor3d(0, 1, 0);

	glNormal3dv(vec_normalize(vec_dot_product(get_vec(f, g), get_vec(a, g))));

	glBegin(GL_TRIANGLES);
		glVertex3dv(a);
		glVertex3dv(g);
		glVertex3dv(f);
	glEnd();

	glColor3d(0, 1, 0);

	glNormal3dv(vec_normalize(vec_dot_product(get_vec(f, b), get_vec(a, b))));

	glBegin(GL_TRIANGLES);
		glVertex3dv(a);
		glVertex3dv(b);
		glVertex3dv(f);
	glEnd();

	glNormal3dv(vec_normalize(vec_dot_product(get_vec(e, f), get_vec(b, f))));

	glBegin(GL_TRIANGLES);
		glVertex3dv(b);
		glVertex3dv(f);
		glVertex3dv(e);
	glEnd();

	glNormal3dv(vec_normalize(vec_dot_product(get_vec(c, e), get_vec(b, e))));

	glBegin(GL_TRIANGLES);
		glVertex3dv(b);
		glVertex3dv(e);
		glVertex3dv(c);
	glEnd();

	glNormal3dv(vec_normalize(vec_dot_product(get_vec(d, e), get_vec(c, e))));

	glBegin(GL_TRIANGLES);
		glVertex3dv(c);
		glVertex3dv(e);
		glVertex3dv(d);
	glEnd();

	glColor3d(1, 0, 0);

	glBegin(GL_QUADS);
	glVertex3dv(a);
	glVertex3dv(b);
	a[2] += hight;
	b[2] += hight;
	glVertex3dv(b);
	glVertex3dv(a);
	b[2] -= hight;
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(b);
	glVertex3dv(c);
	b[2] += hight;
	c[2] += hight;
	glVertex3dv(c);
	glVertex3dv(b);
	c[2] -= hight;
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(c);
	glVertex3dv(d);
	c[2] += hight;
	d[2] += hight;
	glVertex3dv(d);
	glVertex3dv(c);
	d[2] -= hight;
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(d);
	glVertex3dv(e);
	d[2] += hight;
	e[2] += hight;
	glVertex3dv(e);
	glVertex3dv(d);
	e[2] -= hight;
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(e);
	glVertex3dv(f);
	e[2] += hight;
	f[2] += hight;
	glVertex3dv(f);
	glVertex3dv(e);
	f[2] -= hight;
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(f);
	glVertex3dv(g);
	f[2] += hight;
	g[2] += hight;
	glVertex3dv(g);
	glVertex3dv(f);
	g[2] -= hight;
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(g);
	glVertex3dv(h);
	g[2] += hight;
	h[2] += hight;
	glVertex3dv(h);
	glVertex3dv(g);
	h[2] -= hight;
	glEnd();

	glBegin(GL_QUADS);
	a[2] -= hight;
	glVertex3dv(h);
	glVertex3dv(a);
	h[2] += hight;
	a[2] += hight;
	glVertex3dv(a);
	glVertex3dv(h);
	glEnd();

	glColor3d(0, 0, 1);

	glBegin(GL_TRIANGLES);
	glVertex3dv(h);
	glVertex3dv(a);
	glVertex3dv(g);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex3dv(a);
	glVertex3dv(g);
	glVertex3dv(f);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(f);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex3dv(b);
	glVertex3dv(f);
	glVertex3dv(e);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex3dv(b);
	glVertex3dv(e);
	glVertex3dv(c);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex3dv(c);
	glVertex3dv(e);
	glVertex3dv(d);
	glEnd();
}


void Render(OpenGL *ogl)
{

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	draw_prizme();

	//������ ��������� ���������� ��������
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}