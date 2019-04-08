# License-plate-recognition
使用 "Darknet yolov3-tiny" 训练检测模型

*1*. 下载[data.zip](https://pan.baidu.com/s/1_Wgy_3mBgNREXXn7HRfAHw),提取码: j7c2.

*2*. 将data.zip解压到darknet.exe所在目录下.

*3*. 进入data/voc目录下运行voc_label.bat重新生成2019_train.txt, 2019_val.txt.

*4*. 修改cfg/yolov3-tiny.cfg

    [net]
    
    batch=64
    
    subdivisions=4    // 这里根据自己内存大小修改(我11G显存设置2时,中途会out of memory. 所以设置4, 训练时显存占用约6G)
    
    angle=5           // 增加旋转角度产生样本
    
    max_batches = 220000        //最大迭代次数
    
    steps=70000,200000         //调整学习率变化点
  
    ...
    
    filters=225                 //[yolo]前一个filters=(classes类别数+ coords坐标数 +1) * mask个数
    
    [yolo]
    
    anchors = 12,27,  17,45,  23,61,  37,58,  198,140,  344,319
    
    classes=70
    
    ignore_thresh = .7
    
    ...
   其他参数说明可参考: [https://blog.csdn.net/weixin_42731241/article/details/81474920](https://blog.csdn.net/weixin_42731241/article/details/81474920)
    
*5*. 执行 

    darknet.exe detector train data/voc.data cfg/yolov3-tiny.cfg
   
*6*. 训练过程(以其中一次过程为例)

    训练过程也一直在调参数. 所以中断了几次.主要调节anchors 和 ignore_thresh
    
    A. iter_0-34900 
    
          第一个 anchors=12,27,  15,34,  17,45,  23,61,  30,84,  198,140
          
          第二个 anchors=10,14,  23,27,  37,58,  81,82,  135,169,  344,319
          
          ignore_thresh=.8
          
    B. iter_34900-42700
    
          两个 anchors=10,14,  23,27,  37,58,  81,82,  135,169,  344,319
          
    C. iter_42700-65000
    
          ignore_thresh=.7
          
    D. iter_65000-74700
    
          ignore_thresh=0.05
          
    E. iter_74700-
    
          ignore_thresh=.7
          
    F. 最终因为loss趋于平缓, 选择了结束训练. 当然还可以再降低learning_rate继续训练, 我这里没继续了.
    
![loss](https://github.com/Feeyao/License-plate-recognition/blob/master/image/chart-iter_34900_42700_65000_74700_149000.png)
    
*7*. 测试了Test目录下的73张图片, 最终识别对了70张, 识别率约95.89%.[weights](https://pan.baidu.com/s/16sP0QZ9UmrB2A6v8TI_v4Q), 提取码:rlh7

![test](https://github.com/Feeyao/License-plate-recognition/blob/master/image/20190404092136.jpg)

**[总结]**
1. 此方法对输入图片存在一定要求, 车牌区域在图片上较小时, 字符可能检测不出或漏检. 所以测试时为输入改为608是提升字符检出率,不过识别时间约增加一倍. *更好的解决方法是不改变输入尺寸, 将车牌区域检测和字符检测分开两个模型, 先检测车牌区域, 将车牌区域(范围可稍微扩张一些)再进行字符检测*.
   
2. 训练数据里面倾斜样本和模糊样本少了一些, 对角度大且车牌区域小的图片字符检测效果不太好.
![image1](https://github.com/Feeyao/License-plate-recognition/blob/master/image/20190404083445.jpg)![image2](https://github.com/Feeyao/License-plate-recognition/blob/master/image/20190404083520.jpg)![image3](https://github.com/Feeyao/License-plate-recognition/blob/master/image/20190404083550.jpg)
3. 想起来再写.
