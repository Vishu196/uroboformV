#include "grid_pos01.h"

using namespace std;
using namespace cv;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

std::ostream& operator<<(std::ostream& ostr, const stage34& s34)
{
	std::cout << "GridRows: " << s34.gridRows << endl;
	std::cout << "GridCols: " << s34.gridCols << endl;
	std::cout << "Stage 3 complete" << endl;
	return ostr;
}

vector<double> grid_pos01::gradient(const vector<double> &x)
{
	const auto x_size = x.size();
	const int dx = 1;
	vector<double> grad(x_size);

	grad[0] = (x[1] - x[0]) / dx;

	for (auto i = 1; i <= (x_size-2); ++i)
		grad[i] = (x[i + 1] - x[i - 1]) / (2 * dx);  // for i in [1,N-2]

	grad[x_size - 1] = (x[x_size - 1] - x[x_size - 2]) / dx;

	return grad;
}

Mat grid_pos01::cutGrid(const Mat &grid_rot)
{
	int grid_cutRows = 0;
	Mat grid_cut(grid_rot.rows, grid_rot.cols, CV_8U, (int)grid_rot.step);
	uint8_t* grid_cutData = grid_cut.data;

	const int len = grid_rot.rows / 2;
	const int wid = grid_rot.cols / 2;

	Mat grid_rot2(len,wid,CV_8U);
	resize(grid_rot, grid_rot2, Size(), 0.5, 0.5, cv::INTER_NEAREST);
	
	vector<double> mean_row =  Evaluation::Mean1R(grid_rot2);
	const double im_mean = Evaluation::MeanR(mean_row);

	double max_val, min_val;
	minMaxLoc(grid_rot2, &min_val, &max_val);

	const int val_range = (int)(max_val - min_val);

	vector<int> where_out;
	where_out.reserve(10);
	for (int i = 0; i < len; ++i)
	{
		if (((mean_row[i] < im_mean - 0.075 * val_range) || (mean_row[i] >= im_mean + 0.075 * val_range)) && (i < len / 4 || i >= len * 3 / 4)) 
			where_out.emplace_back(i);
	}

	if (where_out.size() >= 1) 
	{
		where_out.insert(where_out.begin(), 0);
		where_out.push_back(len - 1);

		vector <int> where_arg1 = Evaluation::decumulate(where_out);
		const int where_arg = max_element(where_arg1.begin(), where_arg1.end()) - where_arg1.begin();
		const int x11 = where_out[where_arg] * 2;
		const int x20 = where_arg + 1;
		const int x22 = where_out[x20] * 2;
		grid_cutRows = x22 - x11;
		
		Mat sourceRegion = grid_rot.rowRange(x11, x22);
		sourceRegion.copyTo(grid_cut.rowRange(0, x22 - x11));
		grid_cut.resize(grid_cutRows);
	}
	else
		grid_cut = grid_rot.clone();

	return grid_cut;
}

struct FP grid_pos01::Find_Peaks(const vector<double>& arr, double dist, double prom)
{
	size_t n = arr.size();
	vector<int> peaksIndices(50);
	vector<double> peaksValues(50);
	vector<double> peaksProminence(50);
	int count = 0;
	int a = 0;

	for (int i = 1; i < n - 1; ++i)
	{
		if (arr[i] >= arr[i - 1] && arr[i] >= arr[i + 1])
		{
			peaksIndices[a] = i;
			peaksValues[a] = arr[i];
			a++;
			if (peaksIndices[a] - peaksIndices[a - 1] < dist)
			{
				peaksIndices[a] = peaksIndices[a + 1];
				peaksValues[a] = peaksValues[a + 1];
				count++;						
			}	
		}
	}
	
	for (int i = 0; i < count; ++i) {
		int peakIndex = peaksIndices[i];

		// Find left and right bases
		int leftBaseIndex = peakIndex;
		while (leftBaseIndex > 0 && arr[leftBaseIndex - 1] < arr[leftBaseIndex]) 
		{
			--leftBaseIndex;
		}

		int rightBaseIndex = peakIndex;
		while (rightBaseIndex < arr.size() - 1 && arr[rightBaseIndex + 1] < arr[rightBaseIndex]) 
		{
			++rightBaseIndex;
		}

		double leftBaseValue = arr[leftBaseIndex];
		double rightBaseValue = arr[rightBaseIndex];

		//double peakValue = arr[peakIndex];

		// Calculate prominence as the difference between peak value and the maximum of left and right bases
		peaksProminence[i] = peaksValues[i] - std::max(leftBaseValue, rightBaseValue);
		if (peaksProminence[i] < prom)
		{
			peaksValues.erase(peaksValues.begin() + i);
			peaksIndices.erase(peaksIndices.begin() + i);
			count--;
		}
	}
	
	peaksIndices.resize(count);
	peaksValues.resize(count);

	FP peaks;
	peaks.stripes = peaksIndices;
	peaks.s_dic = peaksValues;

	return peaks;
}

//to do
double* gauss_limited(double x, double k, double sigma, double mu, double offset,  int max_cut)
{
	return 0;
}

void grid_pos01::subpx_gauss(const vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m, subPX& p)
{
	p.max_pos;
	p.pres;
	int xmin = 0;
	int xmax = 0;

	for (int i_b = 0; i_b < B_max.stripes.size(); ++i_b)
	{
		int mid = B_max.stripes[i_b];
		int b_min_size = B_min.stripes.size();
		if (b_min_size >=2)
		{
			for (int i_0 = 0; i_0 < b_min_size; ++i_0)
			{
				if (B_min.stripes[i_0] < mid)
					xmin = i_0;
				if (B_min.stripes[b_min_size - i_0] > mid)
					xmax = i_0;
			}
			/*for (int i_1 = 0; i_1 <= B_min.stripes.size(); i_1++)
			{
				if (B_min.stripes[B_min.stripes.size() - i_1] > mid)
					xmax = i_1;
			}*/

			//try
			//{
			//	if (xmin!=xmax)
			//	{
			//		//to do
			//		//curve fit function pending
			//	}
			//}
			//catch (const runtime_error)
			//{
			//	//cout << "Optimal Parameters not found for image at x =' " << mid << endl;
			//}
			//catch (std::exception&)
			//{
			//	continue;
			//}
		}
	}
}

void grid_pos01::subpx_parabel(const vector<double> &B_cut, struct FP B_max, struct FP B_min, double d_m, subPX& p)
{
	for (int va = 0; va < B_max.stripes.size(); ++va)
	{
		int i_b = B_max.stripes[va];
		int mid = i_b;

		if (B_min.stripes.size() >= 2)
		{
			int xmin{};
			int xmax{};

			for (int vb = 0; vb < B_min.stripes.size(); ++vb)
			{
				int i_0 = B_min.stripes[vb];
				if (i_0 < mid)
					xmin = int(i_b - d_m/4)+3;
			}
			for (int c = B_min.stripes.size()-1; c>=0 ; c--)
			{
				int i_1 = B_min.stripes[c];
				if (i_1 > mid)
					xmax = int(i_b + d_m / 4);
			}

			try
			{
				if ((xmin != xmax) && (xmin != 0) && (xmax != 0))
				{
					int x_size = xmax - xmin;
					vector<int> x (x_size);
					iota(x.begin(), x.end(), xmin);
					
					Mat W0 = Mat::eye(x_size,x_size, CV_64F);
					Mat Phi(x_size, 3, CV_64F);					
					Phi.col(0).setTo(1);

					for (int i = 0; i < Phi.rows; ++i)
					{
						//Phi.at<double>(i, 0) = 1;
						Phi.at<double>(i, 1) = x[i];
						Phi.at<double>(i, 2) = x[i] * x[i];
					}
					/*cout << "Phi:" << Phi << endl;
					cout << "W0:" << W0 << endl;*/
					Mat PhiT = Phi.t();
					
					int u = 0;
					Mat B_cut01(x_size, 1, CV_64F);
					copy(B_cut.begin() + xmin, B_cut.begin() + xmax, B_cut01.ptr<double>());

					Mat a_dach0(3, 1, CV_64F);
					a_dach0 = (PhiT * W0 * Phi).inv() * PhiT * W0 * B_cut01;
					p.max_pos.emplace_back(-a_dach0.at<double>(1,0) / a_dach0.at<double>(2,0) / 2);
				}
			}
			catch (const exception)
			{
				continue;
			}
		}
	}
	p.pres = {};
}

void grid_pos01::subpx_phase(const Mat &cutGrid, subPX& p)
{
	vector<double> B0 = Evaluation::Mean0R(cutGrid);
	double B = Evaluation::MeanR(B0);
	int y = cutGrid.cols;
	if (y >=60)
	{
		struct MFreq m = signal_evaluation::Main_FreqR(B0, 0, y);
		double d_mean = 1 / m.f_g;

		vector<int> B_arange(y);
		iota(begin(B_arange), end(B_arange), 0);

		vector<complex<double>> exp_fac;
		vector<complex<double>> F_k1(y);
		complex<double> F_k = (0, 0); 

		for (int i = 0; i < y; ++i)
		{
			exp_fac[i] = (0, (-2 * M_PI) * (B_arange[i]) / y);
			F_k1[i] = m.Image_window[i] * exp(exp_fac[i] * m.n_g);
			F_k += F_k1[i];
		}

		double Phi = std::arg(F_k);
		double d = Phi / (2 * M_PI * m.f_g);

		double A1 = (real(F_k))* (real(F_k));
		double A2 = (imag(F_k)) * (imag(F_k));
		double A = sqrt((A1 + A2));

		size_t lenImg = m.Image_window.size();
		vector<int> ImgWin_arange(lenImg);
		iota(begin(ImgWin_arange), end(ImgWin_arange), 0);

		vector<double> y1(lenImg);
		vector<double> y_cos(lenImg);
		for (int i = 0; i < lenImg; ++i)
		{
			y1[i] = (2 * M_PI * m.f_g * (ImgWin_arange[i])) - Phi;
			y_cos[i] = A * cos(y1[i]);
		}
		if (d < d_mean/2)
			d += d_mean;

		for (int i_max = 0; i_max < (m.n_g-1); ++i_max)
			p.max_pos.emplace_back(d + i_max * d_mean);
	}
	else
		p.max_pos.clear();
	
	p.pres.clear();
}

void grid_pos01::subpx_max_pos(const Mat& cutGrid, string mode, subPX &p)
{
	int y = cutGrid.cols;
	
	double px_size0 = px_size / 1000;
	if (mode == "phase")
	{
		subpx_phase(cutGrid, p);
	}
	else
	{
		//vector<double>B = Evaluation::Mean0R(cutGrid);

		double filt = (double)y/10;
		vector<double> B_cut = signal_evaluation::Bandfilter(Evaluation::Mean0R(cutGrid), 0, (int)filt);		
		double d_min = stripe_width / (double)1000 * 2 / px_size0 / 2;
		double prom = ((*std::max_element(B_cut.begin(), B_cut.begin() + y)) - (*std::min_element(B_cut.begin(), B_cut.begin() + y)))*0.2;
		
		vector<double> B_cut_N(y);
		transform(B_cut.begin(), B_cut.end(), B_cut_N.begin(), std::negate<double>());

		FP B_max = Find_Peaks(B_cut, d_min, prom);
		FP B_min = Find_Peaks(B_cut_N, d_min, 0.0);

		if ((B_max.stripes.size() >= 1) && (B_min.stripes.size() >= 1))
		{
			if ((B_min.stripes[0]>B_max.stripes[0]) && (B_max.stripes[0] >= 0.9*d_min))
				B_min.stripes.insert(B_min.stripes.begin(),0);
			if ((B_min.stripes[B_min.stripes.size() - 1] < B_max.stripes[B_max.stripes.size() - 1]) && (y - B_max.stripes[B_max.stripes.size() - 1] >= 0.9 * d_min))
				B_min.stripes.insert(B_min.stripes.end(), (y - 1));
			if ((B_max.stripes[0] - B_min.stripes[0] > 0) && (B_max.stripes[0] - B_min.stripes[0] < 0.8*d_min))
				B_min.stripes.erase(B_min.stripes.begin());

			double d_m = Evaluation::MeanR(Evaluation::decumulate(B_max.stripes));
			//to do
			if (mode == "gauss")
			{
			   subpx_gauss(B_cut, B_max,B_min, d_m, p);
			}

			else if (mode == "parabel")
			{
			    subpx_parabel(B_cut, B_max, B_min, d_m,p);
			}
		}
	}
	//return p;
}

void get_grids(stage23 &s23, stage34 &s34)
{
	if (s23.cut_ver.front() * 2 < 10)
		s23.cut_ver.erase(s23.cut_ver.begin());

	if (s23.cut_hor.front() * 2 < 10)
		s23.cut_hor.erase(s23.cut_hor.begin());

	s23.cut_hor.insert(s23.cut_hor.begin(),0);
	s23.cut_ver.insert(s23.cut_ver.begin(), 0);

	s23.cut_hor.push_back(s23.img.rows / 2);
	s23.cut_ver.push_back(s23.img.cols / 2);

	s34.grids = new Grid * [s23.cut_hor.size()];
	for (int h = 0; h < (int)s23.cut_hor.size(); ++h)
	{
		s34.grids[h] = new Grid[(int)s23.cut_ver.size()];
	}

	//return s34.grids;
}

Mat grid_pos01::get_gridrot(stage23& s23, const int row, const int col, string &orientation)
{
	const int x1 = s23.cut_hor[row] * 2;
	const int x2 = (s23.cut_hor[row + 1] * 2);
	const int y1 = s23.cut_ver[col] * 2;
	const int y2 = (s23.cut_ver[col + 1] * 2);
	const int s1 = x2 - x1;
	const int s2 = y2 - y1;

	Mat grid0(s1, s2, CV_8U);
	uint8_t* grid0Data = grid0.data;
	Mat grid00(s1, s23.img.cols, CV_8U);

	if (x2 > x1 && y2 > y1)
	{
		Mat sourceRegion = s23.img.rowRange(x1, x2);
		sourceRegion.copyTo(grid00.rowRange(0, s1));
		Mat sourceRegion1 = grid00.colRange(y1, y2);
		sourceRegion1.copyTo(grid0.colRange(0, s2));
	}

	vector<double> mean_grad = get_mean_grad(s23, row, col);

	if (mean_grad[1] > mean_grad[0])
	{
		orientation = "hor";
		return grid0.t();
	}
	else
	{
		orientation = "ver";
		return grid0;
	}
}

vector<double> grid_pos01::get_mean_grad(stage23 &s23, const int row, const int col)
{
	const int x11 = s23.cut_hor[row ];
	const int x22 = s23.cut_hor[row + 1];
	const int y11 = s23.cut_ver[col];
	const int y22 = s23.cut_ver[col + 1];
	const int w1 = x22 - x11;
	const int w2 = y22 - y11;

	Mat mean22(w1, s23.img2.cols, CV_8U);
	Mat mean2 (w1, w2, CV_8U, (int)s23.img2.step);
	uint8_t* mean2Data = mean2.data;

	if (x22 > x11 && y22 > y11)
	{
		Mat sourceRegion = s23.img2.rowRange(x11, x22);
		sourceRegion.copyTo(mean22.rowRange(0, w1));
		Mat sourceRegion1 = mean22.colRange(y11, y22);
		sourceRegion1.copyTo(mean2.colRange(0, w2));
	}

	vector<double> mean_grad00 = gradient(signal_evaluation::Bandfilter(Evaluation::Mean0R(mean2), 0, (w2 / 6)));
	vector<double> mean_grad11 = gradient(signal_evaluation::Bandfilter(Evaluation::Mean1R(mean2), 0, (w1 / 6)));

	vector<double> mean_grad0(w2);
	vector<double> mean_grad1(w1);

	int t = w1 > w2 ? w1 : w2;
	for (int i = 0; i < t; ++i)
	{
		if (i < w2)
			mean_grad0[i] = abs(mean_grad00[i]);
		if (i < w1)
			mean_grad1[i] = abs(mean_grad11[i]);
	}

	double mean_0grad0 = Evaluation::MeanR(mean_grad0);
	double mean_1grad1 = Evaluation::MeanR(mean_grad1);

	vector<double> mean_grad = {mean_0grad0, mean_1grad1 };
	return mean_grad;
}

void modify_max_pos(subPX &p)
{
	size_t r = p.max_pos.size();
	vector <double> max_pos_de = Evaluation::decumulate(p.max_pos);

	if ((r > 1) && (max_pos_de.back() > 65))
		p.max_pos.pop_back();

	if ((r > 1) && (max_pos_de[0] > 65))
		p.max_pos.erase(p.max_pos.begin());
}

void display_time02(const chrono::steady_clock::time_point& t01,
	const chrono::steady_clock::time_point& t02)
{
	/* Getting number of milliseconds as an integer. */
	auto ms_int = duration_cast<milliseconds>(t02 - t01);
	duration<double, milli> ms_double = t02 - t01;
	cout << "grid_pos01 Time: " << ms_double.count() << "ms" << endl;
	cout << endl;
}

void grid_pos01::Execute(stage23 s23) 
{
	stage34 s34;
	string mode = "parabel";
	string orientation;
	s34.grids = {};

	get_grids(s23, s34);
	
	const int image_size = s23.img.cols * s23.img.rows;
	const double five_percent = image_size * 0.05;

	for (int row = 0; row < (s23.cut_hor.size()-1); ++row)
	{
		for (int col = 0; col < (s23.cut_ver.size()-1); ++col)
		{
			int s1 = (s23.cut_hor[row + 1] * 2) - (s23.cut_hor[row] * 2);
			int s2 = (s23.cut_ver[col + 1] * 2) - (s23.cut_ver[col] * 2);
		
			Mat grid_rot = get_gridrot(s23, row, col, orientation);
			struct subPX p;
			const int grid_rot_size = grid_rot.rows * grid_rot.cols;

			if ((grid_rot_size >= five_percent) || (orientation == "ver" && row == 0 && col == 1) || (orientation == "hor" && row == 1 && col == 0))
			{
				Mat grid_cut = cutGrid(grid_rot);
				subpx_max_pos(grid_cut, mode, p);
				modify_max_pos(p);
			}
			else
			{
				p.max_pos = {};
				p.pres.clear();
			}
			vector<int>coord = { (s23.cut_hor[row] * 2), (s23.cut_ver[col] * 2) };
			s34.grids[row][col] = Grid(grid_rot, orientation, coord, p.max_pos);
			
			/*vector<double> max_p = s34.grids[row][col].max_pos;
			for(auto vi:max_p)
				cout << vi << endl;*/
		}
	}
	s34.img = s23.img;
	s34.gridRows = (int)s23.cut_hor.size()-1;
	s34.gridCols = (int)s23.cut_ver.size()-1;

	fifo.push(s34);

	#if DEBUGMODE == DebugMode::AllTimesValues
	#elif DEBUGMODE == DebugMode::AllValues
	std::cout << s34;
	#endif
}

