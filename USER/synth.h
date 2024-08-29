
#define PI 3.1415926
#define FS 20000.0

#define STOP -1

#define C2 0
#define C2_ 1
#define D2 2
#define D2_ 3
#define E2 4
#define F2 5
#define F2_ 6
#define G2 7
#define G2_ 8
#define A2 9
#define A2_ 10
#define B2 11

#define C3 12
#define C3_ 13
#define D3 14
#define D3_ 15
#define E3 16
#define F3 17
#define F3_ 18
#define G3 19
#define G3_ 20
#define A3 21
#define A3_ 22
#define B3 23

#define C4 24
#define C4_ 25
#define D4 26
#define D4_ 27
#define E4 28
#define F4 29
#define F4_ 30
#define G4 31
#define G4_ 32
#define A4 33
#define A4_ 34
#define B4 35

#define C5 36
#define C5_ 37
#define D5 38
#define D5_ 39
#define E5 40
#define F5 41
#define F5_ 42
#define G5 43
#define G5_ 44
#define A5 45
#define A5_ 46
#define B5 47

#define C6 48
#define C6_ 49
#define D6 50
#define D6_ 51
#define E6 52
#define F6 53
#define F6_ 54
#define G6 55
#define G6_ 56
#define A6 57
#define A6_ 58
#define B6 59

#define C7 60

struct ADSR
{
    unsigned char ADSR_enable;
    unsigned char ADSR_statu;
    unsigned int ADSR_count;
    unsigned char ADSR_div;
    unsigned char ADSR_reset;  //ADSR复位信号，只要为1就让ADSR状态为Attack	
	
    unsigned char A_time;
    unsigned char D_time;
    unsigned char R_time;
    unsigned char A_speed;
    unsigned char D_speed;
    unsigned char R_speed;
	
		unsigned char velocity;
	
    unsigned char ADSR_in;  //ADSR输入触发信号，为1时产生ADSR波形，为0时不产生波形
    unsigned int ADSR_out_temp;
		unsigned int ADSR_out;

};

struct VCO
{
    char VCO_wavetype;
    short phase;
    short idx;
    short VCO_freq;
    short VCO_Out;
};

struct VCF
{
    short VCF_BW;
    short VCF_freq;
    int VCF_A;
    int VCF_B;
    int VCF_C;
    int VCF_Out;
    int VCF_reg1;
    int VCF_reg2;
    int VCF_type;
};

struct MIDI_Note
{
    unsigned int time;  //最小分度10ms
    char note;
    unsigned char velocity;
};

struct MIDI
{
    unsigned int MIDI_div2;
    unsigned int MIDI_div;
    unsigned int MIDI_trig;
    unsigned int MIDI_time_sum;
    unsigned int MIDI_note_count;
    unsigned char MIDI_statu;
};

void ADSR_default(struct ADSR *ADSR_1);
void VCO_gen(struct VCO *VCO_1);
void VCF_set(struct VCF *VCF_1);
void VCF_fast_set(struct VCF *VCF_1);
void VCF_gen(short VCO_Out, struct VCF *VCF_1);
int SUB_synth_default(struct VCO *VCO_1, struct VCF *VCF_1, struct ADSR *ADSR_1);
int SUB_synth_drum(struct VCO *VCO_1, struct ADSR *ADSR_1, struct VCF *VCF_1, struct ADSR *ADSR_2);
void MIDI_Set(struct MIDI *MIDI_control, struct VCO *VCO_1, struct ADSR *ADSR_1,
			struct MIDI_Note MIDI_Note_1[], int NOTE_NUM);

//音色预制
void Drum_init(struct VCO *VCO_1, struct ADSR *ADSR_1, struct VCF *VCF_1, struct ADSR *ADSR_2);
