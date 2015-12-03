#include "GLWidget.h"

#include "MainWindow.h"

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

	// print glError messages
	logger = new QOpenGLDebugLogger(this);
	logger->initialize();
	connect(logger, &QOpenGLDebugLogger::messageLogged, this, &GLWidget::printDebugMsg);
	logger->startLogging();

	glf->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glf->glEnable(GL_BLEND);
	glf->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// load, compile and link vertex and fragment shaders
	shader = new QOpenGLShaderProgram(QOpenGLContext::currentContext());
	shader->addShaderFromSourceFile(QOpenGLShader::Vertex, "../src/shaders/pointshader.vert");
	shader->addShaderFromSourceFile(QOpenGLShader::Fragment, "../src/shaders/pointshader.frag");
	shader->link();
	shader->bind();

	// init view matrix (inverse camera model matrix) and projection matrix
	QMatrix4x4 cameraModelMat;
	cameraModelMat.rotate(10, 0, 0.2, 0.2);
	cameraModelMat.translate(0, 0, 100);
	viewMat = cameraModelMat.inverted();
	projMat.perspective(90.0, this->width()/this->height(), 1.0, 500.0);


	// generate vertex array object (vao).
	// subsequent bindings related to vertex buffer data and shader variables
	// are stored by the bound vao, so they can conveniently be reused later.
	vao.create();
	vao.bind();

	// generate vertex buffer object (vbo) for raw vertex data
	vbo.create();
	vbo.bind();
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo.allocate(&vertices[0], vertices.size() * sizeof(float));

	// enable shader attributes at given indices to supply vertex data to them
	// the order / layout of the shader attribute are defined in the shader source file
	int vertexDataAttribIndex     = shader->attributeLocation("vertexData");
	int modelMatUniformIndex    = shader->uniformLocation("modelMat");
	int viewMatUniformIndex		= shader->uniformLocation("viewMat");
	int projMatUniformIndex		= shader->uniformLocation("projMat");
	int colorUniformIndex       = shader->uniformLocation("color");
	shader->enableAttributeArray(vertexDataAttribIndex); // attribute to receive vertex data
	shader->setAttributeBuffer(vertexDataAttribIndex, GL_FLOAT, 0, 4); // use 4 components (x,y,z, intensity) per vertex
	shader->setUniformValue(modelMatUniformIndex, modelMat);
	shader->setUniformValue(viewMatUniformIndex, viewMat);
	shader->setUniformValue(projMatUniformIndex, projMat);
	shader->setUniformValue(colorUniformIndex, QColor(0, 150, 255, 255));

	// unbind buffers
	vao.release();
	vbo.release();
	shader->release();


}

void GLWidget::resizeGL(int w, int h)
{
	// update projection matrix
	shader->bind();
	vao.bind();
	projMat.setToIdentity();
	projMat.perspective(90.0, w/h, 1.0, 500.0);
	shader->setUniformValue(shader->uniformLocation("projMat"), projMat);
	vao.release();
	shader->release();


}

void GLWidget::paintGL()
{
	glf->glClear(GL_COLOR_BUFFER_BIT);

	shader->bind();
	vao.bind();
	glf->glEnable(GL_VERTEX_PROGRAM_POINT_SIZE); // size is set by vertex shader depending on distance
	//glf->glPointSize(10);
	glf->glDrawArrays(GL_POINTS, 0, vertices.size());
	vao.release();
	shader->release();

	// TODO
}

void GLWidget::dataLoaded(Volume *volumeData)
{
	// get volume data
	this->volumeData = volumeData;

	int emptyVoxelCount = 0;
	if (volumeData != nullptr) {

		vertices.clear();

		for (int i = 0; i < volumeData->width(); ++i) {
			for (int j = 0; j < volumeData->height(); ++j) {
				for (int k = 0; k < volumeData->depth(); ++k) {
					if (volumeData->voxel(i, j, k).getValue() == 0) {
						++emptyVoxelCount;
						continue;
					}
					vertices.push_back(-volumeData->width()/2 + i);
					vertices.push_back(-volumeData->height()/2 + j);
					vertices.push_back(-volumeData->depth()/2 + k);
					vertices.push_back(volumeData->voxel(i, j, k).getValue());
				}
			}
		}
	}

	qWarning("volume data loaded: %d / %d voxels (skipped %d empty)", vertices.size()/4, volumeData->width()*volumeData->height()*volumeData->depth(), emptyVoxelCount);

	shader->bind();
	vao.bind();

	vbo.bind();
	vbo.allocate(&vertices[0], vertices.size() * sizeof(float));

	// enable shader attributes at given indices to supply vertex data to them
	// the order / layout of the shader attribute are defined in the shader source file
	int vertexDataAttribIndex   = shader->attributeLocation("vertexData");
	shader->enableAttributeArray(vertexDataAttribIndex); // attribute to receive vertex data
	shader->setAttributeBuffer(vertexDataAttribIndex, GL_FLOAT, 0, 4); // use 4 components (x,y,z, intensity) per vertex

	// unbind buffers
	vao.release();
	vbo.release();
	shader->release();
}

void GLWidget::setFoV(int angle)
{
	// update projection matrix
	shader->bind();
	vao.bind();
	projMat.setToIdentity();
	projMat.perspective(angle, this->width()/this->height(), 1.0, 500.0);
	shader->setUniformValue(shader->uniformLocation("projMat"), projMat);
	vao.release();
	shader->release();

	repaint();
}

