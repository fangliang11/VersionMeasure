///////////////////////////////////////////////////////////////////////////////
// DialogWindow.cpp
// =================
// A class of Dialog Box for MS Windows
// It creates a dialog box with CreateDialogParam() API call.
//
//  AUTHOR: Fang Liang (fangliang1313@gmail.com)
// CREATED: 2018-12-20
// UPDATED: 2018-12-20
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iostream>
#include <cstring>
#include "DialogWindow.h"
#include "procedure.h"
using namespace Win;




///////////////////////////////////////////////////////////////////////////////
// constructor with params
///////////////////////////////////////////////////////////////////////////////
DialogWindow::DialogWindow(HINSTANCE hInst, WORD id, HWND hParent, Controller* ctrl) : handle(0), instance(hInst), id(id),
                                                                                       parentHandle(hParent), controller(ctrl),
                                                                                       x(0), y(0)
{
}



///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////
DialogWindow::~DialogWindow()
{
}



///////////////////////////////////////////////////////////////////////////////
// create a dialog box window
///////////////////////////////////////////////////////////////////////////////
HWND DialogWindow::create()
{
    handle = ::CreateDialogParam(instance, MAKEINTRESOURCE(id), parentHandle, Win::dialogProcedure, (LPARAM)controller);

    //this->show(SW_SHOWDEFAULT);                     // make it visible

    return handle;
}



///////////////////////////////////////////////////////////////////////////////
// show the window on the screen
///////////////////////////////////////////////////////////////////////////////
void DialogWindow::show(int cmdShow)
{
    ::ShowWindow(handle, cmdShow);
    ::UpdateWindow(handle);
}



