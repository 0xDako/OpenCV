#include <opencv2/opencv.hpp>

char g_mean(char a, char b) {

	char result = (char)sqrt((int)a * (int)a + (int)b * (int)b);
	if (result > 255)
		return (char)255;
	else
		return result;
}

int main() {
	cv::Mat grayscale_mat(300, 450, CV_32FC1, cv::Scalar(255, 255, 255));


	for (int i = 0; i <= 2; i++) {
		cv::Rect2d r = cv::Rect2d((double)150 * i, 0.0, (double)150 * (i + 1), 150.0);
		cv::rectangle(grayscale_mat, r, cv::Scalar(127 * i, 127 * i, 127 * i), -1);
		cv::circle(grayscale_mat, { 75 + 150 * i,75 }, 70, cv::Scalar(127 * ((i + 1) % 3), 127 * ((i + 1) % 3), 127 * ((i + 1) % 3)), -1);

		r = cv::Rect2d((double)150 * i, 150.0, (double)150 * (i + 1), 300.0);
		cv::rectangle(grayscale_mat, r, cv::Scalar(127 * ((i + 1) % 3), 127 * ((i + 1) % 3), 127 * ((i + 1) % 3)), -1);
		cv::circle(grayscale_mat, { 75 + 150 * i,225 }, 70, cv::Scalar(127 * i, 127 * i, 127 * i), -1);
	}
	cv::imwrite("1.png", grayscale_mat);


	cv::Mat I1;
	float kerI1[9]{ 1, 0, -1, 
					2, 0, -2, 
					1, 0, -1 };
	cv::Mat kerI1_matrix(3, 3, CV_32FC1, kerI1);
	cv::filter2D(grayscale_mat, I1, -1, kerI1_matrix);

	cv::imwrite("I1.png", I1);

	cv::Mat I2;
	float kerI2[9]{ 1,	2,	1, 
					0,	0,	0, 
					-1, -2, -1};
	cv::Mat kerI2_matrix(3, 3, CV_32FC1, kerI2);
	cv::filter2D(grayscale_mat, I2, -1, kerI2_matrix);

	cv::imwrite("I2.png", I2);

	cv::Mat sum(300, 450, CV_32FC1, cv::Scalar(255, 255, 255));
	for (int i = 0; i < I1.cols; i++) {
		for (int j = 0; j < I1.rows; j++) {
			float value = sqrt(pow(I1.at<float>(j, i), 2) + pow(I2.at<float>(j, i), 2));
			sum.at<float>(j, i) = value < 0 ? (value + 1) / 2 : value;
		}
	}

	cv::imwrite("sum.png", sum);
	
	
	cv::imshow("result", grayscale_mat);
	cv::waitKey();
	cv::imshow("result", I1);
	cv::waitKey();
	cv::imshow("result", I2);
	cv::waitKey();
	cv::imshow("result", sum);
	cv::waitKey();

	return 0;
}