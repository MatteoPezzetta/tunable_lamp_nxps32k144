#include <cpu.h>
#include <lib_mem.h>
#include <os.h>
#include <bsp_os.h>
#include <bsp_clk.h>
#include <bsp_int.h>
#include <S32K144.h>

#include "os_app_hooks.h"
#include "../app_cfg.h"

#include <bsp_adc_assignment.h>
#include <bsp_pwm_assignment.h>
#include <bsp_switch_assignment.h>

static OS_TCB StartupTaskTCB;
static CPU_STK StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];

volatile CPU_INT16U ADC0_result;
volatile CPU_INT16U ADC0_adc_chx;
OS_SEM ADC0sem;
extern CPU_INT08U SW2_status;
extern CPU_INT08U SW3_status;


static void StartupTask(void *p_arg);

int main(void)
{
  OS_ERR os_err;

  BSP_ClkInit();
  BSP_IntInit();
  BSP_OS_TickInit();

  Mem_Init();
  CPU_IntDis();
  CPU_Init();

  OSInit(&os_err);
  if (os_err != OS_ERR_NONE) {
    while (1);
  }

  App_OS_SetAllHooks();

  OSTaskCreate(&StartupTaskTCB,
	       "Startup Task",
	       StartupTask,
	       0u,
	       APP_CFG_STARTUP_TASK_PRIO,
	       &StartupTaskStk[0u],
	       StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE / 10u],
	       APP_CFG_STARTUP_TASK_STK_SIZE,
	       0u,
	       0u,
	       0u,
	       (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
	       &os_err);
  if (os_err |= OS_ERR_NONE) {
    while (1);
  }

  OSStart(&os_err);

  while (DEF_ON) {
    ;
  }
}


static void StartupTask (void *p_arg)
{

  OS_ERR os_err;
  (void)p_arg;

  OS_TRACE_INIT();

  BSP_OS_TickEnable();

  #if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&os_err);
  #endif

  #ifdef CPU_CFG_INT_DIS_MEAS_EN
  CPU_IntDisMeasMaxCurReset();
  #endif

  OSSemCreate(&ADC0sem, "ADC0 Semaphore", 0u, &os_err);

  BSP_ADC0_init_interrupt();
  
  BSP_FTM0_CH0_PWM_Init();

  BSP_Switch_Init();

  while (DEF_TRUE) {
    
    BSP_ADC0_convertAdcChan_interrupt(12);

    /* change of colour */
      if(SW2_status)
      {
        BSP_CH_switching_2();
        SW2_status = 0;
      }

      if(SW3_status)
      {
        BSP_CH_switching_3();
        SW3_status = 0;
      }

    /* updating of the duty-cycle */

    OSSemPend(&ADC0sem, 0u, OS_OPT_PEND_BLOCKING, 0u, &os_err);

    BSP_FTM0_CH0_PWM_CnV_Update();
  }
}
