//这里使用 windows版Darknet[https://github.com/AlexeyAB/darknet]编译的GPU版动态库yolo_cpp_dll测试


#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <io.h>
#include "yolo_v2_class.hpp"
#pragma comment(lib, "yolo_cpp_dll.lib")

struct greation
{
	bool operator()(const int& _Left, const int& _Right) const
	{
		return (_Left < _Right);
	}
};

void GetFiles(string path, vector<string>& files, vector<string>& fileNames)
{
	long   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					string q;
					GetFiles(q.assign(path).append("\\").append(fileinfo.name), files, fileNames);
				}
				else
					continue;
			}
			else
			{
				string q;
				files.push_back(q.append(path).append("\\").append(fileinfo.name));
				fileNames.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

int main(int argc, char* argv[])
{
	if (argc < 5) {
		std::cout << "Usage: " << argv[0] << 
      			" path/to/yolov3-tiny.cfg path/to/yolov3-tiny_****.weights path/to/voc.names path/to/image or imagedir [isShow]" << std::endl;
		return 0;
	}
	
	bool isShow = false;
	if (argc >= 6)
	{
		if (0 == strcmp(argv[5], "1") || 0 == strcmp(argv[5], "true"))
			isShow = true;
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

	std::vector <std::string> files;
	std::vector <std::string> filenames;
	std::string imagetype = argv[4];
	if (imagetype.find(".jpg") != std::string::npos || 
	    imagetype.find(".bmp") != std::string::npos || 
	    imagetype.find(".jpeg") != std::string::npos || 
	    imagetype.find(".png") != std::string::npos){
		files.push_back(argv[4]);
	}else
		GetFiles(argv[4], files, filenames);
	
	//记录总时间, 识别文件数, 匹配数的变量
	std::chrono::duration<double> spenttimeAll;
	int imagefilesize = 0, matchnum = 0;
	
	for (size_t index = 0; index < files.size(); index++) {
		cv::Mat image = cv::imread(files[index]);
		if (image.empty()) continue;
		
		std::chrono::duration<double> spenttime;
		//计时
		auto start = std::chrono::steady_clock::now();

		std::vector<bbox_t> box = detect->detect(image, 0.7f);

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
					cv::rectangle(image, cv::Rect(box[i].x, box[i].y, box[i].w, box[i].h), cvScalar(0, 255, 255));

			}
			else {        //未检测到车牌区域, 几下所有检测号码
				mapPlate[box[i].x] = classnames[box[i].obj_id];
				cv::rectangle(image, cv::Rect(box[i].x, box[i].y, box[i].w, box[i].h), cvScalar(0, 0, 255));
			}
		}

		//车牌区域的对象排序
		for (std::map<int, std::string>::iterator ite = mapPlate.begin(); ite != mapPlate.end(); ++ite)
		{
			plate += ite->second;
		}

		auto end = std::chrono::steady_clock::now();
		spenttime = end - start;
		spenttimeAll += spenttime;

		if (filenames.size() > 0){
			std::cout << filenames[index] << std::endl;
			if (plate == filenames[index].substr(0, filenames[index].find('.')))
				matchnum++;
		}
		else
			std::cout << files[index] << std::endl;
		
		std::cout << "license plate: " << plate << std::endl;
		std::cout << "detect time: " << spenttime.count() * 1000 << " ms" << std::endl;
		std::cout << "-----------------------------" << std::endl;
		imagefilesize++;
		if (isShow) {
			cv::imshow("show", image);
			cv::waitKey();
		}
	}
	if (filenames.size() > 0) {
		std::cout << "识别文件数: " << imagefilesize << std::endl;
		std::cout << "车牌匹配数: " << matchnum << std::endl;
		std::cout << "识别率: " << matchnum * 1.f / imagefilesize * 100 << "%" << std::endl;
		std::cout << "总耗时: " << spenttimeAll.count() * 1000 << " ms" << std::endl;
		std::cout << "平均耗时: " << spenttimeAll.count() * 1000 / imagefilesize << " ms" << std::endl;
	}
	delete detect;
  	return 0;
}
