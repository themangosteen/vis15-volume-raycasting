#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>

class GLWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	explicit GLWidget(QWidget *parent) : QOpenGLWidget(parent) { }

protected:

	void initializeGL();

	void resizeGL(int w, int h);

	void paintGL();

private:

	QOpenGLDebugLogger *logger;
	void printDebugMsg(const QOpenGLDebugMessage &msg) { qDebug() << qPrintable(msg.message()); }

	QOpenGLFunctions_3_3_Core *glf;
	QOpenGLShaderProgram *shader;


};

#endif // GLWIDGET_H
