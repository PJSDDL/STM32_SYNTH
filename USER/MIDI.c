#include "synth.h"

//C2到C7
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

//读取MIDI音符数组，并控制合成器参数
void MIDI_Set(struct MIDI *MIDI_control, struct VCO *VCO_1, struct ADSR *ADSR_1,
			struct MIDI_Note MIDI_Note_1[], int NOTE_NUM)
{
//MIDI_count每10ms加一，作为控制信号控制合成器其他参数
    if(MIDI_control->MIDI_div >= 200)
    {
        MIDI_control->MIDI_div = 0;
        MIDI_control->MIDI_div2++;
        MIDI_control->MIDI_trig = 1;  //每10ms MIDI控制器触发一次
    }
    else
    {
        MIDI_control->MIDI_div++;
    }

    //MIDI_count加一时调整合成器参数
    if(MIDI_control->MIDI_trig)
    {
        int temp = MIDI_control->MIDI_note_count;

        //播放下一个音符
        if(MIDI_control->MIDI_statu == 1)
        {
            VCO_1->VCO_freq = Note_Freq[ (MIDI_Note_1[temp].note) ];  //根据音符改变VCO频率
						ADSR_1->velocity = (MIDI_Note_1[temp].velocity);  //根据音符改变力度
					
            //开启下一个音符
            ADSR_1->ADSR_in = 1; //ADSR开启
            ADSR_1->ADSR_reset = 0; //ADSR取消重置

            MIDI_control->MIDI_statu = 0;

            MIDI_control->MIDI_trig = 0;  //MID控制器每10ms只触发一次
        }

        //判断是否播放到下一个音符
        if(MIDI_control->MIDI_div2 > MIDI_Note_1[temp].time + MIDI_control->MIDI_time_sum)   //跳转到下一音符
        {
            MIDI_control->MIDI_time_sum += MIDI_Note_1[temp].time;  //计算之前的音符总时长是多少
            MIDI_control->MIDI_note_count++;  //跳转到下一个音符
            temp = MIDI_control->MIDI_note_count;

            if(temp > NOTE_NUM)  //播放结束，跳到乐曲头部
            {
                MIDI_control->MIDI_note_count = 0;
                MIDI_control->MIDI_time_sum = 0;
                MIDI_control->MIDI_div2 = 0;
            }

            //结束上一个音符
            ADSR_1->ADSR_in = 0; //ADSR停止触发
            ADSR_1->ADSR_reset = 1; //ADSR关闭

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

