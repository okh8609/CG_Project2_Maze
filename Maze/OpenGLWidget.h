#pragma once
#include <QGLWidget>
#include <QString>
#include <QDir>

using namespace std;

class OpenGLWidget :public QGLWidget
{
	Q_OBJECT
public:
	explicit OpenGLWidget(QWidget *parent = 0);

	void initializeGL();
	void paintGL();
	void resizeGL(int ,int );

	//Maze Setting
	void Mini_Map();
	void Map_3D();
	void loadTexture2D(QString, GLuint &);
	float deg2rad(float );
private:
	GLuint grass_ID;
	GLuint sky_ID;
	QString pic_path;

	float top_z;
	float but_z;

	// �_�I sx sy�F���I ex ey�F�C�� r g b�F�ഫ�x�}�F�ഫtan()
	void drawWall(float sx, float sy, float ex, float ey, float r, float g, float b, float world2camera[4][4], float perspectiveTangent);
};

