#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>  // time_t, struct tm, time, localtime
#include <softPwm.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <wiringPiSPI.h>
#include <string.h>

#define max_string_length 100
unsigned char Data[2]; //16bits
unsigned char text_bina;
char temp_str[5];
char rev_str[300];
char temp_string[8];
char slave_strings[100][9];
int num_slave_strings, i;
int count = 0;
int a;
int b;
int c;
int g;

unsigned char font[][2] = {
  {'A',0b1110111},{'B',0b1111111},{'C',0b1001110},{'D',0b1111110},{'E',0b1001111},{'F',0b1000111},       
  {'G',0b1011110},{'H',0b0110111},{'I',0b0110000},{'J',0b0111100},{'L',0b0001110},{'N',0b1110110},       
  {'O',0b1111110},{'P',0b1100111},{'R',0b0000101},{'S',0b1011011},{'T',0b0001111},{'U',0b0111110},       
  {'Y',0b0100111},{'[',0b1001110},{']',0b1111000},{'_',0b0001000},{'a',0b1110111},{'b',0b0011111},       
  {'c',0b0001101},{'d',0b0111101},{'e',0b1001111},{'f',0b1000111},{'g',0b1011110},{'h',0b0010111},       
  {'i',0b0010000},{'j',0b0111100},{'l',0b0001110},{'n',0b0010101},{'o',0b0011101},{'p',0b1100111},       
  {'r',0b0000101},{'s',0b1011011},{'t',0b0001111},{'u',0b0011100},{'y',0b0100111},{'-',0b0000001},
  {' ',0b0000000},{'0',0b1111110},{'1',0b0110000},{'2',0b1101101},{'3',0b1111001},{'4',0b0110011},
  {'5',0b1011011},{'6',0b1011111},{'7',0b1110000},{'8',0b1111111},{'9',0b1111011},{'V',0b0111110},
  {'k',0b0001111},{'m',0b0010101},{'q',0b1110011},{'v',0b0111110},{'.',0b10000000},{'?',0b1100101},
  {'\0',0b0000000},};

void send_data(unsigned char address, unsigned char value){
	//Set decode mode
	Data[0] = address;
	Data[1] = value;
	wiringPiSPIDataRW(0, Data, 2);
}

float temp()
{
	FILE *tempfile;
	float temp;
	tempfile = fopen("/sys/class/thermal/thermal_zone0/temp","r");
	fscanf(tempfile, "%f",&temp);
	temp = temp / 1000;
	fclose(tempfile);
	// dd/mm/yyyy hh:mm:ss temp
	sleep(1);	
	return temp;
}

void Display_char_no_decode(unsigned char position, char text){
	for(int i = 0; font[i][0] != '\0'; i++){
		if(font[i][0] == text){
			text_bina = font[i][1];
			send_data(position, text_bina);
		}
	}
}
void Display_temp(){
	sprintf(temp_str, "%.2f", temp());
	for(int i = 0; temp_str[i] != '\0'; i++){
		Display_char_no_decode(8 - i, temp_str[i]);
	//	Display_char_no_decode(1, '.');
		Display_char_no_decode(2, 'C');
		Display_char_no_decode(3, 'o');
	}
}
void clear_text(){
	char str[8] = "        ";
	for(int i = 0; str[i] != '\0'; i++){
		Display_char_no_decode(8-i, str[i]);
	}
	delay(500);
	
}
void Display_text_position(char* str, unsigned char position){
	for(int i = 0; str[i] != '\0'; i++){
		Display_char_no_decode(position-i, str[i]);
	}
}

void Text_run(char* str,int speed,unsigned char mode){
	if(mode == 's'){		
		for(int i = 0; str[i] != '\0'; i++){
			Display_char_no_decode(8-i, str[i]);
		}
	}
	if(mode == 'l'){
		for(int n = 8; n >= 1; n--){ //n is position
			if(n < strlen(str)){
				a = strlen(str) - n;
				//on the right
				for(int i = 0; i < n; i++){
					Display_char_no_decode(n - i, str[i]);
				}
				for(int i = 0; i < strlen(str) - n; i++){
					Display_char_no_decode(8 - i, str[strlen(str) - a]);
					a--;
				}
				delay(speed);
				Display_char_no_decode(n, ' ');
			}
			else{
				for(int i = 0; str[i] != '\0'; i++){
					Display_char_no_decode(n - i, str[i]);
					
				}
				delay(speed);
				Display_char_no_decode(n, ' ');					
			}
		}
	}
	if(mode == 'r'){
		for(int n = 1; n <= 8; n++){ //n is position		
			if(n > 8 - strlen(str) + 1){
				b = n-1;
				for(int i = 0; i < 8 - n + 1; i++){
					Display_char_no_decode(n + i, str[(strlen(str) - 1) - i]);
				}
				for(int i = 0; i < n - (8 - strlen(str)) - 1; i++){
					Display_char_no_decode((i + 1), str[b - (8 - strlen(str))-1]);
					b--;
				}
				delay(speed);
				Display_char_no_decode(n, ' '); //Direction		
				
			}
			else{
				for(int i = 0; str[i] != '\0'; i++){
					Display_char_no_decode(n + i, str[(strlen(str) - 1) - i]);
				}
				delay(speed);
				Display_char_no_decode(n, ' '); //Direction				
			}

		}
	}
	
}
void create_slave_strings(char* string, char slave_strings[][9], int *num_slave_strings) {
    int i, j, k = 0;

    // Iterate through the original string and create substrings of 8 characters
    for (i = 0; i < strlen(string); i++) {
        if (i + 8 <= strlen(string)) {
            for (j = 0; j < 8; j++) {
                slave_strings[k][j] = string[i+j];
            }
            slave_strings[k][8] = '\0'; // Add null terminator to make it a valid string
            k++;
        }
    }
    *num_slave_strings = k;
}
void reverse_string(char *str, char *rev_str) {
    char *ptr, *last;	
    int len;
	//Take the length of the string
    len = strlen(str);
    ptr = str;
    last = str + len - 1;

    // Reverse the entire string
    while (last > ptr) {
        char temp = *last;
        *last = *ptr;
        *ptr = temp;
        last--;
        ptr++;
    }
	
    // Reverse each word in the string
    ptr = str;
    while (*ptr) {
        char *start = ptr;
        while (*ptr != ' ' && *ptr != '\0') {
            ptr++;
        }
        last = ptr - 1;
        while (last > start) {
            char temp = *last;
            *last = *start;
            *start = temp;
            last--;
            start++;
        }
        if (*ptr == ' ') {
            ptr++;
        }
    }
	
	//When it convert to the reverse string, in the while loop, it's can't convert repeatedly
	//so we need to assign it into a subvariable, and declare a count so as to assign only 1 times
	//to avoid being the same mistake spoken above.
	for(int i = 0; str[i] != '\0'; i++){
		if(count == 0){rev_str[i] = str[i];}
	}
	count = 1;
}
void Display_text(char* str, int speed, unsigned char mode){
	if(mode == 'r'){
		if(strlen(str) < 8){
			Text_run(str,speed,'r');
		}
		else{
			create_slave_strings(str, slave_strings,&num_slave_strings);
			for (int i = 0; i < num_slave_strings; i++) {
				for(int j = 0; slave_strings[i][j] != '\0'; j++){
					Display_char_no_decode(8 - j, slave_strings[i][j]);
				}
				delay(speed);
			}
			for(int n = 1; n <= 7; n++){
				b = 7;
				//Display the last string from right to left
				for(int i = 0; i < 8 - n; i++){
					Display_char_no_decode(n + i + 1, slave_strings[num_slave_strings - 1][b]);
					b--;
				}
				//Display the first string from right to left
				for(int i = 0; i < n; i++){
					Display_char_no_decode(n - i, slave_strings[0][i]);
				}
				delay(speed);
			}
		}
	}
	if(mode == 'l'){
		if(strlen(str) < 8){
			Text_run(str,speed,'l');
		}
		else{
			reverse_string(str,rev_str);
			create_slave_strings(rev_str, slave_strings,&num_slave_strings);
			for(int i = num_slave_strings - 1; i >= 0; i--) {
				for(int j = 0; slave_strings[i][j] != '\0'; j++){
					Display_char_no_decode(8 - j, slave_strings[i][j]);
				}
				delay(speed);
			}
			for(int n = 7; n >= 1; n--){
				b = 7;
				for(int i = 0; i < n; i++){
					Display_char_no_decode(n - i, slave_strings[0][i]);
				}
				for(int i = 0; i < 8 - n; i++){ // i is the times iterating to put the char in the led
					Display_char_no_decode(n + i + 1, slave_strings[num_slave_strings - 1][b]);
					b--;
				}
				delay(speed);
			}	
		}		
	}
}

void init(void){
	//Set no-decode mode
	send_data(0x09, 0x00);
	//Set intensity
	send_data(0x0A, 0x09);
	//Set scan limit
	send_data(0x0B, 0x07);
	//No shutdown, turn off display test
	send_data(0x0C, 0x01);
	send_data(0x0F, 0x00);
}
