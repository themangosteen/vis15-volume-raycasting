#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "Volume.h"

class MainWindow;

class GLWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	explicit GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
		mainWindow = qobject_cast<MainWindow *>(this->parent()->parent()->parent());
	}

public slots:
	void dataLoaded();

protected:

	void initializeGL();

	void resizeGL(int w, int h);

	void paintGL();

private:

	void render();

	MainWindow * mainWindow;

	QOpenGLDebugLogger *logger;
	void printDebugMsg(const QOpenGLDebugMessage &msg) { qDebug() << qPrintable(msg.message()); }

	QOpenGLFunctions_3_3_Core *glf;

	QOpenGLShaderProgram *shader;
	QOpenGLBuffer vbo;
	QOpenGLVertexArrayObject vao;

	QMatrix4x4 modelMat;
	QMatrix4x4 viewMat;
	QMatrix4x4 projMat;

	Volume *volumeData;
	std::vector<float> vertices = {
		0.0f,   2.0f,   1.0f,  0.9f,
		4.0f,  3.0f,   4.0f,  0.4f,
		2.0f,   10.0f,  0.0f,  0.6f,
		0.0f,   5.0f,  3.0f, 0.2f
	}; // delete this



};

#endif // GLWIDGET_H
