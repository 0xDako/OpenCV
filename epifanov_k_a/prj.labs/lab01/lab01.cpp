#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
	
	double gamma = 2.3;
	std::setlocale(LC_ALL, "Russian");

	cv::Mat img(180, 768, CV_8UC1);
	// draw dummy image
	img = 0;
	cv::Rect2d rc = {0, 0, 768, 60 };
	// draw gradient I_1
	for (int i(0); i < img.cols; i = i + 3)
		cv::line(img,
			cv::Point(i, 0),
			cv::Point(i, 180),
			cv::Scalar(i / 3, i / 3, i / 3),
			3,
			cv::FILLED);

	// draw gamma corrected image G_1
	cv::rectangle(img, rc, { 100 }, 1);
	rc.y += rc.height;

	auto pow_begin = std::chrono::high_resolution_clock::now();
		
	cv::Mat img_pow(img);

	img_pow.convertTo(img_pow, CV_64F, 1 / 255.0);
	cv::pow(img_pow, gamma, img_pow);
	img_pow.convertTo(img_pow, CV_8UC1, 255);

	auto pow_result = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - pow_begin);
	
	img_pow(rc).copyTo(img(rc));

	// draw gamma corrected image G_2
	
	cv::rectangle(img, rc, { 250 }, 1); 
	rc.y += rc.height;

	auto pixel_begin = std::chrono::high_resolution_clock::now();

	cv::Mat img_pixel(img);

	for (int i = rc.y; i < 180; i++)
		for (int j = 0; j < 768; j++) {
			img.at<uchar>(i, j) = pow(img.at<uchar>(i, j) / 255.0f, gamma) *
				255.0f;
		}

	img_pow(rc).copyTo(img(rc));

	auto pixel_result = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - pixel_begin);

	cv::rectangle(img, rc, { 150 }, 1);


	// save result
	cv::imwrite("lab01.png", img);
	
	// time result output

	std::cout << "Время гамма коррекции с помощью pow:" << pow_result.count() / 1000 << "ms" << std::endl;
	std::cout << "Время попиксельной гамма коррекции:" << pixel_result.count() / 1000<< "ms" << std::endl;

}
