#include "Basic.hpp"
////////////////////////////////////////////////
class CRotaCoordinatAnsys//
{
public:
	DOUBLE_VCT A_phaseDatas, B_phaseDatas, C_phaseDatas;
	
	DOUBLE_VCT d_staticDatas, q_staticDatas;
	DOUBLE_VCT d_rotateDatas, q_rotateDatas;
	
	CRotaCoordinatAnsys(DOUBLE_VCT A_phaseDatas0,
						DOUBLE_VCT B_phaseDatas0,
						DOUBLE_VCT C_phaseDatas0);
	~CRotaCoordinatAnsys();

public:
	int rotaCoordinatAnsys();
	int getStatic_DQ_data();
	int getRotate_DQ_data();
};
