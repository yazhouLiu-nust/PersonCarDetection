# Person/Car Detection

## Synopsis:
This folder contains the works of person/car detection that carried out by Yazhou Liu in Navlab, CMU, from 2018/07/13 to 2018/08/15. The target of this research is to build a person/car detector that works for both RGB/IR image.  

 
## File structure: 
All the code, data and models are backuped in the drive of Navlab:
```
Yazhou_20170812/  
+-- [Code]/
?  +-- api/			: the self defined API functions for person/car detection  
?  +-- window/			: the vs2015 project of person/car detector  
?  +-- darknet-master.zip	: the yolo dart source, available from: https://github.com/pjreddie/darknet.git  
?  +-- darknet-master_navlib.rar: the darknet code that has been changed for windows platform  
+-- [Data]/  
?  +-- CATS_Release/		: the thermal-color image pair dataset publish in CVPR2017"CATS: A Color and Thermal Stereo Benchmark", available from http://bigdatavision.org/CATS/.
?  +-- inter_res/		: the intermediate results. We have labeled 919 IR images for benchmarking. The images and labels (Yolo format) are in IR_groundTruth/ folder. And the labels in the VOC format are in the labels/ folder.
+-- [Doc]/  
?  +-- navlab.20170725.pptx	: biweekly report 1  
?  +-- navlab.20170808.pptx	: biweekly report 2  
?  +-- navlab.20170814.pptx	: biweekly report 3  
?  +-- res.wmv			: video source in biweekly report 1  
?  +-- res_ir.wmv		: video source in biweekly report 2  
?  +-- res_ir_RGB.wmv		: video source in biweekly report 3  
?  +-- res_night.wmv		: video source in biweekly report 3  
+-- [Models]/  
?  +-- Model1/			: the model from yolo as the benchmark, refer to biweekly report 1  
?  ?  +-- yolo-voc.cfg		: the configuration of the model  
?  ?  +-- yolo-voc.weights	: the weights of the model  
?  +-- Model2/			: the re-trained model for person/car/bus/truck, refer to biweekly report 1  
?  ?  +-- navlab.cfg		: the configuration of the model  
?  ?  +-- navlab_80000.weights	: the weights of the model, 80000 is the training iteration  
?  +-- Model3/			: the re-trained model using gray image and color image, refer to biweekly report 2  
?  ?  +-- navlab.cfg		: the configuration of the model  
?  ?  +-- navlab_150000.weights	: the weights of the model, 150000 is the training iteration  
?  +-- Model4/			: the re-trained model using gray image, negative image and color image, refer to biweekly report 2  
?  ?  +-- navlab.cfg		: the configuration of the model  
?  ?  +-- navlab_300000.weights	: the weights of the model, 300000 is the training iteration  
+-- [Tools]/  
?  +-- LabelImage/		: the image label tools, output is in pascal voc format, available from: https://github.com/tzutalin/labelImg  
```


## 3) Code instruction:

* The backend of training and testing is Darknet(Yolo), which is c/c++ code. The original platform is Liunx but can be adopted to Windows with minor changes.
* The API functions for ojects detection has been provided in [Code]/api/person_lib_api.h, this API is plaform independent and can be used in both Linux and Windows.*
* The demo functions showing how to use APIs have been provided in [Code]/api/main.cpp. These demo functions are for windows and use opencv as image I/O.
	* dummy_test1(): this function is for a single image detection.
	* dummy_test2(): this function is to test the images from a webcamera.
	* dummy_test3(): this function is to test the image from a list, and provide the blured ROI.
	* dummy_test4(): this function is to test the image from a video sequence.
	* make_video() : This code is to make the video in the report navlab.20170725.
	* make_video1(): This code is to make the video in the report navlab.20170808.
	* make_video2(): This code is to make the video in the report navlab.20170814.
