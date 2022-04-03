#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "json.hpp"
#include <fstream>
using namespace nlohmann;

std::vector<cv::Mat> markedRectMat;
std::vector<cv::Mat> algRectMat;
std::vector<cv::Mat> sourceMat;
std::vector<cv::Mat> alphaBlend(15, cv::Mat());

void drawPolly(cv::Mat image, cv::Point left_down, cv::Point left_up, cv::Point right_up, cv::Point right_down, int color) {
    cv::Point points[1][4];
    points[0][0] = left_down;
    points[0][1] = left_up;
    points[0][2] = right_up;
    points[0][3] = right_down;

    const cv::Point* ppt[1] = { points[0] };
    int npt[] = { 4 };

    cv::fillPoly(image, ppt, npt, 1, cv::Scalar(255, color, 255), cv::LINE_8);
}

void test_results() {
    std::ifstream i("../data/Kirill_E.json");
    json fl = json::parse(i);

    auto video_array = fl["video_array"];

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            cv::Mat img(algRectMat[i * (j + 1) + j].rows, algRectMat[i * (j + 1)].cols, algRectMat[i * (j + 1)].type(), cv::Scalar(0, 0, 0));
            cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
            auto first = video_array[i][j][0];
            auto second = video_array[i][j][1];
            auto third = video_array[i][j][2];
            auto fourth = video_array[i][j][3];
            drawPolly(img,
                cv::Point(first["x"], first["y"]),
                cv::Point(second["x"], second["y"]),
                cv::Point(third["x"], third["y"]),
                cv::Point(fourth["x"], fourth["y"]), 0);
            markedRectMat.push_back(img);
        }
    }
}

cv::Point find_center(cv::Rect2i rect) {
    return cv::Point(rect.width / 2 + rect.x, rect.height / 2 + rect.y);
}

double centreDist(cv::Point a, cv::Point b) {
    return std::abs((std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y))));
}

double rectDiff(cv::Rect2i recta, cv::Rect2i rectb) {
    return(centreDist(find_center(recta), find_center(rectb)));
}

double errorCheck(cv::Mat frame, cv::Rect a, cv::Rect b) {
    double error;
    error = std::max(a.width * a.height, b.width * b.height) / std::min(a.width * a.height, b.width * b.height) * (rectDiff(a, b) / std::sqrt((int)(frame.cols * frame.cols) + (int)(frame.rows * frame.rows))) * 100;
    return error;
}

std::vector<cv::Mat> splitVideo(cv::VideoCapture& capture) {
    std::vector<cv::Mat> result;
    result.reserve(500);
    while (1) {
        cv::Mat frame;
        capture >> frame;
        if (frame.empty())
            break;
        result.push_back(frame);
    }
    capture.release();
    return result;
}

cv::Mat quantize_frame(cv::Mat& frame) {
    cv::Mat quant;
    cv::cvtColor(frame, quant, cv::COLOR_BGRA2GRAY, 0);
    return quant;
}

cv::Mat morph(cv::Mat& frame) {
    cv::Mat Morph;
    cv::morphologyEx(frame, Morph, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    cv::morphologyEx(Morph, Morph, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));
    cv::dilate(Morph, Morph, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 25)));
    cv::erode(Morph, Morph, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 25)));
    return Morph;
}

cv::Mat binarize_frame(cv::Mat& frame) {

    cv::Mat binar;
    cv::threshold(quantize_frame(frame), binar, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    return binar;
}

cv::Mat banknote_connected_component(const cv::Mat& image) {
    cv::Mat labels, stats, centroids;
    cv::connectedComponentsWithStats(image, labels, stats, centroids);

    uint max_i = 0;
    uint max_s = 0;

    for (auto i = 1; i < stats.rows; ++i) {

        if (stats.at<int>(i, 4) > max_s) {
            max_i = i;
            max_s = stats.at<int>(i, 4);

        }
    }

    auto x = stats.at<int>(max_i, 0), y = stats.at<int>(max_i, 1);
    auto width = stats.at<int>(max_i, 2), height = stats.at<int>(max_i, 3);

    cv::Mat banknote(image.rows, image.cols, image.type(), cv::Scalar(0, 0, 0));
    for (int i = x; i < x + width; ++i) {
        for (int j = y; j < y + height; ++j) {
            if (labels.at<int>(j, i) == max_i)
                banknote.at<uchar>(j, i) = 255;
        }
    }
    return banknote;
}

int lab4(const std::string filename) {
    std::string path = "../data/" + filename + ".mp4";
    cv::VideoCapture video(path);
    if (!video.isOpened()) {
        std::cerr << "Failed to open the video device, video file or image sequence!\n";
        return 1;
    }
    cv::Mat frame;
    auto frames = splitVideo(video);
    cv::Mat frame_quantized, frame_binar, frame_morph, rectalg;
    for (int i = 0; i < 3; ++i) {
        frame = frames.at((int)frames.size() * (i + 2) / 5);
        sourceMat.push_back(frame);
        frame_quantized = quantize_frame(frame);
        frame_binar = binarize_frame(frame);
        frame_morph = morph(frame_binar);
        rectalg = banknote_connected_component(frame_morph);
        algRectMat.push_back(rectalg);
    }
}

double compare_results(cv::Mat detected_result, cv::Mat correct_result) {
    double equal_white_pixels_count = 0;
    double total_white_pixels_count = 0;

    for (auto i = 0; i < detected_result.cols; i++) {
        for (auto j = 0; j < detected_result.rows; j++) {
            if (detected_result.at<uchar>(j, i) != 0 || correct_result.at<cv::Vec3b>(j, i) != cv::Vec3b(0, 0, 0)) {
                total_white_pixels_count++;
            }
            if (detected_result.at<uchar>(j, i) != 0 && correct_result.at<cv::Vec3b>(j, i) != cv::Vec3b(0, 0, 0)) {
                equal_white_pixels_count++;
            }
        }
    }

    return equal_white_pixels_count / total_white_pixels_count;
}

void createMozaik(std::vector<cv::Mat> src, std::vector<cv::Mat> alg, std::vector<cv::Mat> est, std::vector<cv::Mat> aB) {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 3; ++j) {
            cv::cvtColor(alg[i * 3 + j], alg[i * 3 + j], cv::COLOR_RGB2BGR);
            cv::addWeighted(alg[i * 3 + j], 0.3, est[i * 3 + j], 0.7, 0.0, aB[i * 3 + j]);
            cv::hconcat(src[i * 3 + j], aB[i * 3 + j], src[i * 3 + j]);
        }
        cv::vconcat(src[i * 3], src[i * 3 + 1], src[i * 3]);
        cv::vconcat(src[i * 3], src[i * 3 + 2], src[i * 3]);
        cv::imwrite("lab4_" + std::to_string(i + 1) + ".png", src[i * 3]);
    }
}

int main() {
    lab4("1");
    lab4("2");
    lab4("3");
    lab4("4");
    lab4("5");
    test_results();
    for (int i = 0; i < 15; ++i) {
        std::cout << compare_results(algRectMat[i], markedRectMat[i]) << std::endl;
    }
    createMozaik(sourceMat, algRectMat, markedRectMat, alphaBlend);
}
