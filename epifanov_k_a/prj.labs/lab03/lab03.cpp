#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

int main() 
{
	const std::string initial_img_path = "./data/cross_0256x0256.png";

	cv::Mat initial_img = cv::imread(initial_img_path, cv::IMREAD_COLOR);

	cv::imwrite("lab03_rgb.png", initial_img);
		
	cv::Mat grayscale_img;
	
	cv::cvtColor(initial_img, grayscale_img, cv::COLOR_BGRA2GRAY);

	cv::imwrite("lab03_gre.png", grayscale_img);


	return 0;
}