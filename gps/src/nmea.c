#include "stm32l1xx.h"
#include "stm32l_discovery_lcd.h"
#include "stm32l1xx_gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nmea.h"

void parse_gll(gll_message * gll, char * buffer){
	char * curr = strstr(buffer, ",");
	curr++;
	strncpy(gll -> latitude, curr, 7);
	curr = strstr(curr, ",");
	curr++;
	gll -> lat_direction = *curr;
	curr = strstr(curr, ",");
	curr++;
	strncpy(gll -> longitude, curr, 8);
	curr = strstr(curr, ",");
	curr++;
	gll -> long_direction = *curr;
}

void print_gll(gll_message * gll){
    //printf("%s, %c, %s, %c", gll -> latitude, gll -> lat_direction, gll -> longitude, gll -> long_direction);
}

//Parse an nmea gsv message
void parse_gsv(gsv_message * gsv, char * buffer){
	char * curr = strstr(buffer, ",");
	curr++;
	curr = strstr(curr, ",");
	curr++;
	curr = strstr(curr, ",");
	curr++;
	gsv -> num_satellites = atoi(curr);
}

void print_gsv(gsv_message * gsv){
    printf("%d", gsv -> num_satellites);
}

//Parse an nmea rmc message
void parse_rmc(rmc_message * rmc, char * buffer){
	char * curr = strstr(buffer, ",");
	curr++;
	strncpy(rmc->time, curr, 6);

	curr = strstr(curr, ",");
	curr++;
	rmc -> valid = *curr;

	curr = strstr(curr, ",");
	curr++;
	copyUntil(rmc -> latitude, curr, ',');
	curr = strstr(curr, ",");
	curr++;
	rmc -> lat_direction = *curr;
	curr = strstr(curr, ",");
	curr++;
	copyUntil(rmc -> longitude, curr, ',');
	curr = strstr(curr, ",");
	curr++;
	rmc -> long_direction = *curr;
}

void copyUntil(char * dest, char * source, char c){
	while(*source != c){
		*dest = *source;
		dest++;
		source++;
	}
	dest++;
	*dest = '\0';
}

void print_rmc(rmc_message * rmc){
	//printf("%s:%s:%s  %c", rmc -> hour, rmc -> minutes, rmc -> seconds, rmc -> valid);
}

//Obtains the type of nmea message
nmea_type get_message_type(char * buffer){
	if(strstr(buffer, "GLL") != NULL){
		return GLL;
	}
	else if(strstr(buffer, "GSV") != NULL){
		return GSV;
	}
	else if(strstr(buffer, "RMC") != NULL){
		return RMC;
	}

	return NONE;
}

//Obtains the type of nmea message then parses it
//and returns the message
nmea_message * parse_sentence(nmea_message * message, char * buffer){
	nmea_type type = get_message_type(buffer);
	message->type = type;

	switch(type){
		case GLL:
			parse_gll(message -> gll, buffer);
			break;
		case GSV:
			parse_gsv(message -> gsv, buffer);
			break;
		case RMC:
			parse_rmc(message -> rmc, buffer);
			break;
		case NONE:
			break;
	}

	return message;
}

void free_nmea_message(nmea_message * message){
	if(message -> gll != NULL){
		free(message -> gll);
	}else if(message -> gsv != NULL){
		free(message -> gsv);
	}else if(message -> rmc != NULL){
		free(message -> rmc);
	}

	free(message);
}

void print_nmea_message(nmea_message * message){
	if(message -> type == GLL){
		print_gll(message -> gll);
	}else if(message -> type == GSV){
		print_gsv(message -> gsv);
	}else if(message -> type == RMC){
		print_rmc(message -> rmc);
	}

}
