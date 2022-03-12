#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

cv::Mat get_img_channel(cv::Mat img, int color = 0) {

	cv::Mat output_img(img.cols, img.rows, img.type());
	cv::Mat zero_mat = cv::Mat::zeros(cv::Size(img.cols, img.rows), CV_8UC1);

	std::vector<cv::Mat> channels;
	std::vector<cv::Mat> splited;

	cv::split(img, splited);

	switch (color)
	{
	case 0:
		channels.push_back(splited[0]);
		channels.push_back(zero_mat);
		channels.push_back(zero_mat);
		break;
	case 1:
		channels.push_back(zero_mat);
		channels.push_back(splited[1]);
		channels.push_back(zero_mat);
		break;
	case 2:
		channels.push_back(zero_mat);
		channels.push_back(zero_mat);
		channels.push_back(splited[2]);
		break;
	default:
		break;
	}

	cv::merge(channels, output_img);
	
	return output_img;
}

cv::Mat draw_img_hist(cv::Mat img, int hist_w = 700, int hist_h = 400) 
{
	std::vector<int> hist_size = { 256 };
	std::vector<float> range = { 0, 256 };

	std::vector<cv::Mat> png_channels(3);
	cv::split(img, png_channels);

	cv::Mat blue_hist, green_hist, red_hist;

	cv::calcHist(std::vector<cv::Mat>{ png_channels[0] }, { 0 }, cv::Mat(), blue_hist, hist_size, range, true);
	cv::calcHist(std::vector<cv::Mat>{ png_channels[1] }, { 0 }, cv::Mat(), green_hist, hist_size, range, true);
	cv::calcHist(std::vector<cv::Mat>{ png_channels[2] }, { 0 }, cv::Mat(), red_hist, hist_size, range, true);

	int bin_w = cvRound((double)hist_w / hist_size[0]);

	cv::Mat hist_image(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	cv::normalize(blue_hist, blue_hist, 0, hist_image.rows, cv::NORM_MINMAX, -1, cv::Mat());
	cv::normalize(green_hist, green_hist, 0, hist_image.rows, cv::NORM_MINMAX, -1, cv::Mat());
	cv::normalize(red_hist, red_hist, 0, hist_image.rows, cv::NORM_MINMAX, -1, cv::Mat());

	cv::imshow("134", hist_image);

	for (int i = 1; i < hist_size[0]; i++)
	{
		cv::line(hist_image, cv::Point(bin_w * (i - 1), hist_h - cvRound(blue_hist.at<float>(i - 1))),
			cv::Point(bin_w * (i), hist_h - cvRound(blue_hist.at<float>(i))),
			cv::Scalar(255, 0, 0), 2, 8, 0);
		cv::line(hist_image, cv::Point(bin_w * (i - 1), hist_h - cvRound(green_hist.at<float>(i - 1))),
			cv::Point(bin_w * (i), hist_h - cvRound(green_hist.at<float>(i))),
			cv::Scalar(0, 255, 0), 2, 8, 0);
		cv::line(hist_image, cv::Point(bin_w * (i - 1), hist_h - cvRound(red_hist.at<float>(i - 1))),
			cv::Point(bin_w * (i), hist_h - cvRound(red_hist.at<float>(i))),
			cv::Scalar(0, 0, 255), 2, 8, 0);
	}

	return hist_image;
}

int main() {
	// read initial image
	const std::string initial_img_path = "./data/cross_0256x0256.png";

	cv::Mat initial_img = cv::imread(initial_img_path, cv::IMREAD_COLOR);
	
	// save inital image in jpeg with quality 25%
	cv::imwrite("cross_0256x0256_025.jpg", initial_img, { cv::ImwriteFlags::IMWRITE_JPEG_QUALITY, 25 });
	
	// create cross_png_img
	cv::Mat cross_png_img, temp, temp2;
	cv::Mat red_channel = get_img_channel(initial_img, 2);
	cv::Mat green_channel = get_img_channel(initial_img, 1);
	cv::Mat blue_channel = get_img_channel(initial_img, 0);
	
	cv::hconcat(initial_img, red_channel, temp);
	cv::hconcat(green_channel, blue_channel, temp2);
	cv::vconcat(temp, temp2, cross_png_img);

	cv::imwrite("cross_0256x0256_png_channels.png", cross_png_img);

	//create cross_jpg_img 

	cv::Mat jpg_img = cv::imread("./cross_0256x0256_025.jpg", cv::IMREAD_COLOR);

	cv::Mat cross_jpg_img, temp3, temp4;
	cv::Mat red_channel_j = get_img_channel(jpg_img, 2);
	cv::Mat green_channel_j = get_img_channel(jpg_img, 1);
	cv::Mat blue_channel_j = get_img_channel(jpg_img, 0);

	cv::hconcat(jpg_img, red_channel_j, temp3);
	cv::hconcat(green_channel_j, blue_channel_j, temp4);
	cv::vconcat(temp3, temp4, cross_jpg_img);

	cv::imwrite("cross_0256x0256_jpg_channels.png", cross_jpg_img);
	
	// create and draw hist

	cv::Mat png_hist_image = draw_img_hist(initial_img);

	cv::imwrite("png_hist.png", png_hist_image);
	
	cv::Mat jpg_hist_image = draw_img_hist(jpg_img);

	cv::imwrite("jpg_hist.png", jpg_hist_image);

	return 0;
}