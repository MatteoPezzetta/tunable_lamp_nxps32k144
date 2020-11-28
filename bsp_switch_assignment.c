#include <lib_def.h>
#include <bsp_int.h>
#include <cpu.h>
#include <os.h>
#include "bsp_switch_assignment.h"
#include <S32K144.h>

volatile CPU_INT08U SW2_status = 0;
volatile CPU_INT08U SW3_status = 0;

static void SW_int_hdlr(void);

void BSP_Switch_Init (void)
{
 PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;

 PTC->PDDR &= ~(DEF_BIT_12 | DEF_BIT_13);
 PORTC->PCR[12] = 0x100u;
 PORTC->PCR[13] = 0x100u;

 PORTC->PCR[12] |= 0x00090000u;
 PORTC->PCR[13] |= 0x00090000u;

 BSP_IntVectSet(PORTC_IRQn, 0,0, SW_int_hdlr);
 BSP_IntEnable( PORTC_IRQn );
}

CPU_INT08U BSP_Switch_Read (BSP_SWITCH sw)
{
switch( sw )
	{
		case SW2:
			return( ((PTC->PDIR) >> 12) & 0x01u);
			break;
		case SW3:
			return( ((PTC->PDIR) >> 13) & 0x01u);
			break;
	}
	return( 0x00u );
}

static void SW_int_hdlr( void )
{
	uint32_t ifsr2;
	uint32_t ifsr3;

	//CPU_CRITICAL_ENTER();
	OSIntEnter();

	ifsr2 = (PORTC->PCR[12]) & 0x01000000;
	ifsr3 = (PORTC->PCR[13]) & 0x01000000;

	if((ifsr2))
	{
		SW2_status = BSP_Switch_Read( SW2 );
		PORTC->PCR[12] |= 0x01000000;

	}
	if((ifsr3))
		{
		SW3_status = BSP_Switch_Read( SW3 );
		PORTC->PCR[13] |= 0x01000000;
		}

	//CPU_CRITICAL_EXIT();
	OSIntExit();
}

void BSP_CH_switching_2(void)
{
  uint32_t colour_mask;
  colour_mask = ((FTM0->SC) & (FTM_SC_PWMEN0_MASK | FTM_SC_PWMEN1_MASK | FTM_SC_PWMEN2_MASK));

  switch(colour_mask)
  {
  	  case FTM_SC_PWMEN0_MASK :
  		  FTM0->SC &= ~FTM_SC_PWMEN0_MASK;
  		  FTM0->SC |= FTM_SC_PWMEN1_MASK;
      break;
  	  case FTM_SC_PWMEN1_MASK :
  		  FTM0->SC &= ~FTM_SC_PWMEN1_MASK;
  		  FTM0->SC |= FTM_SC_PWMEN2_MASK;
  	  break;
  	  case FTM_SC_PWMEN2_MASK :
  		  FTM0->SC &= ~FTM_SC_PWMEN2_MASK;
  		  FTM0->SC |= FTM_SC_PWMEN0_MASK;
      break;
  }
}

void BSP_CH_switching_3(void)
{
  uint32_t colour_mask;
  colour_mask = ((FTM0->SC) & (FTM_SC_PWMEN0_MASK | FTM_SC_PWMEN1_MASK | FTM_SC_PWMEN2_MASK));

		switch(colour_mask) {
		  case FTM_SC_PWMEN0_MASK :
		    FTM0->SC &= ~FTM_SC_PWMEN0_MASK;
		    FTM0->SC |= FTM_SC_PWMEN2_MASK;
		      break;
		  case FTM_SC_PWMEN1_MASK :
		    FTM0->SC &= ~FTM_SC_PWMEN1_MASK;
		    FTM0->SC |= FTM_SC_PWMEN0_MASK;
		    break;
		  case FTM_SC_PWMEN2_MASK :
		    FTM0->SC &= ~FTM_SC_PWMEN2_MASK;
		    FTM0->SC |= FTM_SC_PWMEN1_MASK;
		    break;
		}
}
