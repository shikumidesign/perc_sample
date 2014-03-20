#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <pxcsession.h>
#include <pxcsmartptr.h>
#include <pxcprojection.h>
#include <pxcmetadata.h>
#include <util_capture.h>

#include "wave.h"
#include "refract.h"

void OnWaveCChanged(int value, void* user_data) {
	Wave* wave_object = (Wave*)user_data;
	wave_object->set_c((float)value / 100.0f);
}

void OnWaveDChanged(int value, void* user_data) {
	Wave* wave_object = (Wave*)user_data;
	wave_object->set_D((float)value / 100.0f);
}

void OnWaveKChanged(int value, void* user_data) {
	Wave* wave_object = (Wave*)user_data;
	wave_object->set_K((float)value / 100.0f);
}

void OnWaveIteratorChanged(int value, void* user_data) {
	Wave* wave_object = (Wave*)user_data;
	wave_object->set_num_iteration(value);
}

void OnWaveFixedBoundaryChanged(int value, void* user_data) {
	Wave* wave_object = (Wave*)user_data;
	wave_object->set_fixed_boundary(value);
}

void OnRefractAlphaChanged(int value, void* user_data) {
	Refract* refract_object = (Refract*)user_data;
	refract_object->set_alpha((float)value);
}

void Binalize(const cv::Mat& depth_image, const cv::Mat& uv_map, cv::Mat1b& binary_image, short depth_threshold_mm) {
	assert(depth_image.size() == uv_map.size());
	assert(depth_image.type() == CV_16SC1);
	assert(uv_map.type() == CV_32FC2);

	cv::Size binary_size = binary_image.size();
	//Clear all pixels of binary_image
	binary_image = 0;

	cv::Size loop_size = depth_image.size();
	if(depth_image.isContinuous() && uv_map.isContinuous()) {
		loop_size.width *= loop_size.height;
		loop_size.height = 1;
	}
	for(int i = 0; i < loop_size.height; ++i) {
		const short* depth = depth_image.ptr<short>(i);
		const cv::Vec2f* uv = uv_map.ptr<cv::Vec2f>(i);
		for(int j = 0; j < loop_size.width; ++j) {
			if(depth[j] < depth_threshold_mm) {
				int x = cvRound(uv[j][0] * binary_size.width);
				int y = cvRound(uv[j][1] * binary_size.height);
				if(0 <= x && x < binary_size.width && 0 <= y && y < binary_size.height) {
					binary_image.at<unsigned char>(cv::Point(x, y)) = 255;
				}
			}
		}
	}
}

void BlendScreen(cv::Mat3b& target_image, const cv::Mat1b& binary_image, cv::Vec3b color) {
	assert(target_image.size() == binary_image.size());

	cv::Size loop_size = target_image.size();
	if(target_image.isContinuous() && binary_image.isContinuous()) {
		loop_size.width *= loop_size.height;
		loop_size.height = 1;
	}
	for(int i = 0; i < loop_size.height; ++i) {
		cv::Vec3b* target = target_image.ptr<cv::Vec3b>(i);
		const unsigned char* binary = binary_image.ptr<unsigned char>(i);
		for(int j = 0; j < loop_size.width; ++j) {
			if(!binary[j]) {
				target[j][0] = target[j][0] + color[0] - (target[j][0] * color[0]) / 255;
				target[j][1] = target[j][1] + color[1] - (target[j][1] * color[1]) / 255;
				target[j][2] = target[j][2] + color[2] - (target[j][2] * color[2]) / 255;
			}
		}
	}
}

int main() {
	std::cout << "Wave Effect Sample" << std::endl;

	//Initialize PerC Device
	PXCSmartPtr<PXCSession> session;
	pxcStatus sts = PXCSession_Create(&session);
	if(sts < PXC_STATUS_NO_ERROR) {
		std::cout << "PXCSession_Create error " << sts << std::endl;
		return 1;
	}
	UtilCapture capture(session);

	PXCCapture::VideoStream::DataDesc req;
	memset(&req, 0, sizeof(req));
	req.streams[0].format = PXCImage::COLOR_FORMAT_RGB24;
	req.streams[1].format = PXCImage::COLOR_FORMAT_DEPTH;

	sts = capture.LocateStreams(&req);
	if(sts < PXC_STATUS_NO_ERROR) {
		return 2;
	}

	PXCCapture::VideoStream::ProfileInfo camera_info, depth_info;
	capture.QueryVideoStream(0)->QueryProfile(&camera_info);
	const cv::Size camera_size(camera_info.imageInfo.width, camera_info.imageInfo.height);
	capture.QueryVideoStream(1)->QueryProfile(&depth_info);
	const cv::Size depth_size(depth_info.imageInfo.width, depth_info.imageInfo.height);

	cv::Mat3b camera_image(camera_size);
	cv::Mat1b binary_image;
	cv::Mat1b binary_show_image;
	cv::Mat1b resized_binary_image;
	cv::Mat1b dilated_binary_image;
	cv::Mat1f wave_image;
	cv::Mat1f wave_show_image;
	cv::Mat3b refract_image;

	Wave wave_object(depth_size);
	Refract refract_object;

	int distance_threshold_10cm = 5;
	int dilate_interation = 5;

	cv::namedWindow("binary");
	cv::namedWindow("wave");
	cv::namedWindow("refract");
	cv::createTrackbar("distance", "binary", &distance_threshold_10cm, 10);
	cv::createTrackbar("dilate", "binary", &dilate_interation, 10);
	cv::createTrackbar("C/100", "wave", NULL, 70, OnWaveCChanged, &wave_object);
	cv::setTrackbarPos("C/100", "wave", (int)(wave_object.get_c() * 100));
	cv::createTrackbar("D/100", "wave", NULL, 100, OnWaveDChanged, &wave_object);
	cv::setTrackbarPos("D/100", "wave", (int)(wave_object.get_D() * 100));
	cv::createTrackbar("K/100", "wave", NULL, 100, OnWaveKChanged, &wave_object);
	cv::setTrackbarPos("K/100", "wave", (int)(wave_object.get_K() * 100));
	cv::createTrackbar("iteration", "wave", NULL, 10, OnWaveIteratorChanged, &wave_object);
	cv::setTrackbarPos("iteration", "wave", wave_object.get_num_iteration());
	cv::createTrackbar("fixed", "wave", NULL, 1, OnWaveFixedBoundaryChanged, &wave_object);
	cv::setTrackbarPos("fixed", "wave", wave_object.get_fixed_boundary());
	cv::createTrackbar("alpha", "refract", NULL, 100, OnRefractAlphaChanged, &refract_object);
	cv::setTrackbarPos("alpha", "refract", (int)refract_object.get_alpha());

	while(true) {

		PXCSmartArray<PXCImage> images;
		PXCSmartSP sp;
		sts = capture.ReadStreamAsync(images, &sp);
		if(sts < PXC_STATUS_NO_ERROR) {
			break;
		}
		sts = sp->Synchronize();
		if(sts < PXC_STATUS_NO_ERROR) {
			break;
		}

		//Get camera image
		PXCImage::ImageData camera_data;
		images[0]->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::COLOR_FORMAT_RGB24, &camera_data);
		const cv::Mat wrap_camera(camera_size, CV_8UC3, camera_data.planes[0], camera_data.pitches[0]);
		cv::flip(wrap_camera, camera_image, 1);//Flip around y-axis
		images[0]->ReleaseAccess(&camera_data);

		//Get depth image
		PXCImage::ImageData depth_data;
		images[1]->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::COLOR_FORMAT_DEPTH, &depth_data);
		const cv::Mat wrap_depth(depth_size, CV_16SC1, depth_data.planes[0], depth_data.pitches[0]);
		const cv::Mat wrap_uv(depth_size, CV_32FC2, depth_data.planes[2], depth_data.pitches[2]);

		binary_image.create(depth_size);
		Binalize(wrap_depth, wrap_uv, binary_image, distance_threshold_10cm * 100);
		cv::flip(binary_image, binary_image, 1);//Flip around y-axis

		images[0]->ReleaseAccess(&depth_data);

		//Dilate
		cv::dilate(binary_image, dilated_binary_image, cv::Mat(), cv::Point(-1, -1), dilate_interation, cv::BORDER_REPLICATE);
		//Resize to display
		cv::resize(dilated_binary_image, binary_show_image, cv::Size(400, 300));

		//Apply wave propagation
		wave_object(dilated_binary_image, wave_image);
		//Convert to display
		cv::resize(wave_image, wave_show_image, cv::Size(400, 300));
		wave_show_image.convertTo(wave_show_image, wave_show_image.type(), 0.5, 0.5);

		//Apply refraction
		refract_object(camera_image, wave_image, refract_image);

		//Blend screen
		cv::resize(dilated_binary_image, resized_binary_image, camera_size);
		BlendScreen(refract_image, resized_binary_image, cv::Vec3b(40, 30, 0));

		//Display binary image
		cv::imshow("binary", binary_show_image);
		cv::imshow("wave", wave_show_image);
		cv::imshow("refract", refract_image);

		//Exit if ESC key is pressed
		if(cv::waitKey(1) == 0x1b) {
			break;
		}
	}

	cv::destroyAllWindows();

	return 0;
}
