# License-plate-recognition
使用 "Darknet yolov3-tiny" 识别车牌

1. 下载[data.zip](https://pan.baidu.com/s/1_Wgy_3mBgNREXXn7HRfAHw),提取码: j7c2.
2. 将data.zip解压到darknet.exe所在目录下.
3. 进入data/voc目录下运行voc_label.bat重新生成2019_train.txt, 2019_val.txt.
4. 修改cfg/yolov3-tiny.cfg
    batch=64
    subdivisions=4    //这里根据自己内存大小修改(我11G显存设置2时,中途会out of memory. 所以设置4)
    ...
    filters=225
    [yolo]    //两个地方修改
    classes=70
    ...
5. 执行 
    darknet.exe detector train data/voc.data cfg/yolov3-tiny.cfg
    
    
