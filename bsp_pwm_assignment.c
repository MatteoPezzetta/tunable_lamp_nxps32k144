#include <lib_def.h>
#include <bsp_int.h>
#include <os.h>
#include "S32K144.h"
#include <bsp_adc_assignment.h>

volatile CPU_INT32S FTM0_CH0_period;
extern CPU_INT16U ADC0_adc_chx;
extern OS_SEM ADC0sem;
OS_ERR os_err;

void BSP_FTM0_CH0_PWM_Init(void)
{
  
  PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
  PCC->PCCn[PCC_FTM0_INDEX] = PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;
  
  PORTD->PCR[15] = PORT_PCR_MUX(2); 
  PORTD->PCR[16] = PORT_PCR_MUX(2);
  PORTD->PCR[0] = PORT_PCR_MUX(2); 

  FTM0->MODE = FTM_MODE_FTMEN_MASK;

  FTM0->MOD = FTM_MOD_MOD(8000-1);

  FTM0->CNTIN = FTM_CNTIN_INIT(0);

  FTM0->CONTROLS[0].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
  FTM0->CONTROLS[1].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
  FTM0->CONTROLS[2].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;

  BSP_ADC0_convertAdcChan_interrupt(12);

  OSSemPend(&ADC0sem, 0u, OS_OPT_PEND_BLOCKING, 0u, &os_err);

  FTM0->CONTROLS[0].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);
  FTM0->CONTROLS[1].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);
  FTM0->CONTROLS[2].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);
  FTM0->CNT = 0;

  FTM0->SC = FTM_SC_CLKS(1) | FTM_SC_PS(0) | FTM_SC_PWMEN0_MASK;

  FTM0->MODE |= FTM_MODE_FTMEN_MASK;

  FTM0->MODE &= 0xF7;

  FTM0->SYNCONF |= 0x00000080;

  FTM0->SYNCONF |= 0x00000005;

  FTM0->COMBINE |= 0x00002020;
}

void BSP_FTM0_CH0_PWM_CnV_Update(void)
{

  FTM0->CONTROLS[0].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);
  FTM0->CONTROLS[1].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);
  FTM0->CONTROLS[2].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);

  FTM0->PWMLOAD |= 0x200;
}
