//这里使用 windows版Darknet[https://github.com/AlexeyAB/darknet]编译的GPU版动态库yolo_cpp_dll测试


#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <vector>
#include <atomic>
#include <thread>

#include "yolo_v2_class.hpp"
#pragma comment(lib, "yolo_cpp_dll.lib")

struct greation
{
	bool operator()(const int& _Left, const int& _Right) const
	{
		return (_Left < _Right);
	}
};

int main(int argc, char* argv[])
{
	if (argc < 5) {
		std::cout << "Usage: " << argv[0] << 
      			" path/to/yolov3-tiny.cfg path/to/yolov3-tiny_****.weights path/to/voc.names path/to/image.jpg" << std::endl;
		return 0;
	}
	
	Detector* detect = new Detector(argv[1], argv[2]);

	std::vector<std::string> classnames;

	//读取标签
	std::ifstream labelfile(argv[3]);
	if (labelfile.is_open()) {
		std::string line;
		while (std::getline(labelfile, line))
			classnames.push_back(line);
	}

	cv::Mat image = cv::imread(argv[4]);

	//计时
	auto start = std::chrono::steady_clock::now();

	std::vector<bbox_t> box = detect->detect(image, 0.6f);

	std::map<int, std::string, greation> mapPlate;
	bbox_t plateBox;
	std::string plate;
	bool is_detect_plate = false;

	for (size_t i = 0; i < box.size(); i++) {
		if (box[i].obj_id == 0)
		{
			plateBox = box[i];
			is_detect_plate = true;
		}
	}

	//找出在车牌区域内的检测对象
	for (size_t i = 0; i < box.size(); i++) {

		if (is_detect_plate) {
			if (box[i].obj_id != 0) {
				if (box[i].x > plateBox.x && box[i].y > plateBox.y && box[i].x + box[i].w < plateBox.x + plateBox.w && box[i].y + box[i].h < plateBox.y + plateBox.h)
				{
					mapPlate[box[i].x] = classnames[box[i].obj_id];
					cv::rectangle(image, cv::Rect(box[i].x, box[i].y, box[i].w, box[i].h), cvScalar(0, 0, 255));
				}
			}
			else
				cv::rectangle(image, cv::Rect(box[i].x, box[i].y, box[i].w, box[i].h), cvScalar(0, 0, 255));

		}
		else         //未检测到车牌区域, 几下所有检测号码
			mapPlate[box[i].x] = classnames[box[i].obj_id];
	}

	//车牌区域的对象排序
	for (std::map<int, std::string>::iterator ite = mapPlate.begin(); ite != mapPlate.end(); ++ite)
	{
		plate += ite->second;
	}

	auto end = std::chrono::steady_clock::now();

	std::cout << "license plate: " << plate << std::endl;
	std::cout << "detect time: " << (end - start).count() * 1000 << " ms" << std::endl;

	cv::imshow("show", image);
	cv::waitKey();

	delete detect;
  	return 0;
}
