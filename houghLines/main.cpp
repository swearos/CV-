#include "Buffer.h"
#include "houghlines.h"


int main()
{
	string filename = "shudu.png";
	int limit = 50;

	cv::Mat src = cv::imread(filename, 0);

	cv::Mat dst, cdst;
	Canny(src, dst, 50, 200, 3);
	cvtColor(dst, cdst, cv::COLOR_GRAY2BGR);

	std::vector<cv::Vec2f> lines;
	//HoughLines(dst, lines, 1, CV_PI / 180, 150, 0, 0);
	HoughLinesStandard(dst, 1, CV_PI / 180, 150, lines, INT_MAX, 0, 50);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(cdst, pt1, pt2, cv::Scalar(0, 0, 255), 1, CV_AA);
	}
	cv::imshow("source", src);
	cv::imshow("detected lines", cdst);

	cv::waitKey();

	return 0;
}
