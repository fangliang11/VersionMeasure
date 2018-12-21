///////////////////////////////////////////////////////////////////////////////
// ControllerFormGL.h
// ==================
// Derived Controller class for OpenGL dialog window
//派生控制类，对话框
//  AUTHOR: Song Ho Ahn (song.ahn@gamil.com)
// CREATED: 2008-09-15
// UPDATED: 2018-03-01
///////////////////////////////////////////////////////////////////////////////

#ifndef WIN_CONTROLLER_FORM_GL_H
#define WIN_CONTROLLER_FORM_GL_H

//#include<Windows.h>
#include<vector>
#include "Controller.h"
#include "ViewFormGL.h"
#include "ModelGL.h"
using namespace std;


namespace Win
{
    class ControllerFormGL : public Controller
    {
    public:
        ControllerFormGL(ModelGL* model, ViewFormGL* view);
        ~ControllerFormGL() {};

        int command(int id, int cmd, LPARAM msg);   // for WM_COMMAND
        int close();                                // for WM_CLOSE
        int create();                               // for WM_CREATE
        int hScroll(WPARAM wParam, LPARAM lParam);  // for WM_HSCROLL
        int notify(int id, LPARAM lParam);          // for WM_NOTIFY
        int timer(WPARAM eventId, LPARAM callback); // for WM_TIMER

		bool READFINISHFLAG = false;
		bool SELECTFINISHFLAG;
		vector<float> coordinateX;
		vector<float> coordinateY;
		vector<float> coordinateZ;
		int ROWNUM;

    private:
        ModelGL* model;                             // pointer to model component
        ViewFormGL* view;                           // pointer to view component
    };
}

#endif
