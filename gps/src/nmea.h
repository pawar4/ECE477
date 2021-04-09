//Used to parse NMEA messages
//for this project we will utilize GLL, GSV and RMC
#define GPS_BUFFER_SIZE 82

typedef enum{
	GSV, //Number of satellites
	RMC,  //time and date
	NONE
}nmea_type;

typedef struct{
	int num_satellites;
}gsv_message;

typedef struct{
	char time[10];
	char latitude[25];
	char lat_direction;
	char longitude[25];
	char long_direction;
	char valid;
}rmc_message;

typedef struct{
	gsv_message * gsv;
	rmc_message * rmc;
	int init;
}nmea_message;

void parse_gsv(gsv_message * gsv, char * buffer);
void parse_rmc(rmc_message * rmc, char * buffer);
nmea_type get_message_type(char * buffer);
void parse_sentence(nmea_message * message, char * buffer);
void copyUntil(char * dest, char * source, char c);
void psm_in();
void psm_out();
void convertLong(char * longitude);
void convertLat(char * lat);
void ftoa(float n, char* res, int afterpoint);
int intToStr(int x, char str[], int d);
void reverse(char* str, int len);
void free_nmea_message(nmea_message * message);
void mon_ver();
void initUART2();
void initTimer2();
void initTimer3();
void TIM2_IRQHandler();
void initParsedMessage();
void deinit_UART();
void TIM3_IRQHandler();
char * createGPSmsg();

