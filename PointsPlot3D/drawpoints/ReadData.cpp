///////////////////////////////////////////////////////////////////////////////
// ReadData.cpp
// ===========
//read data file of .txt/.dat , and convert data into vector coordinate
//also do some compute to make the coordinate display in the 3D coordinate
//
//  AUTHOR: Fang Liang (fangliang1313@gmail.com)
// CREATED: 2018-12-20
// UPDATED: 2018-12-20
///////////////////////////////////////////////////////////////////////////////

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

wstring ReadData::selectFile() {

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���

	ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С
	ofn.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
	//ofn.lpstrFilter = TEXT("�����ļ�\0*.*\0.txt/.dat Flie\0*.txt;*.dat\0\0");//���ù���
	ofn.lpstrFilter = TEXT(".txt/.dat �ļ�\0*.txt;*.dat\0\0");//���ù���

	ofn.nFilterIndex = 1;//����������
	ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
	ofn.nMaxFile = sizeof(strFilename);//����������
	ofn.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��
	ofn.lpstrTitle = TEXT("��ѡ�������ļ�");//ʹ��ϵͳĬ�ϱ������ռ���
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
	if (GetOpenFileName(&ofn))
	{
		SELECTFILEFLAG = true;
		//MessageBox(NULL, strFilename, TEXT("��ѡ������"), MB_OK | MB_SYSTEMMODAL | MB_ICONINFORMATION);
	}
	else SELECTFILEFLAG = false;

	TCHAR *tchar = strFilename;  // TCHAR��ת�� string ��
	wstring ws(tchar);
	//string str(ws.begin(), ws.end());

	return ws;
}

string ReadData::selectImage() {

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���

	ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С
	ofn.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
	//ofn.lpstrFilter = TEXT("�����ļ�\0*.*\0.png/.jpg/.bmp Flie\0*.png;*.jpg;*.bmp\0\0");//���ù���
	ofn.lpstrFilter = TEXT("�����ļ�\0*.*\0.png/.jpg/.bmp �ļ�\0*.png;*.jpg;*.bmp\0\0");//���ù���

	//ofn.lpstrFilter = TEXT(".png/.jpg/.bmp �ļ�\0*.png;*.jpg;*.bmp\0\0");//���ù���

	ofn.nFilterIndex = 1;//����������
	ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
	ofn.nMaxFile = sizeof(strFilename);//����������
	ofn.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��
	ofn.lpstrTitle = TEXT("��ѡ��ͼ���ļ�");//ʹ��ϵͳĬ�ϱ������ռ���
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
	if (GetOpenFileName(&ofn))
	{
		SELECTIMAGEFLAG = true;
		//MessageBox(NULL, strFilename, TEXT("��ѡ������"), MB_OK | MB_SYSTEMMODAL | MB_ICONINFORMATION);
	}
	else SELECTIMAGEFLAG = false;

	TCHAR *tchar = strFilename;  // TCHAR��ת�� string ��
	wstring ws(tchar);
	string str(ws.begin(), ws.end());

	return str;
}

void ReadData::show(string selectfilename) {

	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONIN$", "r+t", stdin);
	printf("%s\n", selectfilename.c_str());

}


bool ReadData::readFile(string filename, int AXES_LEN, int column_num_X, int column_num_Y, int column_num_Z,
	                   int &ROWNUM, vector<float> &x, vector<float> &y, vector<float> &z) {

	ifstream myfile(filename);
	if (!myfile.is_open()) {
		MessageBox(NULL, TEXT("��ȡ����ʧ��\n\n��������ȷ���ļ���"), TEXT("����"), MB_OK| MB_SYSTEMMODAL | MB_ICONERROR);

		return false;
	}
	else if (myfile.is_open()) {

		vector<string> vec;
		vector<float> vectorX;
		vector<float> vectorY;
		vector<float> vectorZ;

		//fseek(myfile, 100, SEEK_SET);

		string temp;
		while (getline(myfile, temp))                    //����getline������ȡÿһ�У���������Ϊ��λ���뵽vector
		{
			if (!temp.empty()) {
				if (temp[0] == 'x' || temp[0] == '#') {
					temp.clear();  //������ͷ
				}
				else vec.push_back(temp);
			}
		}
		myfile.close();

		ROWNUM = vec.size();
		for (auto it = vec.begin(); it != vec.end(); it++)
		{
			//cout << *it << endl;
			istringstream is(*it);                    //��ÿһ�е����ݳ�ʼ��һ���ַ�����������
			string s;
			int pam = 1;                              //�ӵ�һ�п�ʼ

			while (is >> s)                          //�Կո�Ϊ�磬��istringstream������ȡ�����뵽����s��
			{
				if (pam == column_num_X)                       //��ȡ�� numX �е�����
				{
					float r = atof(s.c_str());     //����������ת������string����ת����float
					vectorX.push_back(r);
				}
				if (pam == column_num_Y)                     //��ȡ�� numY �е�����
				{
					float y = atof(s.c_str());
					vectorY.push_back(y);
				}
				if (pam == column_num_Z)                     //��ȡ�� numZ �е�����
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

		for (int i = 0; i < ROWNUM; i++) {
			float coeffX = 1.5*AXES_LEN / (maxX - minX); //������ϵ�µ�������ֵ
			coordinateX.push_back(coeffX * vectorX[i]);

			float coeffY = 1.5*AXES_LEN / (maxY - minY);
			coordinateY.push_back(coeffY * vectorY[i]);

			float coeffZ = 2 / (maxZ - minZ);  ////��ȵı仯��Χ��Ӧ����ϵZ��2����λ�ĳ���
			coordinateZ.push_back(coeffZ * (vectorZ[i] - minZ));

		}
		x = coordinateX;
		y = coordinateY;
		z = coordinateZ;

	}
	return true;
}


void ReadData::selectCoordinate() {

}

