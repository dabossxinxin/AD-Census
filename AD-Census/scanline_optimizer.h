#ifndef AD_CENSUS_SCANLNE_OPTIMIZER_H_
#define AD_CENSUS_SCANLNE_OPTIMIZER_H_

#include <algorithm>

#include "adcensus_types.h"

/** \brief ɨ�����Ż���*/
class ScanlineOptimizer {
public:
	ScanlineOptimizer();
	~ScanlineOptimizer();


	/**
	 * \brief ��������
	 * \param img_left		// ��Ӱ�����ݣ���ͨ�� 
	 * \param img_right 	// ��Ӱ�����ݣ���ͨ��
	 * \param cost_init 	// ��ʼ��������
	 * \param cost_aggr 	// �ۺϴ�������
	 */
	void SetData(const uint8* img_left, const uint8* img_right, float32* cost_init, float32* cost_aggr);

	/**
	 * \brief 
	 * \param width			// Ӱ���
	 * \param height		// Ӱ���
	 * \param min_disparity	// ��С�Ӳ�
	 * \param max_disparity // ����Ӳ�
	 * \param p1			// p1
	 * \param p2			// p2
	 * \param tso			// tso
	 */
	void SetParam(const sint32& width,const sint32& height, const sint32& min_disparity, const sint32& max_disparity, const float32& p1, const float32& p2, const sint32& tso);

	/**
	 * \brief �Ż� */
	void Optimize();

private:
	/**
	* \brief ����·���Ż� �� ��
	* \param cost_so_src		���룬SOǰ��������
	* \param cost_so_dst		�����SO���������
	* \param is_forward			���룬�Ƿ�Ϊ������������Ϊ�����ң�������Ϊ���ҵ���
	*/
	void ScanlineOptimizeLeftRight(const float32* cost_so_src, float32* cost_so_dst, bool is_forward = true);

	/**
	* \brief ����·���Ż� �� ��
	* \param cost_so_src		���룬SOǰ��������
	* \param cost_so_dst		�����SO���������
	* \param is_forward			���룬�Ƿ�Ϊ������������Ϊ���ϵ��£�������Ϊ���µ��ϣ�
	*/
	void ScanlineOptimizeUpDown(const float32* cost_so_src, float32* cost_so_dst, bool is_forward = true);

	/** \brief ������ɫ���� */
	inline sint32 ColorDist(const ADColor& c1, const ADColor& c2) {
		return std::max(abs(c1.r - c2.r), std::max(abs(c1.g - c2.g), abs(c1.b - c2.b)));
	}
	
private:
	/** \brief ͼ��ߴ� */
	sint32	width_;
	sint32	height_;

	/** \brief Ӱ������ */
	const uint8* img_left_;
	const uint8* img_right_;
	
	/** \brief ��ʼ�������� */
	float32* cost_init_;
	/** \brief �ۺϴ������� */
	float32* cost_aggr_;

	/** \brief ��С�Ӳ�ֵ */
	sint32 min_disparity_;
	/** \brief ����Ӳ�ֵ */
	sint32 max_disparity_;
	/** \brief ��ʼ��p1ֵ */
	float32 so_p1_;
	/** \brief ��ʼ��p2ֵ */
	float32 so_p2_;
	/** \brief tso��ֵ */
	sint32 so_tso_;
};
#endif
