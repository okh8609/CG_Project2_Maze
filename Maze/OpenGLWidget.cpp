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
	//右
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + (MazeWidget::maze->max_yp) * len * sin(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));
	//左
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
	++frameCount;
	frameCount %= 10000;
#endif

	glLoadIdentity();

	// 畫右邊區塊的所有東西

	//觀察者位置
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X] * -1; // x1
	float camPosY = 0; // y1
	float camPosZ = MazeWidget::maze->viewer_posn[Maze::Y] * -1; // z1
	float camFOV = MazeWidget::maze->viewer_fov; //視野大小FOV
	float camFOV_half_rad = deg2rad(camFOV / 2); //視野大小FOV 的一半 (徑度)
	float camLeftFOV = 90 + camFOV / 2;
	float camRightFOV = 90 - camFOV / 2;
	float camDirection = MazeWidget::maze->viewer_dir; //看向的角度(跟X軸的夾角)
	float camDirectionRad = -1 * (-90 + camDirection) * 3.14159 / 180; //看向的角度(跟X軸的夾角)(徑度)
	//看向的座標
	float camDirectionX = camPosX + cos(deg2rad(MazeWidget::maze->viewer_dir)); // x2
	float camDirectionY = 0; // y2
	float camDirectionZ = camPosY + sin(deg2rad(MazeWidget::maze->viewer_dir)); // z2
	//camDirectionZ *= -1;
	//鏡頭上方
	//	(0.0, 1.0, 0.0)
	//鏡頭左方(Cross product)
	float camLeftX = camPosY * camDirectionZ - camDirectionY * camPosZ;
	float camLeftY = camPosZ * camDirectionX - camDirectionZ * camPosX;
	float camLeftZ = camPosX * camDirectionY - camDirectionX * camPosY;
	//定義可以拉出視錐中左右切邊的點
	float dist = 0.1; //左右切邊的長度 
	//右切邊
	float vRx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
	float vRy = camPosY + (MazeWidget::maze->max_yp) * dist * sin(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
	Edge eR(camPosX, camPosZ, vRx, vRy);
	//左切邊
	float vLx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2));
	float vLy = camPosY + (MazeWidget::maze->max_yp) * dist *  sin(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2));
	Edge eL(camPosX, camPosZ, vLx, vLy);
#ifdef _DEBUG  
	if (frameCount % DEBUG_OUTPUT_RATE == 0) {
		system("CLS");
		printf("Pos(%f, %f, %f), FOV=%f, Direction=%f (%f, %f, %f)\n",
			camPosX, camPosY, camPosZ, camFOV, camDirection, camDirectionX, camDirectionY, camDirectionZ);
	}
#endif

	//拿取一些等等會用到的東西
	auto vertices = MazeWidget::maze->vertices;
	auto edges = MazeWidget::maze->edges;
	auto cells = MazeWidget::maze->cells;

	//找目前相機所在的Cell
	int inCellIndex = -1;//相機所在的cell 的index
	for (int i = 0; i != MazeWidget::maze->num_cells; ++i)
	{
		if (cells[i]->Point_In_Cell(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y]))
		{
			inCellIndex = i;
			break;
		}
	}

	//遞迴去畫cell吧	
	//drawCell(inCellIndex, camLeftFOV, camRightFOV, -1);

	// 準備畫牆
	//for (int i = 0; i < MazeWidget::maze->num_edges; i++) {
	//	drawWall(i);
	//}
	drawWall(0);
	//drawWall(1);
	//drawWall(2);
	//drawWall(3);


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
float OpenGLWidget::deg2rad(float num)
{
	return num * 3.14159265f / 180.0f;
}

void OpenGLWidget::drawWall(float sx, float sy, float ex, float ey, float r, float g, float b)
{
	// Step 1: 基底轉換
	// World space -> Camera Space (2D)

	// 算這些矩陣 其實可以拿到外面去算 ㄏ
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float camPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float camDv[2] = { cos(deg2rad(MazeWidget::maze->viewer_dir)), sin(deg2rad(MazeWidget::maze->viewer_dir)) }; //相機看向的向量(camera direction vector) --這角度跟我想得不太一樣哦 0.0(試誤法)
	float camRv[2] = { cos(deg2rad(MazeWidget::maze->viewer_dir - 90)), sin(deg2rad(MazeWidget::maze->viewer_dir - 90)) }; //相機右方的向量(camera right vector)
#ifdef _DEBUG  
	if (frameCount % DEBUG_OUTPUT_RATE == 0) {
		printf("[drawWall] Pos(%f, %f), camDv=(%f, %f), camRv=(%f, %f)\n",
			camPosX, camPosY, camDv[0], camDv[1], camRv[0], camRv[1]);
	}
#endif

	//正規化 (長度變1)
	float temp = 0.0;
	temp = camDv[0] * camDv[0] + camDv[1] * camDv[1];
	camDv[0] /= temp;
	camDv[1] /= temp;
	temp = camRv[0] * camRv[0] + camRv[1] * camRv[1];
	camRv[0] /= temp;
	camRv[1] /= temp;

	//找[Rv Dv]反矩陣  ??聽說 旋轉矩陣的直接transpose就是inverse了 -.-
	temp = camRv[0] * camDv[1] - camDv[0] * camRv[1];
	float invRvDv[2][2] = { {      camDv[1] / temp, -1 * camDv[0] / temp },
							{ -1 * camRv[1] / temp,      camRv[0] / temp } };

	// x y 轉換到 Camera Space (2D)
	// (原座標 - camPos) 在乘上[Rv Dv]反矩陣
	float sp[2] = { (sx - camPosX)  * (invRvDv[0][0]) + (sy - camPosY) * (invRvDv[0][1]),
					(sx - camPosX)  * (invRvDv[1][0]) + (sy - camPosY)  *(invRvDv[1][1]) }; //start point
	float ep[2] = { (ex - camPosX)  * (invRvDv[0][0]) + (ey - camPosY) * (invRvDv[0][1]),
					(ex - camPosX)  * (invRvDv[1][0]) + (ey - camPosY)  *(invRvDv[1][1]) }; //end point

	//Camera Space -> Perspective Projection
	//轉到三維，再轉到螢幕 [-1, -1] to [1, 1] 
	float d = 1 / tan(deg2rad(MazeWidget::maze->viewer_fov / 2)); //相機與屏幕的距離
	float p[4][2] = { 0 }; //最後要畫在屏幕上的點
	p[0][0] = sp[0] * d / sp[1];
	p[0][1] = sqrt((p[0][0] * p[0][0] + d * d) / (sp[0] * sp[0] + sp[1] * sp[1]));
	p[1][0] = p[0][0];
	p[1][1] = -1 * p[0][1];
	p[2][0] = ep[0] * d / ep[1];
	p[2][1] = -1 * sqrt((p[2][0] * p[2][0] + d * d) / (ep[0] * ep[0] + ep[1] * ep[1]));
	p[3][0] = p[2][0];
	p[3][1] = -1 * p[2][1];

	//畫牆
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	glVertex2f(p[0][0], p[0][1]);
	glVertex2f(p[1][0], p[1][1]);
	glVertex2f(p[2][0], p[2][1]);
	glVertex2f(p[3][0], p[3][1]);
	glEnd();
}

void OpenGLWidget::drawWall(int i)
{
	auto edges = MazeWidget::maze->edges;
	if (edges[i]->opaque) // 不透明才畫
	{
		Vertex *start = edges[i]->endpoints[Edge::START];
		Vertex *end = edges[i]->endpoints[Edge::END];
		drawWall(start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y],
			edges[i]->color[0], edges[i]->color[1], edges[i]->color[2]);
	}
}

void OpenGLWidget::drawCell(int currCellIndex, float leftFOV, float rightFOV, int prevEdge)
{
	//return;
	//拿取一些等等會用到的東西
	auto vertices = MazeWidget::maze->vertices;
	auto edges = MazeWidget::maze->edges;
	auto cells = MazeWidget::maze->cells;
	auto cc = cells[currCellIndex];

	for (int i = 0; i != 4; ++i)
	{
		auto ee = cc->edges[i];

		if (ee->opaque) //不透明的牆邊
		{
			//觀察者位置
			float camPosX = MazeWidget::maze->viewer_posn[Maze::X] * -1; // x1
			float camPosY = 0; // y1
			float camPosZ = MazeWidget::maze->viewer_posn[Maze::Y] * -1; // z1
			//定義可以拉出視錐中左右切邊的點
			float dist = 0.1; //左右切邊的長度 
			//右切邊
			float vRx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir - rightFOV));
			float vRy = camPosY + (MazeWidget::maze->max_yp) * dist * sin(deg2rad(MazeWidget::maze->viewer_dir - rightFOV));
			Edge eR(camPosX, camPosZ, vRx, vRy);
			//左切邊
			float vLx = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir + leftFOV));
			float vLy = camPosY + (MazeWidget::maze->max_yp) * dist * sin(deg2rad(MazeWidget::maze->viewer_dir + leftFOV));
			Edge eL(camPosX, camPosZ, vLx, vLy);

			Vertex *start = ee->endpoints[Edge::START];
			Vertex *end = ee->endpoints[Edge::END];
			auto sr = eR.Point_Side(start->posn[Vertex::X], start->posn[Vertex::Y]); //起始點 和 右切邊 的關係
			auto sl = eL.Point_Side(start->posn[Vertex::X], start->posn[Vertex::Y]); //起始點 和 左切邊 的關係
			auto er = eR.Point_Side(end->posn[Vertex::X], end->posn[Vertex::Y]); //終點 和 右切邊 的關係
			auto el = eL.Point_Side(end->posn[Vertex::X], end->posn[Vertex::Y]); //終點 和 左切邊 的關係

			// 下列幾種情況，才有可能畫出來
			if ((sr == Edge::LEFT || sr == Edge::ON) && //在視錐內/上
				(sl == Edge::RIGHT || sl == Edge::ON) &&
				(er == Edge::LEFT || er == Edge::ON) &&
				(el == Edge::RIGHT || el == Edge::ON))
			{
				// 剪裁
				// 畫牆
				drawWall(start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y],
					ee->color[0], ee->color[1], ee->color[2]);
			}
			else if ((sr == Edge::RIGHT) && (sl == Edge::RIGHT) && //交右
				(er == Edge::LEFT || er == Edge::ON) &&
				(el == Edge::RIGHT || el == Edge::ON))
			{
				float xi = 0, yi = 0;
				// 剪裁
				if (getIntersection(camPosX, camPosZ, vRx, vRy,
					start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], &xi, &yi) == 1)
				{
					// 畫牆
					drawWall(xi, yi, end->posn[Vertex::X], end->posn[Vertex::Y],
						ee->color[0], ee->color[1], ee->color[2]);
				}
			}
			else if ((er == Edge::RIGHT) && (el == Edge::RIGHT) && //交右
				(sr == Edge::LEFT || sr == Edge::ON) &&
				(sl == Edge::RIGHT || sl == Edge::ON))
			{
				float xi = 0, yi = 0;
				// 剪裁
				if (getIntersection(camPosX, camPosZ, vRx, vRy,
					start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], &xi, &yi) == 1)
				{
					// 畫牆
					drawWall(start->posn[Vertex::X], start->posn[Vertex::Y], xi, yi,
						ee->color[0], ee->color[1], ee->color[2]);
				}
			}
			else if ((sr == Edge::LEFT) && (sl == Edge::LEFT) && //交左
				(er == Edge::LEFT || er == Edge::ON) &&
				(el == Edge::RIGHT || el == Edge::ON))
			{
				float xi = 0, yi = 0;
				// 剪裁
				if (getIntersection(camPosX, camPosZ, vLx, vLy,
					start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], &xi, &yi) == 1)
				{
					// 畫牆
					drawWall(xi, yi, end->posn[Vertex::X], end->posn[Vertex::Y],
						ee->color[0], ee->color[1], ee->color[2]);
				}
			}
			else if ((er == Edge::LEFT) && (el == Edge::LEFT) && //交左
				(sr == Edge::LEFT || sr == Edge::ON) &&
				(sl == Edge::RIGHT || sl == Edge::ON))
			{
				float xi = 0, yi = 0;
				// 剪裁
				if (getIntersection(camPosX, camPosZ, vLx, vLy,
					start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], &xi, &yi) == 1)
				{
					// 畫牆
					drawWall(start->posn[Vertex::X], start->posn[Vertex::Y], xi, yi,
						ee->color[0], ee->color[1], ee->color[2]);
				}
			}
			else if ((er == Edge::LEFT) && (el == Edge::LEFT) && //雙交
				(sr == Edge::RIGHT) && (sl == Edge::RIGHT))
			{
				float xi0 = 0, yi0 = 0, xi1 = 0, yi1 = 0;
				// 剪裁
				if ((getIntersection(camPosX, camPosZ, vRx, vRy, start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], &xi0, &yi0) == 1) &&
					(getIntersection(camPosX, camPosZ, vLx, vLy, start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], &xi1, &yi1) == 1))
				{
					//相機的左方 邊
					float cL_line_x = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
					float cL_line_y = camPosY + (MazeWidget::maze->max_yp) * dist * sin(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
					Edge eL_line(camPosX, camPosZ, cL_line_x, cL_line_y);

					//剪裁後，兩個點都位於相機前方才畫。
					//兩點都在 "相機左方向量線" 的右邊，才畫。
					if (eL_line.Point_Side(xi0, yi0) == Edge::RIGHT &&
						eL_line.Point_Side(xi1, yi1) == Edge::RIGHT)
					{
						// 畫牆
						drawWall(xi0, yi0, xi1, yi1,
							ee->color[0], ee->color[1], ee->color[2]);
					}
				}
			}
			else if ((er == Edge::RIGHT) && (el == Edge::RIGHT) && //雙交
				(sr == Edge::LEFT) && (sl == Edge::LEFT))
			{
				float xi0 = 0, yi0 = 0, xi1 = 0, yi1 = 0;
				// 剪裁
				if ((getIntersection(camPosX, camPosZ, vRx, vRy, start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], &xi0, &yi0) == 1) &&
					(getIntersection(camPosX, camPosZ, vLx, vLy, start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], &xi1, &yi1) == 1))
				{
					//相機的左方 邊
					float cL_line_x = camPosX + (MazeWidget::maze->max_xp) * dist * cos(deg2rad(MazeWidget::maze->viewer_dir - 90));
					float cL_line_y = camPosY + (MazeWidget::maze->max_yp) * dist * sin(deg2rad(MazeWidget::maze->viewer_dir - 90));
					Edge eL_line(camPosX, camPosZ, cL_line_x, cL_line_y);

					//剪裁後，兩個點都位於相機前方才畫。
					//兩點都在 "相機左方向量線" 的右邊，才畫。
					if (eL_line.Point_Side(xi0, yi0) == Edge::RIGHT &&
						eL_line.Point_Side(xi1, yi1) == Edge::RIGHT)
					{
						// 畫牆
						drawWall(xi0, yi0, xi1, yi1,
							ee->color[0], ee->color[1], ee->color[2]);
					}
				}
			}
		}
		else //透明的，找鄰居來畫
		{

		}
	}
}

int OpenGLWidget::getIntersection(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float * x, float * y)
{
	float s02x, s02y, s10x, s10y, s32x, s32y, s_numer, t_numer, denom, t;
	s10x = x1 - x0;
	s10y = y1 - y0;
	s32x = x3 - x2;
	s32y = y3 - y2;

	denom = s10x * s32y - s32x * s10y;
	if (denom == 0)//平行或共線
		return 0; // Collinear

	s02x = x0 - x2;
	s02y = y0 - y2;
	s_numer = s10x * s02y - s10y * s02x;

	//參數是大於等於0且小於等於1的，分子分母必須同號且分子小於等於分母
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
