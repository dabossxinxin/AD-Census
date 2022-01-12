#include <iostream>
#include "ADCensusStereo.h"
#include <chrono>
using namespace std::chrono;

// opencv library
#include <opencv2/opencv.hpp>
#ifdef _DEBUG
#pragma comment(lib,"opencv_world401d.lib")
#else
#pragma comment(lib,"opencv_world401.lib")
#endif

/*��ʾ�Ӳ�ͼ*/
void ShowDisparityMap(const float32* disp_map, const sint32& width, const sint32& height, const std::string& name);
/*�����Ӳ�ͼ*/
void SaveDisparityMap(const float32* disp_map, const sint32& width, const sint32& height, const std::string& path);
/*�����Ӳ����*/
void SaveDisparityCloud(const uint8* img_bytes, const float32* disp_map, const sint32& width, const sint32& height, const std::string& path);
/*�������*/
void SavePointCloud(const uint8* img_bytes, const float32* disp_map, const sint32& width, const sint32& height, const std::string& path);
int main(int argv, char** argc)
{
	printf("Image Loading...");

	// ��ȡӰ��
	std::string path_left = "E:\\Code\\AD-Census-master\\Data\\Cone\\im2.png";
	std::string path_right = "E:\\Code\\AD-Census-master\\Data\\Cone\\im6.png";

	cv::Mat img_left = cv::imread(path_left, cv::IMREAD_COLOR);
	cv::Mat img_right = cv::imread(path_right, cv::IMREAD_COLOR);

	if (img_left.data == nullptr || img_right.data == nullptr) {
		std::cout << "��ȡӰ��ʧ�ܣ�" << std::endl;
		return -1;
	}
	if (img_left.rows != img_right.rows || img_left.cols != img_right.cols) {
		std::cout << "����Ӱ��ߴ粻һ�£�" << std::endl;
		return -1;
	}

	// ����ͼ�����
	const sint32 width = static_cast<uint32>(img_left.cols);
	const sint32 height = static_cast<uint32>(img_right.rows);

	// ����Ӱ��Ĳ�ɫ����
	auto bytes_left = new uint8[width * height * 3];
	auto bytes_right = new uint8[width * height * 3];
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			bytes_left[i * 3 * width + 3 * j] = img_left.at<cv::Vec3b>(i, j)[0];
			bytes_left[i * 3 * width + 3 * j + 1] = img_left.at<cv::Vec3b>(i, j)[1];
			bytes_left[i * 3 * width + 3 * j + 2] = img_left.at<cv::Vec3b>(i, j)[2];
			bytes_right[i * 3 * width + 3 * j] = img_right.at<cv::Vec3b>(i, j)[0];
			bytes_right[i * 3 * width + 3 * j + 1] = img_right.at<cv::Vec3b>(i, j)[1];
			bytes_right[i * 3 * width + 3 * j + 2] = img_right.at<cv::Vec3b>(i, j)[2];
		}
	}
	printf("Done!\n");

	// AD-Censusƥ��������
	ADCensusOption ad_option;
	// ��ѡ�ӲΧ
	ad_option.min_disparity = 0;
	ad_option.max_disparity = 200;
	// һ���Լ����ֵ
	ad_option.lrcheck_thres = 1.0f;

	// �Ƿ�ִ��һ���Լ��
	ad_option.do_lr_check = true;

	// �Ƿ�ִ���Ӳ����
	// �Ӳ�ͼ���Ľ�������ɿ��������̣���������䣬�����У�������
	ad_option.do_filling = false;
	
	printf("w = %d, h = %d, d = [%d,%d]\n\n", width, height, ad_option.min_disparity, ad_option.max_disparity);

	// ����AD-Censusƥ����ʵ��
	ADCensusStereo ad_census;

	printf("AD-Census Initializing...\n");
	auto start = steady_clock::now();
	
	// ��ʼ��
	if (!ad_census.Initialize(width, height, ad_option)) {
		std::cout << "AD-Census��ʼ��ʧ�ܣ�" << std::endl;
		return -2;
	}
	auto end = steady_clock::now();
	auto tt = duration_cast<milliseconds>(end - start);
	printf("AD-Census Initializing Done! Timing :	%lf s\n\n", tt.count() / 1000.0);

	printf("AD-Census Matching...\n");
	// disparity���鱣�������ص��Ӳ���
	auto disparity = new float32[uint32(width * height)]();

	start = steady_clock::now();
	
	// ƥ��
	if (!ad_census.Match(bytes_left, bytes_right, disparity)) {
		std::cout << "AD-Censusƥ��ʧ�ܣ�" << std::endl;
		return -2;
	}
	end = steady_clock::now();
	tt = duration_cast<milliseconds>(end - start);
	printf("\nAD-Census Matching...Done! Timing :	%lf s\n", tt.count() / 1000.0);

	// ��ʾ�Ӳ�ͼ
	ShowDisparityMap(disparity, width, height, "disp-left");
	// �����Ӳ�ͼ
	SaveDisparityMap(disparity, width, height, path_left);
	// �����Ӳ����
	const std::string cloudFile = "F:\\Users\\Admin\\Desktop\\left";
	SaveDisparityCloud(bytes_left,disparity, width, height, cloudFile);
	SavePointCloud(bytes_left, disparity, width, height, cloudFile);

	cv::waitKey(0);

	// �ͷ��ڴ�
	delete[] disparity;
	disparity = nullptr;
	delete[] bytes_left;
	bytes_left = nullptr;
	delete[] bytes_right;
	bytes_right = nullptr;

	system("pause");
	return 0;
}

void ShowDisparityMap(const float32* disp_map, const sint32& width, const sint32& height, const std::string& name)
{
	// ��ʾ�Ӳ�ͼ
	const cv::Mat disp_mat = cv::Mat(height, width, CV_8UC1);
	float32 min_disp = float32(width), max_disp = -float32(width);
	for (sint32 i = 0; i < height; i++) {
		for (sint32 j = 0; j < width; j++) {
			const float32 disp = abs(disp_map[i * width + j]);
			if (disp != Invalid_Float) {
				min_disp = std::min(min_disp, disp);
				max_disp = std::max(max_disp, disp);
			}
		}
	}
	for (sint32 i = 0; i < height; i++) {
		for (sint32 j = 0; j < width; j++) {
			const float32 disp = abs(disp_map[i * width + j]);
			if (disp == Invalid_Float) {
				disp_mat.data[i * width + j] = 0;
			}
			else {
				disp_mat.data[i * width + j] = static_cast<uchar>((disp - min_disp) / (max_disp - min_disp) * 255);
			}
		}
	}

	cv::imshow(name, disp_mat);
	cv::Mat disp_color;
	applyColorMap(disp_mat, disp_color, cv::COLORMAP_JET);
	cv::imshow(name + "-color", disp_color);
}

void SaveDisparityMap(const float32* disp_map, const sint32& width, const sint32& height, const std::string& path)
{
	// �����Ӳ�ͼ
	const cv::Mat disp_mat = cv::Mat(height, width, CV_8UC1);
	float32 min_disp = float32(width), max_disp = -float32(width);
	for (sint32 i = 0; i < height; i++) {
		for (sint32 j = 0; j < width; j++) {
			const float32 disp = abs(disp_map[i * width + j]);
			if (disp != Invalid_Float) {
				min_disp = std::min(min_disp, disp);
				max_disp = std::max(max_disp, disp);
			}
		}
	}
	for (sint32 i = 0; i < height; i++) {
		for (sint32 j = 0; j < width; j++) {
			const float32 disp = abs(disp_map[i * width + j]);
			if (disp == Invalid_Float) {
				disp_mat.data[i * width + j] = 0;
			}
			else {
				disp_mat.data[i * width + j] = static_cast<uchar>((disp - min_disp) / (max_disp - min_disp) * 255);
			}
		}
	}

	cv::imwrite(path + "-d.png", disp_mat);
	cv::Mat disp_color;
	applyColorMap(disp_mat, disp_color, cv::COLORMAP_JET);
	cv::imwrite(path + "-c.png", disp_color);
}

void SavePointCloud(const uint8* img_bytes, const float32* disp_map, const sint32& width, const sint32& height, const std::string& path)
{
	const float fx = 493.30257;
	const float fy = 493.00193;
	const float cxl = 284.59309;
	const float cxr = 306.88674;
	const float cyl = 223.81316;
	const float b = 61.200508;
	const float f = 0.5*(fx + fy);
	// ������ʵ�������
	FILE* fp_disp_cloud = nullptr;
	fopen_s(&fp_disp_cloud, (path + "-cloud.txt").c_str(), "w");
	if (fp_disp_cloud) {
		for (sint32 i = 0; i < height; i++) {
			for (sint32 j = 0; j < width; j++) {
				const float32 disp = abs(disp_map[i * width + j]);
				if (disp == Invalid_Float) {
					continue;
				}

				const float position_z = f*b / disp+(cxr-cxl);
				const float position_x = (j - cxl)*position_z / fx;
				const float position_y = (i - cyl)*position_z / fy;

				fprintf_s(fp_disp_cloud, "%f %f %f %d %d %d\n", position_x, position_y, position_z,
					 img_bytes[i * width * 3 + 3 * j + 2], img_bytes[i * width * 3 + 3 * j + 1], img_bytes[i * width * 3 + 3 * j]);
			}
		}
		fclose(fp_disp_cloud);
	}

}

void SaveDisparityCloud(const uint8* img_bytes, const float32* disp_map, const sint32& width, const sint32& height, const std::string& path)
{
	float32 B = 193.001;		// ����
	float32 f = 999.421;		// ����
	float32 x0l = 294.182;		// ����ͼ������x0
	float32 y0l = 252.932;		// ����ͼ������y0
	float32 x0r = 326.95975;	// ����ͼ������x0

								// �������
	FILE* fp_disp_cloud = nullptr;
	fopen_s(&fp_disp_cloud, (path + "-disparity.txt").c_str(), "w");
	if (fp_disp_cloud) {
		for (sint32 y = 0; y < height; y++) {
			for (sint32 x = 0; x < width; x++) {
				const float32 disp = abs(disp_map[y * width + x]);
				if (disp == Invalid_Float) {
					continue;
				}
				float32 Z = B * f / (disp + (x0r - x0l));
				float32 X = Z * (x - x0l) / f;
				float32 Y = Z * (y - y0l) / f;
				fprintf_s(fp_disp_cloud, "%f %f %f %d %d %d\n", X, Y,
					Z, img_bytes[y * width * 3 + 3 * x + 2], img_bytes[y * width * 3 + 3 * x + 1], img_bytes[y * width * 3 + 3 * x]);
			}
		}
		fclose(fp_disp_cloud);
	}
}