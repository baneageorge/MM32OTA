#include <string.h>
#include <stdlib.h>
#include "HAL_conf.h"
#include "BSP.h"
#include "mg_api.h"
void LED_ONOFF(unsigned char onFlag);
void LED_ONOFFPB1(unsigned char onFlag);
void LED_ONOFFPB9(unsigned char onFlag);

extern volatile unsigned int SysTick_Count;

unsigned char *ble_mac_addr;
unsigned char* get_local_addr(void) //used for ble pairing case
{
    return ble_mac_addr;
}
const unsigned char kkkk[28000] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0x90, 0xfe
};

const unsigned char OTAAdvDat[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0x90, 0xfe
};
unsigned char OTARspDat[] = {
    0x4, 0x09, 'O', 'T', 'B',
    0x05, 0xff, 0xcd, 0xab, 0x23, 0x56
};
static void delayloop(u32 delay);


/********************************************************************************************************
**������Ϣ ��PVU_CheckStatus(void)                       
**�������� �����������Ź�Ԥ��Ƶλ״̬
**������� ����
**������� ����
********************************************************************************************************/
void PVU_CheckStatus(void)
{
    while(1)
    {
        /*���Ԥ��Ƶλ״̬,ΪRESET�ſɸı�Ԥ��Ƶֵ*/
        if(IWDG_GetFlagStatus(IWDG_FLAG_PVU)==RESET)                                                       
        {
            break;
        }
    }
}
/********************************************************************************************************
**������Ϣ ��RVU_CheckStatus(void)                  
**�������� �����������Ź����ر�־
**������� ����
**������� ����
********************************************************************************************************/
void RVU_CheckStatus(void)
{
    while(1)
    {
        /*������ر�־״̬*/
        if(IWDG_GetFlagStatus(IWDG_FLAG_RVU)==RESET)  
        {
            break;
        }
    }
}
/********************************************************************************************************
**������Ϣ ��Write_Iwdg_PR(void)             
**�������� �������������Ź�
**������� ��IWDG_Prescaler ��ѡIWDG_Prescaler_X, XΪ4,8,16,32,64,128,256,��Ӧ��Ƶֵ��Xȡֵ��ͬ
Reload<=0xfff,Ϊ����������ֵ
**������� ����
**    ��ע ����λʱ��������LSI 40KHzΪ�ο�
Tiwdg=(X/LSI)*Reload
********************************************************************************************************/
void Write_Iwdg_ON(unsigned short int IWDG_Prescaler,unsigned short int Reload)
{
    /*�����ڲ�����ʱ��,�ȴ�ʱ�Ӿ���*/
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);
    
    /*����ʱ��Ԥ��Ƶ*/	
    PVU_CheckStatus();
    IWDG_WriteAccessCmd(0x5555);
    IWDG_SetPrescaler(IWDG_Prescaler);
    
    /*�������ؼĴ���ֵ*/	
    RVU_CheckStatus();
    IWDG_WriteAccessCmd(0x5555);
    IWDG_SetReload(Reload&0xfff);	
    
    /*װ�ز�ʹ�ܼ�����*/	
    IWDG_ReloadCounter();
    IWDG_Enable();
}

/********************************************************************************************************
**������Ϣ ��void Write_Iwdg_RL(void)           
**�������� ��ι������
**������� ��
**������� ����
********************************************************************************************************/
void Write_Iwdg_RL(void)
{
    IWDG_ReloadCounter();
}

int main(void)
{
    int i;
    unsigned long temp = 0x180000;
    if(kkkk[19000]==0)
    {
        temp = 0x180000;
    }
    else{
        temp = 0x180001;
    }
    while(temp--);//wait a while for hex programming if using the MCU stop mode, default NOT used.

    BSP_Init();
    //below Blink code can be deleted
    for(i = 0; i < 23000; i++)
    {
        temp+=kkkk[i];
    }
        if(temp==0)
    {
        for(i = 0; i < 5; i++) { LED_ONOFFPB9(0); delayloop(300); LED_ONOFFPB9(1); delayloop(300);}
    }
    else{
        for(i = 0; i < 6; i++) { LED_ONOFFPB9(0); delayloop(300); LED_ONOFFPB9(1); delayloop(300);}
    }
 //   Write_Iwdg_ON(IWDG_Prescaler_32, 0x4E2); //1s

    SetBleIntRunningMode();
    //end Blink code
    radio_initBle(TXPWR_0DBM, &ble_mac_addr);
//    SetFixAdvChannel(1);
    ble_set_adv_data((unsigned char *)OTAAdvDat, sizeof(OTAAdvDat));
    ble_set_adv_rsp_data((unsigned char *)OTARspDat, sizeof(OTARspDat));


    SysTick_Count = 0;
    while(SysTick_Count < 5){}; //delay at least 5ms between radio_initBle() and ble_run...

    ble_run_interrupt_start(160*2); //320*0.625=200 ms
    
    while(1){
       IrqMcuGotoSleepAndWakeup();
        
    }
}
static void delayloop(u32 delay)
{

    u32 i, j;
    for(i = 0; i < delay; i++) {
        for(j = 0; j < 10000; j++) {
            __NOP();
        }
    }
}
