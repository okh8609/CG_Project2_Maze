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
		/*gluPerspective 定義透視
		//視野大小, nearplane, farplane, distance
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		//gluPerspective(MazeWidget::maze->viewer_fov, 1 , 0.01 , 200);

		/* gluLookAt
		//原本相機位置
		//看的方向
		//哪邊是上面
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
	glLoadIdentity();

	// 畫右邊區塊的所有東西

	//觀察者位置
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X]; // x1
	float camPosY = 0; // y1
	float camPosZ = MazeWidget::maze->viewer_posn[Maze::Y]; // z1
	camPosZ *= -1;
	float camFOV = MazeWidget::maze->viewer_fov; //視野大小
	float camDirection = MazeWidget::maze->viewer_dir; //看向的角度(跟X軸的夾角)
	//看向的座標
	float camDirectionX = camPosX + cos(degree_change(MazeWidget::maze->viewer_dir)); // x2
	float camDirectionY = 0; // y2
	float camDirectionZ = camPosY + sin(degree_change(MazeWidget::maze->viewer_dir)); // z2
	camDirectionZ *= -1;
	//鏡頭上方(0.0, 1.0, 0.0)
	//鏡頭左方(Cross product)
	float camLeftX = camPosY * camDirectionZ - camDirectionY * camPosZ;
	float camLeftY = camPosZ * camDirectionX - camDirectionZ * camPosX;
	float camLeftZ = camPosX * camDirectionY - camDirectionX * camPosY;
#ifdef _DEBUG  
	printf("Pos(%f, %f, %f), FOV=%f, Direction=%f (%f, %f, %f)\n",
		camPosX, camPosY, camPosZ, camFOV, camDirection, camDirectionX, camDirectionY, camDirectionZ);
#endif

	//定義可以拉出視錐中左右切邊的點
	float dist = 10; //左右切邊的長度 
	//左切邊
	float vLx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
	float vLy = camPosY + (MazeWidget::maze->max_yp) * dist * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
	//右切邊
	float vRx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2));
	float vRy = camPosY + (MazeWidget::maze->max_yp) * dist *  sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2));

	////在視窗右方畫圖的範例
	////左下角(-1,-1)；右上角(1,1)
	//glColor3f(1.0, 0.0, 0.0);
	////glBegin(GL_POLYGON)
	//glBegin(GL_TRIANGLES);
	//glVertex2f(-0.4, -0.4);
	//glVertex2f(0.0, 0.4);
	//glVertex2f(0.4, -0.4);
	//glEnd();

	// World space -> Camera Space -> Perspective Projection

	// 定義各種轉換矩陣
	float world2camera[4][4] = { // World space -> Camera Space
		{camLeftX, 0, camDirectionX, camPosX},
		{camLeftY, 1, camDirectionY, camPosY},
		{camLeftZ, 0, camDirectionZ, camPosZ},
		{       0, 0,             0,       1}
	};
	float camera_perspective2screen[4][4] = { //Camera Space -> Perspective Projection；並壓縮到 (-1,-1) 到 (1,1) 之間
		//參考：http://www.songho.ca/opengl/gl_projectionmatrix.html
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0.1, 0}
	};
	auto allMatrixTogether = Helper::matrix44_X_matrix44(camera_perspective2screen, world2camera);

	// 準備畫牆
	auto vertices = MazeWidget::maze->vertices;
	auto edges = MazeWidget::maze->edges;

	for (int i = 0; i < MazeWidget::maze->num_edges; i++) {
		if (edges[i]->opaque) // 不透明才畫
		{
			//準備好要畫的四個點
			Vertex *start = edges[i]->endpoints[Edge::START];
			Vertex *end = edges[i]->endpoints[Edge::END];

			float x1 = start->posn[Vertex::X];
			float y1 = start->posn[Vertex::Y];
			float z1 = 1;

			float x2 = x1;
			float y2 = y1;
			float z2 = -1;

			float x3 = end->posn[Vertex::X];
			float y3 = end->posn[Vertex::Y];
			float z3 = -1;

			float x4 = x3;
			float y4 = y3;
			float z4 = 1;

			float p1[4] = { x1, y1, z1, 1.0 }; //等等記憶體可能會有問題!! 要注意!!
			float p2[4] = { x2, y2, z2, 1.0 };
			float p3[4] = { x3, y3, z3, 1.0 };
			float p4[4] = { x4, y4, z4, 1.0 };			

			//乘上轉換矩陣 並 正規化
			float* pp1 = Helper::matrix44_X_vector4(allMatrixTogether, p1);
			pp1 = Helper::norm_vector4(pp1);
			float* pp2 = Helper::matrix44_X_vector4(allMatrixTogether, p2);
			pp2 = Helper::norm_vector4(pp2);
			float* pp3 = Helper::matrix44_X_vector4(allMatrixTogether, p3);
			pp3 = Helper::norm_vector4(pp3);
			float* pp4 = Helper::matrix44_X_vector4(allMatrixTogether, p4);
			pp4 = Helper::norm_vector4(pp4);

			//取顏色
			float r = edges[i]->color[0], g = edges[i]->color[1], b = edges[i]->color[2];

			//畫牆
			glColor3f(r, g, b);
			glBegin(GL_QUADS);
			glVertex2f(pp1[0], pp1[1]);
			glVertex2f(pp2[0], pp2[1]);
			glVertex2f(pp3[0], pp3[1]);
			glVertex2f(pp4[0], pp4[1]);
			glEnd();
		}
	}

	/*若有興趣的話, 可以為地板或迷宮上貼圖, 此項目不影響評分*/
	glBindTexture(GL_TEXTURE_2D, sky_ID);

	// 畫貼圖 & 算 UV

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