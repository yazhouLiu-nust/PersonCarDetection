#include <direct.h>
#include <opencv.hpp>
#include <time.h>
#include <stdio.h>
#include "person_lib_api.h"

using namespace cv;

#define MEASURE_TIME(op) \
{ \
	TickMeter tm; \
	tm.start(); \
	op; \
	tm.stop(); \
	printf("%f sec\n", tm.getTimeSec());\
}

char *names[] = { "Person", "Car", "Bus", "Truck" };

static Scalar color_map[] = {
	Scalar(255,0,0),
	Scalar(0,255,0),
	Scalar(0,0,255),
	Scalar(255,255,0),
	Scalar(255,0,255),
	Scalar(0,255,255),
	Scalar(200,100,0),
	Scalar(0,200,100),
	Scalar(100,0,200),
	Scalar(200,0,100),
	Scalar(100,200,0),
	Scalar(0,100,200),
	Scalar(150,75,75)
};

void dummy_test0() {
	char img_name[] =
		"E:\\[CODE]\\[Yolo]\\darknet-master\\data\\person.jpg";

	Mat img = imread(img_name, CV_LOAD_IMAGE_COLOR);

	imshow("before", img);

	dummy_test(img.data, img.cols, img.rows, img.channels());

	//Person_Release();

	imshow("after", img);

	waitKey(0);
}
//..
// single image test
//..
void dummy_test1() {

	char cfg_name[] =
		//"F:\\Navlab\\[model]\\voc\\yolo-voc.cfg";
		"F:\\Navlab\\[model]\\Densenet\\densenet201.cfg";
	char wei_name[] =
		"F:\\Navlab\\[model]\\voc\\yolo-voc.weights";
	char img_name[] =
		"F:\\Navlab\\[code]\\darknet-master\\data\\person.jpg";

	Person_Init(cfg_name, wei_name);

	int obj_No;
	int obj_ID[MAX_REGION_NO];
	int obj_box[MAX_REGION_NO * 4];
	float obj_score[MAX_REGION_NO];

	Mat img = imread(img_name, CV_LOAD_IMAGE_COLOR);
	Person_Detect(img.data, img.cols, img.rows, img.channels(), &obj_No, obj_ID, obj_box, obj_score, 0.1/* 0.24*/);

	//printf("Obj No: %d; ROI:%d,%d,%d,%d; Score:%f\n", obj_No, obj_box[0], obj_box[1], obj_box[2], obj_box[3], obj_score);
	for (int i = 0; i < obj_No; i++) {
		rectangle(img, Rect(obj_box[i * 4 + 0], obj_box[i * 4 + 1], obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1]), color_map[obj_ID[i]], 3);
		char sc[512];
		sprintf(sc, "%s:%.2f%%", names[i], obj_score[i]);
		putText(img, sc, Point(obj_box[i * 4 + 0], obj_box[i * 4 + 1] - 3), CV_FONT_HERSHEY_COMPLEX, 0.6, color_map[obj_ID[i]], 1);

	}

	imshow("after", img);
	waitKey(0);
	Person_Release();
}
//..
// test image from webcam
//..
void dummy_test2() {

	char cfg_name[] =
		"F:\\Navlab\\[model]\\voc\\yolo-voc.cfg";
	char wei_name[] =
		"F:\\Navlab\\[model]\\voc\\yolo-voc.weights";

	Person_Init(cfg_name, wei_name);
	int obj_No;
	int obj_ID[MAX_REGION_NO];
	int obj_box[MAX_REGION_NO * 4];
	float obj_score[MAX_REGION_NO];

	//Mat img = imread(img_name, CV_LOAD_IMAGE_COLOR);
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		return;
	}

	Mat img;
	bool stop = false;
	while (!stop)
	{
		cap >> img;
		Person_Detect(img.data, img.cols, img.rows, img.channels(), &obj_No, obj_ID, obj_box, obj_score, 0.24);

		//printf("Obj No: %d; ROI:%d,%d,%d,%d; Score:%f\n", obj_No, obj_box[0], obj_box[1], obj_box[2], obj_box[3], obj_score);
		for (int i = 0; i < obj_No; i++) {
			rectangle(img, Rect(obj_box[i * 4 + 0], obj_box[i * 4 + 1], obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1]), color_map[obj_ID[i]], 3);
			char sc[512];
			sprintf(sc, "%s:%.2f%%", names[i], obj_score[i]);
			putText(img, sc, Point(obj_box[i * 4 + 0], obj_box[i * 4 + 1] - 3), CV_FONT_HERSHEY_COMPLEX, 0.6, color_map[obj_ID[i]], 1);

		}

		imshow("after", img); waitKey(10);
		//if (waitKey(10) >= 0)
			//stop = true;
	}

	Person_Release();

	return;
}
/*
	Blur the image
*/
void smooth_img(Mat& img, Rect rec) {
	//Mat img_sub = img(rec);
	//int k_size = MAX(MIN(rec.width, rec.height)/ 8, 1) * 2 + 1;
	//GaussianBlur(img_sub, img_sub, Size(k_size, k_size), 0);


	Rect rec1 = rec;
	int step = MAX(MAX(rec.width, rec.height) / 30, 1);
	int k_size = step * 2 + 1;
	//int k_size = 3;// 

	int max_iter = 6;
	for (int i = 0; i < max_iter; i++) {
		Mat img_sub = img(rec1);
		GaussianBlur(img_sub, img_sub, Size(k_size, k_size), 0);

		rec1.x = rec1.x + step;
		rec1.y = rec1.y + step;
		rec1.width = rec1.width - 2 * step;
		rec1.height = rec1.height - 2 * step;

		if (rec1.width <= 0 || rec1.height <= 0)
			break;
	}

	//Mat img_sub = img(rec);
	//GaussianBlur(img_sub, img_sub, Size(3, 3), 0);
}
//..
//test the image from a list
//..
void dummy_test3() {

	char cfg_name[] =
		//"F:\\Navlab\\[model]\\voc\\yolo-voc.cfg";
		"F:\\Navlab\\[model]\\trail03\\navlab.cfg";
	char wei_name[] =
		//"F:\\Navlab\\[model]\\voc\\yolo-voc.weights";
		"F:\\Navlab\\[model]\\trail03\\navlab_300000.weights";
	char img_lst[] =
		//"F:\\Navlab\\[data]\\ir4.asf";
		"F:\\[data]\\CATS_Release\\left.txt";
	char out_dir[] =
		//"F:\\Navlab\\[data]\\output";
		"F:\\Navlab\\[data]\\left";

	char img_name[512];
	char img_name_out[512];

	mkdir(out_dir);

	Person_Init(cfg_name, wei_name);
	int obj_No;
	int obj_ID[MAX_REGION_NO];
	int obj_box[MAX_REGION_NO * 4];
	float obj_score[MAX_REGION_NO];

	Mat img;
	FILE* fr = fopen(img_lst, "r");
	int cnt = 0;
	while (!feof(fr)) {
		fscanf(fr, "%s\n", img_name);
		Mat img = imread(img_name, CV_LOAD_IMAGE_COLOR);
		Mat img_blur = img.clone();
		MEASURE_TIME(
			Person_Detect(img.data, img.cols, img.rows, img.channels(), &obj_No, obj_ID, obj_box, obj_score, 0.24);
		)


		//printf("Obj No: %d; ROI:%d,%d,%d,%d; Score:%f\n", obj_No, obj_box[0], obj_box[1], obj_box[2], obj_box[3], obj_score);
		for (int i = 0; i < obj_No; i++) {
			rectangle(img, Rect(obj_box[i * 4 + 0], obj_box[i * 4 + 1], obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1]), color_map[obj_ID[i]], 3);
			char sc[512];
			sprintf(sc, "%s:%.2f%%", names[obj_ID[i]], obj_score[i]);
			putText(img, sc, Point(obj_box[i * 4 + 0], obj_box[i * 4 + 1] - 3), CV_FONT_HERSHEY_COMPLEX, 0.6, color_map[obj_ID[i]], 1);

			/*
			Mat img_sub = img_blur(Rect(obj_box[i * 4 + 0], obj_box[i * 4 + 1], obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1]));
			int k_size = MAX(MIN(obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1])/8, 1)*2+1;
			GaussianBlur(img_sub, img_sub, Size(k_size, k_size),0);*/

			smooth_img(img_blur, Rect(obj_box[i * 4 + 0], obj_box[i * 4 + 1], obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1]));

		}

		imshow("after", img); waitKey(10);
		imshow("blur", img_blur); waitKey(10);
		//..
		sprintf(img_name_out, "%s\\%05d.jpg", out_dir, cnt);
		imwrite(img_name_out, img);
		//sprintf(img_name_out, "%s\\%05d_blur.jpg", out_dir, cnt);
		//imwrite(img_name_out, img_blur);
		cnt++;
	}

	

	Person_Release();
}

//..
//test the image from a video
//..
void dummy_test4() {

	char cfg_name[] =
		//"F:\\Navlab\\[model]\\voc\\yolo-voc.cfg";
		"F:\\Navlab\\[model]\\trail03\\navlab.cfg";
	char wei_name[] =
		//"F:\\Navlab\\[model]\\voc\\yolo-voc.weights";
		"F:\\Navlab\\[model]\\trail03\\navlab_300000.weights";
	char img_lst[] =
		"F:\\Navlab\\[data]\\ir14.asf";
		//"F:\\[data]\\CATS_Release\\left.txt";
	char out_dir[] =
		"F:\\Navlab\\[data]\\output14";
		//"F:\\Navlab\\[data]\\left";



	char out_vid[512];
	sprintf(out_vid, "%s\\res_vid.wmv", out_dir);
	//VideoWriter writer(out_vid, CV_FOURCC('M', 'J', 'P', 'G'), 10.0, Size(480, 360));
	VideoWriter writer(out_vid, CV_FOURCC('M', 'J', 'P', 'G'), 10.0, Size(640, 480));

	char img_name[512];
	char img_name_out[512];

	mkdir(out_dir);

	Person_Init(cfg_name, wei_name);
	int obj_No;
	int obj_ID[MAX_REGION_NO];
	int obj_box[MAX_REGION_NO * 4];
	float obj_score[MAX_REGION_NO];

	Mat img;

	VideoCapture fr(img_lst);
	if (!fr.isOpened()) {
		return;
	}

	int cnt = 0;
	Mat srcImg;
	fr >> srcImg;
	while (srcImg.data) {
		Mat img = srcImg.clone();
		//Mat img = imread(img_name, CV_LOAD_IMAGE_COLOR);
		Mat img_blur = img.clone();
		MEASURE_TIME(
			Person_Detect(img.data, img.cols, img.rows, img.channels(), &obj_No, obj_ID, obj_box, obj_score, 0.4);
		)


			//printf("Obj No: %d; ROI:%d,%d,%d,%d; Score:%f\n", obj_No, obj_box[0], obj_box[1], obj_box[2], obj_box[3], obj_score);
			for (int i = 0; i < obj_No; i++) {
				rectangle(img, Rect(obj_box[i * 4 + 0], obj_box[i * 4 + 1], obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1]), color_map[obj_ID[i]], 3);
				char sc[512];
				sprintf(sc, "%s:%.2f%%", names[obj_ID[i]], obj_score[i]);
				putText(img, sc, Point(obj_box[i * 4 + 0], obj_box[i * 4 + 1] - 3), CV_FONT_HERSHEY_COMPLEX, 0.6, color_map[obj_ID[i]], 1);

				/*
				Mat img_sub = img_blur(Rect(obj_box[i * 4 + 0], obj_box[i * 4 + 1], obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1]));
				int k_size = MAX(MIN(obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1])/8, 1)*2+1;
				GaussianBlur(img_sub, img_sub, Size(k_size, k_size),0);*/

				smooth_img(img_blur, Rect(obj_box[i * 4 + 0], obj_box[i * 4 + 1], obj_box[i * 4 + 2] - obj_box[i * 4 + 0], obj_box[i * 4 + 3] - obj_box[i * 4 + 1]));

			}

		imshow("after", img); waitKey(10);
		imshow("blur", img_blur); waitKey(10);
		//..
		sprintf(img_name_out, "%s\\%05d.jpg", out_dir, cnt);
		imwrite(img_name_out, img);
		//sprintf(img_name_out, "%s\\%05d_blur.jpg", out_dir, cnt);
		//imwrite(img_name_out, img_blur);
		//sprintf(img_name_out, "%s\\%05d_src.jpg", out_dir, cnt);
		//imwrite(img_name_out, srcImg);
		cnt++;

		writer << img;
		fr >> srcImg;
	}



	Person_Release();
}
/*
This code is to make the video in the report navlab.20170725
*/
void make_video() {
	char img_lst[] =
		"F:\\Navlab\\[data]\\list.txt";

	char img0_dir[] = 
		"F:\\Navlab\\[data]\\output";

	char img1_dir[] =
		"F:\\Navlab\\[data]\\output1";

	char out_vid[] =
		"F:\\Navlab\\[data]\\res.wmv";

	char img_name0[512];
	char img_name1[512];
	char img_name2[512];
	char img_name3[512];


	int sub_h = 1080 / 2;
	int sub_w = 1920 / 2;

	int margin = 20;

	int can_h = sub_h * 2 + 3 * margin;
	int can_w = sub_w * 2 + 3 * margin;
	Mat canvas = Mat(can_h, can_w, CV_8UC3, Scalar(0));

	VideoWriter writer(out_vid, CV_FOURCC('M', 'J', 'P', 'G'), 10.0, Size(can_w, can_h));

	//...
	Mat log0 = Mat(sub_h / 14, sub_w / 2, CV_8UC3, Scalar(0));
	Mat log1 = log0.clone();
	Mat log2 = log0.clone();
	Mat log3 = log0.clone();
	//char sc[512];
	putText(log0, "Src Image", Point(10 , log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	putText(log1, "Blured Image (model 2)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	putText(log2, "Detection Res (model 1: 74.17%)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	putText(log3, "Detection Res (model 2: 79.71%)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	//..
		
	Mat img0; Mat img1; Mat img2; Mat img3;
	FILE* fr = fopen(img_lst, "r");
	int cnt = 0;
	while (!feof(fr)) {



		fscanf(fr, "%s\n", img_name0);
		sprintf(img_name1, "%s\\%05d_blur.jpg", img1_dir, cnt);
		sprintf(img_name2, "%s\\%05d.jpg", img0_dir, cnt);
		sprintf(img_name3, "%s\\%05d.jpg", img1_dir, cnt);

		Mat img0 = imread(img_name0, CV_LOAD_IMAGE_COLOR);
		Mat img1 = imread(img_name1, CV_LOAD_IMAGE_COLOR);
		Mat img2 = imread(img_name2, CV_LOAD_IMAGE_COLOR);
		Mat img3 = imread(img_name3, CV_LOAD_IMAGE_COLOR);

		Mat sub_0, sub_1, sub_2, sub_3;
		resize(img0, sub_0, Size(), 0.5, 0.5);
		resize(img1, sub_1, Size(), 0.5, 0.5);
		resize(img2, sub_2, Size(), 0.5, 0.5);
		resize(img3, sub_3, Size(), 0.5, 0.5);

		//sub_0(Rect(0, 0, log0.cols, log0.rows)) = 0; //log0;
		log0.copyTo(sub_0(Rect(2, 2, log0.cols, log0.rows)));
		log1.copyTo(sub_1(Rect(2, 2, log1.cols, log1.rows)));
		log2.copyTo(sub_2(Rect(2, 2, log2.cols, log2.rows)));
		log3.copyTo(sub_3(Rect(2, 2, log3.cols, log3.rows)));
		//char sc[512];
	

		int stax = margin;
		int stay = margin;
		//canvas(Rect(stax, stay, sub_w, sub_h)) = sub_0.clone();
		sub_0.copyTo(canvas(Rect(stax, stay, sub_w, sub_h)));
		//canvas(Rect(stax+margin+sub_w, stay, sub_w, sub_h)) = sub_1.clone();
		sub_1.copyTo(canvas(Rect(stax + margin + sub_w, stay, sub_w, sub_h)));
		//canvas(Rect(stax, stay + margin + sub_h, sub_w, sub_h)) = sub_2.clone();
		sub_2.copyTo(canvas(Rect(stax, stay + margin + sub_h, sub_w, sub_h)));
		//canvas(Rect(stax + margin + sub_w, stay + margin + sub_h, sub_w, sub_h)) = sub_3.clone();
		sub_3.copyTo(canvas(Rect(stax + margin + sub_w, stay + margin + sub_h, sub_w, sub_h)));
		
		imshow("video", canvas);
		waitKey(10);

		writer << canvas;
		//imshow("video", canvas);
		cnt++;
	}
}
/*
This code is to make the video in the report navlab.20170808
*/
void make_video1() {
	char img_0st[] =
		"F:\\Navlab\\[data]\\ch10.txt";
	char img_1st[] =
		"F:\\Navlab\\[data]\\ch11.txt";
	char img_2st[] =
		"F:\\Navlab\\[data]\\ch12.txt";
	char img_3st[] =
		"F:\\Navlab\\[data]\\ch13.txt";


	char out_vid[] =
		"F:\\Navlab\\[data]\\res_night.wmv";

	char img_name0[512];
	char img_name1[512];
	char img_name2[512];
	char img_name3[512];


	int sub_h = 1080 / 2;
	int sub_w = 1920 / 2;

	int margin = 20;

	int can_h = sub_h * 2 + 3 * margin;
	int can_w = sub_w * 2 + 3 * margin;
	Mat canvas = Mat(can_h, can_w, CV_8UC3, Scalar(0));

	VideoWriter writer(out_vid, CV_FOURCC('M', 'J', 'P', 'G'), 10.0, Size(can_w, can_h));

	//...
	Mat log0 = Mat(sub_h / 14, sub_w / 2, CV_8UC3, Scalar(0));
	Mat log1 = log0.clone();
	Mat log2 = log0.clone();
	Mat log3 = log0.clone();
	//char sc[512];
	putText(log0, "Model 4 seq Night 00 (right)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	putText(log1, "Model 4 seq Night 01 (right)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	putText(log2, "Model 4 seq Night 00 (left)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	putText(log3, "Model 4 seq Night 01 (left)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	//..

	Mat img0; Mat img1; Mat img2; Mat img3;
	FILE* fr = fopen(img_0st, "r");
	FILE* fr1 = fopen(img_1st, "r");
	FILE* fr2 = fopen(img_2st, "r");
	FILE* fr3 = fopen(img_3st, "r");

	int cnt = 0;
	while (!feof(fr3)) {

		
		fscanf(fr, "%s\n", img_name0);
		fscanf(fr1, "%s\n", img_name1);
		fscanf(fr2, "%s\n", img_name2);
		fscanf(fr3, "%s\n", img_name3);

		Mat img0 = imread(img_name0, CV_LOAD_IMAGE_COLOR);
		Mat img1 = imread(img_name1, CV_LOAD_IMAGE_COLOR);

		Mat img2 = imread(img_name2, CV_LOAD_IMAGE_COLOR);
		Mat img3 = imread(img_name3, CV_LOAD_IMAGE_COLOR);

		if (!img2.data) {
			img2 = img3.clone(); img2 = 0;
		}
		if (!img3.data) {
			img3 = img3.clone(); img3 = 0;
		}	
		if (!img0.data) {
			img0 = img3.clone(); img0 = 0;
		}
		if (!img1.data) {
			img1 = img3.clone(); img1 = 0;
		}

		Mat sub_0, sub_1, sub_2, sub_3;
		resize(img0, sub_0, Size(sub_w, sub_h), 0.0, 0.0);
		resize(img1, sub_1, Size(sub_w, sub_h), 0.0, 0.0);
		resize(img2, sub_2, Size(sub_w, sub_h), 0.0, 0.0);
		resize(img3, sub_3, Size(sub_w, sub_h), 0.0, 0.0);

		//sub_0(Rect(0, 0, log0.cols, log0.rows)) = 0; //log0;
		log0.copyTo(sub_0(Rect(2, 2, log0.cols, log0.rows)));
		log1.copyTo(sub_1(Rect(2, 2, log1.cols, log1.rows)));
		log2.copyTo(sub_2(Rect(2, 2, log2.cols, log2.rows)));
		log3.copyTo(sub_3(Rect(2, 2, log3.cols, log3.rows)));
		//char sc[512];


		int stax = margin;
		int stay = margin;
		//canvas(Rect(stax, stay, sub_w, sub_h)) = sub_0.clone();
		sub_2.copyTo(canvas(Rect(stax, stay, sub_w, sub_h)));
		//canvas(Rect(stax+margin+sub_w, stay, sub_w, sub_h)) = sub_1.clone();
		sub_0.copyTo(canvas(Rect(stax + margin + sub_w, stay, sub_w, sub_h)));
		//canvas(Rect(stax, stay + margin + sub_h, sub_w, sub_h)) = sub_2.clone();
		sub_3.copyTo(canvas(Rect(stax, stay + margin + sub_h, sub_w, sub_h)));
		//canvas(Rect(stax + margin + sub_w, stay + margin + sub_h, sub_w, sub_h)) = sub_3.clone();
		sub_1.copyTo(canvas(Rect(stax + margin + sub_w, stay + margin + sub_h, sub_w, sub_h)));

		imshow("video", canvas);
		waitKey(10);

		writer << canvas;
		//imshow("video", canvas);
		cnt++;
	}
}
/*
This code is to make the video in the report navlab.20170814
*/
void make_video2() {
	char img_0st[] =
		"F:\\Navlab\\[data]\\ch10.txt";
	char img_1st[] =
		"F:\\Navlab\\[data]\\ch11.txt";
	


	char out_vid[] =
		"F:\\Navlab\\[data]\\res_ir_RGB.wmv";

	char img_name0[512];
	char img_name1[512];
	//char img_name2[512];
	//char img_name3[512];


	int sub_h = 1080 / 2;
	int sub_w = 1920 / 2;

	int margin = 20;

	int can_h = sub_h * 1 + 2 * margin;
	int can_w = sub_w * 2 + 3 * margin;
	Mat canvas = Mat(can_h, can_w, CV_8UC3, Scalar(0));

	VideoWriter writer(out_vid, CV_FOURCC('M', 'J', 'P', 'G'), 5.0, Size(can_w, can_h));

	//...
	Mat log0 = Mat(sub_h / 14, sub_w / 2, CV_8UC3, Scalar(0));
	Mat log1 = log0.clone();
	//Mat log2 = log0.clone();
	//Mat log3 = log0.clone();
	//char sc[512];
	putText(log0, "Model 4 IR (69.58%)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	putText(log1, "Model 4 RGB (61.14%)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	//putText(log2, "Model 2 IR (67.79%)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	//putText(log3, "Model 2 RGB (79.71%)", Point(10, log0.rows - 10), CV_FONT_HERSHEY_SIMPLEX, 0.8, color_map[1], 2);
	//..

	Mat img0; Mat img1; //Mat img2; Mat img3;
	FILE* fr = fopen(img_0st, "r");
	FILE* fr1 = fopen(img_1st, "r");
	//FILE* fr2 = fopen(img_2st, "r");
	//FILE* fr3 = fopen(img_3st, "r");

	int cnt = 0;
	while (!feof(fr)) {


		fscanf(fr, "%s\n", img_name0);
		fscanf(fr1, "%s\n", img_name1);
		//fscanf(fr2, "%s\n", img_name2);
		//fscanf(fr3, "%s\n", img_name3);

		Mat img0 = imread(img_name0, CV_LOAD_IMAGE_COLOR);
		Mat img1 = imread(img_name1, CV_LOAD_IMAGE_COLOR);

		//Mat img2 = imread(img_name2, CV_LOAD_IMAGE_COLOR);
		//Mat img3 = imread(img_name3, CV_LOAD_IMAGE_COLOR);

		Mat sub_0, sub_1, sub_2, sub_3;
		resize(img0, sub_0, Size(sub_w, sub_h), 0.0, 0.0);
		resize(img1, sub_1, Size(sub_w, sub_h), 0.0, 0.0);
		//resize(img2, sub_2, Size(sub_w, sub_h), 0.0, 0.0);
		//resize(img3, sub_3, Size(sub_w, sub_h), 0.0, 0.0);

		//sub_0(Rect(0, 0, log0.cols, log0.rows)) = 0; //log0;
		log0.copyTo(sub_0(Rect(2, 2, log0.cols, log0.rows)));
		log1.copyTo(sub_1(Rect(2, 2, log1.cols, log1.rows)));
		//log2.copyTo(sub_2(Rect(2, 2, log2.cols, log2.rows)));
		//log3.copyTo(sub_3(Rect(2, 2, log3.cols, log3.rows)));
		//char sc[512];


		int stax = margin;
		int stay = margin;
		//canvas(Rect(stax, stay, sub_w, sub_h)) = sub_0.clone();
		sub_0.copyTo(canvas(Rect(stax, stay, sub_w, sub_h)));
		//canvas(Rect(stax+margin+sub_w, stay, sub_w, sub_h)) = sub_1.clone();
		sub_1.copyTo(canvas(Rect(stax + margin + sub_w, stay, sub_w, sub_h)));
		//canvas(Rect(stax, stay + margin + sub_h, sub_w, sub_h)) = sub_2.clone();
		//sub_2.copyTo(canvas(Rect(stax, stay + margin + sub_h, sub_w, sub_h)));
		//canvas(Rect(stax + margin + sub_w, stay + margin + sub_h, sub_w, sub_h)) = sub_3.clone();
		//sub_3.copyTo(canvas(Rect(stax + margin + sub_w, stay + margin + sub_h, sub_w, sub_h)));

		imshow("video", canvas);
		waitKey(10);

		writer << canvas;
		//imshow("video", canvas);
		cnt++;
	}
}
void main() {
	make_video1();
	//dummy_test4();
}
