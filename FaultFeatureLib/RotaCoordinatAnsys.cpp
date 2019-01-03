#include "stdafx.h"
#include "RotaCoordinatAnsys.h"
//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 
//////////////////////////////////////////////////
CRotaCoordinatAnsys::CRotaCoordinatAnsys(DOUBLE_VCT A_phaseDatas0,
										 DOUBLE_VCT B_phaseDatas0,
										 DOUBLE_VCT C_phaseDatas0)
{
	A_phaseDatas = A_phaseDatas0;
	B_phaseDatas = B_phaseDatas0;
	C_phaseDatas = C_phaseDatas0;
}
CRotaCoordinatAnsys::~CRotaCoordinatAnsys()
{
}
int CRotaCoordinatAnsys::rotaCoordinatAnsys()
{
	if (getStatic_DQ_data())
	{
		return 1;
	}
	if (getRotate_DQ_data())
	{
		return 1;
	}

	return 0;
}
int CRotaCoordinatAnsys::getStatic_DQ_data()
{
	return 0;
}
int CRotaCoordinatAnsys::getRotate_DQ_data()
{
	return 0;
}

