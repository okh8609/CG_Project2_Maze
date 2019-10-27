#include "OpenGLWidget.h"
#include <iostream>
#include "MazeWidget.h"
#include "Helper.h"
#include <gl\gl.h>
#include <gl\GLU.h>

#ifdef _DEBUG 
;
#endif

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
		glOrtho(-0.1, MazeWidget::maze->max_xp + 0.1, -0.1, MazeWidget::maze->max_yp + 0.1, 0, 10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Mini_Map();

		//View 2
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(MazeWidget::w / 2, 0, MazeWidget::w / 2, MazeWidget::h);
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
	float len = 0.1;
	glColor3f(1, 1, 1);
	//�k
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + (MazeWidget::maze->max_yp) * len * sin(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));
	//��
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + (MazeWidget::maze->max_yp) * len *  sin(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)));
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
	static int frameCount = -1;
	++frameCount;
	frameCount %= 10000;
#endif

	glLoadIdentity();

	// �e�k��϶����Ҧ��F��

	//�[��̦�m
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X] * -1; // x1
	float camPosY = 0; // y1
	float camPosZ = MazeWidget::maze->viewer_posn[Maze::Y] * -1; // z1
	float camFOV = MazeWidget::maze->viewer_fov; //�����j�pFOV
	float camFOV_half_rad = deg2rad(camFOV / 2); //�����j�pFOV ���@�b (�|��)
	float camLeftFOV = 90 + camFOV / 2;
	float camRightFOV = 90 - camFOV / 2;
	float camDirection = MazeWidget::maze->viewer_dir; //�ݦV������(��X�b������)
	float camDirectionRad = -1 * (-90 + camDirection) * 3.14159 / 180; //�ݦV������(��X�b������)(�|��)
	//�ݦV���y��
	float camDirectionX = camPosX + cos(deg2rad(MazeWidget::maze->viewer_dir)); // x2
	float camDirectionY = 0; // y2
	float camDirectionZ = camPosY + sin(deg2rad(MazeWidget::maze->viewer_dir)); // z2
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
	float vLy = camPosY + (MazeWidget::maze->max_yp) * dist *  sin(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2));
	Edge eL(camPosX, camPosZ, vLx, vLy);
#ifdef _DEBUG  
	if (frameCount % 100 == 0) {
		system("CLS");
		printf("Pos(%f, %f, %f), FOV=%f, Direction=%f (%f, %f, %f)\n",
			camPosX, camPosY, camPosZ, camFOV, camDirection, camDirectionX, camDirectionY, camDirectionZ);
	}
#endif

	// �w�q�U���ഫ�x�}
	// World space -> Camera Space
	// rotate matrix + tranlate matrix
	float world2camera[4][4] = {
		{cos(camDirectionRad), 0, -1 * sin(camDirectionRad), cos(camDirectionRad) * camPosX - sin(camDirectionRad) * camPosZ},
		{                   0, 1,                         0,                                                         camPosY},
		{sin(camDirectionRad), 0,      cos(camDirectionRad), sin(camDirectionRad) * camPosX + cos(camDirectionRad) * camPosZ},
		{                   0, 0,                         0,                                                               1}
	};

	//Camera Space -> Perspective Projection�F�����Y�� (-1,-1) �� (1,1) ����
	//�ۦ��T���ΡA����tan()�N�n�F
	float perspectiveTangent = tan(camFOV_half_rad);

	//�����@�ǵ����|�Ψ쪺�F��
	auto vertices = MazeWidget::maze->vertices;
	auto edges = MazeWidget::maze->edges;
	auto cells = MazeWidget::maze->cells;

	//��ثe�۾��Ҧb��Cell
	int inCellIndex = -1;//�۾��Ҧb��cell ��index
	for (int i = 0; i != MazeWidget::maze->num_cells; ++i)
	{
		if (cells[i]->Point_In_Cell(camPosX, camPosZ))
		{
			inCellIndex = i;
			break;
		}
	}

	//���j�h�ecell�a	
	drawCell(inCellIndex, camLeftFOV, camRightFOV, -1, world2camera, perspectiveTangent);

	//// �ǳƵe��
	//for (int i = 0; i < MazeWidget::maze->num_edges; i++) {
	//	//int i = 2; {
	//	if (edges[i]->opaque) // ���z���~�e
	//	{
	//		//�ǳƦn�n�e���|���I
	//		Vertex *start = edges[i]->endpoints[Edge::START];
	//		Vertex *end = edges[i]->endpoints[Edge::END];
	//		drawWall(start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y],
	//			edges[i]->color[0], edges[i]->color[1], edges[i]->color[2], world2camera, perspectiveTangent);
	//	}
	//}

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

void OpenGLWidget::drawWall(float sx, float sy, float ex, float ey, float r, float g, float b, float world2camera[4][4], float perspectiveTangent)
{
	//�ǳƦn�n�e���|���I
	float p0[4] = { sx, 0, sy, 1 };
	float p1[4] = { ex, 0, ey, 1 };

	//�����۾��Ŷ� (camera point)
	float* cp0 = Helper::matrix44_X_vector4(world2camera, p0);
	float* cp1 = Helper::matrix44_X_vector4(world2camera, p1);

	//��� [-1, -1] to [1, 1] 
	float xxx, zzz;
	xxx = cp0[0];
	zzz = cp0[2];
	cp0[0] = xxx / (zzz * perspectiveTangent);
	cp0[2] = 1 / (zzz * perspectiveTangent);
	xxx = cp1[0];
	zzz = cp1[2];
	cp1[0] = xxx / (zzz * perspectiveTangent);
	cp1[2] = 1 / (zzz * perspectiveTangent);

	//�e��
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	glVertex2f(cp0[0], cp0[2]);
	glVertex2f(cp0[0], cp0[2] * -1);
	glVertex2f(cp1[0], cp1[2] * -1);
	glVertex2f(cp1[0], cp1[2]);
	glEnd();
}

void OpenGLWidget::drawCell(int currCellIndex, float leftFOV, float rightFOV, int prevEdge, float world2camera[4][4], float perspectiveTangent)
{
	return;
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
			float camPosX = MazeWidget::maze->viewer_posn[Maze::X] * -1; // x1
			float camPosY = 0; // y1
			float camPosZ = MazeWidget::maze->viewer_posn[Maze::Y] * -1; // z1
			//�w�q�i�H�ԥX���@�����k���䪺�I
			float dist = 0.1; //���k���䪺���� 
			//�k����
			float vRx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir - rightFOV));
			float vRy = camPosY + (MazeWidget::maze->max_yp) * dist * sin(deg2rad(MazeWidget::maze->viewer_dir - rightFOV));
			Edge eR(camPosX, camPosZ, vRx, vRy);
			//������
			float vLx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir + leftFOV));
			float vLy = camPosY + (MazeWidget::maze->max_yp) * dist * sin(deg2rad(MazeWidget::maze->viewer_dir + leftFOV));
			Edge eL(camPosX, camPosZ, vLx, vLy);

			Vertex *start = ee->endpoints[Edge::START];
			Vertex *end = ee->endpoints[Edge::END];
			auto sr = eR.Point_Side(start->posn[Vertex::X], start->posn[Vertex::Y]); //�_�l�I �M �k���� �����Y
			auto sl = eL.Point_Side(start->posn[Vertex::X], start->posn[Vertex::Y]); //�_�l�I �M ������ �����Y
			auto er = eR.Point_Side(end->posn[Vertex::X], end->posn[Vertex::Y]); //���I �M �k���� �����Y
			auto el = eL.Point_Side(end->posn[Vertex::X], end->posn[Vertex::Y]); //���I �M ������ �����Y


			//ee->color[0], ee->color[1], ee->color[2], world2camera, perspectiveTangent);


			// �ŵ�

			// �U�C�X�ر��p�A�~���i��e�X��
			if ((sr == Edge::LEFT || sr == Edge::ON) && //�b���@��/�W
				(sl == Edge::RIGHT || sl == Edge::ON) &&
				(er == Edge::LEFT || er == Edge::ON) &&
				(el == Edge::RIGHT || el == Edge::ON))
			{

			}
			else if ((sr == Edge::RIGHT) && (sl == Edge::RIGHT) && //��k
				(er == Edge::LEFT || er == Edge::ON) &&
				(el == Edge::RIGHT || el == Edge::ON))
			{

			}
			else if ((er == Edge::RIGHT) && (el == Edge::RIGHT) && //��k
				(sr == Edge::LEFT || sr == Edge::ON) &&
				(sl == Edge::RIGHT || sl == Edge::ON))
			{

			}
			else if ((sr == Edge::LEFT) && (sl == Edge::LEFT) && //�楪
				(er == Edge::LEFT || er == Edge::ON) &&
				(el == Edge::RIGHT || el == Edge::ON))
			{

			}
			else if ((er == Edge::LEFT) && (el == Edge::LEFT) && //�楪
				(sr == Edge::LEFT || sr == Edge::ON) &&
				(sl == Edge::RIGHT || sl == Edge::ON))
			{

			}
			else if ((er == Edge::LEFT) && (el == Edge::LEFT) && //����
				(sr == Edge::RIGHT) && (sl == Edge::RIGHT))
			{


				//�M��n�h�P�_�ŵ���A�b���b�Ӧb���u�W�A�A�h�P�_�n���n�e�C
				if (0)
				{
					//
				}
			}
			else if ((er == Edge::RIGHT) && (el == Edge::RIGHT) && //����
				(sr == Edge::LEFT) && (sl == Edge::LEFT))
			{


				//�M��n�h�P�_�ŵ���A�b���b�Ӧb���u�W�A�A�h�P�_�n���n�e�C
				if (0)
				{
					//
				}
			}


			// �e��
			//drawWall();
		}
		else //�z�����A��F�~�ӵe
		{

		}
	}
}
