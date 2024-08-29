#include "synth.h"
#include "WaveTable.h"
#include <math.h>

void ADSR_default(struct ADSR *ADSR_1)
{
    if(ADSR_1->ADSR_enable == 0)
    {
        return;
    }

    if(ADSR_1->ADSR_reset == 1)
    {
        ADSR_1->ADSR_statu = 0;  //ǿ����ADSR�ر�
        ADSR_1->ADSR_count = 0;
    }

    if(ADSR_1->ADSR_div == 200)  //ADSR��Ƶ������ΪADSR�ǵ�Ƶ���ź�������Ҫ��Ƶ
    {
        ADSR_1->ADSR_div = 0;
        switch(ADSR_1->ADSR_statu)  //ADSR״̬��
        {
        case 2:  //Attack
            if(ADSR_1->ADSR_count >= ADSR_1->A_time)
            {
                ADSR_1->ADSR_count = 0;
                ADSR_1->ADSR_statu = 3;
								break;
            }
						
						ADSR_1->ADSR_out_temp += ADSR_1->A_speed;
            ADSR_1->ADSR_count++;
						
            if(ADSR_1->ADSR_in == 0)
            {
                ADSR_1->ADSR_statu = 5;
            }
            break;
        case 3:  //Decay
            if(ADSR_1->ADSR_count >= ADSR_1->D_time)
            {
                ADSR_1->ADSR_count = 0;
                ADSR_1->ADSR_statu = 4;
								break;
            }
						
						ADSR_1->ADSR_out_temp -= ADSR_1->D_speed;
            ADSR_1->ADSR_count++;
						
            if(ADSR_1->ADSR_in == 0)
            {
                ADSR_1->ADSR_statu = 5;
            }
            break;
        case 4:  //Sustain
            if(ADSR_1->ADSR_in == 0)
            {
                ADSR_1->ADSR_statu = 5;
            }
            break;
        case 5:  //Release
						if(ADSR_1->ADSR_count >= ADSR_1->R_time)
            {
                ADSR_1->ADSR_count = 0;
                ADSR_1->ADSR_statu = 1;
								break;
            }
				
            ADSR_1->ADSR_out_temp -= ADSR_1->R_speed;
            ADSR_1->ADSR_count++;
						
            break;
        default:  //������ADSR����
            ADSR_1->ADSR_count = 0;
            (ADSR_1->ADSR_in == 1) ? (ADSR_1->ADSR_statu = 2) : 0;
            ADSR_1->ADSR_out_temp = 0;
            break;
        };
				
				ADSR_1->ADSR_out = (ADSR_1->velocity * ADSR_1->ADSR_out_temp) / 256;
    }
    else
    {
        ADSR_1->ADSR_div++;
    }
		
}

void VCO_gen(struct VCO *VCO_1)
{
    //����DDS��λ
    VCO_1->phase = (VCO_1->phase>=20000-1)
                   ? ((VCO_1->phase)+(VCO_1->VCO_freq)-20000)
                   : (VCO_1->phase)+(VCO_1->VCO_freq);

    switch(VCO_1->VCO_wavetype)  //��������
    {
    case 1:		//����
        VCO_1->idx = VCO_1->phase/20;
        VCO_1->VCO_Out = SinTable[VCO_1->idx];
        break;
    case 2:		//����
        VCO_1->idx = VCO_1->phase/20;
        VCO_1->VCO_Out = NoiseTable[VCO_1->idx];
        break;
    default:		//���
        VCO_1->idx = VCO_1->phase/80;
        VCO_1->VCO_Out = VCO_1->idx - 20000 / 80 / 2;
        break;
    };
}

void VCF_set(struct VCF *VCF_1)
{
    //����VCF����
    VCF_1->VCF_C = - (int) 16384 * exp(- 2 * PI * VCF_1->VCF_BW / FS);
    VCF_1->VCF_B = (int) 16384 * 2 * exp(- PI * VCF_1->VCF_BW / FS) * cos(2 * PI * VCF_1->VCF_freq / FS);
    VCF_1->VCF_A = (int) 16384 - VCF_1->VCF_B - VCF_1->VCF_C;
}

void VCF_fast_set(struct VCF *VCF_1)
{
    //����VCF����
    VCF_1->VCF_A = (int) 16384 - VCF_1->VCF_B - VCF_1->VCF_C;
}

void VCF_gen(short VCO_Out, struct VCF *VCF_1)
{
    VCF_1->VCF_Out = VCF_1->VCF_A * VCO_Out + VCF_1->VCF_B * VCF_1->VCF_reg1 + VCF_1->VCF_C * VCF_1->VCF_reg2;  //��ͨ�˲���
    VCF_1->VCF_reg2 = VCF_1->VCF_reg1;
    VCF_1->VCF_reg1 = VCF_1->VCF_Out / 16384;
}

int SUB_synth_default(struct VCO *VCO_1, struct VCF *VCF_1, struct ADSR *ADSR_1)
{
    //-------------VCO-------------//
    VCO_gen(VCO_1);

    //-------------VCF-------------//
    VCF_gen(VCO_1->VCO_Out, VCF_1);

    //-------------ADSR-------------//
    ADSR_default(ADSR_1);

    return ADSR_1->ADSR_out / 10 * VCF_1->VCF_reg1;
}

int SUB_synth_drum(struct VCO *VCO_1, struct ADSR *ADSR_1, struct VCF *VCF_1, struct ADSR *ADSR_2)
{
    ADSR_2->ADSR_in = ADSR_1->ADSR_in;		//����ADSR������ͬʱ����

    //-------------VCO-------------//
    ADSR_default(ADSR_1);
    VCO_1->VCO_freq = ADSR_1->ADSR_out;  //��һ��ADSR���������ƹĵ�Ƶ��
    VCO_gen(VCO_1);

    //-------------VCF-------------//
    VCF_gen(VCO_1->VCO_Out, VCF_1);

    //-------------ADSR-------------//
    ADSR_default(ADSR_2);								//�ڶ���ADSR���������ƹĵ����

    return ADSR_2->ADSR_out / 10 * VCF_1->VCF_reg1;
}


//����ɫԤ��
void Drum_init(struct VCO *VCO_1, struct ADSR *ADSR_1, struct VCF *VCF_1, struct ADSR *ADSR_2)
{
    {
        VCO_1->VCO_wavetype = 0;
        VCO_1->phase = 0;
        VCO_1->idx = 0;
        VCO_1->VCO_freq = 100;
        VCO_1->VCO_Out = 0;
    }

    {
        VCF_1->VCF_BW = 1000;
        VCF_1->VCF_freq = 1000;
        VCF_set(VCF_1);
        VCF_1->VCF_Out = 0;
        VCF_1->VCF_reg1 = 0;
        VCF_1->VCF_reg2 = 0;
        VCF_1->VCF_type = 0;
    }

    {
        ADSR_1->ADSR_enable = 1;
        ADSR_1->ADSR_statu = 0;
        ADSR_1->ADSR_count = 0;
        ADSR_1->ADSR_div = 0;
        ADSR_1->A_time = 1;
        ADSR_1->A_speed = 200;
        ADSR_1->D_time = 10;
        ADSR_1->D_speed = 20;
        ADSR_1->R_time = 0;
        ADSR_1->R_speed = 0;
        ADSR_1->ADSR_in = 0;
        ADSR_1->ADSR_out = 0;
    }

    {
        ADSR_2->ADSR_enable = 1;
        ADSR_2->ADSR_statu = 0;
        ADSR_2->ADSR_count = 0;
        ADSR_2->ADSR_div = 0;
        ADSR_2->A_time = 1;
        ADSR_2->A_speed = 200;
        ADSR_2->D_time = 20;
        ADSR_2->D_speed = 10;
        ADSR_2->R_time = 0;
        ADSR_2->R_speed = 0;
        ADSR_2->ADSR_in = 0;
        ADSR_2->ADSR_out = 0;
    }
}
