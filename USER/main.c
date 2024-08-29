#include  <math.h>
#include "stdio.h"
#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "synth.h"
#include "bmp.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dac.h"


//TIM1���ʱ���ã���Դ����оƬ�ڲ�
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// ʹ��TIM1ʱ��

    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM������ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

    TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��

    TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

    TIM_SetCounter(TIM1,0);//���ü�����ֵ

    //TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1

}

//DACͨ��1�����ʼ��
void Dac1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //ʹ��PORTAͨ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //ʹ��DACͨ��ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 // �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //ģ������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_4)	;//PA.4 �����

    DAC_InitType.DAC_Trigger=DAC_Trigger_None;	//��ʹ�ô������� TEN1=0
    DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//��ʹ�ò��η���
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//���Ρ���ֵ����
    DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Enable ;	//DAC1�������ر� BOFF1=1
    DAC_Init(DAC_Channel_1,&DAC_InitType);	 //��ʼ��DACͨ��1

    DAC_Cmd(DAC_Channel_1, ENABLE);  //ʹ��DAC1

    DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ

}

extern struct MIDI_Note MIDI_Note_1[];

int main(void)
{
    struct MIDI MIDI_control;
    {
        MIDI_control.MIDI_div2 = 0;
        MIDI_control.MIDI_div = 0;
        MIDI_control.MIDI_trig = 0;
        MIDI_control.MIDI_time_sum = 0;
        MIDI_control.MIDI_note_count = 0;
        MIDI_control.MIDI_statu = 0;
    }

    struct VCO VCO_1;
    {
        VCO_1.VCO_wavetype = 1;
        VCO_1.phase = 0;
        VCO_1.idx = 0;
        VCO_1.VCO_freq = 100;
        VCO_1.VCO_Out = 0;
    }

    struct VCO VCO_2;
    VCO_2 = VCO_1;

    struct VCF VCF_1;
    {
        VCF_1.VCF_BW = 1000;
        VCF_1.VCF_freq = 1000;
        VCF_set(&VCF_1);
        VCF_1.VCF_Out = 0;
        VCF_1.VCF_reg1 = 0;
        VCF_1.VCF_reg2 = 0;
        VCF_1.VCF_type = 0;
    }

    struct ADSR  ADSR_1;
    {
        ADSR_1.ADSR_enable = 1;
        ADSR_1.ADSR_statu = 0;
        ADSR_1.ADSR_count = 0;
        ADSR_1.ADSR_div = 0;
        ADSR_1.A_time = 40;
        ADSR_1.A_speed = 5;
        ADSR_1.D_time = 0;
        ADSR_1.D_speed = 0;
        ADSR_1.R_time = 0;
        ADSR_1.R_speed = 0;
        ADSR_1.ADSR_in = 0;
        ADSR_1.ADSR_out = 0;
    }

    struct ADSR  ADSR_2;
    {
        ADSR_2.ADSR_enable = 1;
        ADSR_2.ADSR_statu = 0;
        ADSR_2.ADSR_count = 0;
        ADSR_2.ADSR_div = 0;
        ADSR_2.A_time = 1;
        ADSR_2.A_speed = 200;
        ADSR_2.D_time = 20;
        ADSR_2.D_speed = 10;
        ADSR_2.R_time = 0;
        ADSR_2.R_speed = 0;
        ADSR_2.ADSR_in = 0;
        ADSR_2.ADSR_out = 0;
    }

    SystemInit();
    delay_init();	    	 //��ʱ������ʼ��
    TIM1_Init(65535,(72000000/200000)-1); //��72MHz��ƵΪ200kHz
    Dac1_Init();

    OLED_Init();			//��ʼ��OLED
    OLED_Clear();

    unsigned char i = 100;
		
    while(1)
    {
        TIM_SetCounter(TIM1,1); //���ü�����ֵ
        TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1

        //-------------MIDI_control-------------//
        MIDI_Set(&MIDI_control, &VCO_1, &ADSR_1, MIDI_Note_1, 11);

        //-------------SUB_synth-------------//
        long int OUT1 = SUB_synth_default(&VCO_1, &VCF_1, &ADSR_1);

        //-------------VCA & Out-------------//
        DAC_SetChannel1Data(DAC_Align_12b_R, 2048 + OUT1 / 10);

        while(TIM_GetCounter(TIM1) <= 10); //ˢ��Ƶ��20kHz
    }


}