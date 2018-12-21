#include <Windows.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "ReadData.h"

using namespace std;

ReadData::ReadData() {

}
ReadData::~ReadData() {

}

string ReadData::selectFile() {

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名

	ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小
	ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
	//ofn.lpstrFilter = TEXT("所有文件\0*.*\0.txt/.dat Flie\0*.txt;*.dat\0\0");//设置过滤
	ofn.lpstrFilter = TEXT(".txt/.dat 文件\0*.txt;*.dat\0\0");//设置过滤

	ofn.nFilterIndex = 1;//过滤器索引
	ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL
	ofn.nMaxFile = sizeof(strFilename);//缓冲区长度
	ofn.lpstrInitialDir = NULL;//初始目录为默认
	ofn.lpstrTitle = TEXT("请选择数据文件");//使用系统默认标题留空即可
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项
	if (GetOpenFileName(&ofn))
	{
		SELECTFINISHFLAG = true;
		MessageBox(NULL, strFilename, TEXT("已选择数据"), 0);
	}

	TCHAR *tchar = strFilename;  // TCHAR型转成 string 型
	wstring ws(tchar);
	string str(ws.begin(), ws.end());


	//AllocConsole();
	//freopen("CONOUT$", "w+t", stdout);
	//freopen("CONIN$", "r+t", stdin);
	//printf("%s\n", str.c_str());

	return str;
}

void ReadData::show(string selectfilename) {

	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONIN$", "r+t", stdin);
	printf("%s\n", selectfilename.c_str());

}

int ReadData::readFile(string filename, int AXES_LEN,
	                   int &ROWNUM, vector<float> &x, vector<float> &y, vector<float> &z) {

	ifstream myfile(filename);
	if (!myfile.is_open()) {
		MessageBox(NULL, TEXT("读取数据失败\n\n请选择或输入数据文件的名称"), TEXT("错误"), 0);

		return 0;
	}
	else if (myfile.is_open()) {

		vector<string> vec;
		vector<float> vectorX;
		vector<float> vectorY;
		vector<float> vectorZ;

		string temp;
		while (getline(myfile, temp))                    //利用getline（）读取每一行，并按照行为单位放入到vector
		{
			vec.push_back(temp);
		}
		myfile.close();
		ROWNUM = vec.size();
		//cout << " the num of row is: " << ROWNUM << endl;
		for (auto it = vec.begin(); it != vec.end(); it++)
		{
			//cout << *it << endl;
			istringstream is(*it);                    //用每一行的数据初始化一个字符串输入流；
			string s;
			int pam = 1;                              //从第一列开始

			while (is >> s)                          //以空格为界，把istringstream中数据取出放入到依次s中
			{
				if (pam == 3)                       //获取第 P 列的数据
				{
					float r = atof(s.c_str());     //做数据类型转换，将string类型转换成float
					vectorX.push_back(r);
				}
				if (pam == 4) {
					float y = atof(s.c_str());
					vectorY.push_back(y);
				}
				if (pam == 5) {
					float z = atof(s.c_str());
					vectorZ.push_back(z);
				}

				pam++;

			}
		}

		float maxX, maxY, maxZ, minX, minY, minZ;
		maxX = *max_element(begin(vectorX), end(vectorX));
		minX = *min_element(begin(vectorX), end(vectorX));

		maxY = *max_element(begin(vectorY), end(vectorY));
		minY = *min_element(begin(vectorY), end(vectorY));

		maxZ = *max_element(begin(vectorZ), end(vectorZ));
		minZ = *min_element(begin(vectorZ), end(vectorZ));

		vector<float> coordinateX;
		vector<float> coordinateY;
		vector<float> coordinateZ;

		//coordinateX.clear();
		//coordinateY.clear();
		//coordinateZ.clear();

		for (int i = 0; i < ROWNUM; i++) {
			float coeffX = 1.5*AXES_LEN / (maxX - minX); //求坐标系下的坐标数值
			coordinateX.push_back(coeffX * vectorX[i]);

			float coeffY = 1.5*AXES_LEN / (maxY - minY);
			coordinateY.push_back(coeffY * vectorY[i]);

			float coeffZ = 2 / (maxZ - minZ);  ////深度的变化范围对应坐标系Z轴2个单位的长度
			coordinateZ.push_back(coeffZ * (vectorZ[i] - minZ));

		}
		x = coordinateX;
		y = coordinateY;
		z = coordinateZ;

		return 1;
	}

	return 0;
}

int ReadData::readFile(string filename, int AXES_LEN, int column_num_X, int column_num_Y, int column_num_Z,
	                   int &ROWNUM, vector<float> &x, vector<float> &y, vector<float> &z) {

	ifstream myfile(filename);
	if (!myfile.is_open()) {
		MessageBox(NULL, TEXT("读取数据失败\n\n请选择或输入数据文件的名称"), TEXT("错误"), 0);

		return 0;
	}
	else if (myfile.is_open()) {

		vector<string> vec;
		vector<float> vectorX;
		vector<float> vectorY;
		vector<float> vectorZ;

		//fseek(myfile, 100, SEEK_SET);

		string temp;
		while (getline(myfile, temp))                    //利用getline（）读取每一行，并按照行为单位放入到vector
		{
			vec.push_back(temp);
		}
		myfile.close();
		ROWNUM = vec.size();
		//cout << " the num of row is: " << ROWNUM << endl;
		for (auto it = vec.begin(); it != vec.end(); it++)
		{
			//cout << *it << endl;
			istringstream is(*it);                    //用每一行的数据初始化一个字符串输入流；
			string s;
			int pam = 1;                              //从第一列开始

			while (is >> s)                          //以空格为界，把istringstream中数据取出放入到依次s中
			{
				if (pam == column_num_X)                       //获取第 numX 列的数据
				{
					float r = atof(s.c_str());     //做数据类型转换，将string类型转换成float
					vectorX.push_back(r);
				}
				if (pam == column_num_Y)                     //获取第 numY 列的数据
				{
					float y = atof(s.c_str());
					vectorY.push_back(y);
				}
				if (pam == column_num_Z)                     //获取第 numZ 列的数据
				{
					float z = atof(s.c_str());
					vectorZ.push_back(z);
				}

				pam++;

			}
		}

		float maxX, maxY, maxZ, minX, minY, minZ;
		maxX = *max_element(begin(vectorX), end(vectorX));
		minX = *min_element(begin(vectorX), end(vectorX));

		maxY = *max_element(begin(vectorY), end(vectorY));
		minY = *min_element(begin(vectorY), end(vectorY));

		maxZ = *max_element(begin(vectorZ), end(vectorZ));
		minZ = *min_element(begin(vectorZ), end(vectorZ));

		vector<float> coordinateX;
		vector<float> coordinateY;
		vector<float> coordinateZ;

		//coordinateX.clear();
		//coordinateY.clear();
		//coordinateZ.clear();

		for (int i = 0; i < ROWNUM; i++) {
			float coeffX = 1.5*AXES_LEN / (maxX - minX); //求坐标系下的坐标数值
			coordinateX.push_back(coeffX * vectorX[i]);

			float coeffY = 1.5*AXES_LEN / (maxY - minY);
			coordinateY.push_back(coeffY * vectorY[i]);

			float coeffZ = 2 / (maxZ - minZ);  ////深度的变化范围对应坐标系Z轴2个单位的长度
			coordinateZ.push_back(coeffZ * (vectorZ[i] - minZ));

		}
		x = coordinateX;
		y = coordinateY;
		z = coordinateZ;

		return 1;
	}


	return 0;
}


void ReadData::selectCoordinate() {

}

