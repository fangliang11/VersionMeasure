///////////////////////////////////////////////////////////////////////////////
// ControllerFormGL.cpp
// ====================
// Derived Controller class for OpenGL dialog window
//
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2008-09-15
// UPDATED: 2018-03-01
///////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
using namespace std;

#include "Window.h"
#include "ControllerGL.h"
#include "ControllerFormGL.h"
#include "resource.h"
#include "Log.h"
#include "ReadData.h"
using namespace Win;

INT_PTR CALLBACK aboutDialogProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



///////////////////////////////////////////////////////////////////////////////
// default contructor
///////////////////////////////////////////////////////////////////////////////
ControllerFormGL::ControllerFormGL(ModelGL* model, ViewFormGL* view) : model(model), view(view)
{
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CLOSE
///////////////////////////////////////////////////////////////////////////////
int ControllerFormGL::close()
{
    ::DestroyWindow(handle);                    // close it
    Win::log("Form dialog is destroyed.");
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_CREATE
///////////////////////////////////////////////////////////////////////////////
int ControllerFormGL::create()
{
    // initialize all controls
    view->initControls(handle);

    // init the matrices
    model->setViewMatrix(0, 0, 10, 0, 0, 0);
    view->setViewMatrix(0, 0, 10, 0, 0, 0);

	SELECTFINISHFLAG = false;

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_COMMAND 选择数据文件对话框
///////////////////////////////////////////////////////////////////////////////
ReadData myData;
string filename;
int numX=3, numY=4, numZ=5;

int ControllerFormGL::command(int id, int command, LPARAM msg)
{
    switch(id)
    {
    case IDC_BUTTON_VIEW_RESET:
        if(command == BN_CLICKED)
        {
            model->setViewMatrix(0, 0, 10, 0, 0, 0);
            view->setViewMatrix(0, 0, 10, 0, 0, 0);
        }
        break;

    case IDC_BUTTON_MODEL_RESET:
        if(command == BN_CLICKED)
        {
            model->setModelMatrix(0, 0, 0, 0, 0, 0);
            view->setModelMatrix(0, 0, 0, 0, 0, 0);
        }
        break;

    case IDC_BUTTON_ABOUT:
        if(command == BN_CLICKED)
        {
            // open About dialog
            ::DialogBox((HINSTANCE)::GetWindowLongPtr(handle, GWLP_HINSTANCE), MAKEINTRESOURCE(IDD_ABOUT), handle, aboutDialogProcedure);
        }
        break;

	case IDC_BUTTON_OPEN:    
		if (command == BN_CLICKED)   //打开文件按钮
		{
			//打开数据文件
			filename = myData.selectFile();
			model->CTRDRAWFLAG = false; //绘图重置
			view->setEditText(filename);

			//HWND hwnd = FindWindow(L"三维点云图生成软件", NULL);
			//HWND hwndGL = GetWindow(hwnd, GW_CHILD);//获取 glWin 窗口的句柄, 即OpenGL窗口的句柄
			//::SendMessage(hwndGL, WM_MBUTTONDOWN, 0, 0); //绘图标志位复位
			SELECTFINISHFLAG = true;
		}
		break;
	case IDC_BUTTON_DRAW:
		if (command == BN_CLICKED)    // 重绘按钮
		{
			if (!SELECTFINISHFLAG) {
				//filename = view->getEditText();
			}

			//myData.readFile(filename, 5,ROWNUM, coordinateX, coordinateY, coordinateZ);
			myData.readFile(filename, 5, numX, numY, numZ, ROWNUM, coordinateX, coordinateY, coordinateZ);

			model->modelCoordinateX = coordinateX;
			model->modelCoordinateY = coordinateY;
			model->modelCoordinateZ = coordinateZ;
			model->modelROWNUM = ROWNUM;
			model->CTRDRAWFLAG = true;

			//HWND hwnd = FindWindow(L"三维点云图生成软件", NULL);// 获取主窗口句柄 mainWin
			//HWND hwndGL = GetWindow(hwnd, GW_CHILD);//获取 glWin 窗口的句柄, 即OpenGL窗口的句柄
			//::SendMessage(hwndGL, WM_PAINT, 0, 0);//发送绘图消息

			//InvalidateRect(hwnd, NULL, true);  //使用InvalidateRect函数触发WM_PAINT消息
		}
		break;
	case IDC_COMBO_X:  // 选择 X 轴坐标
		if (command == CBN_SELENDOK)
		{
			numX = 0;
			numX = view->getComboSelect(IDC_COMBO_X) + 1;
		}
		break;
	case IDC_COMBO_Y:  // 选择 Y 轴坐标
		if (command == BN_CLICKED)
		{
			numY = 0;
			numY = view->getComboSelect(IDC_COMBO_Y) + 1;

		}
		break;
	case IDC_COMBO_Z:  // 选择 Z 轴坐标
		if (command == BN_CLICKED)
		{
			numZ = 0;
			numZ = view->getComboSelect(IDC_COMBO_Z) + 1;

		}
		break;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// handle horizontal scroll notification
///////////////////////////////////////////////////////////////////////////////
int ControllerFormGL::hScroll(WPARAM wParam, LPARAM lParam)
{
    // check if the message comming from trackbar
    HWND trackbarHandle = (HWND)lParam;

    int position = HIWORD(wParam);              // current tick mark position
    if(trackbarHandle)
    {
        // get control ID
        int trackbarId = ::GetDlgCtrlID(trackbarHandle);

        switch(LOWORD(wParam))
        {
        case TB_THUMBTRACK:     // user dragged the slider
            //Win::log("trackbar: %d", position);
            // NOTE: view will update model component
            view->updateTrackbars(trackbarHandle, position);
            break;

        case TB_THUMBPOSITION:  // by WM_LBUTTONUP
            break;

        case TB_LINEUP:         // by VK_RIGHT, VK_DOWN
            break;

        case TB_LINEDOWN:       // by VK_LEFT, VK_UP
            break;

        case TB_TOP:            // by VK_HOME
            break;

        case TB_BOTTOM:         // by VK_END
            break;

        case TB_PAGEUP:         // by VK_PRIOR (User click the channel to prior.)
            break;

        case TB_PAGEDOWN:       // by VK_NEXT (User click the channel to next.)
            break;

        case TB_ENDTRACK:       // by WM_KEYUP (User release a key.)
            // NOTE: view will update model component
            position = (int)::SendMessage(trackbarHandle, TBM_GETPOS, 0, 0);
            view->updateTrackbars(trackbarHandle, position);
            break;
        }
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_NOTIFY
// The id is not guaranteed to be unique, so use NMHDR.hwndFrom and NMHDR.idFrom.
///////////////////////////////////////////////////////////////////////////////
int ControllerFormGL::notify(int id, LPARAM lParam)
{
    // first cast lParam to NMHDR* to know what the control is
    NMHDR* nmhdr = (NMHDR*)lParam;
    HWND from = nmhdr->hwndFrom;
    NMUPDOWN* nmUpDown = 0;

    switch(nmhdr->code)
    {
    // UpDownBox notifications =========
    case UDN_DELTAPOS:         // the change of position has begun
        // cast again lParam to NMUPDOWN*
        //nmUpDown = (NMUPDOWN*)lParam;
        //return view->changeUpDownPosition(from, nmUpDown->iPos + nmUpDown->iDelta);
        break;

    default:
        break;
    }

    // handled notifications
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_TIMER notification
///////////////////////////////////////////////////////////////////////////////
int ControllerFormGL::timer(WPARAM eventId, LPARAM callback)
{
    switch(eventId)
    {
    case IDT_TIMER:
        // not needed
        //view->updateMatrices();
        break;
    }

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// dialog procedure for About window帮助对话框的处理
///////////////////////////////////////////////////////////////////////////////
INT_PTR CALLBACK aboutDialogProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_INITDIALOG:
        break;

    case WM_CLOSE:
        {
            ::EndDialog(hwnd, 0);
        }
        break;

    case WM_COMMAND:
        if(LOWORD(wParam) == IDC_OK && HIWORD(wParam) == BN_CLICKED)
        {
            ::EndDialog(hwnd, 0);
        }
        break;

    case WM_NOTIFY:
        NMHDR* nmhdr = (NMHDR*)lParam;
        HWND from = nmhdr->hwndFrom;
        if(from == ::GetDlgItem(hwnd, IDC_SYSLINK1) && (nmhdr->code == NM_CLICK || nmhdr->code == NM_RETURN))
        {
            // cast again lParam to NMLINK*
            NMLINK* nmlink = (NMLINK*)lParam;
            ::ShellExecute(0, L"open", nmlink->item.szUrl, 0, 0, SW_SHOW);
        }
        break;
    }

    return false;
}
