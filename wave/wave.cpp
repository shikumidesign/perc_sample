#include "wave.h"

Wave::Wave(cv::Size input_size) : m_input_size(input_size), m_t1(0), m_t2(1), m_t3(2) {

	m_wave_size = cv::Size(input_size.width + 2, input_size.height + 2);
	for(int i = 0; i < 3; ++i) {
		m_u[i].create(m_wave_size);
		m_u[i] = 0.0f;
	}

	m_c = 0.56f;
	m_d = 0.08f;
	m_k = 0.08f;
	m_num_iteration = 3;
	m_fixed_boundary = 0;
	calc_coef();
}

Wave::~Wave() {}

void Wave::operator()(const cv::Mat1b& binary_image, cv::Mat1f& wave_image) {

	assert(binary_image.size() == m_input_size);

	for(int n = 0; n < m_num_iteration; ++n) {
		//swap
		int temp = m_t1;
		m_t1 = m_t2;
		m_t2 = m_t3;
		m_t3 = temp;

		int width = m_wave_size.width;
		for(int i = 0; i < m_input_size.height; ++i) {
			const unsigned char* input = binary_image.ptr(i);
			const float* ut1 = m_u[m_t1].ptr<float>(i + 1) + 1;
			const float* ut2 = m_u[m_t2].ptr<float>(i + 1) + 1;
			float* output = m_u[m_t3].ptr<float>(i + 1) + 1;
			for(int j = 0; j < m_input_size.width; ++j) {
				if(input[j]) {
					//near-mode
					//assign 1
					output[j] = 1.0f;
				} else {
					//far-mode
					//apply wave equation
					output[j] = m_xy_coef * (ut2[j - 1] + ut2[j + 1] + ut2[j - width] + ut2[j + width])
						+ m_t1_coef * ut1[j] + m_t2_coef * ut2[j];
					if(output[j] < -1.0f) {
						output[j] = -1.0f;
					}
					if(output[j] > 1.0f) {
						output[j] = 1.0f;
					}
				}
			}
		}

		if(m_fixed_boundary) {
			//Fixed boundary
			for(int i = 1; i < m_wave_size.height - 1; ++i) {
				float* p = m_u[m_t3].ptr<float>(i);
				p[0] = 0.0f;
				p[m_wave_size.width - 1] = 0.0f;
			}
			float* top = m_u[m_t3].ptr<float>(0);
			float* bottom = m_u[m_t3].ptr<float>(m_wave_size.height - 1);
			for(int i = 0; i < m_wave_size.width; ++i) {
				top[i] = 0.0f;
				bottom[i] = 0.0f;
			}
		} else {
			//Free boundary
			for(int i = 1; i < m_wave_size.height - 1; ++i) {
				float* p = m_u[m_t3].ptr<float>(i);
				p[0] = p[1];
				p[m_wave_size.width - 1] = p[m_wave_size.width - 2];
			}
			float* top0 = m_u[m_t3].ptr<float>(0);
			const float* top1 = m_u[m_t3].ptr<float>(1);
			float* bottom0 = m_u[m_t3].ptr<float>(m_wave_size.height - 1);
			const float* bottom1 = m_u[m_t3].ptr<float>(m_wave_size.height - 2);
			for(int i = 0; i < m_wave_size.width; ++i) {
				top0[i] = top1[i];
				bottom0[i] = bottom1[i];
			}
		}
	}

	wave_image = m_u[m_t3];
}

void Wave::set_c(float c) {
	m_c = c;
	calc_coef();
}

void Wave::set_D(float d) {
	m_d = d;
	calc_coef();
}

void Wave::set_K(float k) {
	m_k = k;
	calc_coef();
}

void Wave::calc_coef() {
	m_xy_coef = 2.0f * m_c * m_c / (m_d + 2);
	m_t1_coef = (m_d - 2.0f) / (m_d + 2.0f);
	m_t2_coef = (4.0f - 8.0f * m_c * m_c - m_k) / (m_d + 2.0f);
}

void Wave::set_num_iteration(int n) {
	m_num_iteration = n;
}

void Wave::set_fixed_boundary(int is_fixed) {
	m_fixed_boundary = is_fixed;
}