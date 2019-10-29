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
	//右
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + len * cos(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + len * sin(deg2rad(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)));
	//左
	glVertex2f(viewerPosX, viewerPosY);
	glVertex2f(viewerPosX + len * cos(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
		viewerPosY + len * sin(deg2rad(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)));
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

	// 畫右邊區塊的所有東西===============================================================

	//找目前相機所在的 Cell index
	auto cells = MazeWidget::maze->cells;
	int inCellIndex = -1;//相機所在的cell 的index
	for (int i = 0; i != MazeWidget::maze->num_cells; ++i)
	{
		if (cells[i]->Point_In_Cell(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y]))
		{
			inCellIndex = i;
			break;
		}
	}

	//觀察者位置
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float camPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float camFOV = MazeWidget::maze->viewer_fov; //視野大小FOV
	//定義可以拉出視錐中左右切邊的點
	float dist = 1; //左右切邊的長度 (預設0.1)
	//右切邊
	float vRx = camPosX + dist * cos(deg2rad(MazeWidget::maze->viewer_dir - camFOV / 2));
	float vRy = camPosY + dist * sin(deg2rad(MazeWidget::maze->viewer_dir - camFOV / 2));
	QLineF eyeR(camPosX, camPosY, vRx, vRy);
	//左切邊
	float vLx = camPosX + dist * cos(deg2rad(MazeWidget::maze->viewer_dir + camFOV / 2));
	float vLy = camPosY + dist * sin(deg2rad(MazeWidget::maze->viewer_dir + camFOV / 2));
	QLineF eyeL(camPosX, camPosY, vLx, vLy);

#ifdef _DEBUG  
	//看向的角度(跟X軸的夾角)
	float camDirection = MazeWidget::maze->viewer_dir;
	//看向 向量
	float camDirectionX = cos(deg2rad(MazeWidget::maze->viewer_dir));
	float camDirectionY = sin(deg2rad(MazeWidget::maze->viewer_dir));

	if (frameCount % DEBUG_OUTPUT_RATE == 0) {
		system("CLS");
		printf("Pos(%f, %f), FOV=%f, Direction=%f (%f, %f)\n",
			camPosX, camPosY, camFOV, camDirection, camDirectionX, camDirectionY);
	}
#endif

	//遞迴去畫cell吧	
	drawCell(inCellIndex, eyeL, eyeR, -1);

	// ====================================================================================


	// 若有興趣的話, 可以為地板或迷宮上貼圖, 此項目不影響評分
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

// 畫一道牆的函式
// 給定 起點 s(x,y)、終點 e(x,y)、顏色
// 透過透視相似三角形的概念，畫出3D的牆
// 注意：輸入的邊，必須已經剪裁至視錐內
void OpenGLWidget::drawWall(float sx, float sy, float ex, float ey, float rr, float gg, float bb)
{

	// World space -> Camera Space (2D)

	// 算這些矩陣 其實可以拿到外面去算 ㄏ 
	// 但是放在這裡整個程式看起來比較有架構
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float camPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float camDv[2] = { cos(deg2rad(MazeWidget::maze->viewer_dir)), sin(deg2rad(MazeWidget::maze->viewer_dir)) }; //相機看向的向量(camera direction vector) (長度=1，不用在正規化)
	float camRv[2] = { cos(deg2rad(MazeWidget::maze->viewer_dir - 90)), sin(deg2rad(MazeWidget::maze->viewer_dir - 90)) }; //相機右方的向量(camera right vector) (長度=1，不用在正規化)
#ifdef _DEBUG  
	if (frameCount % DEBUG_OUTPUT_RATE == 0) {
		printf("[drawWall] Pos(%f, %f), camDv=(%f, %f), camRv=(%f, %f)\n",
			camPosX, camPosY, camDv[0], camDv[1], camRv[0], camRv[1]);
	}
#endif

	// 找[Rv Dv]反矩陣 
	float temp = camRv[0] * camDv[1] - camDv[0] * camRv[1];
	float invRvDv[2][2] = { {      camDv[1] / temp, -1 * camDv[0] / temp },
							{ -1 * camRv[1] / temp,      camRv[0] / temp } };
	// ??聽說 旋轉矩陣的直接transpose就是inverse了 =.=??
	//float invRvDv[2][2] = { {camRv[0], camRv[1]},
	//						{camDv[0], camDv[1]} };

	// x y 轉換到 Camera Space (2D)
	// (原座標 - camPos) 在乘上[Rv Dv]反矩陣
	float sp[2] = { (sx - camPosX)  * (invRvDv[0][0]) + (sy - camPosY) * (invRvDv[0][1]),
					(sx - camPosX)  * (invRvDv[1][0]) + (sy - camPosY) * (invRvDv[1][1]) }; //start point
	float ep[2] = { (ex - camPosX)  * (invRvDv[0][0]) + (ey - camPosY) * (invRvDv[0][1]),
					(ex - camPosX)  * (invRvDv[1][0]) + (ey - camPosY) * (invRvDv[1][1]) }; //end point

	//Camera Space -> Perspective Projection
	//轉到三維，再轉到螢幕 [-1, -1] to [1, 1] 
	float d = 1 / tan(deg2rad(MazeWidget::maze->viewer_fov / 2)); //相機與屏幕的距離
	float p[4][2] = { 0 }; //最後要畫在屏幕上的點
	p[0][0] = sp[0] * d / sp[1];
	p[0][1] = sqrt((p[0][0] * p[0][0] + d * d) / (sp[0] * sp[0] + sp[1] * sp[1]));
	p[1][0] = p[0][0];
	p[1][1] = -1 * p[0][1];
	p[2][0] = ep[0] * d / ep[1];
	p[2][1] = sqrt((p[2][0] * p[2][0] + d * d) / (ep[0] * ep[0] + ep[1] * ep[1]));
	p[3][0] = p[2][0];
	p[3][1] = -1 * p[2][1];

	//畫牆
	glColor3f(rr, gg, bb);
	glBegin(GL_QUADS);
	glVertex2f(p[0][0], p[0][1]);
	glVertex2f(p[1][0], p[1][1]);
	glVertex2f(p[3][0], p[3][1]);
	glVertex2f(p[2][0], p[2][1]);
	glEnd();


	/* 下面這段code也是可以正常運作的，它是用矩陣下去乘。

	// World space -> Camera Space

	float camPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float camPosY = MazeWidget::maze->viewer_posn[Maze::Y];
	float camDv[2] = { cos(deg2rad(-MazeWidget::maze->viewer_dir)), sin(deg2rad(-MazeWidget::maze->viewer_dir)) }; //相機看向的向量(camera direction vector)
	float camLv[2] = { cos(deg2rad(-MazeWidget::maze->viewer_dir + 90)), sin(deg2rad(-MazeWidget::maze->viewer_dir + 90)) }; //相機左方的向量(camera left vector)

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

	//畫牆
	glColor3f(rr, gg, bb);
	glBegin(GL_QUADS);
	glVertex2f(p1.x(), p1.y());
	glVertex2f(p2.x(), p2.y());
	glVertex2f(p3.x(), p3.y());
	glVertex2f(p4.x(), p4.y());
	glEnd();
	*/
}

// Debug用的function
// 只畫出指定index的牆面
void OpenGLWidget::drawWall(int i)
{
	auto edges = MazeWidget::maze->edges;
	if (edges[i]->opaque) // 不透明才畫
	{
		Vertex *start = edges[i]->endpoints[Edge::START];
		Vertex *end = edges[i]->endpoints[Edge::END];
		drawWall(start->posn[Vertex::X], start->posn[Vertex::Y], end->posn[Vertex::X], end->posn[Vertex::Y], //
			edges[i]->color[0], edges[i]->color[1], edges[i]->color[2]);
	}
}

// 參數說明：
// currCellIndex: 目前所在的cell的index
// eyeL: 視錐左邊切線
// eyeR: 視錐右邊切線
// prevEdge: 上一個遞迴進來的邊的index，防止遇到透明的邊又跳回去
void OpenGLWidget::drawCell(int currCellIndex, QLineF eyeL, QLineF eyeR, int prevEdge)
{
	float camPosX = MazeWidget::maze->viewer_posn[Maze::X];
	float camPosY = MazeWidget::maze->viewer_posn[Maze::Y];

	for (int i = 0; i != 4; ++i)
	{
		//要畫的邊
		auto ee = MazeWidget::maze->cells[currCellIndex]->edges[i];
		//要畫的邊
		QLineF edgeLine(ee->endpoints[Edge::START]->posn[Vertex::X], ee->endpoints[Edge::START]->posn[Vertex::Y],
			ee->endpoints[Edge::END]->posn[Vertex::X], ee->endpoints[Edge::END]->posn[Vertex::Y]);

		// 將edgeLine剪裁到目前的視錐內============================================================================
#ifdef _DEBUG  
		if (frameCount % DEBUG_OUTPUT_RATE == 0) {
			printf("[drawCell] eyeL(%f, %f, %f, %f), eyeR(%f, %f, %f, %f)\n",
				eyeL.x1(), eyeL.y1(), eyeL.x2(), eyeL.y2(), eyeR.x1(), eyeR.y1(), eyeR.x2(), eyeR.y2());
		}
#endif
		auto sr = pAtWhichSide(eyeR.x1(), eyeR.y1(), eyeR.x2(), eyeR.y2(), edgeLine.x1(), edgeLine.y1()); //起始點 和 右切邊 的關係
		auto er = pAtWhichSide(eyeR.x1(), eyeR.y1(), eyeR.x2(), eyeR.y2(), edgeLine.x2(), edgeLine.y2()); //終點 和 右切邊 的關係

		//針對右切邊做剪裁
		if (sr == Side::RIGHT && er == Side::RIGHT) // OUT OUT
		{
			continue;
		}
		else if (sr == Side::RIGHT && er != Side::RIGHT) //起點超出右視錐
		{
			QPointF *ppp = new QPointF();
			if (eyeR.intersect(edgeLine, ppp) != QLineF::NoIntersection)
				edgeLine.setP1(*ppp);
		}
		else if (sr != Side::RIGHT && er == Side::RIGHT) //終點超出右視錐
		{
			QPointF *ppp = new QPointF();
			if (eyeR.intersect(edgeLine, ppp) != QLineF::NoIntersection)
				edgeLine.setP2(*ppp);
		}
		//else {} //兩點都沒超出


		auto sl = pAtWhichSide(eyeL.x1(), eyeL.y1(), eyeL.x2(), eyeL.y2(), edgeLine.x1(), edgeLine.y1()); //起始點 和 左切邊 的關係
		auto el = pAtWhichSide(eyeL.x1(), eyeL.y1(), eyeL.x2(), eyeL.y2(), edgeLine.x2(), edgeLine.y2()); //終點 和 左切邊 的關係

	   //針對左切邊做剪裁
		if (sl == Side::LEFT && el == Side::LEFT) // OUT OUT
		{
			continue;
		}
		else if (sl == Side::LEFT && el != Side::LEFT) //起點超出左視錐
		{
			QPointF *ppp = new QPointF();
			if (eyeL.intersect(edgeLine, ppp) != QLineF::NoIntersection)
				edgeLine.setP1(*ppp);
		}
		else if (sl != Side::LEFT && el == Side::LEFT) //終點超出左視錐
		{
			QPointF *ppp = new QPointF();
			if (eyeL.intersect(edgeLine, ppp) != QLineF::NoIntersection)
				edgeLine.setP2(*ppp);
		}
		//else {} //兩點都沒超出

		// ============================================================================================================

		if (ee->opaque) // 不透明的牆邊
		{
			// 畫牆
			drawWall(edgeLine.x1(), edgeLine.y1(), edgeLine.x2(), edgeLine.y2(), ee->color[0], ee->color[1], ee->color[2]);
		}
		else // 透明的，找鄰居來畫
		{
			// 不要跳回去前一個cell
			if (ee->index == prevEdge)
				continue;

			// 設定新的視錐 為這條透明邊可以看出去的範圍
			QLineF* newEyeR = NULL; // 新視錐的 右切邊
			QLineF* newEyeL = NULL; // 新視錐的 左切邊
			// 確定左右
			if (pAtWhichSide(camPosX, camPosY, edgeLine.x1(), edgeLine.y1(), edgeLine.x2(), edgeLine.y2()) == Side::LEFT)
			{
				newEyeR = new QLineF(camPosX, camPosY, edgeLine.x1(), edgeLine.y1());
				newEyeL = new QLineF(camPosX, camPosY, edgeLine.x2(), edgeLine.y2());
			}
			else
			{
				newEyeL = new QLineF(camPosX, camPosY, edgeLine.x1(), edgeLine.y1());
				newEyeR = new QLineF(camPosX, camPosY, edgeLine.x2(), edgeLine.y2());
			}

			// 遞迴呼叫
			if (currCellIndex != ee->neighbors[Edge::LEFT]->index)
				drawCell(ee->neighbors[Edge::LEFT]->index, *newEyeL, *newEyeR, ee->index);
			else
				drawCell(ee->neighbors[Edge::RIGHT]->index, *newEyeL, *newEyeR, ee->index);

		}
	}
}

// 判斷(xp,yp)在ab向量的哪一惻
OpenGLWidget::Side OpenGLWidget::pAtWhichSide(float ax, float ay, float bx, float by, float xp, float yp)
{
	//float A = by - ay, B = ax - bx, C = bx * ay - ax * by; //直線方程為Ax+By+C=0
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

