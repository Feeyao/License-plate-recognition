# License-plate-recognition
使用 "Darknet yolov3-tiny" 训练检测模型

1. 下载[data.zip](https://pan.baidu.com/s/1_Wgy_3mBgNREXXn7HRfAHw),提取码: j7c2.
2. 将data.zip解压到darknet.exe所在目录下.
3. 进入data/voc目录下运行voc_label.bat重新生成2019_train.txt, 2019_val.txt.
4. 修改cfg/yolov3-tiny.cfg

    batch=64
    
    subdivisions=4    // 这里根据自己内存大小修改(我11G显存设置2时,中途会out of memory. 所以设置4, 训练时显存占用约6G)
    
    angle=10          // 增加旋转角度产生样本
    
    max_batches = 220000        //最大迭代次数
    
    steps=120000,200000         //调整学习率
  
    ...
    
    其他参数说明可参考: https://blog.csdn.net/weixin_42731241/article/details/81474920
    
5. 执行 

    darknet.exe detector train data/voc.data cfg/yolov3-tiny.cfg
   
6. 训练过程(以其中一次过程为例)
    
    配置文件:
    
    ![yolov3-tiny.cfg](https://github.com/Feeyao/License-plate-recognition/blob/master/image/20190402165417.jpg)
   
    因为忘了改ignore_thresh,中断了两次. 在iter-34900调整anchors回默认值, 在42700调整ignore_thresh=.6
  
    ![iter_34900](https://github.com/Feeyao/License-plate-recognition/blob/master/image/chart-iter_34900.png)
    ![iter_42700](https://github.com/Feeyao/License-plate-recognition/blob/master/image/chart-iter_42700.png)
    
**[总结]**
1. 此方法对输入图片存在一定要求, 车牌区域在图片上较小时, 字符可能检测不出或漏检. 提升方法是将车牌区域检测和字符检测分开两个模型.

2. 

3. 想起来再写.
