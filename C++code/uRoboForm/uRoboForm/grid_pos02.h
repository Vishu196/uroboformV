#pragma once
#include "grid_pos01.h"
#include "grid.h"
#include "cqueue.h"
#include <vector>
#include <opencv2/opencv.hpp>



struct stage45
{
	int gridRows;
	int gridCols;
	Grid** grids;
	int index;
	bool is_hor;

	stage45() :gridRows(0), gridCols(0), grids(0), index(0), is_hor()
	{};
	friend std::ostream& operator<<(std::ostream& ostr, const stage45& s45);
};

struct RdBinary
{
	int index;
	bool is_hor;

	RdBinary() : index(0), is_hor()
	{};
};

class grid_pos02
{
	friend class grid_pos03;
private:

	Grid** checkGrid(const stage34& s34);
	std::vector<int> linspace(double start, double end, int num);
	RdBinary ReadBinary(const stage45& s45, const cv::Mat& img);
	int static get_mask_pos(Grid field, int row, int col, size_t i_max);
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

