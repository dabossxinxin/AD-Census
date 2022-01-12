#pragma once

#include "adcensus_types.h"
#include "cost_computor.h"
#include "cross_aggregator.h"
#include "scanline_optimizer.h"
#include "multistep_refiner.h"

class ADCensusStereo {	
public:
	ADCensusStereo();
	~ADCensusStereo();

	/**
	* \brief ��ĳ�ʼ�������һЩ�ڴ��Ԥ���䡢������Ԥ���õ�
	* \param width		���룬�������Ӱ���
	* \param height		���룬�������Ӱ���
	* \param option		���룬�㷨����
	*/
	bool Initialize(const sint32& width, const sint32& height, const ADCensusOption& option);

	/**
	* \brief ִ��ƥ��
	* \param img_left	���룬��Ӱ������ָ�룬3ͨ����ɫ����
	* \param img_right	���룬��Ӱ������ָ�룬3ͨ����ɫ����
	* \param disp_left	�������Ӱ���Ӳ�ͼָ�룬Ԥ�ȷ����Ӱ��ȳߴ���ڴ�ռ�
	*/
	bool Match(const uint8* img_left, const uint8* img_right, float32* disp_left);

	/**
	* \brief ����
	* \param width		���룬�������Ӱ���
	* \param height		���룬�������Ӱ���
	* \param option		���룬�㷨����
	*/
	bool Reset(const uint32& width, const uint32& height, const ADCensusOption& option);

private:
	/** \brief ���ۼ��� */
	void ComputeCost();

	/** \brief ���۾ۺ� */
	void CostAggregation();

	/** \brief ɨ�����Ż�	 */
	void ScanlineOptimize();

	/** \brief �ಽ���Ӳ��Ż�	*/
	void MultiStepRefine();

	/** \brief �Ӳ���㣨����ͼ��*/
	void ComputeDisparity();

	/** \brief �Ӳ���㣨����ͼ��*/
	void ComputeDisparityRight();

	/** \brief �ڴ��ͷ� */
	void Release();

private:
	/** \brief �㷨���� */
	ADCensusOption option_;

	/** \brief Ӱ��� */
	sint32 width_;
	/** \brief Ӱ��� */
	sint32 height_;

	/** \brief ��Ӱ�����ݣ�3ͨ����ɫ���� */
	const uint8* img_left_;
	/** \brief ��Ӱ������	��3ͨ����ɫ���� */
	const uint8* img_right_;

	/** \brief ���ۼ����� */
	CostComputor cost_computer_;
	/** \brief ���۾ۺ��� */
	CrossAggregator aggregator_;
	/** \brief ɨ�����Ż��� */
	ScanlineOptimizer scan_line_;
	/** \brief �ಽ�Ż��� */
	MultiStepRefiner refiner_;

	/** \brief ��Ӱ���Ӳ�ͼ */
	float32* disp_left_;
	/** \brief ��Ӱ���Ӳ�ͼ */
	float32* disp_right_;

	/** \brief �Ƿ��ʼ����־	*/
	bool is_initialized_;
};
