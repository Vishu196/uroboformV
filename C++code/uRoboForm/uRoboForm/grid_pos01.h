#pragma once
#include "find_edges.h"
#include "grid.h"
#include"Evaluation.h"
#include "signal_evaluation.h"
#include "constants.h"
#include "cqueue.h"
#include "debug_logs.h"
#include <numeric>
#include <chrono>
#include <fstream>
#include <iostream>


struct stage34
{
	cv::Mat img;
	int gridRows;
	int gridCols;
	Grid** grids; 

	stage34() :gridRows(0), gridCols(0), grids(0)
	{}
	friend std::ostream& operator<<(std::ostream& ostr, const stage34& s34);

};

struct subPX
{
	std::vector<double> max_pos;
	std::vector<double> pres;
};

struct FP
{
	std::vector<int> stripes;
	std::vector<double> s_dic;
};

class grid_pos01
{
private:
	
	std::vector<double> gradient(const std::vector<double> &x);
	struct FP Find_Peaks(const std::vector<double> &arr, double dist, double prom);
	cv::Mat cutGrid(const cv::Mat &grid_rot);
	void subpx_max_pos(const cv::Mat &cutGrid, string mode, subPX& p);
	void subpx_gauss(const std::vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m, subPX& p);
	void subpx_parabel(const std::vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m, subPX& p);
	void subpx_phase(const cv::Mat &cutGrid, subPX& p);
	std::vector<double> get_mean_grad(stage23& s23, const int row, const int col);
	cv::Mat get_gridrot(stage23& s23, const int row, const int col, std::string& orientation);
	cqueue<stage34> fifo;

public:

	void Execute(struct stage23 s23);
	stage34 getNext()
	{
		stage34 s34;
		fifo.pop(s34);
		return s34;
	}
};

