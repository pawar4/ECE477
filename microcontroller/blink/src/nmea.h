//Used to parse NMEA messages
//for this project we will utilize GLL, GSV and RMC
#define GPS_BUFFER_SIZE 82

typedef enum{
	GLL, //Latitude and longitude
	GSV, //Number of satellites
	RMC,  //time and date
	NONE
}nmea_type;

typedef struct{
	char latitude[6];
	char lat_direction;
	char longitude[7];
	char long_direction;
	int valid;
}gll_message;

typedef struct{
	int num_satellites;
}gsv_message;

typedef struct{
	char time[9];
	char latitude[14];
	char lat_direction;
	char longitude[15];
	char long_direction;
	char valid;
}rmc_message;

typedef struct{
	nmea_type type;
	gll_message * gll;
	gsv_message * gsv;
	rmc_message * rmc;
}nmea_message;

void parse_gll(gll_message * gll, char * buffer);
void print_gll(gll_message * gll);
void parse_gsv(gsv_message * gsv, char * buffer);
void print_gsv(gsv_message * gsv);
void parse_rmc(rmc_message * rmc, char * buffer);
void print_rmc(rmc_message * rmc);
nmea_type get_message_type(char * buffer);
nmea_message * parse_sentence(nmea_message * message, char * buffer);
void copyUntil(char * dest, char * source, char c);
