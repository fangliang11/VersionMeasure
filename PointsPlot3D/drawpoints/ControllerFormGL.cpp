///////////////////////////////////////////////////////////////////////////////
// ControllerFormGL.cpp
// ====================
// Derived Controller class for OpenGL dialog window
//
//  AUTHOR: Fang Liang (fangliang1313@gmail.com)
// CREATED: 2018-12-20
// UPDATED: 2018-12-20
///////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
using namespace std;

#include "Window.h"
//#include "ControllerGL.h"
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

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// handle WM_COMMAND ѡ�������ļ��Ի���
///////////////////////////////////////////////////////////////////////////////
ReadData myData;
string filename, imagename1, imagename2;
string strselectname, strselectimagename1, strselectimagename2;
wstring wstrselectfilename, wstrselectimagename1, wstrselectimagename2;
int numX=3, numY=4, numZ=5;  //ViewFormGL ��Ҳ�г�ʼ����combobo��ѡ������

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
			::DialogBox((HINSTANCE)::GetWindowLongPtr(handle, GWLP_HINSTANCE), MAKEINTRESOURCE(IDD_TEST), handle, aboutDialogProcedure);

        }
        break;

	case IDC_BUTTON_OPEN:    
		if (command == BN_CLICKED)   //���ļ���ť
		{
			//�������ļ�
			wstrselectfilename = myData.selectFile();
			if (myData.SELECTFILEFLAG) {
				model->CTRDRAWFLAG = false; //��ͼ����
				view->setEditText(wstrselectfilename);

			}
		}
		break;
	case IDC_BUTTON_DRAW:
		if (command == BN_CLICKED)    // �ػ水ť
		{
			filename = view->getEditText(handle);
			READDATAFLAG = myData.readFile(filename, 5, numX, numY, numZ, ROWNUM, coordinateX, coordinateY, coordinateZ);
			if (READDATAFLAG) {
				model->modelCoordinateX = coordinateX;
				model->modelCoordinateY = coordinateY;
				model->modelCoordinateZ = coordinateZ;
				model->modelROWNUM = ROWNUM;
				model->CTRDRAWFLAG = true;
			}
			//else MessageBox(NULL, TEXT("���ݶ�ȡʧ��"), TEXT("����"), MB_OK | MB_SYSTEMMODAL | MB_ICONINFORMATION);
		}
		break;
	case IDC_BUTTON_SAVE:
		if (command == BN_CLICKED)  //  ����ͼƬ
		{
			//model->grabScreen();
		}
		break;
	case IDC_COMBO_X:  // ѡ�� X ������
		if (command == CBN_SELENDOK)
		{
			numX = 0;
			numX = view->getComboSelect(IDC_COMBO_X) + 1;

			float *ptcolor;
			ptcolor = view->getColorSelect();

		}
		break;
	case IDC_COMBO_Y:  // ѡ�� Y ������
		if (command == CBN_SELENDOK)
		{
			numY = 0;
			numY = view->getComboSelect(IDC_COMBO_Y) + 1;

		}
		break;
	case IDC_COMBO_Z:  // ѡ�� Z ������
		if (command == CBN_SELENDOK)
		{
			numZ = 0;
			numZ = view->getComboSelect(IDC_COMBO_Z) + 1;

		}
		break;
	case IDC_COMBO_COLOR:  // ͼ����ɫѡ��
		if (command == CBN_SELENDOK)
		{
			float *ptcolor;
			ptcolor = view->getColorSelect();

			float R = *ptcolor;
			float G = *(ptcolor+1);
			float B = *(ptcolor + 2);
			float A = *(ptcolor + 3);
			model->imagecolorR = *ptcolor;
			model->imagecolorG = *(ptcolor+1);
			model->imagecolorB = *(ptcolor+2);
			model->imagecolorA = *(ptcolor+3);

		}
		break;
	case IDC_COMBO_BACKCOLOR:  // ������ɫѡ��
		if (command == CBN_SELENDOK)
		{
			float *ptbackcolor;
			ptbackcolor = view->getBackColorSelect();

			float R = *ptbackcolor;
			float G = *(ptbackcolor + 1);
			float B = *(ptbackcolor + 2);
			float A = *(ptbackcolor + 3);
			model->backcolorR = *ptbackcolor;
			model->backcolorG = *(ptbackcolor + 1);
			model->backcolorB = *(ptbackcolor + 2);
			model->backcolorA = *(ptbackcolor + 3);
		}
		break;
	case IDC_RADIO_SETCAMERA:    //��ʾ���
		if (command == BN_CLICKED)
		{
			model->CAMERAFLAG = true;
		}
		break;
	case IDC_RADIO_HIDECAMERA:   //�������
		if (command == BN_CLICKED)
		{
			model->CAMERAFLAG = false;
		}
		break;
	case IDC_BUTTON_IMAGE1OPEN:   //ѡ��ͼƬ1
		if (command == BN_CLICKED)
		{
			strselectimagename1 = myData.selectImage();
			if (myData.SELECTIMAGEFLAG) {
				view->setEditImage1Text(strselectimagename1);
				char* charname1 = (char*)strselectimagename1.c_str();  // string to char*

				model->imagename1 = charname1;
				model->BUILDBUFFER = true;  //���´������㻺��
			}
		}
		break;
	case IDC_BUTTON_IMAGE2OPEN:   //ѡ��ͼƬ2
		if (command == BN_CLICKED)
		{
			strselectimagename2 = myData.selectImage();
			if (myData.SELECTIMAGEFLAG) {
				view->setEditImage2Text(strselectimagename2);
				char* charname2 = (char*)strselectimagename2.c_str();   // string to char*

				model->imagename2 = charname2;
				model->BUILDBUFFER = true;  //���´������㻺��
			}
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
// dialog procedure for About window���ڶԻ���Ĵ���
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
        //NMHDR* nmhdr = (NMHDR*)lParam;
        //HWND from = nmhdr->hwndFrom;
        //if(from == ::GetDlgItem(hwnd, IDC_SYSLINK1) && (nmhdr->code == NM_CLICK || nmhdr->code == NM_RETURN))
        //{
        //    // cast again lParam to NMLINK*
        //    NMLINK* nmlink = (NMLINK*)lParam;
        //    ::ShellExecute(0, L"open", nmlink->item.szUrl, 0, 0, SW_SHOW);
        //}
        break;
    }

    return false;
}
