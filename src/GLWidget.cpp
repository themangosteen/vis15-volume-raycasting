#include "GLWidget.h"

void GLWidget::initializeGL()
{
	// obtain a functions object and resolve all entry points
	// in this case we use QOpenGLFunctions_3_3_Core* glFunctions
	glf = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
	if (!glf) {
		qWarning("Could not obtain OpenGL versions object");
		exit(1);
	}
	glf->initializeOpenGLFunctions();

	logger = new QOpenGLDebugLogger(this);
	logger->initialize();
	connect(logger, &QOpenGLDebugLogger::messageLogged, this, &GLWidget::printDebugMsg);
	logger->startLogging();

	glf->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// TEST CODE BEGIN --------------------

	// TODO load shaders from file like in seganku

	shader = new QOpenGLShaderProgram(QOpenGLContext::currentContext());
	shader->addShaderFromSourceCode(QOpenGLShader::Vertex,
		"attribute vec4 vertex;\n"
		"uniform mat4 matrix;\n"
		"void main(void)\n"
		"{\n"
		"   gl_Position = matrix * vertex;\n"
		"}");
	shader->addShaderFromSourceCode(QOpenGLShader::Fragment,
		"uniform vec4 color;\n"
		"void main(void)\n"
		"{\n"
		"   gl_FragColor = color;\n"
		"}");
	shader->link();
	shader->bind();

	// TEST CODE END --------------------


}

void GLWidget::resizeGL(int w, int h)
{
	// Update projection matrix and other size related settings:
	// projection.setToIdentity();
	// projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);

	// TODO
}

void GLWidget::paintGL()
{
	glf->glClear(GL_COLOR_BUFFER_BIT);

	// TEST CODE BEGIN --------------------

	static GLfloat const triangleVertices[] = {
		60.0f,  10.0f,  0.0f,
		110.0f, 110.0f, 0.0f,
		10.0f,  110.0f, 0.0f
	};

	QMatrix4x4 mvpMatrix;
	mvpMatrix.ortho(rect());

	// TODO setup vbo, vao etc.

	int vertexLocation = shader->attributeLocation("vertex");
	int matrixLocation = shader->uniformLocation("matrix");
	int colorLocation = shader->uniformLocation("color");
	shader->enableAttributeArray(vertexLocation);
	shader->setAttributeArray(vertexLocation, triangleVertices, 3);
	shader->setUniformValue(matrixLocation, mvpMatrix);
	shader->setUniformValue(colorLocation, QColor(0, 255, 0, 255));
	glDrawArrays(GL_TRIANGLES, 0, 3);
	shader->disableAttributeArray(vertexLocation);

	// TEST CODE END --------------------

	// TODO
}

