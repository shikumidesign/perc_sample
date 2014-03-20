#ifndef SHIKUMI_REFRACT_H_
#define SHIKUMI_REFRACT_H_

#include <opencv2/core/core.hpp>

class Refract {

public:
	Refract();
	~Refract();

	void operator()(const cv::Mat3b& input_image, const cv::Mat1f& wave_image, cv::Mat3b& output_image);

	void set_alpha(float value) {
		m_alpha = value;
	}
	float get_alpha() const {
		return m_alpha;
	}

private:
	float m_alpha;
	cv::Mat2f m_derivation;
	cv::Mat2f m_derivation_resized;
};

#endif /*SHIKUMI_REFRACT_H_*/
