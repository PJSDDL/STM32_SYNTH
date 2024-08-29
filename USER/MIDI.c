#include "synth.h"

//C2��C7
unsigned int Note_Freq[61] = {
    65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123,
    130, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 246,
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 493,
    523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
    1046, 1109, 1175, 1245, 1319, 1480, 1568, 1661, 1760, 1865, 1976,
    2093
};

struct MIDI_Note MIDI_Note_1[] =
{
    {21*3, C2, 0, },
    {21*3, A4, 200, },
    {21*3, D5, 200, },
    {14*3, E5, 100, },
    {21*3, A4, 200, },
    {21*3, D5, 200, },
    {14*3, E5, 100, },
    {14*3, F5, 10, },
    {14*3, D5, 40, },
    {14*3, E5, 70, },
    {14*3, C5, 100, },
    {28*3, D5, 210, },
};

//��ȡMIDI�������飬�����ƺϳ�������
void MIDI_Set(struct MIDI *MIDI_control, struct VCO *VCO_1, struct ADSR *ADSR_1,
			struct MIDI_Note MIDI_Note_1[], int NOTE_NUM)
{
//MIDI_countÿ10ms��һ����Ϊ�����źſ��ƺϳ�����������
    if(MIDI_control->MIDI_div >= 200)
    {
        MIDI_control->MIDI_div = 0;
        MIDI_control->MIDI_div2++;
        MIDI_control->MIDI_trig = 1;  //ÿ10ms MIDI����������һ��
    }
    else
    {
        MIDI_control->MIDI_div++;
    }

    //MIDI_count��һʱ�����ϳ�������
    if(MIDI_control->MIDI_trig)
    {
        int temp = MIDI_control->MIDI_note_count;

        //������һ������
        if(MIDI_control->MIDI_statu == 1)
        {
            VCO_1->VCO_freq = Note_Freq[ (MIDI_Note_1[temp].note) ];  //���������ı�VCOƵ��
						ADSR_1->velocity = (MIDI_Note_1[temp].velocity);  //���������ı�����
					
            //������һ������
            ADSR_1->ADSR_in = 1; //ADSR����
            ADSR_1->ADSR_reset = 0; //ADSRȡ������

            MIDI_control->MIDI_statu = 0;

            MIDI_control->MIDI_trig = 0;  //MID������ÿ10msֻ����һ��
        }

        //�ж��Ƿ񲥷ŵ���һ������
        if(MIDI_control->MIDI_div2 > MIDI_Note_1[temp].time + MIDI_control->MIDI_time_sum)   //��ת����һ����
        {
            MIDI_control->MIDI_time_sum += MIDI_Note_1[temp].time;  //����֮ǰ��������ʱ���Ƕ���
            MIDI_control->MIDI_note_count++;  //��ת����һ������
            temp = MIDI_control->MIDI_note_count;

            if(temp > NOTE_NUM)  //���Ž�������������ͷ��
            {
                MIDI_control->MIDI_note_count = 0;
                MIDI_control->MIDI_time_sum = 0;
                MIDI_control->MIDI_div2 = 0;
            }

            //������һ������
            ADSR_1->ADSR_in = 0; //ADSRֹͣ����
            ADSR_1->ADSR_reset = 1; //ADSR�ر�

            MIDI_control->MIDI_statu = 1;
        }


        /*
        if(MIDI_count%100 == 0)
        {
        VCF_1.VCF_BW = MIDI_count;

        VCF_set(&VCF_1);
        }
        */
        /*
        if(MIDI_count % 20 == 1)
        {
        ADSR_1.ADSR_in = (ADSR_1.ADSR_in == 1 ? 0 : 1);
        }
        */

    }
}

