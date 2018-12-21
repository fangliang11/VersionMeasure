///////////////////////////////////////////////////////////////////////////////
// ControllerGL.cpp
// ================
// Derived Controller class for OpenGL window
// It is the controller of OpenGL rendering window. It initializes DC and RC,
// when WM_CREATE called, then, start new thread for OpenGL rendering loop.
//
// When this class is constructed, it gets the pointers to model and view
// components.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2008-09-15
// UPDATED: 2018-03-01
///////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "ControllerGL.h"
#include "Log.h"
#include "ViewFormGL.h"
#include "ControllerFormGL.h"
using namespace Win;



///////////////////////////////////////////////////////////////////////////////
// default contructor
///////////////////////////////////////////////////////////////////////////////
ControllerGL::ControllerGL(ModelGL* model, ViewGL* view) : model(model), view(view),
                                                           loopFlag(false)
{
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CLOSE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::close()
{
    // wait for rendering thread is terminated 等待渲染线程 glThread 执行完毕
    loopFlag = false;
    glThread.join();  // 结束openGL线程，回到主线程
	//MessageBox(NULL, TEXT("opengl thread 线程结束"), TEXT("ControllerGL中 close函数触发"), 0);

    ::DestroyWindow(handle);
    Win::log("OpenGL window is destroyed.");
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CREATE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::create()
{
    //// create a OpenGL rendering context 创建openGL 渲染环境 RC
    if(view->createContext(handle, 32, 24, 8, 8))
    {
        Win::log(L"Created OpenGL rendering context.");
    }
    else
    {
        Win::log(L"[ERROR] Failed to create OpenGL rendering context from ControllerGL::create().");
        return -1;
    }

    // create a thread for OpenGL rendering
    //glThread = std::thread(&ControllerGL::runThread, CTRDRAWFLAG, THREADCLOSEFLAG, this);  //创建openGL线程
	glThread = std::thread(&ControllerGL::runThread, this);  //创建openGL线程
    
    loopFlag = true;
	Win::log(L"Created a rendering thread for OpenGL.");

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// handle WM_PAINT
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::paint()
{
	//Win::ViewFormGL viewFormGL(model);
	//Win::ControllerFormGL myControllerFormGL(model, &viewFormGL);

	//model->modelCoordinateX = myControllerFormGL.coordinateX;
	//model->modelCoordinateY = myControllerFormGL.coordinateY;
	//model->modelCoordinateZ = myControllerFormGL.coordinateZ;
	//model->modelROWNUM = myControllerFormGL.ROWNUM;


	model->top = 3;


	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// handle WM_COMMAND
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::command(int id, int cmd, LPARAM msg)
{
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// rendering thread 渲染线程， 初始化 openGL 的状态并开始渲染循环
// initialize OpenGL states and start rendering loop
///////////////////////////////////////////////////////////////////////////////
void ControllerGL::runThread()
{
    // set the current RC in this thread 在线程中设置为当前渲染环境
    ::wglMakeCurrent(view->getDC(), view->getRC());

    // initialize OpenGL states 初始化 openGL 的状态
    model->init();  //模型初始化
    Win::log(L"Initialized OpenGL states.");

    bool result = model->initShaders();  //初始化着色器
    if(result)
        Win::log("GLSL shader objects are initialized.");
    else
        Win::log("[ERROR] Failed to initialize GLSL.");

    // cofigure projection matrix 设置透视矩阵
    RECT rect;
    ::GetClientRect(handle, &rect);  //获取当前窗口的矩形
    model->setWindowSize(rect.right, rect.bottom);  //设置渲染窗口的尺寸
    Win::log(L"Initialized OpenGL window size.");

	// rendering loop 渲染循环
    Win::log(L"Entering OpenGL rendering thread...");
    while(loopFlag)
    {
        //std::this_thread::yield();      // yield to other processes or threads
        std::this_thread::sleep_for(std::chrono::milliseconds(30)); // yield to other processes or threads

		model->draw();
		view->swapBuffers();  //缓存区交换
    }

    // close OpenGL Rendering Context (RC)
    model->quit();  //模型退出
    view->closeContext(handle);  //关闭渲染环境
    Win::log(L"Exit OpenGL rendering thread.");
}



///////////////////////////////////////////////////////////////////////////////
// handle Left mouse down鼠标左键按下
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::lButtonDown(WPARAM state, int x, int y)
{
    // update mouse position
    model->setMousePosition(x, y);

    // set focus to receive wm_mousewheel event
    ::SetFocus(handle);

	//model->CTRDRAWFLAG = false;
	//MessageBox(NULL, TEXT("lButtonDown函数"), TEXT("消息响应"), 0);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// handle Middle mouse down //鼠标中键按下
///////////////////////////////////////////////////////////////////////////////

int ControllerGL::mButtonDown(WPARAM state, int x, int y)  
{
	model->top = 5;


	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// handle Left mouse up
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::lButtonUp(WPARAM state, int x, int y)
{
    // update mouse position
    model->setMousePosition(x, y);

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse down鼠标右键按下
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::rButtonDown(WPARAM state, int x, int y)
{
    // update mouse position
    model->setMousePosition(x, y);

    // set focus to receive wm_mousewheel event
    ::SetFocus(handle);

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle reft mouse up
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::rButtonUp(WPARAM state, int x, int y)
{
    // update mouse position
    model->setMousePosition(x, y);
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEMOVE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::mouseMove(WPARAM state, int x, int y)
{
    if(state == MK_LBUTTON)
    {
        model->rotateCamera(x, y);
    }
    if(state == MK_RBUTTON)
    {
        model->zoomCamera(y);
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_MOUSEWHEEL
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::mouseWheel(int state, int delta, int x, int y)
{
    model->zoomCameraDelta(delta / 120.0f);
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_SIZE
///////////////////////////////////////////////////////////////////////////////
int ControllerGL::size(int w, int h, WPARAM wParam)
{
    model->setWindowSize(w, h);
    Win::log(L"Changed OpenGL rendering window size: %dx%d.", w, h);
    return 0;
}
