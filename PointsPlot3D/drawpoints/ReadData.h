#ifndef READDATA_H
#define READDATA_H

#include <vector>
using namespace std;

class ReadData
{
public:
	ReadData();
	~ReadData();


	string selectFile();
	int readFile(string filename, int AXES_LEN,int &ROWNUM, vector<float> &x, vector<float> &y, vector<float> &z);
	int readFile(string filename, int AXES_LEN, int column_num_X, int column_num_Y, int column_num_Z, int &ROWNUM, vector<float> &x, vector<float> &y, vector<float> &z);

	void show(string selectfilename);
	void selectCoordinate(); //选择数据列
	
	//vector<float> coordinateX;
	//vector<float> coordinateY;
	//vector<float> coordinateZ;
	int ROWNUM;

	////定义并声明结构体变量
	//typedef struct Coordinate {
	//	vector<float> coordinateX;
	//	vector<float> coordinateY;
	//	vector<float> coordinateZ;
	//	int ROWNUM;
	//};
	//struct Coordinate;
	//Coordinate getCoordinate(string filename, int AXES_LEN, vector<float> x, vector<float> y, vector<float> z);

	bool READFINISHFLAG = false;
	bool SELECTFINISHFLAG = false;

};



#endif
