#pragma once
#include "raw_edges.h"

struct Detect_throu
{
	std::vector<int> through_loc;
	std::vector<int> cut_through;
};

struct indexes 
{
	double s_max;
	double s_min;

	indexes(): s_max(0.0),s_min(0.0)
	{}
};

struct stage23
{
	cv::Mat img;
	cv::Mat img2;
	std::vector<int> cut_hor;
	std::vector<int> cut_ver;

	stage23()
	{
		cut_hor = {};
		cut_ver = {};
	}
	friend std::ostream& operator<<(std::ostream& ostr, const stage23& s23);
};

class find_edges
{
private:
	
	indexes Line_Index(const std::vector<double> &mean_range_in, double th_edge,int i0,int rank);
	Detect_throu Detect_Through(const std::vector<double> &im_col, double th_edge);
	std::vector<int> Delete_Edges(std::vector<int> cut_arr, int ideal_d);
	std::vector <double> Execute_1(const stage12& s12, int& rank, indexes& index);
	void get_cut_hor(int& rank, const stage12& s12, stage23& s23);
	void get_cut_ver(int &rank, const stage12& s12, stage23& s23);
	cqueue<stage23> fifo;

public:
	void Execute(const stage12 &s12);
	stage23 getNext()
	{
		stage23 s23;
		fifo.pop(s23);
		return s23;
	}
};

