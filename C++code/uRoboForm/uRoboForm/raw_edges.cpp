#include "raw_edges.h"

static struct stage12 s12;

raw_edges::raw_edges(Mat ImageR)
{
	Mat image = ImageR;
}

Mat raw_edges::ImageSliceR(Mat image, int n)
{
	Mat imgCopy = Mat(image.rows / n, image.cols / n, CV_8U, (int)image.step / n);

	uint8_t* myData = image.data;
	uint8_t* myCopyData = imgCopy.data;
	int width = image.cols;
	int height = image.rows;
	int _stride = (int)image.step / n;//in case cols != strides
	for (int i = 0; i < height; i += n)
	{
		for (int j = 0; j < width; j += n)
		{
			int val = myData[i * image.step + j];
			int idx = (i / n * _stride + j / n);
			*(myCopyData + idx) = val;
		}
	}
	return imgCopy;
}

vector<vector<int>> raw_edges::Image2ArrayR(const Mat &imageR)
{
	uint8_t* myData = imageR.data;
	vector<vector<int>>array2D(imageR.rows, vector<int>(imageR.cols));

	for (int h = 0; h < imageR.rows; h++)
	{
		for (int w = 0; w < imageR.cols; w++)
		{
			array2D[h][w] = myData[h * imageR.step + w];
		}
	}
	return array2D;
}

vector<double> raw_edges::Mean0R(const Mat& image2)
{
	vector<double> Mean0Arr(image2.cols);

	double avg = 0.0;
	int sum = 0;
	int x = 0;
	for (int w = 0; w < image2.cols; w++)
	{
		sum = 0;
		for (int h = 0; h < image2.rows; h++)
		{
			x = image2.data[h * image2.step + w];
			sum += x;
		}
		avg = (double)sum / (double)image2.rows;
		Mean0Arr[w] = avg;
	}

	return Mean0Arr;
}

vector<double> raw_edges::Mean1R(const Mat& image2)
{
	vector<double> Mean1Arr(image2.rows);
	
	double avg = 0.0;
	double sum = 0.0;
	int x = 0;
	for (int h = 0; h < image2.rows; h++)
	{
		sum = 0;
		for (int w = 0; w < image2.cols; w++)
		{
			x = image2.data[h * image2.step + w];
			sum += (double)x;
		}
		avg = sum / (double)image2.cols;
		Mean1Arr[h] = avg;
	}
	
	return Mean1Arr;
}

double raw_edges::MeanR(const vector<double> &mean0)
{
	double sum = 0.0;
	double meanR = 0.0;
	for (int i = 0; i < mean0.size(); i++) 
	{
		sum += mean0[i];
	}

	meanR = sum / mean0.size();
	return meanR;
}

double raw_edges::Median(vector<double> array)
{
	int size = (int)array.size();
	for (int i = 0; i < size; i++) 
	{
		for (int j = i; j < (size-1); j++) 
		{
			const int w = j + 1;
			if (array[i] > array[w])
			{
				swap(array[i], array[w]);
				/*double temp = array[i];
				array[i] = array[w];
				array[w] = temp;*/
			}
		}
	}

	int a = size - 1;
	if (size % 2 != 0)
		return (double)array[size / 2];
	else
		return (double)(array[a / 2] + array[size / 2]) / 2.0;

}

vector<double> raw_edges::BlackmanWindowR(int n)
{
	const double a0 = 0.42;
	const double a1 = 0.5;
	const double a2 = 0.08;
	int wLen = n - 1;
	vector<double> wFun(n);

	for (int i = 0; i < n; ++i)
	{
		double wi = 0.0;
		wi = a0 - (a1 * cos((2 * M_PI * i) / wLen)) + (a2 * cos((4 * M_PI * i) / wLen));
		wFun[i] = wi;
	}
	/*wFun.shrink_to_fit();*/
	return wFun;
}

vector<double> raw_edges::FFTR(const vector<double> &image_windowR)
{
	int size = (int)image_windowR.size();
	const int N = 256;
	fftw_complex*  y = 0;
	y = new fftw_complex[N];
	double in[N]{};
	fftw_plan p;

	for (int i = 0; i < N; i++) {
		if (i < size) {
			in[i] = image_windowR[i];
		}
		else {
			in[i] = 0;
		}
	}
	p = fftw_plan_dft_r2c_1d(N, in, y, FFTW_ESTIMATE);//fftw_plan_dft_1d(N, in, y, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);
	std::complex<double>* yy;
	yy = reinterpret_cast<std::complex<double> *>(y);
	vector<double> y1(N);

	for (int i = 0; i < N; i++)
	{
		y1[i] = abs(yy[i]);
	}

	fftw_destroy_plan(p);
	//delete[] y;

	return y1;
}

double raw_edges::Spek_InterpolR(const vector<double> &A) {

	uint32_t A_size = 256;
	uint32_t A2_size = A_size / 2;

	vector<double> A2 (A2_size);
	for (uint32_t i = 0; i < A2_size; i++)
	{
		A2[i] = A[i];
	}

	int n_0 = (int)std::distance(A2.begin(), std::max_element(A2.begin(), A2.begin() + A2_size));

	int a = n_0 + 1;
	int b = n_0 - 1;
	double y_ln1 = log(A[a]);
	double y_ln0 = log(A[n_0]);
	double y_ln_1 = log(A[b]);
	double tmp = (y_ln_1 - y_ln1) / (y_ln_1 - (2 * y_ln0) + y_ln1);
	double n_g = (n_0 + tmp / 2);

	return n_g;
}

double raw_edges::Main_FreqR(const vector<double> &B0, int start, int stop)
{
	double f_g = 0.0;
	const int size = stop - start;

	vector<double> B(size);

	vector<double> image_window(size);
	
	for (int(k) = 0; k < size; k++) 
	{
		int w = k + start;
		B[k] = B0[w];
	}
	
	double Mean = MeanR(B);

	vector<double> B1(size);

	for (int i = 0; i < size; i++)
	{
		double x = B[i] - Mean;
		B1[i] = x;
	}

	vector<double> wFun = BlackmanWindowR(size);
	
	for (int i = 0; i < size; i++)
	{
		double iw = B1[i] * wFun[i];
		image_window[i] = iw;
	}

	vector<double> y1 = FFTR(image_window);

	double n_g = Spek_InterpolR(y1);
	uint32_t size_B = size;
	f_g = n_g / size_B;

	return f_g;
}

double raw_edges::Calc_main_d(const vector<double> &mean0, int freq_range)
{
	double main_d;
	const int n1 = (((int)mean0.size() - freq_range) / 50) + 1;
	
	vector<double> t1(n1);
	int j = 0;

	for (int i = 0; i < (mean0.size() - freq_range); i += 50)
	{
		double tmp = Main_FreqR(mean0, i, i + freq_range);
		t1[j] = (1 / tmp);
		j++;
	}

	main_d = Median(t1);

	return main_d;
}

struct stage12 raw_edges::ExecuteR(Mat Image, int freq_range)
{
	//vector<vector<int>> ImgArr = Image2ArrayR(Image);
	Mat Image2 = ImageSliceR(Image, 2);
	//vector<vector<int>> ImgArr2 = Image2ArrayR(Image2);
	
	s12.mean0 = Mean0R(Image2);
	s12.main_d_0 = Calc_main_d(s12.mean0, freq_range);
	
	s12.mean1 = Mean1R(Image2);
	s12.main_d_1 = Calc_main_d(s12.mean1, freq_range);

	s12.th_edge = MeanR(s12.mean0);

	s12.img = Image;
	s12.img2 = Image2;
	
	return s12;
}
