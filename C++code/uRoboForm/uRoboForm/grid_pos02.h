#pragma once
#include "grid_pos01.h"
#include "Evaluation.h"
#include "cqueue.h"
#include <list>

using namespace std;

struct stage45
{
	int gridRows;
	int gridCols;
	Grid** grids;
	int index;
	double k;
	string ind_ori;

	stage45() :gridRows(0), gridCols(0), index(0), k(0.0), grids(0),ind_ori()
	{};
};

struct RdBinary
{
	int index;
	string ind_ori;

	RdBinary() : index(0), ind_ori()
	{};
};

class grid_pos02
{
private:

	Grid** checkGrid(Grid** &grids01, int gRows, int gCols);
	vector<int> linspace(double start, double end, int num);
	struct RdBinary ReadBinary(Grid** &cgrids, const Mat &img);
	int get_mask_pos(Grid field, int row, int col, size_t i_max);
	double calc_d_k(vector<vector<double>> lines);
	double get_d_k(Grid** &cgrids, int gRows, int gCols);
	void DisplayResult(const stage45& s45);
	cqueue<stage45> fifo;

public:

	void Execute(stage34 s34);
	stage45 getNext()
	{
		stage45 s45;
		fifo.pop(s45);
		return s45;
	}
};

