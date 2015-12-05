#include "GLWidget.h"

#include "MainWindow.h"

void GLWidget::initializeGL()
{
	// obtain a gl functions object and resolve all entry points
	// in this case we use QOpenGLFunctions_3_3_Core* gl functions
	glf = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
	if (!glf) { qWarning("Could not obtain OpenGL versions object"); exit(1); }
	glf->initializeOpenGLFunctions();

	// print glError messages
	logger = new QOpenGLDebugLogger(this);
	logger->initialize();
	connect(logger, &QOpenGLDebugLogger::messageLogged, this, &GLWidget::printDebugMsg);
	logger->startLogging();

	glf->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// load, compile and link vertex and fragment shaders
	initShaders();

	// initialize vertex buffer object for raw data of the cube defining the volume bounding box
	// the cube vertex positions are interpolated as colors in fragment shader
	// to yield all possible ray volume exit positions to be stored in a texture for ray traversal.
	initVolumeBBoxCubeVBO();

	// load 1D transfer function texture from image
	loadTransferFunction1DTex();

	// create a frame buffer with depth buffer attachment
	// and init empty 2D ray volume exit position texture
	// which will later be generated using the frame buffer
	rayVolumeExitPosMapFramebuffer = new QOpenGLFramebufferObject(this->width(), this->height(), QOpenGLFramebufferObject::Depth);
	initRayVolumeExitPosMap2DTex();

}

void GLWidget::initShaders()
{
	raycastShader = new QOpenGLShaderProgram(QOpenGLContext::currentContext());
	raycastShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "../src/shaders/raycast_shader.vert");
	raycastShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "../src/shaders/raycast_shader.frag");
	raycastShader->link();

	rayVolumeExitPosMapShader = new QOpenGLShaderProgram(QOpenGLContext::currentContext());
	rayVolumeExitPosMapShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "../src/shaders/rayvolumeexitposmap_shader.vert");
	rayVolumeExitPosMapShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "../src/shaders/rayvolumeexitposmap_shader.frag");
	rayVolumeExitPosMapShader->link();

}

void GLWidget::initVolumeBBoxCubeVBO()
{
	// generate vertex array object (vao).
	// subsequent bindings related to vertex buffer data and shader variables
	// are stored by the bound vao, so they can conveniently be reused later.
	volumeBBoxCubeVAO.create();
	volumeBBoxCubeVAO.bind();

	// generate vertex buffer object (vbo) for raw vertex data
	QOpenGLBuffer volumeBBoxCubeVBO(QOpenGLBuffer::VertexBuffer);
	volumeBBoxCubeVBO.create();
	volumeBBoxCubeVBO.bind();
	volumeBBoxCubeVBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
	volumeBBoxCubeVBO.allocate(&cubeVertices[0], 24 * sizeof(GLfloat));

	// generate index buffer object (vbo) listing vertex indices used for drawing
	QOpenGLBuffer volumeBBoxCubeIBO(QOpenGLBuffer::IndexBuffer);
	volumeBBoxCubeIBO.create();
	volumeBBoxCubeIBO.bind();
	volumeBBoxCubeIBO.setUsagePattern(QOpenGLBuffer::StaticDraw);
	volumeBBoxCubeIBO.allocate(&cubeTriangleIndices[0], 36 * sizeof(GLint));

	// bind vertex and index data to shader attributes
	rayVolumeExitPosMapShader->bind();
	int vertexPositionAttribIndex = rayVolumeExitPosMapShader->attributeLocation("vertexPosition");
	rayVolumeExitPosMapShader->enableAttributeArray(vertexPositionAttribIndex); // enable bound vertex buffer at this index
	rayVolumeExitPosMapShader->setAttributeBuffer(vertexPositionAttribIndex, GL_FLOAT, 0, 3); // 3 components x,y,z

	// unbind buffers and shader
	volumeBBoxCubeVAO.release();
	volumeBBoxCubeIBO.release();
	volumeBBoxCubeVBO.release();
	rayVolumeExitPosMapShader->release();

}

void GLWidget::loadTransferFunction1DTex()
{
	// load transfer function 1D texture from image file
	transferFunction1DTex = new QOpenGLTexture(QOpenGLTexture::Target1D);
	transferFunction1DTex->create();
	transferFunction1DTex->setFormat(QOpenGLTexture::RGB8_UNorm);
	transferFunction1DTex->setData(QImage("../transferfunctions/tff_blue.png").convertToFormat(QImage::Format_RGB888));
	transferFunction1DTex->setWrapMode(QOpenGLTexture::Repeat);
	transferFunction1DTex->setMinificationFilter(QOpenGLTexture::Nearest);
	transferFunction1DTex->setMagnificationFilter(QOpenGLTexture::Nearest);
}

void GLWidget::initRayVolumeExitPosMap2DTex()
{
	// init empty 2D texture for volume exit positions of orthogonal rays
	// will be generated later
	rayVolumeExitPosMap2DTex = new QOpenGLTexture(QOpenGLTexture::Target2D);
	rayVolumeExitPosMap2DTex->create();
	rayVolumeExitPosMap2DTex->setFormat(QOpenGLTexture::RGBA8_UNorm);
	// TODO set texture size?
	rayVolumeExitPosMap2DTex->setWrapMode(QOpenGLTexture::Repeat);
	rayVolumeExitPosMap2DTex->setMinificationFilter(QOpenGLTexture::Nearest);
	rayVolumeExitPosMap2DTex->setMagnificationFilter(QOpenGLTexture::Nearest);
}

void GLWidget::loadVolume3DTex()
{
	// fill volumeData into a 3D texture
	volume3DTex = new QOpenGLTexture(QOpenGLTexture::Target3D);
	volume3DTex->create();
	volume3DTex->setFormat(QOpenGLTexture::R32F);
	volume3DTex->setWrapMode(QOpenGLTexture::Repeat);
	volume3DTex->setMinificationFilter(QOpenGLTexture::Linear); // this is trilinear interpolation
	volume3DTex->setMagnificationFilter(QOpenGLTexture::Linear);

	// TODO pass volume data to texture (maybe use plain OpenGL functions)
	//volume3DTex->setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, data);

	//	int emptyVoxelCount = 0;
	//	if (volumeData != nullptr) {

	//		voxels.clear();

	//		for (int i = 0; i < volumeData->width(); ++i) {
	//			for (int j = 0; j < volumeData->height(); ++j) {
	//				for (int k = 0; k < volumeData->depth(); ++k) {
	//					if (volumeData->voxel(i, j, k).getValue() == 0) {
	//						++emptyVoxelCount;
	//						continue;
	//					}
	//					voxels.push_back(-volumeData->width()/2 + i);
	//					voxels.push_back(-volumeData->height()/2 + j);
	//					voxels.push_back(-volumeData->depth()/2 + k);
	//					voxels.push_back(volumeData->voxel(i, j, k).getValue());
	//				}
	//			}
	//		}
	//	}

	//	qWarning("volume data loaded: %d / %d voxels (skipped %d empty)", int(voxels.size()/4.0f), volumeData->size(), emptyVoxelCount);
}

void GLWidget::dataLoaded(Volume *volumeData)
{
	this->volumeData = volumeData;
	loadVolume3DTex();

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

void GLWidget::paintGL()
{
	glf->glClearColor(backgroundColor.red()/256.0f, backgroundColor.green()/256.0f, backgroundColor.blue()/256.0f, 1.0f);
	glf->glClear(GL_COLOR_BUFFER_BIT);

	if (!volumeData) {
		return;
	}

	glf->glEnable(GL_DEPTH_TEST);

	///////////////////////////////////////////////////////////////////////////////
	// FIRST PASS
	// generate ray volume exit position map later used to construct rays
	///////////////////////////////////////////////////////////////////////////////

	rayVolumeExitPosMapFramebuffer->bind();
	glf->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rayVolumeExitPosMap2DTex->textureId(), 0); // TODO does this work?

	rayVolumeExitPosMapShader->bind();

	// draw volume cube back faces (front face culling enabled)
	// rayVolumeExitPosMapShader stores interpolated back face (ray exit) positions in texture
	drawVolumeBBoxCube(GL_FRONT, rayVolumeExitPosMapShader);

	///////////////////////////////////////////////////////////////////////////////
	// SECOND PASS
	// calculate ray volume entry positions and together with exit position map
	// do raycasting from entry to exit position of each fragment
	///////////////////////////////////////////////////////////////////////////////

	QOpenGLFramebufferObject::bindDefault();

	raycastShader->bind();
	raycastShader->setUniformValue("screenDimensions", QVector2D(this->width(), this->height()));
	raycastShader->setUniformValue("rayStepSize", 0.001f);
	raycastShader->setUniformValue("transferFunction", 0); // bind shader uniform to texture unit 0
	transferFunction1DTex->bind(0); // bind texture to texture unit 0
	raycastShader->setUniformValue("exitPositions", 1);
	rayVolumeExitPosMap2DTex->bind(1);
	raycastShader->setUniformValue("volume", 2);
	volume3DTex->bind(2);

	// draw volume cube front faces (back face culling enabled)
	// raycastShader then uses interpolated front face (ray entry) positions with exit positions from first pass
	// to cast rays through the volume texture.
	// raycasting determines pixel intensity by sampling the volume voxel intensities
	// and mapping desired values to colors via the transfer function
	drawVolumeBBoxCube(GL_BACK, raycastShader);

	/*/ DEBUG VIEW FIRST PASS TEXTURE
	// blit framebuffer from first pass to default framebuffer
	// blit = bit block image transfer, combine bitmaps via boolean operation
	if (!QOpenGLFramebufferObject::hasOpenGLFramebufferBlit()) qWarning() << "Frame Buffer blitting not supported by local OpenGL implementation.";
	QOpenGLFramebufferObject::blitFramebuffer(0, rayVolumeExitPosMapFramebuffer); */

}

void GLWidget::drawVolumeBBoxCube(GLenum glFaceCullMode, QOpenGLShaderProgram *shader)
{
	glf->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelMat.setToIdentity();
	modelMat.rotate(90.0f, QVector3D(1.0f, 0.0f, 0.0f)); // TODO do we really need to rotate the volume?
	modelMat.translate(QVector3D(-0.5f, -0.5f, -0.5f)); // move volume bounding box cube to center
	viewMat.setToIdentity();
//	QMatrix4x4 cameraModelMat;
//	cameraModelMat.rotate(10, 0, 0.2, 0.2);
//	cameraModelMat.translate(0, 0, 100);
//	viewMat = cameraModelMat.inverted();
	viewMat.lookAt(QVector3D(0.0f, 0.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)); // move camera slightly back and look at center
	projMat.setToIdentity();
	projMat.perspective(60.0f, this->width()/this->height(), 0.1f, 400.f);

	shader->bind();
	int mvpMatUniformIndex = shader->uniformLocation("modelViewProjMat");
	shader->setUniformValue(mvpMatUniformIndex, projMat * viewMat * modelMat);

	glf->glEnable(GL_CULL_FACE);
	glf->glCullFace(glFaceCullMode);
	volumeBBoxCubeVAO.bind();
	glf->glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)NULL);
	glf->glDisable(GL_CULL_FACE);

}

void GLWidget::resizeGL(int w, int h)
{
	// TODO

	rayVolumeExitPosMapFramebuffer = new QOpenGLFramebufferObject(w, h, QOpenGLFramebufferObject::Depth);

}

void GLWidget::setBackgroundColor(int intensity)
{	
	backgroundColor = QColor(intensity, intensity, intensity);
	repaint();
}

