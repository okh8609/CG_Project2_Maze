#include "OpenGLWidget.h"
#include <iostream>
#include "MazeWidget.h"
#include "Helper.h"
#include <gl\gl.h>
#include <gl\GLU.h>

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
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + (MazeWidget::maze->max_yp) * len * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));

	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + (MazeWidget::maze->max_yp) * len *  sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)));
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
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X]; // x1
	float camPosY = 0; // y1
	float camPosZ = MazeWidget::maze->viewer_posn[Maze::Y]; // z1
	camPosZ *= -1;
	float camFOV = MazeWidget::maze->viewer_fov; //�����j�p
	float camDirection = MazeWidget::maze->viewer_dir; //�ݦV������(��X�b������)
	float camDirectionRad = -1 * (-90 + camDirection) * 3.14159 / 180; //�ݦV������(��X�b������)(�|��)
	//�ݦV���y��
	float camDirectionX = camPosX + cos(degree_change(MazeWidget::maze->viewer_dir)); // x2
	float camDirectionY = 0; // y2
	float camDirectionZ = camPosY + sin(degree_change(MazeWidget::maze->viewer_dir)); // z2
	camDirectionZ *= -1;
	//���Y�W��(0.0, 1.0, 0.0)
	//���Y����(Cross product)
	float camLeftX = camPosY * camDirectionZ - camDirectionY * camPosZ;
	float camLeftY = camPosZ * camDirectionX - camDirectionZ * camPosX;
	float camLeftZ = camPosX * camDirectionY - camDirectionX * camPosY;
#ifdef _DEBUG  
	if (frameCount % 100 == 0) {
		system("CLS");
		printf("Pos(%f, %f, %f), FOV=%f, Direction=%f (%f, %f, %f)\n",
			camPosX, camPosY, camPosZ, camFOV, camDirection, camDirectionX, camDirectionY, camDirectionZ);
	}
#endif

	//�w�q�i�H�ԥX���@�����k���䪺�I
	float dist = 0.1; //���k���䪺���� 
	//������
	float vLx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
	float vLy = camPosY + (MazeWidget::maze->max_yp) * dist * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
	//�k����
	float vRx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2));
	float vRy = camPosY + (MazeWidget::maze->max_yp) * dist *  sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2));

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
	float n = 0.01; // near
	float r = 10; // right
	float t = 10; // top
	float f = 999999; // far (inf)
	float camera_perspective2screen[4][4] = {
		//�ѦҡGhttp://www.songho.ca/opengl/gl_projectionmatrix.html
			{n / r,     0,                 0,                    0},
			{    0, n / t,                 0,                    0},
			{    0,     0, (f + n) / (n - f), (-2 * f*n) / (f - n)},
			{    0,     0,                -1,                    0}
	};
	//Helper::matrix44_X_matrix44(m1, m2, allMatrixTogether);
	//Helper::matrix44_X_matrix44(camera_perspective2screen, allMatrixTogether, allMatrixTogether);

	//Helper::matrix44_X_matrix44(camera_perspective2screen, world2camera, allMatrixTogether);
	//Helper::matrix44_X_matrix44(camera_perspective2screen, m1, allMatrixTogether);
#ifdef _DEBUG 
	if (frameCount % 100 == 0)
	{
		std::cout << endl;
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				//std::cout << allMatrixTogether[i][j] << '\t' << '\t';
			}
			std::cout << endl;
		}
	}
#endif
	// �ǳƵe��
	auto vertices = MazeWidget::maze->vertices;
	auto edges = MazeWidget::maze->edges;

	for (int i = 0; i < MazeWidget::maze->num_edges; i++) {
		if (edges[i]->opaque) // ���z���~�e
		{
			//�ǳƦn�n�e���|���I
			Vertex *start = edges[i]->endpoints[Edge::START];
			Vertex *end = edges[i]->endpoints[Edge::END];

			float x0 = start->posn[Vertex::X];
			float z0 = start->posn[Vertex::Y];

			float x1 = end->posn[Vertex::X];
			float z1 = end->posn[Vertex::Y];

			float p0[4] = { x0, 0, z0, 1 }; //�����O����i��|�����D!! �n�`�N!!
			float p1[4] = { x1, 0, z1, 1 };

			//�����۾��Ŷ� (camera point)
			float* cp0 = Helper::matrix44_X_vector4(world2camera, p0);
			float* cp1 = Helper::matrix44_X_vector4(world2camera, p1);

			//������|���I (view point) (3��)
			float vp0[4] = { cp0[0], 1, cp0[1], 1 };
			float vp1[4] = { cp0[0], -1, cp0[1], 1 };
			float vp2[4] = { cp1[0], 1, cp1[1], 1 };
			float vp3[4] = { cp1[0], -1, cp1[1], 1 };

			//������|���I (wall point) (��v��2��)
			//perspective
			float *wp0 = Helper::matrix44_X_vector4(camera_perspective2screen, vp0);
			float *wp1 = Helper::matrix44_X_vector4(camera_perspective2screen, vp1);
			float *wp2 = Helper::matrix44_X_vector4(camera_perspective2screen, vp2);
			float *wp3 = Helper::matrix44_X_vector4(camera_perspective2screen, vp3);


			//���W��
			Helper::norm_vector4(wp0);
			Helper::norm_vector4(wp1);
			Helper::norm_vector4(wp2);
			Helper::norm_vector4(wp3);

			//���C��
			float r = edges[i]->color[0], g = edges[i]->color[1], b = edges[i]->color[2];

			//�e��
			glColor3f(r, g, b);
			glBegin(GL_QUADS);
			glVertex2f(wp0[0], wp0[1]);
			glVertex2f(wp1[0], wp1[1]);
			glVertex2f(wp2[0], wp2[1]);
			glVertex2f(wp3[0], wp3[1]);
			glEnd();
		}
	}

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
float OpenGLWidget::degree_change(float num)
{
	return num / 180.0f * 3.1415926535f;
}