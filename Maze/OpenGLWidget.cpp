#include "OpenGLWidget.h"
#include <iostream>
#include "MazeWidget.h"
#include "Helper.h"
#include <gl\gl.h>
#include <gl\GLU.h>

#ifdef _DEBUG 
;
#endif

int OpenGLWidget::frameCount = -1;

OpenGLWidget::OpenGLWidget(QWidget *parent) : QGLWidget(parent)
{

	top_z = 1.5f;
	but_z = -1;

	QDir dir("Pic");
	if (dir.exists())
		pic_path = "Pic/";
	else
		pic_path = "../x64/Release/Pic/";
}
void OpenGLWidget::initializeGL()
{
	glClearColor(0, 0, 0, 1);
	glEnable(GL_TEXTURE_2D);
	loadTexture2D(pic_path + "grass.png", grass_ID);
	loadTexture2D(pic_path + "sky.png", sky_ID);
}
void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (MazeWidget::maze != NULL)
	{
		//View 1
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, MazeWidget::w / 2, MazeWidget::h);
		float maxLength = max(MazeWidget::maze->max_xp, MazeWidget::maze->max_yp);
		glOrtho(-0.1, maxLength + 0.1, -0.1, maxLength + 0.1, 0, 10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Mini_Map();

		//View 2
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(MazeWidget::w / 2, 0, MazeWidget::w / 2, MazeWidget::h);
		// glOrtho(-1, 1, -1, 1, 0, 10);

		/*gluPerspective �w�q�z��
		//�����j�p, nearplane, farplane, distance
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		//gluPerspective(MazeWidget::maze->viewer_fov, 1 , 0.01 , 200);

		/* gluLookAt
		//�쥻�۾���m
		//�ݪ���V
		//����O�W��
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		//float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
		//float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
		//float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];
		/*gluLookAt(viewerPosX, viewerPosZ, viewerPosY,
			viewerPosX + cos(degree_change(MazeWidget::maze->viewer_dir)), viewerPosZ, viewerPosY + sin(degree_change(MazeWidget::maze->viewer_dir)),
			0.0, -1.0, 0.0);*/
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Map_3D();
	}
}
void OpenGLWidget::resizeGL(int w, int h)
{
}

//Draw Left Part
void OpenGLWidget::Mini_Map()
{
	glBegin(GL_LINES);

	float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];

	for (int i = 0; i < (int)MazeWidget::maze->num_edges; i++)
	{
		float edgeStartX = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::X];
		float edgeStartY = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::Y];
		float edgeEndX = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::X];
		float edgeEndY = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::Y];

		glColor3f(MazeWidget::maze->edges[i]->color[0], MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2]);
		if (MazeWidget::maze->edges[i]->opaque)
		{
			glVertex2f(edgeStartX, edgeStartY);
			glVertex2f(edgeEndX, edgeEndY);
		}
	}

	//draw frustum
	float len = 10;
	glColor3f(1, 1, 1);
	//�k
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + len * cos(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + len * sin(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));
	//��
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + len * cos(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + len *  sin(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)));
	glEnd();
}


//**********************************************************************
//
// * Draws the first-person view of the maze.
//   THIS IS THE FUINCTION YOU SHOULD MODIFY.
//
//Note: You must not use any openGL build-in function to set model matrix, view matrix and projection matrix.
//		ex: gluPerspective, gluLookAt, glTraslatef, glRotatef... etc.
//		Otherwise, You will get 0 !
//======================================================================
void OpenGLWidget::Map_3D()
{
#ifdef _DEBUG  	
	++frameCount;
	frameCount %= 10000;
#endif

	glLoadIdentity();

	// �e�k��϶����Ҧ��F��

	//�[��̦�m
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X] * -1; // x1
	float camPosY = 0; // y1
	float camPosZ = MazeWidget::maze->viewer_posn[Maze::Y] * -1; // z1
	float camDirection = MazeWidget::maze->viewer_dir; //�ݦV������(��X�b������)
	float camFOV = MazeWidget::maze->viewer_fov; //�����j�pFOV
	//�ݦV
	float camDirectionX = cos(deg2rad(MazeWidget::maze->viewer_dir)); // x2
	float camDirectionY = 0; // y2
	float camDirectionZ = sin(deg2rad(MazeWidget::maze->viewer_dir)); // z2
	//camDirectionZ *= -1;
	//���Y�W��
	//	(0.0, 1.0, 0.0)
	//���Y����(Cross product)
	float camLeftX = camPosY * camDirectionZ - camDirectionY * camPosZ;
	float camLeftY = camPosZ * camDirectionX - camDirectionZ * camPosX;
	float camLeftZ = camPosX * camDirectionY - camDirectionX * camPosY;
	//�w�q�i�H�ԥX���@�����k���䪺�I
	float dist = 0.1; //���k���䪺���� 
	//�k����
	float vRx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
	float vRy = camPosY + (MazeWidget::maze->max_yp) * dist * sin(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
	Edge eR(camPosX, camPosZ, vRx, vRy);
	//������
	float vLx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2));
	float vLy = camPosY + (MazeWidget::maze->max_yp) * dist * sin(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2));
	Edge eL(camPosX, camPosZ, vLx, vLy);

#ifdef _DEBUG  
	if (frameCount % DEBUG_OUTPUT_RATE == 0) {
		system("CLS");
		printf("Pos(%f, %f, %f), FOV=%f, Direction=%f (%f, %f, %f)\n",
			camPosX, camPosY, camPosZ, camFOV, camDirection, camDirectionX, camDirectionY, camDirectionZ);
	}
#endif

	//�����@�ǵ����|�Ψ쪺�F��
	auto vertices = MazeWidget::maze->vertices;
	auto edges = MazeWidget::maze->edges;
	auto cells = MazeWidget::maze->cells;

	//��ثe�۾��Ҧb��Cell
	int inCellIndex = -1;//�۾��Ҧb��cell ��index
	for (int i = 0; i != MazeWidget::maze->num_cells; ++i)
	{
		if (cells[i]->Point_In_Cell(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y]))
		{
			inCellIndex = i;
			break;
		}
	}

	//���j�h�ecell�a	
	drawCell(inCellIndex, camFOV / 2, camFOV / 2, -1);

	// �ǳƵe��
	//for (int i = 0; i < MazeWidget::maze->num_edges; i++) {
	//	drawWall(i);
	//}
	//drawWall(0);
	//drawWall(1);
	//drawWall(2);
	//drawWall(3);


	/*�Y�����쪺��, �i�H���a�O�ΰg�c�W�K��, �����ؤ��v�T����*/
	glBindTexture(GL_TEXTURE_2D, sky_ID);

	// �e�K�� & �� UV

	glDisable(GL_TEXTURE_2D);
}

//======================================================================
void OpenGLWidget::loadTexture2D(QString str, GLuint &textureID)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	QImage img(str);
	QImage opengl_grass = QGLWidget::convertToGLFormat(img);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, opengl_grass.width(), opengl_grass.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_grass.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);
}
float OpenGLWidget::deg2rad(float num)
{
	return num * 3.14159265f / 180.0f;
}

void OpenGLWidget::drawWall(float sx, float sy, float ex, float ey, float rr, float gg, float bb)
{
	// World space -> Camera Space

	// ��o�ǯx�} ���i�H����~���h�� �~
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float camPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float camDv[2] = { cos(deg2rad(-MazeWidget::maze->viewer_dir)), sin(deg2rad(-MazeWidget::maze->viewer_dir)) }; //�۾��ݦV���V�q(camera direction vector)
	float camLv[2] = { cos(deg2rad(-MazeWidget::maze->viewer_dir + 90)), sin(deg2rad(-MazeWidget::maze->viewer_dir + 90)) }; //�۾����誺�V�q(camera left vector)

#ifdef _DEBUG  
	if (frameCount % DEBUG_OUTPUT_RATE == 0) {
		printf("[drawWall] Pos(%f, %f), camDv=(%f, %f), camRv=(%f, %f), s(%f, %f), e(%f, %f)\n",
			camPosX, camPosY, camDv[0], camDv[1], camLv[0], camLv[1], sx, sy, ex, ey);
	}
#endif

	QVector4D vs0(sy, 1, sx, 1);
	QVector4D vs1(sy, -1, sx, 1);
	QVector4D ve0(ey, -1, ex, 1);
	QVector4D ve1(ey, 1, ex, 1);

	QMatrix4x4 rotation(camDv[0], 0, camDv[1], 0,
					0, 1, 0, 0,
					camLv[0], 0, camLv[1], 0,
					0, 0, 0, 1);

	QMatrix4x4 translation(1, 0, 0, -camPosY,
					0, 1, 0, 0,
					0, 0, 1, -camPosX,
					0, 0, 0, 1); // :))))))))))))))

	float n = 0.01; // near
	float r = n * tan(deg2rad(MazeWidget::maze->viewer_fov / 2)); // right
	float t = r; // top
	float f = 100; // far


	QMatrix4x4 projection(//http://www.songho.ca/opengl/gl_projectionmatrix.html
		n / r, 0, 0, 0,
		0, n / t, 0, 0,
		0, 0, (f + n) / (n - f), (-2 * f*n) / (f - n),
		0, 0, -1, 0);

	QVector4D p1 = (projection * rotation * translation * vs0);
	QVector4D p2 = (projection * rotation * translation * vs1);
	QVector4D p3 = (projection * rotation * translation * ve0);
	QVector4D p4 = (projection * rotation * translation * ve1);

	// normalize
	p1 /= p1.w();
	p2 /= p2.w();
	p3 /= p3.w();
	p4 /= p4.w();

	//�e��
	glColor3f(rr, gg, bb);
	glBegin(GL_QUADS);
	glVertex2f(p1.x(), p1.y());
	glVertex2f(p2.x(), p2.y());
	glVertex2f(p3.x(), p3.y());
	glVertex2f(p4.x(), p4.y());
	glEnd();
}

void OpenGLWidget::drawWall(int i)
{
	auto edges = MazeWidget::maze->edges;
	if (edges[i]->opaque) // ���z���~�e
	{
		Vertex *start = edges[i]->endpoints[Edge::START];
		Vertex *end = edges[i]->endpoints[Edge::END];
		drawWall(start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], //
			edges[i]->color[0], edges[i]->color[1], edges[i]->color[2]);
	}
}

void OpenGLWidget::drawCell(int currCellIndex, float leftFOV, float rightFOV, int prevEdge)
{
	//�����@�ǵ����|�Ψ쪺�F��
	auto vertices = MazeWidget::maze->vertices;
	auto edges = MazeWidget::maze->edges;
	auto cells = MazeWidget::maze->cells;
	auto cc = cells[currCellIndex];

	for (int i = 0; i != 4; ++i)
	{
		auto ee = cc->edges[i];

		if (ee->opaque) //���z��������
		{
			//�[��̦�m
			float camPosX = MazeWidget::maze->viewer_posn[Maze::X];
			float camPosY = MazeWidget::maze->viewer_posn[Maze::Y];
			//�w�q�i�H�ԥX���@�����k���䪺�I
			float dist = 1; //���k���䪺���� (�w�]0.1)
			//�k����
			float vRx = camPosX + dist * cos(deg2rad(MazeWidget::maze->viewer_dir - rightFOV));
			float vRy = camPosY + dist * sin(deg2rad(MazeWidget::maze->viewer_dir - rightFOV));
			QLineF eyeR(camPosX, camPosY, vRx, vRy);
			//������
			float vLx = camPosX + dist * cos(deg2rad(MazeWidget::maze->viewer_dir + leftFOV));
			float vLy = camPosY + dist * sin(deg2rad(MazeWidget::maze->viewer_dir + leftFOV));
			QLineF eyeL(camPosX, camPosY, vLx, vLy);

#ifdef _DEBUG  
			if (frameCount % DEBUG_OUTPUT_RATE == 0) {
				printf("[drawCell] eR(%f, %f, %f, %f), eL(%f, %f, %f, %f)\n",
					camPosX, camPosY, vRx, vRy, camPosX, camPosY, vLx, vLy);
			}
#endif
			QLineF edgeLine(ee->endpoints[Edge::START]->posn[Vertex::X], ee->endpoints[Edge::START]->posn[Vertex::Y],
				ee->endpoints[Edge::END]->posn[Vertex::X], ee->endpoints[Edge::END]->posn[Vertex::Y]);

			auto sr = pAtWhichSide(eyeR.x1(), eyeR.y1(), eyeR.x2(), eyeR.y2(), edgeLine.x1(), edgeLine.y1()); //�_�l�I �M �k���� �����Y
			auto er = pAtWhichSide(eyeR.x1(), eyeR.y1(), eyeR.x2(), eyeR.y2(), edgeLine.x2(), edgeLine.y2()); //���I �M �k���� �����Y
			// �ŵ�

			//�w��k���䰵�ŵ�
			if (sr == Side::RIGHT && er == Side::RIGHT) // OUT OUT
			{
				continue;
			}
			else if (sr == Side::RIGHT && er != Side::RIGHT) //�_�I�W�X�k���@
			{
				QPointF *ppp = new QPointF();
				if (eyeR.intersect(edgeLine, ppp) != QLineF::NoIntersection)
				{
					edgeLine.setP1(*ppp);
				}
			}
			else if (sr != Side::RIGHT && er == Side::RIGHT) //���I�W�X�k���@
			{
				QPointF *ppp = new QPointF();
				if (eyeR.intersect(edgeLine, ppp) != QLineF::NoIntersection)
				{
					edgeLine.setP2(*ppp);
				}
			}
			//���I���S�W�X

			auto sl = pAtWhichSide(eyeL.x1(), eyeL.y1(), eyeL.x2(), eyeL.y2(), edgeLine.x1(), edgeLine.y1()); //�_�l�I �M ������ �����Y
			auto el = pAtWhichSide(eyeL.x1(), eyeL.y1(), eyeL.x2(), eyeL.y2(), edgeLine.x2(), edgeLine.y2()); //���I �M ������ �����Y

		   //�w�索���䰵�ŵ�
			if (sl == Side::LEFT && el == Side::LEFT) // OUT OUT
			{
				continue;
			}
			else if (sl == Side::LEFT && el != Side::LEFT) //�_�I�W�X�����@
			{
				QPointF *ppp = new QPointF();
				if (eyeL.intersect(edgeLine, ppp) != QLineF::NoIntersection)
					edgeLine.setP1(*ppp);
			}
			else if (sl != Side::LEFT && el == Side::LEFT) //���I�W�X�����@
			{
				QPointF *ppp = new QPointF();
				if (eyeL.intersect(edgeLine, ppp) != QLineF::NoIntersection)
					edgeLine.setP2(*ppp);
			}
			//���I���S�W�X

			// �e��
			drawWall(edgeLine.x1(), edgeLine.y1(), edgeLine.x2(), edgeLine.y2(), ee->color[0], ee->color[1], ee->color[2]);
		}
		else //�z�����A��F�~�ӵe
		{

		}
	}
}

// �P�_(xp,yp)�bab�V�q�����@�l
OpenGLWidget::Side OpenGLWidget::pAtWhichSide(float ax, float ay, float bx, float by, float xp, float yp)
{
	//float A = by - ay, B = ax - bx, C = bx * ay - ax * by; //���u��{��Ax+By+C=0
	//float D = A * xp + B * yp + C;

	//if (D < 0)
	//	return Side::LEFT;
	//else if (D > 0)
	//	return Side::RIGHT;
	//else if (D == 0)
	//	return Side::ON;

	float d = (xp - ax) * (by - ay) - (yp - ay) * (bx - ax);

	return d < 0 ? Side::LEFT : Side::RIGHT;
}

int OpenGLWidget::getIntersection(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float * x, float * y)
{
	float s02x, s02y, s10x, s10y, s32x, s32y, s_numer, t_numer, denom, t;
	s10x = x1 - x0;
	s10y = y1 - y0;
	s32x = x3 - x2;
	s32y = y3 - y2;

	denom = s10x * s32y - s32x * s10y;
	if (denom == 0)//����Φ@�u
		return 0; // Collinear

	s02x = x0 - x2;
	s02y = y0 - y2;
	s_numer = s10x * s02y - s10y * s02x;

	//�ѼƬO�j�󵥩�0�B�p�󵥩�1���A���l���������P���B���l�p�󵥩����
	if ((s_numer < 0) == (denom > 0))
		return 0; // No collision
	t_numer = s32x * s02y - s32y * s02x;
	if ((t_numer < 0) == (denom > 0))
		return 0; // No collision
	if (fabs(s_numer) > fabs(denom) || fabs(t_numer) > fabs(denom))
		return 0; // No collision

	// Collision detected
	t = t_numer / denom;
	if (x != NULL)
		*x = x0 + (t * s10x);
	if (y != NULL)
		*y = y0 + (t * s10y);

	return 1;
}
