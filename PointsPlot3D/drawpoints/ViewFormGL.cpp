///////////////////////////////////////////////////////////////////////////////
// ViewFormGL.cpp
// ==============
// View component of OpenGL dialog window
//
//  AUTHOR: Fang Liang (fangliang1313@gmail.com)
// CREATED: 2018-12-20
// UPDATED: 2018-12-20
///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>
#include <iomanip>
#include "ViewFormGL.h"
#include "resource.h"
#include "Log.h"
#include "wcharUtil.h"
using namespace Win;

const int   SLIDER_POS_RANGE = 20;      // total # of ticks (-1 ~ +10)
const int   SLIDER_POS_SHIFT = 10;      // units from min to center
const int   SLIDER_ROT_RANGE = 360;     // total # of ticks (-180 ~ +180)
const int   SLIDER_ROT_SHIFT = 180;     // units from min to center



///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
ViewFormGL::ViewFormGL(ModelGL* model) : model(model), parentHandle(0)
{
}


///////////////////////////////////////////////////////////////////////////////
// default dtor
///////////////////////////////////////////////////////////////////////////////
ViewFormGL::~ViewFormGL()
{
}



///////////////////////////////////////////////////////////////////////////////
// initialize all controls
///////////////////////////////////////////////////////////////////////////////
void ViewFormGL::initControls(HWND handle)
{
    // remember the handle to parent window
    parentHandle = handle;

    // set all controls
    buttonAbout.set(handle, IDC_BUTTON_ABOUT);
    //buttonAbout.setImage(::LoadIcon(0, IDI_INFORMATION));

    buttonResetView.set(handle, IDC_BUTTON_VIEW_RESET);
    textViewPosX.set(handle, IDC_LABEL_VIEW_PX);
    textViewPosY.set(handle, IDC_LABEL_VIEW_PY);
    textViewPosZ.set(handle, IDC_LABEL_VIEW_PZ);
    textViewRotX.set(handle, IDC_LABEL_VIEW_RX);
    textViewRotY.set(handle, IDC_LABEL_VIEW_RY);
    textViewRotZ.set(handle, IDC_LABEL_VIEW_RZ);
    textViewGL.set(handle, IDC_LABEL_VIEW_GL);

    buttonResetModel.set(handle, IDC_BUTTON_MODEL_RESET);
    textModelPosX.set(handle, IDC_LABEL_MODEL_PX);
    textModelPosY.set(handle, IDC_LABEL_MODEL_PY);
    textModelPosZ.set(handle, IDC_LABEL_MODEL_PZ);
    textModelRotX.set(handle, IDC_LABEL_MODEL_RX);
    textModelRotY.set(handle, IDC_LABEL_MODEL_RY);
    textModelRotZ.set(handle, IDC_LABEL_MODEL_RZ);
    textModelGL.set(handle, IDC_LABEL_MODEL_GL);

    sliderViewPosX.set(handle, IDC_SLIDER_VIEW_PX);
    sliderViewPosX.setRange(0, SLIDER_POS_RANGE);
    sliderViewPosX.setPos(SLIDER_POS_SHIFT);
    textViewPosX.setText(toWchar(sliderViewPosX.getPos() - SLIDER_POS_SHIFT));//显示slider控件的当前位置

    sliderViewPosY.set(handle, IDC_SLIDER_VIEW_PY);
    sliderViewPosY.setRange(0, SLIDER_POS_RANGE);
    sliderViewPosY.setPos(SLIDER_POS_SHIFT);
    textViewPosY.setText(toWchar(sliderViewPosY.getPos() - SLIDER_POS_SHIFT));

    sliderViewPosZ.set(handle, IDC_SLIDER_VIEW_PZ);
    sliderViewPosZ.setRange(0, SLIDER_POS_RANGE);
    sliderViewPosZ.setPos(SLIDER_POS_SHIFT);
    textViewPosZ.setText(toWchar(sliderViewPosZ.getPos() - SLIDER_POS_SHIFT));

    sliderViewRotX.set(handle, IDC_SLIDER_VIEW_RX);
    sliderViewRotX.setRange(0, SLIDER_ROT_RANGE);
    sliderViewRotX.setPos(SLIDER_ROT_SHIFT);
    textViewRotX.setText(toWchar(sliderViewRotX.getPos() - SLIDER_ROT_SHIFT));

    sliderViewRotY.set(handle, IDC_SLIDER_VIEW_RY);
    sliderViewRotY.setRange(0, SLIDER_ROT_RANGE);
    sliderViewRotY.setPos(SLIDER_ROT_SHIFT);
    textViewRotY.setText(toWchar(sliderViewRotY.getPos() - SLIDER_ROT_SHIFT));

    sliderViewRotZ.set(handle, IDC_SLIDER_VIEW_RZ);
    sliderViewRotZ.setRange(0, SLIDER_ROT_RANGE);
    sliderViewRotZ.setPos(SLIDER_ROT_SHIFT);
    textViewRotZ.setText(toWchar(sliderViewRotZ.getPos() - SLIDER_ROT_SHIFT));

    sliderModelPosX.set(handle, IDC_SLIDER_MODEL_PX);
    sliderModelPosX.setRange(0, SLIDER_POS_RANGE);
    sliderModelPosX.setPos(SLIDER_POS_SHIFT);
    textModelPosX.setText(toWchar(sliderViewPosX.getPos() - SLIDER_POS_SHIFT));

    sliderModelPosY.set(handle, IDC_SLIDER_MODEL_PY);
    sliderModelPosY.setRange(0, SLIDER_POS_RANGE);
    sliderModelPosY.setPos(SLIDER_POS_SHIFT);
    textModelPosY.setText(toWchar(sliderViewPosY.getPos() - SLIDER_POS_SHIFT));

    sliderModelPosZ.set(handle, IDC_SLIDER_MODEL_PZ);
    sliderModelPosZ.setRange(0, SLIDER_POS_RANGE);
    sliderModelPosZ.setPos(SLIDER_POS_SHIFT);
    textModelPosZ.setText(toWchar(sliderModelPosZ.getPos() - SLIDER_POS_SHIFT));

    sliderModelRotX.set(handle, IDC_SLIDER_MODEL_RX);
    sliderModelRotX.setRange(0, SLIDER_ROT_RANGE);
    sliderModelRotX.setPos(SLIDER_ROT_SHIFT);
    textModelRotX.setText(toWchar(sliderModelRotX.getPos() - SLIDER_ROT_SHIFT));

    sliderModelRotY.set(handle, IDC_SLIDER_MODEL_RY);
    sliderModelRotY.setRange(0, SLIDER_ROT_RANGE);
    sliderModelRotY.setPos(SLIDER_ROT_SHIFT);
    textModelRotY.setText(toWchar(sliderModelRotY.getPos() - SLIDER_ROT_SHIFT));

    sliderModelRotZ.set(handle, IDC_SLIDER_MODEL_RZ);
    sliderModelRotZ.setRange(0, SLIDER_ROT_RANGE);
    sliderModelRotZ.setPos(SLIDER_ROT_SHIFT);
    textModelRotZ.setText(toWchar(sliderModelRotZ.getPos() - SLIDER_ROT_SHIFT));

	//openGL绘图区控件初始化
	buttonOpenFile.set(handle, IDC_BUTTON_OPEN);
	buttonReDraw.set(handle, IDC_BUTTON_DRAW);
	editboxFileName.set(handle, IDC_EDIT_FILENAME);
	editboxImage1Name.set(handle, IDC_EDIT_IMAGENAME1);
	editboxImage2Name.set(handle, IDC_EDIT_IMAGENAME2);
	comboboxCoordinateX.set(handle, IDC_COMBO_X);
	comboboxCoordinateY.set(handle, IDC_COMBO_Y);
	comboboxCoordinateZ.set(handle, IDC_COMBO_Z);
	comboboxColor.set(handle, IDC_COMBO_COLOR);
	comboboxBackColor.set(handle, IDC_COMBO_BACKCOLOR);
	setCamera.set(handle, IDC_RADIO_SETCAMERA);
	hideCamera.set(handle, IDC_RADIO_HIDECAMERA);

	string strTemp[] = {"X", "Y", "X1", "Y1", "Z1", "D_x", "D_y", "D_z", "X2", "Y2", "Z2", "D_p", "corroef"};
	comboboxCoordinateX.resetContent(); //坐标轴选择条,清除现有内容
	comboboxCoordinateY.resetContent();
	comboboxCoordinateZ.resetContent();
	for (int i = 0; i < 13; i++) {
		wstring widstr = wstring(strTemp[i].begin(), strTemp[i].end());
		const wchar_t *pwidstr = widstr.c_str();
		comboboxCoordinateX.addString(pwidstr);   //设置下拉列表
		comboboxCoordinateY.addString(pwidstr);
		comboboxCoordinateZ.addString(pwidstr);
		*pwidstr++;
	}
	comboboxCoordinateX.setCurrentSelection(2); // 设置默认选项
    comboboxCoordinateY.setCurrentSelection(3);
    comboboxCoordinateZ.setCurrentSelection(4);

	comboboxColor.resetContent();      //  图形颜色设置combobox
	comboboxColor.addString(L"蓝色");
	comboboxColor.addString(L"红色");
	comboboxColor.addString(L"绿色");
	comboboxColor.addString(L"灰色");
	comboboxColor.setCurrentSelection(0);

	comboboxBackColor.resetContent();      //  背景颜色设置combobox
	comboboxBackColor.addString(L"黑色");
	comboboxBackColor.addString(L"白色");
	comboboxBackColor.setCurrentSelection(0);

	setCamera.check();        // 单选框按钮


	//buttonOpenFile.disable();

    // elements for view matrix
    mv[0].set(handle, IDC_M_V_0);
    mv[1].set(handle, IDC_M_V_1);
    mv[2].set(handle, IDC_M_V_2);
    mv[3].set(handle, IDC_M_V_3);
    mv[4].set(handle, IDC_M_V_4);
    mv[5].set(handle, IDC_M_V_5);
    mv[6].set(handle, IDC_M_V_6);
    mv[7].set(handle, IDC_M_V_7);
    mv[8].set(handle, IDC_M_V_8);
    mv[9].set(handle, IDC_M_V_9);
    mv[10].set(handle, IDC_M_V_10);
    mv[11].set(handle, IDC_M_V_11);
    mv[12].set(handle, IDC_M_V_12);
    mv[13].set(handle, IDC_M_V_13);
    mv[14].set(handle, IDC_M_V_14);
    mv[15].set(handle, IDC_M_V_15);

    // elements for model matrix
    mm[0].set(handle, IDC_M_M_0);
    mm[1].set(handle, IDC_M_M_1);
    mm[2].set(handle, IDC_M_M_2);
    mm[3].set(handle, IDC_M_M_3);
    mm[4].set(handle, IDC_M_M_4);
    mm[5].set(handle, IDC_M_M_5);
    mm[6].set(handle, IDC_M_M_6);
    mm[7].set(handle, IDC_M_M_7);
    mm[8].set(handle, IDC_M_M_8);
    mm[9].set(handle, IDC_M_M_9);
    mm[10].set(handle, IDC_M_M_10);
    mm[11].set(handle, IDC_M_M_11);
    mm[12].set(handle, IDC_M_M_12);
    mm[13].set(handle, IDC_M_M_13);
    mm[14].set(handle, IDC_M_M_14);
    mm[15].set(handle, IDC_M_M_15);

    // elements for modelview matrix
    mmv[0].set(handle, IDC_M_MV_0);
    mmv[1].set(handle, IDC_M_MV_1);
    mmv[2].set(handle, IDC_M_MV_2);
    mmv[3].set(handle, IDC_M_MV_3);
    mmv[4].set(handle, IDC_M_MV_4);
    mmv[5].set(handle, IDC_M_MV_5);
    mmv[6].set(handle, IDC_M_MV_6);
    mmv[7].set(handle, IDC_M_MV_7);
    mmv[8].set(handle, IDC_M_MV_8);
    mmv[9].set(handle, IDC_M_MV_9);
    mmv[10].set(handle, IDC_M_MV_10);
    mmv[11].set(handle, IDC_M_MV_11);
    mmv[12].set(handle, IDC_M_MV_12);
    mmv[13].set(handle, IDC_M_MV_13);
    mmv[14].set(handle, IDC_M_MV_14);
    mmv[15].set(handle, IDC_M_MV_15);

    // textboxes for OpenGL function calls
    textViewGL.setFont(L"Courier New", 9);
    textModelGL.setFont(L"Courier New", 9);
}


///////////////////////////////////////////////////////////////////////////////
// 获取组合框的选中值
///////////////////////////////////////////////////////////////////////////////
int ViewFormGL::getComboSelect(int ComboBoxID) {

	int numberX, numberY, numberZ;

	if (ComboBoxID == IDC_COMBO_X) {
		numberX = comboboxCoordinateX.getCurrentSelection();

		//AllocConsole();
		//freopen("CONOUT$", "w+t", stdout);
		//freopen("CONIN$", "r+t", stdin);
		//cout << "numofcolor= " << numberX << endl;

		return numberX;

	}
	if (ComboBoxID == IDC_COMBO_Y) {
		numberY = comboboxCoordinateY.getCurrentSelection();
		return numberY;
	}
	if (ComboBoxID == IDC_COMBO_Z) {
		numberZ = comboboxCoordinateZ.getCurrentSelection();
		return numberZ;
	}



}


///////////////////////////////////////////////////////////////////////////////
// 选择颜色
///////////////////////////////////////////////////////////////////////////////
float *ViewFormGL::getColorSelect() {
	static float color[4];
	int numofcolor;
	numofcolor = comboboxColor.getCurrentSelection();
	

	switch (numofcolor)
	{
	case(0):
		color[0] = 0.0f;
		color[1] = 0.0f;
		color[2] = 1.0f;
		color[3] = 1.0f;
		break;
	case(1):
		color[0] = 1.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		color[3] = 1.0f;
		break;
	case(2):
		color[0] = 0.0f;
		color[1] = 1.0f;
		color[2] = 0.0f;
		color[3] = 1.0f;
		break;
	case(3):
		color[0] = 0.5f;
		color[1] = 0.5f;
		color[2] = 0.5f;
		color[3] = 1.0f;
		break;
	}


	return color;
}

float *ViewFormGL::getBackColorSelect() {
	static float backcolor[4];
	int numbackcolor;
	numbackcolor = comboboxBackColor.getCurrentSelection();

	//AllocConsole();
 //   freopen("CONOUT$", "w+t", stdout);
 //   freopen("CONIN$", "r+t", stdin);
 //   cout << "numofcolor= " << numbackcolor << endl;

	switch (numbackcolor)
	{
	case(0):
		backcolor[0] = 0.0f;
		backcolor[1] = 0.0f;
		backcolor[2] = 0.0f;
		backcolor[3] = 1.0f;
		break;
	case(1):
		backcolor[0] = 1.0f;
		backcolor[1] = 1.0f;
		backcolor[2] = 1.0f;
		backcolor[3] = 1.0f;
		break;
	}

	//cout << "R:" << backcolor[0] << endl;
	//cout << "G:" << backcolor[1] << endl;
	//cout << "B:" << backcolor[2] << endl;
	//cout << "A:" << backcolor[3] << endl;
	
	return backcolor;
}

///////////////////////////////////////////////////////////////////////////////
// 设置文本编辑框内容
///////////////////////////////////////////////////////////////////////////////
void ViewFormGL::setEditText(wstring filename) {

	std::wstring widestr = std::wstring(filename.begin(), filename.end());  //  string 转 wchar_t
	const wchar_t* widecstr = widestr.c_str();
	editboxFileName.setText(widecstr);
}
void ViewFormGL::setEditImage1Text(string filename) {

	std::wstring widestr = std::wstring(filename.begin(), filename.end());  //  string 转 wchar_t
	const wchar_t* widecstr = widestr.c_str();
	editboxImage1Name.setText(widecstr);
}
void ViewFormGL::setEditImage2Text(string filename) {

	std::wstring widestr = std::wstring(filename.begin(), filename.end());  //  string 转 wchar_t
	const wchar_t* widecstr = widestr.c_str();
	editboxImage2Name.setText(widecstr);
}


///////////////////////////////////////////////////////////////////////////////
// 获取文本编辑框内容
///////////////////////////////////////////////////////////////////////////////
string ViewFormGL::getEditText(HWND handle) {

	TCHAR tcharname[128]; 
	GetWindowText(GetDlgItem(handle, IDC_EDIT_FILENAME), tcharname, 128);
	TCHAR *tchar = tcharname;  // TCHAR型转成 string 型
	wstring ws(tchar);
	//string str(ws.begin(), ws.end());

	std::string strLocale = setlocale(LC_ALL, "");   //  wstring  to   string
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char *chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, nDestSize);
	std::string strResult = chDest;
	delete[]chDest;
	setlocale(LC_ALL, strLocale.c_str());

	return strResult;
}

///////////////////////////////////////////////////////////////////////////////
// update trackbars
///////////////////////////////////////////////////////////////////////////////
void ViewFormGL::updateTrackbars(HWND handle, int position)
{
    int value;
    if(handle == sliderViewPosX.getHandle())   // 视图视角改变滑动条
    {
        value = position - SLIDER_POS_SHIFT;
        sliderViewPosX.setPos(position);
        textViewPosX.setText(toWchar(value));
        model->setCameraX((float)value);
    }
    else if(handle == sliderViewPosY.getHandle())
    {
        value = position - SLIDER_POS_SHIFT;
        sliderViewPosY.setPos(position);
        textViewPosY.setText(toWchar(value));
        model->setCameraY((float)value);
    }
    else if(handle == sliderViewPosZ.getHandle())
    {
        value = position - SLIDER_POS_SHIFT;
        sliderViewPosZ.setPos(position);
        textViewPosZ.setText(toWchar(value));
        model->setCameraZ((float)value);
    }
    else if(handle == sliderViewRotX.getHandle())
    {
        value = position - SLIDER_ROT_SHIFT;
        sliderViewRotX.setPos(position);
        textViewRotX.setText(toWchar(value));
        model->setCameraAngleX((float)value);
    }
    else if(handle == sliderViewRotY.getHandle())
    {
        value = position - SLIDER_ROT_SHIFT;
        sliderViewRotY.setPos(position);
        textViewRotY.setText(toWchar(value));
        model->setCameraAngleY((float)value);
    }
    else if(handle == sliderViewRotZ.getHandle())
    {
        value = position - SLIDER_ROT_SHIFT;
        sliderViewRotZ.setPos(position);
        textViewRotZ.setText(toWchar(value));
        model->setCameraAngleZ((float)value);
    }
    else if(handle == sliderModelPosX.getHandle())  // 模型视角改变滑动条
    {
        value = position - SLIDER_POS_SHIFT;
        sliderModelPosX.setPos(position);
        textModelPosX.setText(toWchar(value));
        model->setModelX((float)value);
    }
    else if(handle == sliderModelPosY.getHandle())
    {
        value = position - SLIDER_POS_SHIFT;
        sliderModelPosY.setPos(position);
        textModelPosY.setText(toWchar(value));
        model->setModelY((float)value);
    }
    else if(handle == sliderModelPosZ.getHandle())
    {
        value = position - SLIDER_POS_SHIFT;
        sliderModelPosZ.setPos(position);
        textModelPosZ.setText(toWchar(value));
        model->setModelZ((float)value);
    }
    else if(handle == sliderModelRotX.getHandle())
    {
        value = position - SLIDER_ROT_SHIFT;
        sliderModelRotX.setPos(position);
        textModelRotX.setText(toWchar(value));
        model->setModelAngleX((float)value);
    }
    else if(handle == sliderModelRotY.getHandle())
    {
        value = position - SLIDER_ROT_SHIFT;
        sliderModelRotY.setPos(position);
        textModelRotY.setText(toWchar(value));
        model->setModelAngleY((float)value);
    }
    else if(handle == sliderModelRotZ.getHandle())
    {
        value = position - SLIDER_ROT_SHIFT;
        sliderModelRotZ.setPos(position);
        textModelRotZ.setText(toWchar(value));
        model->setModelAngleZ((float)value);
    }

    updateMatrices();
}



///////////////////////////////////////////////////////////////////////////////
// set view matrix entries
///////////////////////////////////////////////////////////////////////////////
void ViewFormGL::setViewMatrix(float x, float y, float z, float p, float h, float r)
{
    sliderViewPosX.setPos((int)(x + SLIDER_POS_SHIFT));
    textViewPosX.setText(toWchar(sliderViewPosX.getPos() - SLIDER_POS_SHIFT));

    sliderViewPosY.setPos((int)(y + SLIDER_POS_SHIFT));
    textViewPosY.setText(toWchar(sliderViewPosY.getPos() - SLIDER_POS_SHIFT));

    sliderViewPosZ.setPos((int)(z + SLIDER_POS_SHIFT));
    textViewPosZ.setText(toWchar(sliderViewPosZ.getPos() - SLIDER_POS_SHIFT));

    sliderViewRotX.setPos((int)(p + SLIDER_ROT_SHIFT));
    textViewRotX.setText(toWchar(sliderViewRotX.getPos() - SLIDER_ROT_SHIFT));

    sliderViewRotY.setPos((int)(h + SLIDER_ROT_SHIFT));
    textViewRotY.setText(toWchar(sliderViewRotY.getPos() - SLIDER_ROT_SHIFT));

    sliderViewRotZ.setPos((int)(r + SLIDER_ROT_SHIFT));
    textViewRotZ.setText(toWchar(sliderViewRotZ.getPos() - SLIDER_ROT_SHIFT));

    updateMatrices();
}



///////////////////////////////////////////////////////////////////////////////
// set model matrix entries
///////////////////////////////////////////////////////////////////////////////
void ViewFormGL::setModelMatrix(float x, float y, float z, float rx, float ry, float rz)
{
    sliderModelPosX.setPos((int)(x + SLIDER_POS_SHIFT));
    textModelPosX.setText(toWchar(sliderModelPosX.getPos() - SLIDER_POS_SHIFT));

    sliderModelPosY.setPos((int)(y + SLIDER_POS_SHIFT));
    textModelPosY.setText(toWchar(sliderModelPosY.getPos() - SLIDER_POS_SHIFT));

    sliderModelPosZ.setPos((int)(z + SLIDER_POS_SHIFT));
    textModelPosZ.setText(toWchar(sliderModelPosZ.getPos() - SLIDER_POS_SHIFT));

    sliderModelRotX.setPos((int)(rx + SLIDER_ROT_SHIFT));
    textModelRotX.setText(toWchar(sliderModelRotX.getPos() - SLIDER_ROT_SHIFT));

    sliderModelRotY.setPos((int)(ry + SLIDER_ROT_SHIFT));
    textModelRotY.setText(toWchar(sliderModelRotY.getPos() - SLIDER_ROT_SHIFT));

    sliderModelRotZ.setPos((int)(rz + SLIDER_ROT_SHIFT));
    textModelRotZ.setText(toWchar(sliderModelRotZ.getPos() - SLIDER_ROT_SHIFT));

    updateMatrices();
}



///////////////////////////////////////////////////////////////////////////////
// update all elements of 3 matrices and OpenGL function calls
///////////////////////////////////////////////////////////////////////////////
void ViewFormGL::updateMatrices()
{
    const float* matrix;
    std::wstringstream wss;
    int i;

    // convert number to string with limited decimal points
    wss << std::fixed << std::setprecision(2);

    matrix = model->getViewMatrixElements();
    for(i = 0; i < 16; ++i)
    {
        wss.str(L"");
        wss << matrix[i] << std::ends;
        mv[i].setText(wss.str().c_str());
    }

    matrix = model->getModelMatrixElements();
    for(i = 0; i < 16; ++i)
    {
        wss.str(L"");
        wss << matrix[i] << std::ends;
        mm[i].setText(wss.str().c_str());
    }

    matrix = model->getModelViewMatrixElements();
    for(i = 0; i < 16; ++i)
    {
        wss.str(L"");
        wss << matrix[i] << std::ends;
        mmv[i].setText(wss.str().c_str());
    }

    // update OpenGL function calls
    wss.str(L""); // clear
    wss << std::fixed << std::setprecision(0);
    wss << L"glRotatef(" << model->getCameraAngleX() << L",1,0,0);\n"
        << L"glRotatef(" << -model->getCameraAngleY() << L",0,1,0);\n"
        << L"glRotatef(" << model->getCameraAngleZ() << L",0,0,1);\n"
        << L"glTranslatef(" << -model->getCameraX() << L"," << -model->getCameraY() << L"," << -model->getCameraZ() << L");\n"
        << std::ends;
    textViewGL.setText(wss.str().c_str());

    wss.str(L""); // clear
    wss << L"glTranslatef(" << model->getModelX() << L"," << model->getModelY() << L"," << model->getModelZ() << L");\n"
        << L"glRotatef(" << model->getModelAngleX() << L",1,0,0);\n"
        << L"glRotatef(" << model->getModelAngleY() << L",0,1,0);\n"
        << L"glRotatef(" << model->getModelAngleZ() << L",0,0,1);\n"
        << std::ends;
    textModelGL.setText(wss.str().c_str());

    // unset floating format
    wss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
}

