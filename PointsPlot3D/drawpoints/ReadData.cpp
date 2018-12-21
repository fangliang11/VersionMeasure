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
		SELECTFINISHFLAG = true;
		MessageBox(NULL, strFilename, TEXT("��ѡ������"), 0);
	}

	TCHAR *tchar = strFilename;  // TCHAR��ת�� string ��
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
		MessageBox(NULL, TEXT("��ȡ����ʧ��\n\n��ѡ������������ļ�������"), TEXT("����"), 0);

		return 0;
	}
	else if (myfile.is_open()) {

		vector<string> vec;
		vector<float> vectorX;
		vector<float> vectorY;
		vector<float> vectorZ;

		string temp;
		while (getline(myfile, temp))                    //����getline������ȡÿһ�У���������Ϊ��λ���뵽vector
		{
			vec.push_back(temp);
		}
		myfile.close();
		ROWNUM = vec.size();
		//cout << " the num of row is: " << ROWNUM << endl;
		for (auto it = vec.begin(); it != vec.end(); it++)
		{
			//cout << *it << endl;
			istringstream is(*it);                    //��ÿһ�е����ݳ�ʼ��һ���ַ�����������
			string s;
			int pam = 1;                              //�ӵ�һ�п�ʼ

			while (is >> s)                          //�Կո�Ϊ�磬��istringstream������ȡ�����뵽����s��
			{
				if (pam == 3)                       //��ȡ�� P �е�����
				{
					float r = atof(s.c_str());     //����������ת������string����ת����float
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

		return 1;
	}

	return 0;
}

int ReadData::readFile(string filename, int AXES_LEN, int column_num_X, int column_num_Y, int column_num_Z,
	                   int &ROWNUM, vector<float> &x, vector<float> &y, vector<float> &z) {

	ifstream myfile(filename);
	if (!myfile.is_open()) {
		MessageBox(NULL, TEXT("��ȡ����ʧ��\n\n��ѡ������������ļ�������"), TEXT("����"), 0);

		return 0;
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
			vec.push_back(temp);
		}
		myfile.close();
		ROWNUM = vec.size();
		//cout << " the num of row is: " << ROWNUM << endl;
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

		//coordinateX.clear();
		//coordinateY.clear();
		//coordinateZ.clear();

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

		return 1;
	}


	return 0;
}


void ReadData::selectCoordinate() {

}

