#include "network.h"
#include "region_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
#include "option_list.h"

#include "person_lib_api.h"


//extern float get_pixel(image m, int x, int y, int c);

#define MAX_BUF_SIZE 1920 * 1080 * 3

//char *names[] = { "Person", "Car", "Bus", "Truck" };

//int interest_id[] = {14,6,5};//voc
//int interest_id[] = { 0,2,5,7};//coco
int interest_id[] = {0,1,2,3};//Navlab
#define ID_NO sizeof(interest_id)/sizeof(interest_id[0])

network net;
float *img_buf=0;// [MAX_BUF_SIZE];
image g_img_net;

image **alphabet = 0;

float inv_255 = 0.003921568;

float* in_resized = 0;
float* in_part = 0;
float* in_boxed = 0;

image resized, part, boxed;
//box *boxes = calloc(l.w*l.h*l.n, sizeof(box));
//float box_buff[100 * 100 * 100];
//box *boxes = (box*)box_buff;

box *boxes = 0;
float **probs = 0;
float **masks = 0;


PERSON_LIB_API int Person_Init(char* path_ini, char* path_weights){

	net = parse_network_cfg(path_ini);
	if (path_weights){
		load_weights(&net, path_weights);
	}
	set_batch_network(&net, 1);
	srand(2222222);



	layer l = net.layers[net.n - 1];

	img_buf = malloc(MAX_BUF_SIZE * sizeof(float));//new float[net.w*net.h * 3];

	in_resized = malloc(net.w*net.h * 3 * sizeof(float));//new float[net.w*net.h * 3];
	in_part = malloc(net.w * 1920 * 3 * sizeof(float));//new float[net.w*1920 * 3];
	in_boxed = malloc(net.w*net.h * 3 * sizeof(float));//new float[net.w*net.h * 3];

	g_img_net.data = img_buf;

	boxes = (box*)calloc(l.w*l.h*l.n, sizeof(box));
	probs = (float **)calloc(l.w*l.h*l.n, sizeof(float *));

	for (int j = 0; j < l.w*l.h*l.n; ++j) probs[j] = (float *)calloc(l.classes, sizeof(float *));

	if (l.coords > 4) {
		masks = calloc(l.w*l.h*l.n, sizeof(float*));
		for (int j = 0; j < l.w*l.h*l.n; ++j) masks[j] = calloc(l.coords - 4, sizeof(float *));
	}


	return 1;
}
PERSON_LIB_API int Person_Release(
){
	if (in_resized) free(in_resized); in_resized = 0;//delete[]in_resized; in_resized = 0;
	if (in_part) free(in_part); in_part = 0;//delete[]in_part; in_part = 0;
	if (img_buf) free(img_buf); img_buf = 0;
	if (in_boxed) free(in_boxed); in_boxed = 0;

	free(boxes);
	layer l = net.layers[net.n - 1];
	free_ptrs((void **)probs, l.w*l.h*l.n);

	if (l.coords > 4) {
		for (int j = 0; j < l.w*l.h*l.n; ++j)
		{
			if(masks[j]) free(masks[j]); masks[j] = 0;
		}
		if(masks) free(masks); masks = 0;
	}

	free_network(net);
	return 1;
}

image resize_g_image(image im, int w, int h)
{
	//image resized = make_image(w, h, im.c);
	//image part = make_image(w, im.h, im.c);
	resized.w = w;
	resized.h = h;
	resized.c = im.c;
	resized.data = in_resized;

	part.w = w;
	part.h = im.h;
	part.c = im.c;
	part.data = in_part;

	int r, c, k;
	float w_scale = (float)(im.w - 1) / (w - 1);
	float h_scale = (float)(im.h - 1) / (h - 1);
	for (k = 0; k < im.c; ++k){
		for (r = 0; r < im.h; ++r){
			for (c = 0; c < w; ++c){
				float val = 0;
				if (c == w - 1 || im.w == 1){
					val = get_pixel(im, im.w - 1, r, k);
				}
				else {
					float sx = c*w_scale;
					int ix = (int)sx;
					float dx = sx - ix;
					val = (1 - dx) * get_pixel(im, ix, r, k) + dx * get_pixel(im, ix + 1, r, k);
				}
				set_pixel(part, c, r, k, val);
			}
		}
	}
	for (k = 0; k < im.c; ++k){
		for (r = 0; r < h; ++r){
			float sy = r*h_scale;
			int iy = (int)sy;
			float dy = sy - iy;
			for (c = 0; c < w; ++c){
				float val = (1 - dy) * get_pixel(part, c, iy, k);
				set_pixel(resized, c, r, k, val);
			}
			if (r == h - 1 || im.h == 1) continue;
			for (c = 0; c < w; ++c){
				float val = dy * get_pixel(part, c, iy + 1, k);
				add_pixel(resized, c, r, k, val);
			}
		}
	}

	return resized;
}

image letterbox_g_image(image im, int w, int h)
{
	int new_w = im.w;
	int new_h = im.h;
	if (((float)w / im.w) < ((float)h / im.h)) {
		new_w = w;
		new_h = (im.h * w) / im.w;
	}
	else {
		new_h = h;
		new_w = (im.w * h) / im.h;
	}
	//image resized = resize_g_image(im, new_w, new_h);
	resize_g_image(im, new_w, new_h);
	//image boxed = make_image(w, h, im.c);
	boxed.w = w;
	boxed.h = h;
	boxed.c = im.c;
	boxed.data = in_boxed;

	fill_image(boxed, .5);
	//int i;
	//for(i = 0; i < boxed.w*boxed.h*boxed.c; ++i) boxed.data[i] = 0;
	embed_image(resized, boxed, (w - new_w) / 2, (h - new_h) / 2);
	//free_image(resized);
	return boxed;
}


int make_g_image(unsigned char* img_data,	int img_width,	int img_height,	int img_chn)
{
	unsigned char *data = img_data;
	int h = img_height;
	int w = img_width;
	int c = 3;

	g_img_net.w = w;
	g_img_net.h = h;
	g_img_net.c = 3;

	int i, j, k, count = 0;;

	if (img_chn == 1){
		for (k = 0; k < c; ++k){
			for (i = 0; i < h; ++i){
				for (j = 0; j < w; ++j){
					g_img_net.data[count++] = img_data[i*w + j] * inv_255;
				}
			}
		}
		rgbgr_image(g_img_net);
		return 1;
	}else if (img_chn == 3){
		for (k = 0; k < c; ++k){
			for (i = 0; i < h; ++i){
				for (j = 0; j < w; ++j){
					g_img_net.data[count++] = img_data[i*w*c + j*c + k] * inv_255;
					//g_img_net.data[count++] = img_data[i*w*c + j*c + 2-k] * inv_255;
					//g_img_net.data[count++] = img_data[i*w*c + j*c + k]/255.;
				}
			}
		}
		//rgbgr_image(g_img_net);
		return 1;
	} else{
		return -5;
	}

}
int get_detections(int imw, int imh, int num, float thresh, box *boxes, float **probs, /*char **names,*/ image **alphabet, int classes, int* ids, int* regions, float* scores)
{
	int i;

	int cnt = 0;

	for (i = 0; i < num; ++i) {
		int class = max_index(probs[i], classes);
		float prob = probs[i][class];

		int is_interest = -1;
		for (int j = 0; j < ID_NO; j++) {
			if (class == interest_id[j]) {
				is_interest = j;
			}
		}

		if (prob > thresh && is_interest >= 0) {

			int width = imh * .012;

			//if (0){
			//	width = pow(prob, 1. / 2.) * 10 + 1;
			alphabet = 0;
			//}

			//printf("%s: %.0f%%\n", names[is_interest], prob * 100);
			//printf("%s: %.0f%%\n", "Person", prob * 100);
			scores[cnt] = prob * 100;
			ids[cnt] = is_interest;
			//int offset = class * 123457 % classes;
			//float red = get_color(2, offset, classes);
			//float green = get_color(1, offset, classes);
			//float blue = get_color(0, offset, classes);
			//float rgb[3];

			//width = prob*20+2;

			//rgb[0] = red;
			//rgb[1] = green;
			//rgb[2] = blue;
			box b = boxes[i];

			int left = (b.x - b.w / 2.)*imw;
			int right = (b.x + b.w / 2.)*imw;
			int top = (b.y - b.h / 2.)*imh;
			int bot = (b.y + b.h / 2.)*imh;

			if (left < 0) left = 0;
			if (right > imw - 1) right = imw - 1;
			if (top < 0) top = 0;
			if (bot > imh - 1) bot = imh - 1;


			regions[cnt * 4 + 0] = left;
			regions[cnt * 4 + 1] = top;
			regions[cnt * 4 + 2] = right;
			regions[cnt * 4 + 3] = bot;

			cnt++;

			/*
			draw_box_width(im, left, top, right, bot, width, red, green, blue);
			if (alphabet) {
			image label = get_label(alphabet, names[class], (im.h*.03) / 10);
			draw_label(im, top + width, left, label, rgb);
			}*/
#if 0
			if (class == 0) {//yazhou
				rgb[0] = 0;
				rgb[1] = 1;
				rgb[2] = 0;
				width = 3;
				draw_box_width(im, left, top, right, bot, width, 0, 1, 0);

				if (alphabet) {
					image label = get_label(alphabet, "4zvcloud:Human", 1/*(im.h*.03) / 10*/);
					draw_label(im, top + width, left, label, rgb);
				}
			}
#endif
		}
	}

	return cnt;
}
//int get_detections(int imw, int imh, int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes, int* regions, float* scores);
PERSON_LIB_API int Person_Detect(
	unsigned char* img_data,
	int img_width,
	int img_height,
	int img_chn,
	int* obj_No,
	int obj_ID[MAX_REGION_NO],
	int obj_box[MAX_REGION_NO * 4],
	float obj_score[MAX_REGION_NO],
	float thresh
){

	float hier_thresh = 0.5;

	int img_size = img_width*img_height*img_chn;

	if (img_size > MAX_BUF_SIZE)
		return -4;
	if (img_chn != 1 && img_chn != 3)
		return -5;



	//clock_t time;
	float nms = .4;

	make_g_image(img_data, img_width, img_height, img_chn);

	//image im = load_image_color(input, 0, 0);
	//resized = resize_g_image(g_img_net, net.w, net.h);
	image im = g_img_net;
	image sized = letterbox_g_image(g_img_net, net.w, net.h);


	layer l = net.layers[net.n - 1];
	clock_t time;

	float *X = sized.data;
	//time = what_time_is_it_now();
	network_predict(net, X);
	//printf(": Predicted in %f seconds.\n", what_time_is_it_now() - time);
	//get_region_boxes(l, 1, 1, thresh, probs, boxes, 0, 0);
	get_region_boxes(l, im.w, im.h, net.w, net.h, thresh, probs, boxes, masks, 0, 0, hier_thresh, 1);

	if (nms) do_nms_sort(boxes, probs, l.w*l.h*l.n, l.classes, nms);
	
	*obj_No = get_detections(img_width, img_height, l.w*l.h*l.n, thresh, boxes, probs, /*names, */alphabet, l.classes, obj_ID, obj_box, obj_score);

	//save_image(im, "predictions");
	//show_image(im, "predictions");

	//free_image(im);
	//free_image(sized);
};
PERSON_LIB_API int dummy_test(
	unsigned char* img_data,
	int img_width,
	int img_height,
	int img_chn
){
	int w = img_width;
	int h = img_height;
	int c = img_chn;
	int i, j, k;
	int h_q = h / 4;
	int w_q = w / 4;
	for (k = 0; k < c; ++k){
		for (i = 0; i < h; ++i){
			for (j = 0; j < w; ++j){
				if (i > h_q && i < 3*h_q && j > w_q && j < 3*w_q)
					img_data[i*w*c + j*c + k] = 0;
			}
		}
	}
	return 1;
}
