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

    // create model and view components for controller  ΪController�ഴ��ģ�ͺ���ͼ���ʵ��
    ModelGL modelGL;
    Win::ViewGL viewGL;
    Win::ViewFormGL viewFormGL(&modelGL);

    // create main window   ���� main ����
    Win::ControllerMain mainCtrl;
	//Win::ControllerFormGL FormGLCtrl(&modelGL, &viewFormGL);

    Win::Window mainWin(hInst, L"��ά����ͼ�������", 0, &mainCtrl); //ע�� mainWin ����
	mainWin.setIcon(IDI_Win32OpenGL); //����ͼ��
    mainWin.setWindowStyleEx(WS_EX_WINDOWEDGE);  //���ô�����ʽ
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

    // create OpenGL rendering window, glWin, as a child of mainWin���� opengl ��Ⱦ���� glWin ����ΪmainWin���Ӵ���
    Win::ControllerGL glCtrl(&modelGL, &viewGL);
    Win::Window glWin(hInst, L"WindowGL", mainWin.getHandle(), &glCtrl); //ע�� glWin ����
    glWin.setClassStyle(CS_OWNDC);  //���������ʽ
    glWin.setWindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);  //���ô�����ʽ
    // remove client edge of glWin  ȥ��glWin���ڵĿͻ��˱߿�
    styleEx = (DWORD)::GetWindowLongPtr(glWin.getHandle(), GWL_EXSTYLE);
    glWin.setWindowStyleEx(styleEx & ~WS_EX_CLIENTEDGE);
    glWin.setWidth(fullWidth);
    glWin.setHeight(fullHeight);
    if(glWin.create())
        Win::log("OpenGL child window is created.");
    else
        Win::log("[ERROR] Failed to create OpenGL window.");

    // create a child dialog box, glDialog contains all controls�����Ի���,�������еĿ�����
    Win::ControllerFormGL formCtrl(&modelGL, &viewFormGL);
    Win::DialogWindow glDialog(hInst, IDD_FORMVIEW, mainWin.getHandle(), &formCtrl);  //ע��Ի��� glDialog
    glDialog.setWidth(1000);    // temporary width and height ��ʱ�Ŀ�͸�����
    glDialog.setHeight(300);
    if(glDialog.create())
        Win::log("OpenGL form dialog is created.");
    else
        Win::log("[ERROR] Failed to create OpenGL form dialog.");

    // send window handles to mainCtrl, so we can resize the child windows when main window resized
	// ���ݴ��ھ���� ControllerMain �࣬ �Ա㵱 mainWin ���ڴ�С�仯ʱ�� �Ӵ��� glWin��glDialogҲ�仯
    mainCtrl.setGLHandle(glWin.getHandle());
    mainCtrl.setFormHandle(glDialog.getHandle());

    // place windows in the right position ================�������ڵ�����λ��
    // get the dim of glDialog  ��ȡ glDialog ���ڵķֱ���
    ::GetWindowRect(glDialog.getHandle(), &rect);
    int dialogWidth = rect.right - rect.left;
    int dialogHeight = rect.bottom - rect.top;
    Win::log(L"Form dialog dimension: %d x %d", dialogWidth, dialogHeight);
    if(dialogWidth % 2 != 0)
        dialogWidth++;     // make it even

    // compute dim of glWin  ����glWin���ڵķֱ���
    int glWidth = dialogWidth;
    int glHeight = glWidth / 2; // half of width
    ::SetWindowPos(glWin.getHandle(), 0, 0, 0, glWidth, glHeight, SWP_NOZORDER);
    Win::log(L"GL window dimension: %d x %d", glWidth, glHeight);

    // place the glDialog at the bottom of the opengl rendering window �� glDialog ������ glWin�ĵײ�
    ::SetWindowPos(glDialog.getHandle(), 0, 0, glHeight, dialogWidth, dialogHeight, SWP_NOZORDER);

    // set dim of mainWin  ���� mainWin ���ڵķֱ���
    rect.left = 0;
    rect.right = glWidth;
    rect.top = 0;
    rect.bottom = glHeight + dialogHeight;
    style = (DWORD)::GetWindowLongPtr(mainWin.getHandle(), GWL_STYLE);
    styleEx = (DWORD)::GetWindowLongPtr(mainWin.getHandle(), GWL_EXSTYLE);
    ::AdjustWindowRectEx(&rect, style, FALSE, styleEx);
    ::SetWindowPos(mainWin.getHandle(), 0, 100, 100, (rect.right-rect.left), (rect.bottom-rect.top), SWP_NOZORDER);

    // show all windows��ʾ���д���
    glWin.show();//opengl��Ⱦ����
    glDialog.show();//�Ի��򴰿�
    mainWin.show();//������

    // main message loop //////////////////////////////////////////////////////����Ϣѭ��
    int exitCode;
    HACCEL hAccelTable = 0;
    //hAccelTable = ::LoadAccelerators(hInst, MAKEINTRESOURCE(ID_ACCEL));
    exitCode = mainMessageLoop(hAccelTable);

    Win::log("Application is terminated.");
    return exitCode;
}



///////////////////////////////////////////////////////////////////////////////
// main message loop����Ϣѭ������
///////////////////////////////////////////////////////////////////////////////
int mainMessageLoop(HACCEL hAccelTable)
{
    HWND activeHandle; // ����
    MSG msg;

    while(::GetMessage(&msg, 0, 0, 0) > 0)  // loop until WM_QUIT(0) received
    {
        // determine the activated window is dialog box
        // skip if messages are for the dialog windows
		// �жϻ������ �Ի��� ����Ϣ����ԶԻ��򴰿ڵ�������(��Ϊ�Ի����޷���Ӧ��Ϣ)
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
