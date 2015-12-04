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

	//glf->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glf->glEnable(GL_BLEND);
	glf->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// load, compile and link vertex and fragment shaders
	initShaders();

	// load 1D transfer function texture,
	// generate 2D ray volume exit position texture,
	// and fill volume data into a 3D texture
	initTextures();

	// init view matrix (inverse camera model matrix) and projection matrix
	QMatrix4x4 cameraModelMat;
	cameraModelMat.rotate(10, 0, 0.2, 0.2);
	cameraModelMat.translate(0, 0, 100);
	viewMat = cameraModelMat.inverted();
	projMat.perspective(90.0, this->width()/this->height(), 1.0, 500.0); // TODO change to orthographic

	// initialize vertex buffer object for raw data of the cube defining the volume bounding box
	// the cube vertex positions are interpolated as colors in fragment shader
	// to yield all possible ray volume exit positions to be stored in a texture for ray traversal.
	initVolumeCubeBBoxVBO(); // TODO


//	// generate vertex array object (vao).
//	// subsequent bindings related to vertex buffer data and shader variables
//	// are stored by the bound vao, so they can conveniently be reused later.
//	vao.create();
//	vao.bind();

//	// generate vertex buffer object (vbo) for raw vertex data
//	vbo.create();
//	vbo.bind();
//	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
//	vbo.allocate(&vertices[0], vertices.size() * sizeof(float));

//	// enable shader attributes at given indices to supply vertex data to them
//	// the order / layout of the shader attribute are defined in the shader source file
//	int vertexDataAttribIndex     = raycastShader->attributeLocation("vertexData");
//	int modelMatUniformIndex    = raycastShader->uniformLocation("modelMat");
//	int viewMatUniformIndex		= raycastShader->uniformLocation("viewMat");
//	int projMatUniformIndex		= raycastShader->uniformLocation("projMat");
//	int colorUniformIndex       = raycastShader->uniformLocation("color");
//	raycastShader->enableAttributeArray(vertexDataAttribIndex); // attribute to receive vertex data
//	raycastShader->setAttributeBuffer(vertexDataAttribIndex, GL_FLOAT, 0, 4); // use 4 components (x,y,z, intensity) per vertex
//	raycastShader->setUniformValue(modelMatUniformIndex, modelMat);
//	raycastShader->setUniformValue(viewMatUniformIndex, viewMat);
//	raycastShader->setUniformValue(projMatUniformIndex, projMat);
//	raycastShader->setUniformValue(colorUniformIndex, QColor(0, 150, 255, 255));

//	// unbind buffers
//	vao.release();
//	vbo.release();
//	raycastShader->release();


}

void GLWidget::resizeGL(int w, int h)
{


}

void GLWidget::paintGL()
{
	glf->glClearColor(backgroundColor.red()/256.0f, backgroundColor.green()/256.0f, backgroundColor.blue()/256.0f, 1.0f);
	glf->glClear(GL_COLOR_BUFFER_BIT);

//	raycastShader->bind();
//	vao.bind();
//	glf->glEnable(GL_VERTEX_PROGRAM_POINT_SIZE); // size is set by vertex shader depending on distance
//	//glf->glPointSize(10);
//	glf->glDrawArrays(GL_POINTS, 0, vertices.size());
//	vao.release();
//	raycastShader->release();

	// TODO
}

void GLWidget::initShaders()
{
	raycastShader = new QOpenGLShaderProgram(QOpenGLContext::currentContext());
	raycastShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "../src/shaders/raycast_shader.vert");
	raycastShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "../src/shaders/raycast_shader.frag");
	raycastShader->link();
	raycastShader->bind();

	rayVolumeExitPosMapShader = new QOpenGLShaderProgram(QOpenGLContext::currentContext());
	rayVolumeExitPosMapShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "../src/shaders/rayvolumeexitposmap_shader.vert");
	rayVolumeExitPosMapShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "../src/shaders/rayvolumeexitposmap_shader.frag");
	rayVolumeExitPosMapShader->link();
	rayVolumeExitPosMapShader->bind();

}

void GLWidget::initTextures()
{
	// load transfer function 1D texture
	transferFunction1DTex = new QOpenGLTexture(QOpenGLTexture::Target1D);
	transferFunction1DTex->create();
	transferFunction1DTex->setFormat(QOpenGLTexture::RGB8_UNorm);
	transferFunction1DTex->setData(QImage("../transferfunctions/tff_blue.png").convertToFormat(QImage::Format_RGB888));
	transferFunction1DTex->setWrapMode(QOpenGLTexture::Repeat);
	transferFunction1DTex->setMinificationFilter(QOpenGLTexture::Nearest);
	transferFunction1DTex->setMagnificationFilter(QOpenGLTexture::Nearest);

	// generate 2D texture for volume exit positions of orthogonal rays
	rayVolumeExitPosMap2DTex = new QOpenGLTexture(QOpenGLTexture::Target2D);
	rayVolumeExitPosMap2DTex->create();
	rayVolumeExitPosMap2DTex->setFormat(QOpenGLTexture::RGBA8_UNorm);
	rayVolumeExitPosMap2DTex->setWrapMode(QOpenGLTexture::Repeat);
	rayVolumeExitPosMap2DTex->setMinificationFilter(QOpenGLTexture::Nearest);
	rayVolumeExitPosMap2DTex->setMagnificationFilter(QOpenGLTexture::Nearest);
	// TODO use rayVolumeExitPosMapShader to generate texture

	// fill volumeData into a 3D texture
	volume3DTex = new QOpenGLTexture(QOpenGLTexture::Target3D);
	volume3DTex->create();
	volume3DTex->setFormat(QOpenGLTexture::R32F);
	volume3DTex->setWrapMode(QOpenGLTexture::Repeat);
	volume3DTex->setMinificationFilter(QOpenGLTexture::Linear); // this is trilinear interpolation
	volume3DTex->setMagnificationFilter(QOpenGLTexture::Linear);
	// TODO pass volume data to texture (maybe use plain OpenGL functions)
	//volume3DTex->setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, data);

}

void GLWidget::initVolumeCubeBBoxVBO()
{

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

	qWarning("volume data loaded: %d / %d voxels (skipped %d empty)", int(vertices.size()/4.0f), volumeData->size(), emptyVoxelCount);

//	raycastShader->bind();
//	vao.bind();

//	vbo.bind();
//	vbo.allocate(&vertices[0], vertices.size() * sizeof(float));

//	// enable shader attributes at given indices to supply vertex data to them
//	// the order / layout of the shader attribute are defined in the shader source file
//	int vertexDataAttribIndex   = raycastShader->attributeLocation("vertexData");
//	raycastShader->enableAttributeArray(vertexDataAttribIndex); // attribute to receive vertex data
//	raycastShader->setAttributeBuffer(vertexDataAttribIndex, GL_FLOAT, 0, 4); // use 4 components (x,y,z, intensity) per vertex

//	// unbind buffers
//	vao.release();
//	vbo.release();
//	raycastShader->release();
}

void GLWidget::setBackgroundColor(int intensity)
{	
	backgroundColor = QColor(intensity, intensity, intensity);
	repaint();
}

