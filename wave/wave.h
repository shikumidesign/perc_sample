#ifndef SHIKUMI_WAVE_H_
#define SHIKUMI_WAVE_H_

#include <opencv2/core/core.hpp>

class Wave {

public:
	Wave(cv::Size process_size);
	~Wave();

	void operator()(const cv::Mat1b& binary_image, cv::Mat1f& wave_image);

	void set_c(float c);
	float get_c() const {
		return m_c;
	}
	void set_D(float d);
	float get_D() const {
		return m_d;
	}
	void set_K(float k);
	float get_K() const {
		return m_k;
	}
	void set_num_iteration(int n);
	int get_num_iteration() const {
		return m_num_iteration;
	}
	void set_fixed_boundary(int is_fixed);
	int get_fixed_boundary() const {
		return m_fixed_boundary;
	}

private:
	void calc_coef();

	cv::Size m_input_size;
	cv::Size m_wave_size;
	int m_t1, m_t2, m_t3;
	cv::Mat1f m_u[3];
	float m_c, m_d, m_k;
	float m_xy_coef, m_t1_coef, m_t2_coef;
	int m_num_iteration;
	int m_fixed_boundary;
};

#endif /*SHIKUMI_WAVE_H_*/
