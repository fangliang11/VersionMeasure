///////////////////////////////////////////////////////////////////////////////
// ReadData.h
// ===========
//read data file of .txt/.dat , and convert data into vector coordinate
//also do some compute to make the coordinate display in the 3D coordinate
//
//  AUTHOR: Fang Liang (fangliang1313@gmail.com)
// CREATED: 2018-12-20
// UPDATED: 2018-12-20
///////////////////////////////////////////////////////////////////////////////

#ifndef READDATA_H
#define READDATA_H

#include <vector>
#include <iostream>

using namespace std;

class ReadData
{
public:
	ReadData();
	~ReadData();


	wstring selectFile();
	string selectImage();
	bool readFile(string filename, int AXES_LEN, int column_num_X, int column_num_Y, int column_num_Z, int &ROWNUM, vector<float> &x, vector<float> &y, vector<float> &z);

	void show(string selectfilename);
	void selectCoordinate(); //ѡ��������
	
	//vector<float> coordinateX;
	//vector<float> coordinateY;
	//vector<float> coordinateZ;
	int ROWNUM;

	////���岢�����ṹ�����
	//typedef struct Coordinate {
	//	vector<float> coordinateX;
	//	vector<float> coordinateY;
	//	vector<float> coordinateZ;
	//	int ROWNUM;
	//};
	//struct Coordinate;
	//Coordinate getCoordinate(string filename, int AXES_LEN, vector<float> x, vector<float> y, vector<float> z);

	bool READFINISHFLAG;
	bool SELECTFILEFLAG;
	bool SELECTIMAGEFLAG;

};



#endif
