#ifndef AD_CENSUS_COST_COMPUTOR_H_
#define AD_CENSUS_COST_COMPUTOR_H_

#include "adcensus_types.h"

/**
 * \brief ���ۼ�������
 */
class CostComputor {
public:
	CostComputor();
	~CostComputor();

	/**
	 * \brief ��ʼ��
	 * \param width			Ӱ���
	 * \param height		Ӱ���
	 * \param min_disparity	��С�Ӳ�
	 * \param max_disparity	����Ӳ�
	 * \return true: ��ʼ���ɹ�
	 */
	bool Initialize(const sint32& width, const sint32& height, const sint32& min_disparity, const sint32& max_disparity);

	/**
	 * \brief ���ô��ۼ�����������
	 * \param img_left		// ��Ӱ�����ݣ���ͨ��
	 * \param img_right		// ��Ӱ�����ݣ���ͨ��
	 */
	void SetData(const uint8* img_left, const uint8* img_right);

	/**
	 * \brief ���ô��ۼ������Ĳ���
	 * \param lambda_ad		// lambda_ad
	 * \param lambda_census // lambda_census
	 */
	void SetParams(const sint32& lambda_ad, const sint32& lambda_census);

	/** \brief �����ʼ���� */
	void Compute();

	/** \brief ��ȡ��ʼ��������ָ�� */
	float32* get_cost_ptr();

private:
	/** \brief ����Ҷ����� */
	void ComputeGray();

	/** \brief Census�任 */
	void CensusTransform();

	/** \brief ������� */
	void ComputeCost();
private:
	/** \brief ͼ��ߴ� */
	sint32	width_;
	sint32	height_;

	/** \brief Ӱ������ */
	const uint8* img_left_;
	const uint8* img_right_;

	/** \brief ��Ӱ��Ҷ�����	 */
	vector<uint8> gray_left_;
	/** \brief ��Ӱ��Ҷ�����	 */
	vector<uint8> gray_right_;

	/** \brief ��Ӱ��census����	*/
	vector<uint64> census_left_;
	/** \brief ��Ӱ��census����	*/
	vector<uint64> census_right_;

	/** \brief ��ʼƥ�����	*/
	vector<float32> cost_init_;

	/** \brief lambda_ad*/
	sint32 lambda_ad_;
	/** \brief lambda_census*/
	sint32 lambda_census_;

	/** \brief ��С�Ӳ�ֵ */
	sint32 min_disparity_;
	/** \brief ����Ӳ�ֵ */
	sint32 max_disparity_;

	/** \brief �Ƿ�ɹ���ʼ����־	*/
	bool is_initialized_;
};
#endif