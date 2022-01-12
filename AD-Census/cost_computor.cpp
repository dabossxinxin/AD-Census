#include "cost_computor.h"
#include "adcensus_util.h"

CostComputor::CostComputor(): width_(0), height_(0), img_left_(nullptr), img_right_(nullptr),
                              lambda_ad_(0), lambda_census_(0), min_disparity_(0), max_disparity_(0),
                              is_initialized_(false) { }

CostComputor::~CostComputor()
{
	
}

bool CostComputor::Initialize(const sint32& width, const sint32& height, const sint32& min_disparity, const sint32& max_disparity)
{
	width_ = width;
	height_ = height;
	min_disparity_ = min_disparity;
	max_disparity_ = max_disparity;

	const sint32 img_size = width_ * height_;
	const sint32 disp_range = max_disparity_ - min_disparity_;
	if (img_size <= 0 || disp_range <= 0) {
		is_initialized_ = false;
		return false;
	}

	// �Ҷ����ݣ�����Ӱ��
	gray_left_.resize(img_size);
	gray_right_.resize(img_size);
	// census���ݣ�����Ӱ��
	census_left_.resize(img_size,0);
	census_right_.resize(img_size,0);
	// ��ʼ��������
	cost_init_.resize(img_size * disp_range);

	is_initialized_ = !gray_left_.empty() && !gray_right_.empty() && !census_left_.empty() && !census_right_.empty() && !cost_init_.empty();
	return is_initialized_;
}

void CostComputor::SetData(const uint8* img_left, const uint8* img_right)
{
	img_left_ = img_left;
	img_right_ = img_right;
}

void CostComputor::SetParams(const sint32& lambda_ad, const sint32& lambda_census)
{
	lambda_ad_ = lambda_ad;
	lambda_census_ = lambda_census;
}

void CostComputor::ComputeGray()
{
	// ��ɫת�Ҷ�
	for (sint32 n = 0; n < 2; n++) {
		const auto color = (n == 0) ? img_left_ : img_right_;
		auto& gray = (n == 0) ? gray_left_ : gray_right_;
		for (sint32 y = 0; y < height_; y++) {
			for (sint32 x = 0; x < width_; x++) {
				const auto b = color[y * width_ * 3 + 3 * x];
				const auto g = color[y * width_ * 3 + 3 * x + 1];
				const auto r = color[y * width_ * 3 + 3 * x + 2];
				gray[y * width_ + x] = uint8(r * 0.299 + g * 0.587 + b * 0.114);
			}
		}
	}
}

void CostComputor::CensusTransform()
{
	// ����Ӱ��census�任
	adcensus_util::census_transform_9x7(&gray_left_[0], census_left_, width_, height_);
	adcensus_util::census_transform_9x7(&gray_right_[0], census_right_, width_, height_);
}

void CostComputor::ComputeCost()
{
	const sint32 disp_range = max_disparity_ - min_disparity_;

	// Ԥ�����
	const auto lambda_ad = lambda_ad_;
	const auto lambda_census = lambda_census_;

	// �������
	for (sint32 y = 0; y < height_; y++) {
		for (sint32 x = 0; x < width_; x++) {
			const auto bl = img_left_[y * width_ * 3 + 3 * x];
			const auto gl = img_left_[y * width_ * 3 + 3 * x + 1];
			const auto rl = img_left_[y * width_ * 3 + 3 * x + 2];
			const auto& census_val_l = census_left_[y * width_ + x];
			// ���Ӳ�������ֵ
			for (sint32 d = min_disparity_; d < max_disparity_; d++) {
				auto& cost = cost_init_[y * width_ * disp_range + x * disp_range + (d - min_disparity_)];
				const sint32 xr = x - d;
				if (xr < 0 || xr >= width_) {
					cost = 1.0f;
					continue;
				}

				// ad����
				const auto br = img_right_[y * width_ * 3 + 3 * xr];
				const auto gr = img_right_[y * width_ * 3 + 3 * xr + 1];
				const auto rr = img_right_[y * width_ * 3 + 3 * xr + 2];
				const float32 cost_ad = (abs(bl - br) + abs(gl - gr) + abs(rl - rr)) / 3.0f;

				// census����
				const auto& census_val_r = census_right_[y * width_ + xr];
				const float32 cost_census = static_cast<float32>(adcensus_util::Hamming64(census_val_l, census_val_r));

				// ad-census����
				cost = 1 - exp(-cost_ad / lambda_ad) + 1 - exp(-cost_census / lambda_census);
			}
		}
	}
}

void CostComputor::Compute()
{
	if(!is_initialized_) {
		return;
	}

	// ����Ҷ�ͼ
	ComputeGray();

	// census�任
	CensusTransform();

	// ���ۼ���
	ComputeCost();
}

float32* CostComputor::get_cost_ptr()
{
	if (!cost_init_.empty()) {
		return &cost_init_[0];
	}
	else {
		return nullptr;
	}
}
