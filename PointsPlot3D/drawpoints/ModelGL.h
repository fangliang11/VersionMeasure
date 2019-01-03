///////////////////////////////////////////////////////////////////////////////
// ModelGL.h
// =========
// Model component of OpenGL模型构建
// All OpenGL calls should be here.
//
//  AUTHOR: Fang Liang (fangliang1313@gmail.com)
// CREATED: 2018-12-20
// UPDATED: 2018-12-20
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_GL_H
#define MODEL_GL_H

#ifdef _WIN32
#include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <string>
#include "Matrices.h"
#include "glext.h"
#include "glExtension.h"
#include <vector>
using namespace std;

class ModelGL
{
public:
	ModelGL();
	~ModelGL();

	int AXES_LEN = 5;

	void init();                                    // initialize OpenGL states
	bool initShaders();                             // init shader programs
	void quit();                                    // clean up OpenGL objects
	void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
	void draw();

	void setMousePosition(int x, int y) { mouseX = x; mouseY = y; };
	void setDrawMode(int mode);
	void setWindowSize(int width, int height);
	void setViewMatrix(float x, float y, float z, float pitch, float heading, float roll);
	void setModelMatrix(float x, float y, float z, float rx, float ry, float rz);

	void setCameraX(float x) { cameraPosition[0] = x; updateViewMatrix(); }
	void setCameraY(float y) { cameraPosition[1] = y; updateViewMatrix(); }
	void setCameraZ(float z) { cameraPosition[2] = z; updateViewMatrix(); }
	void setCameraAngleX(float p) { cameraAngle[0] = p; updateViewMatrix(); }
	void setCameraAngleY(float h) { cameraAngle[1] = h; updateViewMatrix(); }
	void setCameraAngleZ(float r) { cameraAngle[2] = r; updateViewMatrix(); }
	float getCameraX() { return cameraPosition[0]; }
	float getCameraY() { return cameraPosition[1]; }
	float getCameraZ() { return cameraPosition[2]; }
	float getCameraAngleX() { return cameraAngle[0]; }
	float getCameraAngleY() { return cameraAngle[1]; }
	float getCameraAngleZ() { return cameraAngle[2]; }

	void setModelX(float x) { modelPosition[0] = x; updateModelMatrix(); }
	void setModelY(float y) { modelPosition[1] = y; updateModelMatrix(); }
	void setModelZ(float z) { modelPosition[2] = z; updateModelMatrix(); }
	void setModelAngleX(float a) { modelAngle[0] = a; updateModelMatrix(); }
	void setModelAngleY(float a) { modelAngle[1] = a; updateModelMatrix(); }
	void setModelAngleZ(float a) { modelAngle[2] = a; updateModelMatrix(); }
	float getModelX() { return modelPosition[0]; }
	float getModelY() { return modelPosition[1]; }
	float getModelZ() { return modelPosition[2]; }
	float getModelAngleX() { return modelAngle[0]; }
	float getModelAngleY() { return modelAngle[1]; }
	float getModelAngleZ() { return modelAngle[2]; }

	// return 16 elements of  target matrix
	const float* getViewMatrixElements() { return matrixView.get(); }
	const float* getModelMatrixElements() { return matrixModel.get(); }
	const float* getModelViewMatrixElements() { return matrixModelView.get(); }
	const float* getProjectionMatrixElements() { return matrixProjection.get(); }

	void rotateCamera(int x, int y);
	void zoomCamera(int dist);
	void zoomCameraDelta(float delta);  // for mousewheel

	bool isShaderSupported() { return glslSupported; }

	GLuint compileshader();
	void buildBuffer(char* filename1, char* filename2, GLuint &VAO, GLuint &VBO, GLuint &EBO, GLuint &texture1, GLuint &texture2);
	//void shaderImage(const char* filename1, const char* filename2);   //创建图片纹理
	void shaderImage();
	void deleteBuffer();
	char *imagename1;
    char *imagename2;


	vector<float> modelCoordinateX;  //  点云图 点 的坐标
	vector<float> modelCoordinateY;
	vector<float> modelCoordinateZ;
	int modelROWNUM;   //点数据的 行数

	bool CTRDRAWFLAG = false;  // opengl重绘标志位
	bool CAMERAFLAG = true;  // 显示/隐藏相机
	bool BUILDIMAGE = false; //纹理着色器
	bool BUILDBUFFER = false;
	int color = 0;
	int backcolor = 0;
	float imagecolor[4];
	float backgroundcolor[4];
	float imagecolorR = 0.0f;
	float imagecolorG = 0.0f;
	float imagecolorB = 1.0f;
	float imagecolorA = 1.0f;
	float backcolorR = 0.0f;
	float backcolorG = 0.0f;
	float backcolorB = 0.0f;
	float backcolorA = 1.0f;


protected:

private:
	// member functions
	void initLights();                              // add a white light ti scene
	void setViewport(int x, int y, int width, int height);
	void setViewportSub(int left, int bottom, int width, int height, float nearPlane, float farPlane);

	//坐标结构体定义
	struct coordpoint {
		float x;
		float y;
		float z;
	};
	void drawGrid(coordpoint& pt1, coordpoint& pt2, coordpoint& pt3, coordpoint& pt4, coordpoint& pt5, coordpoint& pt6, int num);
	void drawAxis(float size);                      // draw 3 axis
	void drawPoints(float pointSize);
	void drawSub1();                                // draw upper window
	void drawSub2();                                // draw bottom window
	void drawFrustum(float fovy, float aspect, float near, float far);
	Matrix4 setFrustum(float l, float r, float b, float t, float n, float f);
	Matrix4 setFrustum(float fovy, float ratio, float n, float f);
	Matrix4 setOrthoFrustum(float l, float r, float b, float t, float n = -1, float f = 1);
	void updateModelMatrix();
	void updateViewMatrix();
	bool createShaderPrograms();
	std::string getShaderStatus(GLuint shader);     // return GLSL compile error log
	std::string getProgramStatus(GLuint program);   // return GLSL link error log

	// members
	int windowWidth;
	int windowHeight;
	int povWidth;           // width for point of view screen (left)
	bool windowSizeChanged;
	bool drawModeChanged;
	int drawMode;
	int mouseX;
	int mouseY;
	float cameraPosition[3];
	float cameraAngle[3];
	float modelPosition[3];
	float modelAngle[3];

	// these are for 3rd person view
	float cameraAngleX;
	float cameraAngleY;
	float cameraDistance;
	float bgColor[4];

	// 4x4 transform matrices
	Matrix4 matrixView;
	Matrix4 matrixModel;
	Matrix4 matrixModelView;
	Matrix4 matrixProjection;

	// glsl extension
	bool glslSupported;
	bool glslReady;
	GLuint progId1;             // shader program with color颜色渲染
	GLuint progId2;             // shader program with color + lighting颜色加灯光渲染
	//GLuint shaderProgram;             // 图片显示着色器
	GLuint shaderID;

	GLuint imagetexture1;
	GLuint imagetexture2;
	GLuint imageareaVAO, imageareaVBO, imageareaEBO;

};
#endif
