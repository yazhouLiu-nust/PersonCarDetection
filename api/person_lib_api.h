#ifndef PERSON_LIB_API
#define PERSON_LIB_API _declspec(dllexport)

#ifdef __cplusplus
extern "C"
{
#endif

//..Error codes
#define ERROR_INI -1		//..InI file load error
#define ERROR_WEI -2		//..Weight file load error
#define ERROR_IMG -3		//..image format error
#define ERROR_SIZ -4		//..input image is too big, max size is 1920*1080*3
#define ERROR_CHN -5		//..unsupported channel number, only 1 or 3 channel images are support

//..
#define MAX_REGION_NO	20	//..Max no of returned region per image

//..APIs
/**
* Init the person detection module
* @param [in] path_ini: Init file path
* @param [in] path_weights: Weight file pat
* @return 
*  0  Success
*  <0 Error
*/
PERSON_LIB_API int Person_Init(
	char* path_ini, 
	char* path_weights
);
/**
* Init the person detection module: NOTE: image size limit 1920*1080
* @param [in] img_data: Image data buffer, BGR format [BGR BGR BGR BGR ... BGR],
* @param [in] img_width: Image width
* @param [in] img_heigh: Image height
* @param [in] img_chn: Image channel, 3 channel is suggested
* @param [out] obj_No: No of objects in current frame
* @param [out] obj_ID: the ids of the detections
* @param [out] obj_box: Box of each object, in [l,t,r,b,...,l,t,r,b] format
* @param [out] obj_score: Scores of each object
* @param [in] thre: detection threshold, 0.24 is the default value
* @return
*  0  Success
*  <0 Error
*/
PERSON_LIB_API int Person_Detect(
	unsigned char* img_data, 
	int img_width, 
	int img_height, 
	int img_chn, 
	int* obj_No, 
	int obj_ID[MAX_REGION_NO],
	int obj_box[MAX_REGION_NO * 4], 
	float obj_score[MAX_REGION_NO],
	float thre// = .24
);
/**
* Release the model
*  0  Success
*  <0 Error
*/
PERSON_LIB_API int Person_Release(
);

/**
* Dummy test function set the center 1/4 image as blank
* 
*  0  Success
*  <0 Error
*/
PERSON_LIB_API int dummy_test(
	unsigned char* img_data,
	int img_width,
	int img_height,
	int img_chn
);

#ifdef __cplusplus
}
#endif

#endif