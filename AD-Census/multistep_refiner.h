#ifndef AD_CENSUS_MULTISTEP_REFINER_H_
#define AD_CENSUS_MULTISTEP_REFINER_H_

#include "adcensus_types.h"
#include "cross_aggregator.h"

class MultiStepRefiner
{
public:
	MultiStepRefiner();
	~MultiStepRefiner();

	/**
	 * \brief ��ʼ��
	 * \param width		Ӱ���
	 * \param height	Ӱ���
	 * \return true:��ʼ���ɹ�
	 */
	bool Initialize(const sint32& width, const sint32& height);

	/**
	 * \brief ���öಽ�Ż�������
	 * \param img_left			// ��Ӱ�����ݣ���ͨ��
	 * \param cost				// ��������
	 * \param cross_arms		// ʮ�ֽ��������
	 * \param disp_left			// ����ͼ�Ӳ�����
	 * \param disp_right		// ����ͼ�Ӳ�����
	 */
	void SetData(const uint8* img_left, float32* cost,const CrossArm* cross_arms, float32* disp_left, float32* disp_right);


	/**
	 * \brief ���öಽ�Ż��Ĳ���
	 * \param min_disparity					// ��С�Ӳ�
	 * \param max_disparity					// ����Ӳ�
	 * \param irv_ts						// Iterative Region Voting����ts
	 * \param irv_th						// Iterative Region Voting����th
	 * \param lrcheck_thres					// һ���Լ����ֵ
	 * \param do_lr_check					// �Ƿ�������һ����
	 * \param do_region_voting				// �Ƿ����ڲ����
	 * \param do_interpolating				// �Ƿ�ֲ�ͶƱ���
	 * \param do_discontinuity_adjustment	// �Ƿ���������������
	 */
	void SetParam(const sint32& min_disparity, const sint32& max_disparity, const sint32& irv_ts, const float32& irv_th, const float32& lrcheck_thres,
				  const bool&	do_lr_check, const bool& do_region_voting, const bool& do_interpolating, const bool& do_discontinuity_adjustment);

	/** \brief �ಽ�Ӳ��Ż� */
	void Refine();

private:
	//------4С���Ӳ��Ż�------//
	/** \brief ��Ⱥ���� */
	void OutlierDetection();
	/** \brief �����ֲ�ͶƱ */
	void IterativeRegionVoting();
	/** \brief �ڲ���� */
	void ProperInterpolation();
	/** \brief ��ȷ��������Ӳ���� */
	void DepthDiscontinuityAdjustment();

	/** \brief �Ӳ�ͼ��Ե���	 */
	static void EdgeDetect(uint8* edge_mask, const float32* disp_ptr,const sint32& width,const sint32& height, const float32 threshold);
private:
	/** \brief ͼ��ߴ� */
	sint32	width_;
	sint32	height_;

	/** \brief ��Ӱ�����ݣ���ͨ���� */
	const uint8* img_left_;
	
	/** \brief �������� */
	float32* cost_;
	/** \brief ��������� */
	const CrossArm* cross_arms_;

	/** \brief ����ͼ�Ӳ����� */
	float* disp_left_;
	/** \brief ����ͼ�Ӳ����� */
	float* disp_right_;

	/** \brief ����ͼ��Ե���� */
	vector<uint8> vec_edge_left_;
	
	/** \brief ��С�Ӳ�ֵ */
	sint32 min_disparity_;
	/** \brief ����Ӳ�ֵ */
	sint32 max_disparity_;

	/** \brief Iterative Region Voting����ts */
	sint32	irv_ts_;
	/** \brief Iterative Region Voting����th */
	float32 irv_th_;


	float32 lrcheck_thres_;

	/** \brief �Ƿ�������һ���� */
	bool	do_lr_check_;				
	/** \brief �Ƿ�ֲ�ͶƱ��� */
	bool	do_region_voting_;
	/** \brief �Ƿ����ڲ���� */
	bool	do_interpolating_;
	/** \brief �Ƿ��������������� */
	bool	do_discontinuity_adjustment_;
	
	/** \brief �ڵ������ؼ�	*/
	vector<pair<int, int>> occlusions_;
	/** \brief ��ƥ�������ؼ�	*/
	vector<pair<int, int>> mismatches_;
};
#endif
