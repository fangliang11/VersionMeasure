///////////////////////////////////////////////////////////////////////////////
// main.cpp
// ========
// main driver
//
//  AUTHOR: Fang Liang (fangliang1313@gmail.com)
// CREATED: 2018-12-20
// UPDATED: 2018-12-20
///////////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN             // exclude rarely-used stuff from Windows headers

#pragma comment(lib,"ComCtl32.lib")  // FANG

#include <windows.h>
#include <commctrl.h>                   // common controls
#include "Window.h"
#include "DialogWindow.h"
#include "ControllerMain.h"
#include "ControllerGL.h"
#include "ControllerFormGL.h"
#include "ModelGL.h"
#include "ViewGL.h"
#include "ViewFormGL.h"
#include "resource.h"
#include "Log.h"


// function declarations
int mainMessageLoop(HACCEL hAccelTable=0);


///////////////////////////////////////////////////////////////////////////////
// main function of a windows application
///////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdArgs, int cmdShow)
{

    // register Trackbar control from comctl32.dll before creating windows
    INITCOMMONCONTROLSEX commonCtrls;
    commonCtrls.dwSize = sizeof(commonCtrls);
    commonCtrls.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES | ICC_LINK_CLASS| ICC_UPDOWN_CLASS;
    ::InitCommonControlsEx(&commonCtrls);

    RECT rect;
    DWORD style;
    DWORD styleEx;

    // create model and view components for controller  为Controller类创建模型和视图类的实例
    ModelGL modelGL;
    Win::ViewGL viewGL;
    Win::ViewFormGL viewFormGL(&modelGL);

    // create main window   创建 main 窗口
    Win::ControllerMain mainCtrl;
	//Win::ControllerFormGL FormGLCtrl(&modelGL, &viewFormGL);

    Win::Window mainWin(hInst, L"三维点云图绘制软件", 0, &mainCtrl); //注册 mainWin 窗口
	mainWin.setIcon(IDI_Win32OpenGL); //设置图标
    mainWin.setWindowStyleEx(WS_EX_WINDOWEDGE);  //设置窗口样式
    if(mainWin.create())
        Win::log("Main window is created.");
    else
        Win::log("[ERROR] Failed to create main window.");

    //@@ MS ArticleID: 272222
    // There is a clipping bug when the window is resized larger.
    // Create a window with the max size initially to avoid this clipping bug.
    // Subsequent SetWindowPos() calls to resize the window dimension do not
    // generate the clipping issue.
    int fullWidth = ::GetSystemMetrics(SM_CXSCREEN);    // primary display only
    int fullHeight = ::GetSystemMetrics(SM_CYSCREEN);
    Win::log("Display Resolution: %dx%d", fullWidth, fullHeight);

    // create OpenGL rendering window, glWin, as a child of mainWin创建 opengl 渲染窗口 glWin ，作为mainWin的子窗口
    Win::ControllerGL glCtrl(&modelGL, &viewGL);
    Win::Window glWin(hInst, L"WindowGL", mainWin.getHandle(), &glCtrl); //注册 glWin 窗口
    glWin.setClassStyle(CS_OWNDC);  //设置类的样式
    glWin.setWindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);  //设置窗口样式
    // remove client edge of glWin  去掉glWin窗口的客户端边框
    styleEx = (DWORD)::GetWindowLongPtr(glWin.getHandle(), GWL_EXSTYLE);
    glWin.setWindowStyleEx(styleEx & ~WS_EX_CLIENTEDGE);
    glWin.setWidth(fullWidth);
    glWin.setHeight(fullHeight);
    if(glWin.create())
        Win::log("OpenGL child window is created.");
    else
        Win::log("[ERROR] Failed to create OpenGL window.");

    // create a child dialog box, glDialog contains all controls创建对话框,包含所有的控制条
    Win::ControllerFormGL formCtrl(&modelGL, &viewFormGL);
    Win::DialogWindow glDialog(hInst, IDD_FORMVIEW, mainWin.getHandle(), &formCtrl);  //注册对话框 glDialog
    glDialog.setWidth(1000);    // temporary width and height 临时的宽和高设置
    glDialog.setHeight(300);
    if(glDialog.create())
        Win::log("OpenGL form dialog is created.");
    else
        Win::log("[ERROR] Failed to create OpenGL form dialog.");

    // send window handles to mainCtrl, so we can resize the child windows when main window resized
	// 传递窗口句柄给 ControllerMain 类， 以便当 mainWin 窗口大小变化时， 子窗口 glWin和glDialog也变化
    mainCtrl.setGLHandle(glWin.getHandle());
    mainCtrl.setFormHandle(glDialog.getHandle());

    // place windows in the right position ================调整窗口到合适位置
    // get the dim of glDialog  获取 glDialog 窗口的分辨率
    ::GetWindowRect(glDialog.getHandle(), &rect);
    int dialogWidth = rect.right - rect.left;
    int dialogHeight = rect.bottom - rect.top;
    Win::log(L"Form dialog dimension: %d x %d", dialogWidth, dialogHeight);
    if(dialogWidth % 2 != 0)
        dialogWidth++;     // make it even

    // compute dim of glWin  计算glWin窗口的分辨率
    int glWidth = dialogWidth;
    int glHeight = glWidth / 2; // half of width
    ::SetWindowPos(glWin.getHandle(), 0, 0, 0, glWidth, glHeight, SWP_NOZORDER);
    Win::log(L"GL window dimension: %d x %d", glWidth, glHeight);

    // place the glDialog at the bottom of the opengl rendering window 将 glDialog 放置在 glWin的底部
    ::SetWindowPos(glDialog.getHandle(), 0, 0, glHeight, dialogWidth, dialogHeight, SWP_NOZORDER);

    // set dim of mainWin  设置 mainWin 窗口的分辨率
    rect.left = 0;
    rect.right = glWidth;
    rect.top = 0;
    rect.bottom = glHeight + dialogHeight;
    style = (DWORD)::GetWindowLongPtr(mainWin.getHandle(), GWL_STYLE);
    styleEx = (DWORD)::GetWindowLongPtr(mainWin.getHandle(), GWL_EXSTYLE);
    ::AdjustWindowRectEx(&rect, style, FALSE, styleEx);
    ::SetWindowPos(mainWin.getHandle(), 0, 100, 100, (rect.right-rect.left), (rect.bottom-rect.top), SWP_NOZORDER);

    // show all windows显示所有窗口
    glWin.show();//opengl渲染窗口
    glDialog.show();//对话框窗口
    mainWin.show();//主窗口

    // main message loop //////////////////////////////////////////////////////主消息循环
    int exitCode;
    HACCEL hAccelTable = 0;
    //hAccelTable = ::LoadAccelerators(hInst, MAKEINTRESOURCE(ID_ACCEL));
    exitCode = mainMessageLoop(hAccelTable);

    Win::log("Application is terminated.");
    return exitCode;
}



///////////////////////////////////////////////////////////////////////////////
// main message loop主消息循环函数
///////////////////////////////////////////////////////////////////////////////
int mainMessageLoop(HACCEL hAccelTable)
{
    HWND activeHandle; // 活动句柄
    MSG msg;

    while(::GetMessage(&msg, 0, 0, 0) > 0)  // loop until WM_QUIT(0) received
    {
        // determine the activated window is dialog box
        // skip if messages are for the dialog windows
		// 判断活动窗口是 对话框， 当消息是针对对话框窗口的则跳过(因为对话框无法响应消息)
        activeHandle = GetActiveWindow();
        if(::GetWindowLongPtr(activeHandle, GWL_EXSTYLE) & WS_EX_CONTROLPARENT) // WS_EX_CONTROLPARENT is automatically added by CreateDialogBox()
        {
            if(::IsDialogMessage(activeHandle, &msg))
                continue;   // message handled, back to while-loop
        }

        // now, handle window messages
        if(!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;                 // return nExitCode of PostQuitMessage()
}
