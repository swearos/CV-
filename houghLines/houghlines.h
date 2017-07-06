#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using std::string;

// Classical Hough Transform
struct LinePolar
{
	float rho;
	float angle;
};

struct hough_cmp_gt
{
	hough_cmp_gt(const int* _aux) : aux(_aux) {}
	bool operator()(int l1, int l2) const
	{
		return aux[l1] > aux[l2] || (aux[l1] == aux[l2] && l1 < l2);
	}
	const int* aux;
};

/*
Here image is an input raster;
step is it's step; size characterizes it's ROI;
rho and theta are discretization steps (in pixels and radians correspondingly).
threshold is the minimum number of pixels in the feature for it
to be a candidate for line. lines is the output
array of (rho, theta) pairs. linesMax is the buffer size (number of pairs).
Functions return the actual number of found lines.
*/
static void
HoughLinesStandard(const cv::Mat& img, float rho, float theta,
int threshold, std::vector<cv::Vec2f>& lines, int linesMax,
double min_theta, double max_theta)
{
	int i, j;
	float irho = 1 / rho;

	CV_Assert(img.type() == CV_8UC1);

	const uchar* image = img.ptr();
	int step = (int)img.step;
	int width = img.cols;
	int height = img.rows;

	if (max_theta < min_theta) {
		CV_Error(CV_StsBadArg, "max_theta must be greater than min_theta");
	}

	int numangle = cvRound((max_theta - min_theta) / theta);
	int numrho = cvRound(((width + height) * 2 + 1) / rho);

	Buffer<int> _accum((numangle + 2) * (numrho + 2));
	std::vector<int> _sort_buf;
	Buffer<float> _tabSin(numangle);
	Buffer<float> _tabCos(numangle);
	int *accum = _accum;
	float *tabSin = _tabSin, *tabCos = _tabCos;

	memset(accum, 0, sizeof(accum[0]) * (numangle + 2) * (numrho + 2));

	float ang = static_cast<float>(min_theta);
	for (int n = 0; n < numangle; ang += theta, n++)
	{
		tabSin[n] = (float)(sin((double)ang) * irho);
		tabCos[n] = (float)(cos((double)ang) * irho);
	}

	// stage 1. fill accumulator
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			if (image[i * step + j] != 0)
				for (int n = 0; n < numangle; n++)
				{
					int r = cvRound(j * tabCos[n] + i * tabSin[n]);
					r += (numrho - 1) / 2;
					accum[(n + 1) * (numrho + 2) + r + 1]++;
				}
		}

	// stage 2. find local maximums
	for (int r = 0; r < numrho; r++)
		for (int n = 0; n < numangle; n++)
		{
			int base = (n + 1) * (numrho + 2) + r + 1;
			if (accum[base] > threshold &&
				accum[base] > accum[base - 1] && accum[base] >= accum[base + 1] &&
				accum[base] > accum[base - numrho - 2] && accum[base] >= accum[base + numrho + 2])
				_sort_buf.push_back(base);
		}

	// stage 3. sort the detected lines by accumulator value
	std::sort(_sort_buf.begin(), _sort_buf.end(), hough_cmp_gt(accum));

	// stage 4. store the first min(total,linesMax) lines to the output buffer
	linesMax = std::min(linesMax, (int)_sort_buf.size());
	double scale = 1. / (numrho + 2);
	for (i = 0; i < linesMax; i++)
	{
		LinePolar line;
		int idx = _sort_buf[i];
		int n = cvFloor(idx*scale) - 1;
		int r = idx - (n + 1)*(numrho + 2) - 1;
		line.rho = (r - (numrho - 1)*0.5f) * rho;
		line.angle = static_cast<float>(min_theta)+n * theta;
		lines.push_back(cv::Vec2f(line.rho, line.angle));
	}
}
