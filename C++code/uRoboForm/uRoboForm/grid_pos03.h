#pragma once
#include "grid_pos02.h"
#include "grid.h"
#include "cqueue.h"
#include <vector>


struct stage56
{
	int gridRows;
	int gridCols;
	Grid** grids;
	double k;
	int index;
	bool is_hor;
	double xi;
	double zi;

	stage56() :gridRows(0), gridCols(0), grids(0), k(0.0), index(0), xi(0.0), zi(0.0)
	{};
	friend std::ostream& operator<<(std::ostream& ostr, const stage56& s56);
};

struct axis
{
	vector<vector<double>> center_hor;
	vector<vector<double>> center_ver;
};

class grid_pos03
{
private:

	double calc_d_k(std::vector<vector<double>> lines);
	double get_d_k(const stage45& s45);
	std::vector<vector<list<int>>> grid_params(void);
	double weighted_avg(const std::vector<vector<double>> &center);
	list<int> get_look_el(const stage45& s45);
	axis get_center_arr(const stage45& s45, const stage56& s56);
	cqueue<stage56> fifo;

public:
	stage56 Execute(stage45 s45);
	stage56 getNext()
	{
		stage56 s56;
		fifo.pop(s56);
		return s56;
	}
};

