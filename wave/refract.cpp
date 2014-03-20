#include "refract.h"

#include <opencv2/imgproc/imgproc.hpp>

Refract::Refract() {
	m_alpha = 60.f;
}

Refract::~Refract() {}

void Refract::operator()(const cv::Mat3b& camera_image, const cv::Mat1f& wave_image, cv::Mat3b& output_image) {

	cv::Size wave_size = wave_image.size();
	cv::Size derivation_size(wave_size.width - 2, wave_size.height - 2);
	cv::Size output_size = camera_image.size();

	m_derivation.create(derivation_size);
	for(int i = 0; i < derivation_size.height; ++i) {
		cv::Vec2f* derivation = m_derivation.ptr<cv::Vec2f>(i);
		const float* wave_row0 = wave_image.ptr<float>(i) +1;
		const float* wave_row1 = wave_image.ptr<float>(i + 1) + 1;
		const float* wave_row2 = wave_image.ptr<float>(i + 2) + 1;
		for(int j = 0; j < derivation_size.width; ++j) {
			//Partial differentiation relative to x
			derivation[j][0] = m_alpha * (-wave_row1[j - 1] + wave_row1[j + 1]);
			//Partial differentiation relative to y
			derivation[j][1] = m_alpha * (-wave_row0[j] + wave_row2[j]);
		}
	}

	cv::resize(m_derivation, m_derivation_resized, output_size);
	output_image.create(output_size);

	for(int i = 0; i < output_size.height; ++i) {
		cv::Vec3b* output = output_image.ptr<cv::Vec3b>(i);
		cv::Vec2f* derivation = m_derivation_resized.ptr<cv::Vec2f>(i);
		for(int j = 0; j < output_size.width; ++j) {
			int new_x = j + cvRound(derivation[j][0]);
			int new_y = i + cvRound(derivation[j][1]);
			if(new_y < 0) {
				new_y = 0;
			}
			if(new_y >= output_size.height) {
				new_y = output_size.height - 1;
			}
			if(new_x < 0) {
				new_x = 0;
			}
			if(new_x >= output_size.width) {
				new_x = output_size.width - 1;
			}

			output[j] = camera_image.at<cv::Vec3b>(new_y, new_x);
		}
	}
}
