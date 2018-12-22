///////////////////////////////////////////////////////////////////////////////
// ModelGL.cpp
// ===========
// Model component of OpenGL
// All OpenGL calls should be here.
//
//  AUTHOR: Fang Liang (fangliang1313@gmail.com)
// CREATED: 2018-12-20
// UPDATED: 2018-12-20
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cmath>
#include "Window.h"
#include "ModelGL.h"
#include "ViewGL.h"
#include "ViewFormGL.h"
#include "teapot.h"             // 3D mesh of teapot
#include "cameraSimple.h"       // 3D mesh of camera
//#include "readFile.h"

// constants
const float DEG2RAD = 3.141593f / 180;
const float FOV_Y = 60.0f;              // vertical FOV in degree
const float NEAR_PLANE = 1.0f;
const float FAR_PLANE = 100.0f;
const float CAMERA_ANGLE_X = 45.0f;     // pitch in degree
const float CAMERA_ANGLE_Y = -45.0f;    // heading in degree
const float CAMERA_DISTANCE = 25.0f;    // camera distance



// flat shading ===========================================平面着色
const char* vsSource1 = R"(
void main()
{
    gl_FrontColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
)";
const char* fsSource1 = R"(
void main()
{
    gl_FragColor = gl_Color;
}
)";


// blinn specular shading =================================模拟金属镜面着色
const char* vsSource2 = R"(
varying vec3 esVertex, esNormal;
void main()
{
    esVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
    esNormal = gl_NormalMatrix * gl_Normal;
    gl_FrontColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
)";
const char* fsSource2 = R"(
varying vec3 esVertex, esNormal;
void main()
{
    vec3 normal = normalize(esNormal);
    vec3 view = normalize(-esVertex);
    vec3 light;
    if(gl_LightSource[0].position.w == 0.0)
    {
        light = normalize(gl_LightSource[0].position.xyz);
    }
    else
    {
        light = normalize(gl_LightSource[0].position.xyz - esVertex);
    }
    vec3 halfVec = normalize(light + view);
    vec4 color =  gl_FrontMaterial.ambient * gl_FrontLightProduct[0].ambient;
    float dotNL = max(dot(normal, light), 0.0);
    color += gl_FrontMaterial.diffuse * gl_FrontLightProduct[0].diffuse * dotNL;
    float dotNH = max(dot(normal, halfVec), 0.0);
    /*vec4 specular = (vec4(1.0) - color) * gl_FrontMaterial.specular * gl_FrontLightProduct[0].specular * pow(dotNH, gl_FrontMaterial.shininess);
    color += specular;*/
    color += gl_FrontMaterial.specular * gl_FrontLightProduct[0].specular * pow(dotNH, gl_FrontMaterial.shininess);
    gl_FragColor = color;
}
)";



///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
ModelGL::ModelGL() : windowWidth(0), windowHeight(0), povWidth(0),
                     drawModeChanged(false), drawMode(0),
                     cameraAngleX(CAMERA_ANGLE_X), cameraAngleY(CAMERA_ANGLE_Y),
                     cameraDistance(CAMERA_DISTANCE), windowSizeChanged(false),
                     glslSupported(false), glslReady(false), progId1(0), progId2(0)
{
    cameraPosition[0] = cameraPosition[1] = cameraPosition[2] = 0;
    cameraAngle[0] = cameraAngle[1] = cameraAngle[2] = 0;
    modelPosition[0] = modelPosition[1] = modelPosition[2] = 0;
    modelAngle[0] = modelAngle[1] = modelAngle[2] = 0;
    bgColor[0] = bgColor[1] = bgColor[2] = bgColor[3] = 0;

    matrixView.identity();
    matrixModel.identity();
    matrixModelView.identity();
    matrixProjection.identity();
}



///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////
ModelGL::~ModelGL()
{
}



///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL states and scene
///////////////////////////////////////////////////////////////////////////////

void ModelGL::init()
{
    glShadeModel(GL_SMOOTH);                        // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);          // 4-byte pixel alignment

    // enable/disable features设置特效
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   //指定颜色和纹理坐标的差值质量 | 选择最高质量选项
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);   //当我们需要绘制透明图片时，就需要关闭它,并且打开混合
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);   //开启和关闭光照计算
    glEnable(GL_TEXTURE_2D);  //激活纹理单元
    glEnable(GL_CULL_FACE);   //激活面剔除
    glEnable(GL_BLEND);       //激活混合
    glEnable(GL_SCISSOR_TEST);  //激活多视口

     // track material ambient and diffuse(环境和漫反射) from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    //glEnable(GL_COLOR_MATERIAL);

	//glClearColor(1, 1, 1, 0);
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);   // 背景颜色
    glClearStencil(0);                              // clear stencil buffer指明模板缓冲区的清理值
    glClearDepth(1.0f);                             // 0 is near, 1 is far指明深度缓冲区的清理值
    glDepthFunc(GL_LEQUAL);    //如果目标像素z值<＝当前像素z值，则绘制目标像素

    initLights();   //光源初始化
}

///////////////////////////////////////////////////////////////////////////////
// initialize GLSL programs着色器初始化
// NOTE:shader programs can be shared among multiple contexts, create only once
///////////////////////////////////////////////////////////////////////////////
bool ModelGL::initShaders()
{
    if(!glslReady)
    {
        // check extensions
        glExtension& extension = glExtension::getInstance();
        glslSupported = extension.isSupported("GL_ARB_shader_objects");
        if(glslSupported)
            glslReady = createShaderPrograms();
    }
    return glslReady;
}

///////////////////////////////////////////////////////////////////////////////
// clean up OpenGL objects
///////////////////////////////////////////////////////////////////////////////
void ModelGL::quit()
{
}

///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void ModelGL::initLights()
{
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = {.3f, .3f, .3f, 1.0f};      // ambient light
    GLfloat lightKd[] = {.8f, .8f, .8f, 1.0f};      // diffuse light
    GLfloat lightKs[] = {1, 1, 1, 1};               // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light in eye space
    float lightPos[4] = {1, 1, 1, 0};               // directional light  FANG
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                            // MUST enable each light source after configuration
}

///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
    float forward[4];
    float up[4];
    float left[4];
    float position[4];
    float invLength;

    // determine forward vector (direction reversed because it is camera)
    forward[0] = posX - targetX;    // x
    forward[1] = posY - targetY;    // y
    forward[2] = posZ - targetZ;    // z
    forward[3] = 0.0f;              // w
    // normalize it without w-component
    invLength = 1.0f / sqrtf(forward[0]*forward[0] + forward[1]*forward[1] + forward[2]*forward[2]);
    forward[0] *= invLength;
    forward[1] *= invLength;
    forward[2] *= invLength;

    // assume up direction is straight up
    up[0] = 0.0f;   // x
    up[1] = 1.0f;   // y
    up[2] = 0.0f;   // z
    up[3] = 0.0f;   // w

    // compute left vector with cross product
    left[0] = up[1]*forward[2] - up[2]*forward[1];  // x
    left[1] = up[2]*forward[0] - up[0]*forward[2];  // y
    left[2] = up[0]*forward[1] - up[1]*forward[0];  // z
    left[3] = 1.0f;                                 // w

    // re-compute orthogonal up vector
    up[0] = forward[1]*left[2] - forward[2]*left[1];    // x
    up[1] = forward[2]*left[0] - forward[0]*left[2];    // y
    up[2] = forward[0]*left[1] - forward[1]*left[0];    // z
    up[3] = 0.0f;                                       // w

    // camera position
    position[0] = -posX;
    position[1] = -posY;
    position[2] = -posZ;
    position[3] = 1.0f;

    // copy axis vectors to matrix
    matrixView.identity();
    matrixView.setColumn(0, left);
    matrixView.setColumn(1, up);
    matrixView.setColumn(2, forward);
    matrixView.setColumn(3, position);
}

///////////////////////////////////////////////////////////////////////////////
// set rendering window size
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setWindowSize(int width, int height)
{
    // assign the width/height of viewport
    windowWidth = width;
    windowHeight = height;

    // compute dim for point of view screen
    povWidth = windowWidth / 2;
    if(povWidth > windowHeight)
    {
        // if it is wider than height, reduce to the height (make it square)
        povWidth = windowHeight;
    }

    windowSizeChanged = true;
}

///////////////////////////////////////////////////////////////////////////////
// configure projection and viewport设置透视和视口
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setViewport(int x, int y, int w, int h)
{
    // set viewport to be the entire window
    glViewport((GLsizei)x, (GLsizei)y, (GLsizei)w, (GLsizei)h);

    // set perspective viewing frustum
    Matrix4 matrix = setFrustum(FOV_Y, (float)(w)/h, NEAR_PLANE, FAR_PLANE); // FOV, AspectRatio, NearClip, FarClip

    // copy projection matrix to OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matrix.get());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// configure projection and viewport of sub window设置子窗口的透视和视口
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setViewportSub(int x, int y, int width, int height, float nearPlane, float farPlane)
{
    // set viewport
    glViewport(x, y, width, height);
    glScissor(x, y, width, height);  //定义裁剪窗口

    // set perspective viewing frustum
    Matrix4 matrix = setFrustum(FOV_Y, (float)(width)/height, nearPlane, farPlane); // FOV, AspectRatio, NearClip, FarClip

    // copy projection matrix to OpenGL
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matrix.get());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// draw 2D/3D scene
///////////////////////////////////////////////////////////////////////////////
void ModelGL::draw()
{
    drawSub1();  //左视口
    drawSub2();  //右视口

    // post frame
    if(windowSizeChanged)
    {
        setViewport(0, 0, windowWidth, windowHeight);
        windowSizeChanged = false;
    }

    if(drawModeChanged)
    {
        if(drawMode == 0)           // fill mode填充模式
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        else if(drawMode == 1)      // wireframe mode线框模式
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else if(drawMode == 2)      // point mode点阵模式
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            //glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        drawModeChanged = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
// draw left window (view from the camera) 绘制左侧窗口————二维平面图显示
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawSub1()
{
    // clear buffer (whole area)
    setViewportSub(0, 0, windowWidth, windowHeight, 1, 10);
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // make left viewport square viewport
    if(windowHeight > povWidth)
        setViewportSub(0, (windowHeight - povWidth)/2, povWidth, povWidth, 1, 10);
    else
        setViewportSub((povWidth - windowHeight)/2, 0, windowHeight, windowHeight, 1, 10);
        //setViewportSub((halfWidth - windowHeight)/2, 0, windowHeight, windowHeight, 1, 10);

    // clear buffer (square area)
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glPushMatrix();

    // set view matrix ========================================================
    // copy the matrix to OpenGL GL_MODELVIEW matrix
    // See updateViewMatrix() how matrixView is constructed. The equivalent
    // OpenGL calls are;
    //    glLoadIdentity();
    //    glRotatef(-cameraAngle[2], 0, 0, 1); // roll (Z)
    //    glRotatef(-cameraAngle[1], 0, 1, 0); // heading (Y)
    //    glRotatef(-cameraAngle[0], 1, 0, 0); // pitch (X)
    //    glTranslatef(-cameraPosition[0], -cameraPosition[1], -cameraPosition[2]);
    glLoadMatrixf(matrixView.get());

    // always draw the grid at the origin (before any modeling transform)
    //drawGrid(10, 1);

    // transform objects ======================================================
    // From now, all transform will be for modeling matrix only.
    // (from object space to world space)
    // See updateModelMatrix() how matrixModel is constructed. The equivalent
    // OpenGL calls are;
    //    glLoadIdentity();
    //    glTranslatef(modelPosition[0], modelPosition[1], modelPosition[2]);
    //    glRotatef(modelAngle[0], 1, 0, 0);
    //    glRotatef(modelAngle[1], 0, 1, 0);
    //    glRotatef(modelAngle[2], 0, 0, 1);

    // compute GL_MODELVIEW matrix by multiplying matrixView and matrixModel
    // before drawing the object:
    // ModelView_M = View_M * Model_M
    // This modelview matrix transforms the objects from object space to eye space.
    //glLoadMatrixf(matrixModelView.get());

    // draw a teapot and axis after ModelView transform
    // v' = Mmv * v
    //drawAxis(4);

    if(glslReady)
    {
        // use GLSL
        glUseProgram(progId2);
        glDisable(GL_COLOR_MATERIAL);
        //drawTeapot();

		GLUquadricObj *objCylinder = gluNewQuadric(); //创建二次曲面对象——-圆柱
		glTranslatef(0.0, 0.0, 0.0);
		gluCylinder(objCylinder, 1.0, 0.5, 3, 10, 5);

        glEnable(GL_COLOR_MATERIAL);
        glUseProgram(0);
    }
    else
    {
		MessageBox(NULL, TEXT("渲染失败：glslReady is false!"), TEXT("错误"), 0);
	}

    glPopMatrix();
}

///////////////////////////////////////////////////////////////////////////////
// draw right window (3rd person view)  绘制右侧窗口------三维点云图显示
///////////////////////////////////////////////////////////////////////////////
//int AXES_LEN = 5;
void ModelGL::drawSub2()
{

    // set right viewport
    setViewportSub(povWidth, 0, windowWidth-povWidth, windowHeight, NEAR_PLANE, FAR_PLANE);

    // it is done in drawSub1(), no need to clear buffer
    //glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);   // background color
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glPushMatrix();

    // First, transform the camera (viewing matrix) from world space to eye space
    Matrix4 matView, matModel, matModelView;
    matView.identity();
    matView.rotateY(cameraAngleY);
    matView.rotateX(cameraAngleX);
    matView.translate(0, 0, -cameraDistance);
    glLoadMatrixf(matView.get());

    // transform
    matModel.rotateZ(modelAngle[2]);
    matModel.rotateY(modelAngle[1]);
    matModel.rotateX(modelAngle[0]);
    matModel.translate(modelPosition[0], modelPosition[1], modelPosition[2]);
    matModelView = matView * matModel;
    glLoadMatrixf(matModelView.get());

	// 绘制网格
	coordpoint cpoint1 = { AXES_LEN, AXES_LEN, 0 };
	coordpoint cpoint2 = { -AXES_LEN, -AXES_LEN, 0 };
	coordpoint cpoint3 = { AXES_LEN, AXES_LEN, 0 };
	coordpoint cpoint4 = { -AXES_LEN, -AXES_LEN, 0 };
	coordpoint cpoint5 = { AXES_LEN, AXES_LEN, 0 };
	coordpoint cpoint6 = { -AXES_LEN, -AXES_LEN, 0 };
	drawGrid(cpoint1, cpoint2, cpoint3, cpoint4, cpoint5, cpoint6, 20);

    // 绘制坐标轴
    drawAxis(5);

	//渲染条件具备，绘制点云
    if(glslReady)
    {
        glUseProgram(progId2);
        //glDisable(GL_COLOR_MATERIAL);
		glColor4f(0.0f, 0.0f, 0.8f, 1.0f); //蓝色

		if (CTRDRAWFLAG ) {

			drawPoints(3);   //增加点云
		}

        glEnable(GL_COLOR_MATERIAL);
        glUseProgram(0);    }
    else
    {
		MessageBox(NULL, TEXT("渲染失败：glslReady is false!"), TEXT("错误"), 0);
    }

    //// draw camera axis绘制相机坐标系
    //matModel.identity();
    //matModel.rotateY(180);  // facing to -Z axis
    //matModel.rotateZ(-cameraAngle[2]);
    //matModel.rotateY(cameraAngle[1]);
    //matModel.rotateX(-cameraAngle[0]);
    //matModel.translate(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
    //matModelView = matView * matModel;
    //glLoadMatrixf(matModelView.get());
    //drawAxis(0.75f);

    // transform camera object移动摄像物体
    matModel.identity();
    matModel.rotateZ(-cameraAngle[2]);
    matModel.rotateY(cameraAngle[1]);
    matModel.rotateX(-cameraAngle[0]);
    matModel.translate(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
    matModelView = matView * matModel;
    glLoadMatrixf(matModelView.get());

    //// draw the camera绘制相机和锥形显示区
	if (CAMERAFLAG) {

		drawCamera();
		drawFrustum(FOV_Y, 1, 1, 10);
	}

    glPopMatrix();
}

///////////////////////////////////////////////////////////////////////////////
//生成点云
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawPoints(float pointSize) {

	//glDisable(GL_LIGHTING);
	//glDisable(GL_CULL_FACE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //混合函数(混合因子)

	glPointSize(pointSize);
	//glPushMatrix();
	glBegin(GL_POINTS);
	for (int i = 0; i < modelROWNUM; i++) {
		//glColor4f(0.0f, 0.0f, 0.5*modelCoordinateZ[i] + 0.1, 1.0f);
		glVertex3f(modelCoordinateX[i], modelCoordinateY[i], 1 - modelCoordinateZ[i]);
	}
	glEnd();
	//glPopMatrix();

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_LIGHTING);
}

///////////////////////////////////////////////////////////////////////////////
// draw a grid on the xz plane
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawGrid(coordpoint& pt1, coordpoint& pt2, coordpoint& pt3, coordpoint& pt4, coordpoint& pt5, coordpoint& pt6, int num) {
	const float _xLen = (pt2.x - pt1.x) / num;
	const float _yLen = (pt2.y - pt1.y) / num;
	const float _zLen = (pt2.z - pt1.z) / num;
	glLineWidth(1.0f);
	glLineStipple(1, 0x0303);//线条样式

	int xi1 = 0, xi2 = 0, xi3 = 0;
	int yi1 = 0, yi2 = 0, yi3 = 0;
	int zi1 = 0, zi2 = 0, zi3 = 0;

	float colorLine1[4] = { 0.3f, 0.1f, 0.1f, 0.5f };
	float colorLine2[4] = { 0.1f, 0.4f, 0.1f, 0.5f };
	float colorLine3[4] = { 0.1f, 0.1f, 0.4f, 0.5f };

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/////////////////////////////////////////////////point1  point2
	glPushMatrix();
	//glColor4f(0.9f, 0.9f, 0.9f, 0.4f); //白色网格
	glColor4fv(colorLine2);
	glTranslatef(0, 0, -5);
	glBegin(GL_LINES);
	glEnable(GL_LINE_SMOOTH); //抗锯齿
	//绘制平行于X的直线
	for (zi1 = 0; zi1 <= num; zi1++) {
		float z = _zLen * zi1 + pt1.z;
		for (yi1 = 0; yi1 <= num; yi1++) {
			float y = _yLen * yi1 + pt1.y;
			glVertex3f(pt1.x, y, z);
			glVertex3f(pt2.x, y, z);
		}
	}
	//绘制平行于Y的直线
	for (zi1 = 0; zi1 <= num; zi1++) {
		float z = _zLen * zi1 + pt1.z;
		for (xi1 = 0; xi1 <= num; xi1++) {
			float x = _xLen * xi1 + pt1.x;
			glVertex3f(x, pt1.y, z);
			glVertex3f(x, pt2.y, z);
		}
	}
	//绘制平行于Z的直线
	for (yi1 = 0; yi1 <= num; yi1++) {
		float y = _yLen * yi1 + pt1.y;
		for (xi1 = 0; xi1 <= num; xi1++) {
			float x = _xLen * xi1 + pt1.x;
			glVertex3f(x, y, pt1.z);
			glVertex3f(x, y, pt2.z);
		}
	}
	glEnd();
	glPopMatrix();

	//////////////////////////////////////////////point3  point4
	glPushMatrix();
	glRotatef(-90, 1.0, 0.0, 0.0);
	//glColor4f(0.0f, 0.9f, 0.9f, 0.4f); //紫色网格
	glColor4fv(colorLine1);
	glTranslatef(0, 0, -5);
	glBegin(GL_LINES);
	glEnable(GL_LINE_SMOOTH); //抗锯齿
	//绘制平行于X的直线
	for (zi2 = 0; zi2 <= num; zi2++) {
		float z = _zLen * zi2 + pt3.z;
		for (yi2 = 0; yi2 <= num; yi2++) {
			float y = _yLen * yi2 + pt3.y;
			glVertex3f(pt3.x, y, z);
			glVertex3f(pt4.x, y, z);
		}
	}
	//绘制平行于Y的直线
	for (zi2 = 0; zi2 <= num; zi2++) {
		float z = _zLen * zi2 + pt3.z;
		for (xi2 = 0; xi2 <= num; xi2++) {
			float x = _xLen * xi2 + pt3.x;
			glVertex3f(x, pt3.y, z);
			glVertex3f(x, pt4.y, z);
		}
	}
	//绘制平行于Z的直线
	for (yi2 = 0; yi2 <= num; yi2++) {
		float y = _yLen * yi2 + pt3.y;
		for (xi2 = 0; xi2 <= num; xi2++) {
			float x = _xLen * xi2 + pt3.x;
			glVertex3f(x, y, pt3.z);
			glVertex3f(x, y, pt4.z);
		}
	}
	glEnd();
	glPopMatrix();

	///////////////////////////////////////////////////////////////point5  point6
	glPushMatrix();
	glRotatef(90, 0.0, 1.0, 0.0);
	//glColor4f(0.0f, 0.9f, 0.0f, 0.4f); //绿色网格
	glColor4fv(colorLine3);
	glTranslatef(0, 0, -5);
	glBegin(GL_LINES);
	glEnable(GL_LINE_SMOOTH); //抗锯齿
	//绘制平行于X的直线
	for (zi3 = 0; zi3 <= num; zi3++) {
		float z = _zLen * zi3 + pt5.z;
		for (yi3 = 0; yi3 <= num; yi3++) {
			float y = _yLen * yi3 + pt5.y;
			glVertex3f(pt5.x, y, z);
			glVertex3f(pt6.x, y, z);
		}
	}
	//绘制平行于Y的直线
	for (zi3 = 0; zi3 <= num; zi3++) {
		float z = _zLen * zi3 + pt5.z;
		for (xi3 = 0; xi3 <= num; xi3++) {
			float x = _xLen * xi3 + pt5.x;
			glVertex3f(x, pt5.y, z);
			glVertex3f(x, pt6.y, z);
		}
	}
	//绘制平行于Z的直线
	for (yi3 = 0; yi3 <= num; yi3++) {
		float y = _yLen * yi3 + pt5.y;
		for (xi3 = 0; xi3 <= num; xi3++) {
			float x = _xLen * xi3 + pt5.x;
			glVertex3f(x, y, pt5.z);
			glVertex3f(x, y, pt6.z);
		}
	}
	glEnd();
	glPopMatrix();

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
}

///////////////////////////////////////////////////////////////////////////////
// draw the local axis of an object
///////////////////////////////////////////////////////////////////////////////
//绘制字符
#define MAX_CHAR 128
void drawCNString(const char* str)
{
	int len, i;
	wchar_t* wstring;
	HDC hDC = wglGetCurrentDC(); //获取显示设备
	GLuint list = glGenLists(1); //申请1个显示列表
	//计算字符的个数
	//如果是双字节字符的（比如中文字符），两个字节才算一个字符
	//否则一个字节算一个字符
	len = 0;
	for (i = 0; str[i] != ' '; ++i)
	{
		if (IsDBCSLeadByte(str[i]))
			++i;
		++len;
	}
	// 将混合字符转化为宽字符
	wstring = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L' ';// 只是转义符,它本身的类型是wchar_t
	// 逐个输出字符
	for (i = 0; i < len; ++i)
	{
		wglUseFontBitmapsW(hDC, wstring[i], 1, list);
		glCallList(list);
	}
	// 回收所有临时资源
	free(wstring);
	glDeleteLists(list, 1);
}

void ModelGL::drawAxis(float size)
{
    glDepthFunc(GL_ALWAYS);     // to avoid visual artifacts with grid lines
    glDisable(GL_LIGHTING);
    glPushMatrix();             //NOTE: There is a bug on Mac misbehaviours of
                                //      the light position when you draw GL_LINES
                                //      and GL_POINTS. remember the matrix.

	GLUquadricObj *objCylinder = gluNewQuadric(); //创建二次曲面对象——-圆柱
	glColor3f(0.0f, 0.0f, 1.0f);
	glTranslatef(-5, -5, -5);
	gluCylinder(objCylinder, 0.03, 0.03, 2.1*size, 10, 5);//Z_轴_蓝色
	glTranslatef(0, 0, 2.1*size);
	gluCylinder(objCylinder, 0.08, 0.0, 0.1*size, 10, 5);//Z_箭头
	glRasterPos3f(-0.3, 0.3, 0);
	drawCNString("ZZ ");// Print GL Text ToThe Screen
	glColor3f(0.0f, 1.0f, 0.0f);
	glTranslatef(0, 0, -2.1*size);
	glRotatef(-90, 1.0, 0.0, 0.0);
	gluCylinder(objCylinder, 0.03, 0.03, 2.1*size, 10, 5);//Y_轴_红色
	glTranslatef(0, 0, 2.1*size);
	gluCylinder(objCylinder, 0.08, 0.0, 0.1*size, 10, 5);//Y
	glRasterPos3f(-0.5, 0, 0.5);
	drawCNString("Y ");// Print GL Text ToThe Screen
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(0, 0, -2.1*size);
	glRotatef(90, 0.0, 1.0, 0.0);
	gluCylinder(objCylinder, 0.03, 0.03, 2.1*size, 10, 5);//X_轴_绿色
	glTranslatef(0, 0, 2.1*size);
	gluCylinder(objCylinder, 0.08, 0.0, 0.1*size, 10, 5);//X
	glRasterPos3f(0, 0.3, 0);
	drawCNString("X ");// Print GL Text ToThe Screen


    // restore default settings
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
}

///////////////////////////////////////////////////////////////////////////////
// draw frustum绘制锥形区域
///////////////////////////////////////////////////////////////////////////////
void ModelGL::drawFrustum(float fovY, float aspectRatio, float nearPlane, float farPlane)
{
    float tangent = tanf(fovY/2 * DEG2RAD);
    float nearHeight = nearPlane * tangent;
    float nearWidth = nearHeight * aspectRatio;
    float farHeight = farPlane * tangent;
    float farWidth = farHeight * aspectRatio;

    // compute 8 vertices of the frustum
    float vertices[8][3];
    // near top right
    vertices[0][0] = nearWidth;     vertices[0][1] = nearHeight;    vertices[0][2] = -nearPlane;
    // near top left
    vertices[1][0] = -nearWidth;    vertices[1][1] = nearHeight;    vertices[1][2] = -nearPlane;
    // near bottom left
    vertices[2][0] = -nearWidth;    vertices[2][1] = -nearHeight;   vertices[2][2] = -nearPlane;
    // near bottom right
    vertices[3][0] = nearWidth;     vertices[3][1] = -nearHeight;   vertices[3][2] = -nearPlane;
    // far top right
    vertices[4][0] = farWidth;      vertices[4][1] = farHeight;     vertices[4][2] = -farPlane;
    // far top left
    vertices[5][0] = -farWidth;     vertices[5][1] = farHeight;     vertices[5][2] = -farPlane;
    // far bottom left
    vertices[6][0] = -farWidth;     vertices[6][1] = -farHeight;    vertices[6][2] = -farPlane;
    // far bottom right
    vertices[7][0] = farWidth;      vertices[7][1] = -farHeight;    vertices[7][2] = -farPlane;

    float colorLine1[4] = { 0.7f, 0.7f, 0.7f, 0.7f };
    float colorLine2[4] = { 0.2f, 0.2f, 0.2f, 0.7f };
    float colorPlane[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw the edges around frustum
    glBegin(GL_LINES);
    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);      // near plane
    glColor4fv(colorLine1);
    glVertex3fv(vertices[4]);

    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[5]);

    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);     //far plane
    glColor4fv(colorLine1);
    glVertex3fv(vertices[6]);

    glColor4fv(colorLine2);
    glVertex3f(0, 0, 0);
    glColor4fv(colorLine1);
    glVertex3fv(vertices[7]);
    glEnd();

    glColor4fv(colorLine1);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[7]);
    glEnd();

    glColor4fv(colorLine1);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glEnd();

    // draw near and far plane
    glColor4fv(colorPlane);
    glBegin(GL_QUADS);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[7]);
    glEnd();

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
}

///////////////////////////////////////////////////////////////////////////////
// set the camera position and rotation
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setViewMatrix(float x, float y, float z, float pitch, float heading, float roll)
{
    cameraPosition[0] = x;
    cameraPosition[1] = y;
    cameraPosition[2] = z;
    cameraAngle[0] = pitch;
    cameraAngle[1] = heading;
    cameraAngle[2] = roll;

    updateViewMatrix();
}

///////////////////////////////////////////////////////////////////////////////
// set the object position and rotation
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setModelMatrix(float x, float y, float z, float rx, float ry, float rz)
{
    modelPosition[0] = x;
    modelPosition[1] = y;
    modelPosition[2] = z;
    modelAngle[0] = rx;
    modelAngle[1] = ry;
    modelAngle[2] = rz;

    updateModelMatrix();
}

///////////////////////////////////////////////////////////////////////////////
// update matrix
///////////////////////////////////////////////////////////////////////////////
void ModelGL::updateViewMatrix()
{
    // transform the camera (viewing matrix) from world space to eye space
    // Notice translation nd heading values are negated,
    // because we move the whole scene with the inverse of camera transform
    // ORDER: translation -> rotZ -> rotY ->rotX
    matrixView.identity();
    matrixView.translate(-cameraPosition[0], -cameraPosition[1], -cameraPosition[2]);
    matrixView.rotateZ(cameraAngle[2]);     // roll
    matrixView.rotateY(-cameraAngle[1]);    // heading
    matrixView.rotateX(cameraAngle[0]);     // pitch

    matrixModelView = matrixView * matrixModel;
}

void ModelGL::updateModelMatrix()
{
    // transform objects from object space to world space
    // ORDER: rotZ -> rotY -> rotX -> translation
    matrixModel.identity();
    matrixModel.rotateZ(modelAngle[2]);
    matrixModel.rotateY(modelAngle[1]);
    matrixModel.rotateX(modelAngle[0]);
    matrixModel.translate(modelPosition[0], modelPosition[1], modelPosition[2]);

    matrixModelView = matrixView * matrixModel;
}

///////////////////////////////////////////////////////////////////////////////
// rotate the camera for subWin2 (3rd person view)
///////////////////////////////////////////////////////////////////////////////
void ModelGL::rotateCamera(int x, int y)
{
    cameraAngleY += (x - mouseX);
    cameraAngleX += (y - mouseY);
    mouseX = x;
    mouseY = y;
}

///////////////////////////////////////////////////////////////////////////////
// zoom the camera for subWin2 (3rd person view)
///////////////////////////////////////////////////////////////////////////////
void ModelGL::zoomCamera(int y)
{
    cameraDistance -= (y - mouseY) * 0.1f;
    mouseY = y;
}
void ModelGL::zoomCameraDelta(float delta)
{
    cameraDistance -= delta;
}

///////////////////////////////////////////////////////////////////////////////
// change drawing mode
///////////////////////////////////////////////////////////////////////////////
void ModelGL::setDrawMode(int mode)
{
    if(drawMode != mode)
    {
        drawModeChanged = true;
        drawMode = mode;
    }
}

///////////////////////////////////////////////////////////////////////////////
// set a perspective frustum with 6 params similar to glFrustum()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
Matrix4 ModelGL::setFrustum(float l, float r, float b, float t, float n, float f)
{
    Matrix4 matrix;
    matrix[0]  =  2 * n / (r - l);
    matrix[5]  =  2 * n / (t - b);
    matrix[8]  =  (r + l) / (r - l);
    matrix[9]  =  (t + b) / (t - b);
    matrix[10] = -(f + n) / (f - n);
    matrix[11] = -1;
    matrix[14] = -(2 * f * n) / (f - n);
    matrix[15] =  0;
    return matrix;
}

///////////////////////////////////////////////////////////////////////////////
// set a symmetric perspective frustum with 4 params similar to gluPerspective
// (vertical field of view, aspect ratio, near, far)
///////////////////////////////////////////////////////////////////////////////
Matrix4 ModelGL::setFrustum(float fovY, float aspectRatio, float front, float back)
{
    float tangent = tanf(fovY/2 * DEG2RAD);   // tangent of half fovY
    float height = front * tangent;           // half height of near plane
    float width = height * aspectRatio;       // half width of near plane

    // params: left, right, bottom, top, near, far
    return setFrustum(-width, width, -height, height, front, back);
}

///////////////////////////////////////////////////////////////////////////////
// set a orthographic frustum with 6 params similar to glOrtho()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
Matrix4 ModelGL::setOrthoFrustum(float l, float r, float b, float t, float n, float f)
{
    Matrix4 matrix;
    matrix[0]  =  2 / (r - l);
    matrix[5]  =  2 / (t - b);
    matrix[10] = -2 / (f - n);
    matrix[12] = -(r + l) / (r - l);
    matrix[13] = -(t + b) / (t - b);
    matrix[14] = -(f + n) / (f - n);
    return matrix;
}

///////////////////////////////////////////////////////////////////////////////
// create glsl programs 创建着色器并链接程序
// NOTE: used OpenGL core API instead of ARB extension
///////////////////////////////////////////////////////////////////////////////
bool ModelGL::createShaderPrograms()
{
    // create 1st shader and program
    GLuint vsId1 = glCreateShader(GL_VERTEX_SHADER);//顶点着色器
    GLuint fsId1 = glCreateShader(GL_FRAGMENT_SHADER);//片段着色器
    progId1 = glCreateProgram();

    // load shader sources: flat shader
    glShaderSource(vsId1, 1, &vsSource1, 0);
    glShaderSource(fsId1, 1, &fsSource1, 0);

    // compile shader sources
    glCompileShader(vsId1);
    glCompileShader(fsId1);

    // attach shaders to the program
    glAttachShader(progId1, vsId1);
    glAttachShader(progId1, fsId1);

    // link program
    glLinkProgram(progId1);

    // create 2nd shader and program
    GLuint vsId2 = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsId2 = glCreateShader(GL_FRAGMENT_SHADER);
    progId2 = glCreateProgram();

    // load shader sources:
    glShaderSource(vsId2, 1, &vsSource2, 0);
    glShaderSource(fsId2, 1, &fsSource2, 0);

    // compile shader sources
    glCompileShader(vsId2);
    glCompileShader(fsId2);

    // attach shaders to the program
    glAttachShader(progId2, vsId2);
    glAttachShader(progId2, fsId2);

    // link program
    glLinkProgram(progId2);

    // check status
    GLint linkStatus1, linkStatus2;
    glGetProgramiv(progId1, GL_LINK_STATUS, &linkStatus1);
    glGetProgramiv(progId2, GL_LINK_STATUS, &linkStatus2);
    if(linkStatus1 == GL_TRUE && linkStatus2 == GL_TRUE)
    {
        return true;
    }
    else
    {
        std::cout << "=== GLSL LOG 1 ===\n" << getProgramStatus(progId1) << std::endl;
        std::cout << "=== GLSL LOG 2 ===\n" << getProgramStatus(progId2) << std::endl;
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
// return error message of shader compile status
// if no errors, it returns empty string
///////////////////////////////////////////////////////////////////////////////
std::string ModelGL::getShaderStatus(GLuint shader)
{
    std::string message;
    GLint status;
    glGetShaderiv(shader, GL_LINK_STATUS, &status);

    // failed to compile
    if(status == GL_FALSE)
    {
        // get # of chars of log
        int charCount = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &charCount);

        // get log
        char* buffer = new char[charCount];
        glGetShaderInfoLog(shader, charCount, &charCount, buffer);
        message = buffer;       // copy
        delete [] buffer;       // dealloc
    }

    return message;
}

///////////////////////////////////////////////////////////////////////////////
// return error message of shader program status
// if no errors, it returns empty string
///////////////////////////////////////////////////////////////////////////////
std::string ModelGL::getProgramStatus(GLuint program)
{
    std::string message;
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    // failed to link
    if(status == GL_FALSE)
    {
        // get # of chars of log
        int charCount = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &charCount);

        // get log
        char* buffer = new char[charCount];
        glGetProgramInfoLog(program, charCount, &charCount, buffer);
        message = buffer;   // copy
        delete [] buffer;   // dealloc
    }

    return message;
}
