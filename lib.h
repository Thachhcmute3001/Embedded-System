#include "lib.c"


void send_data(unsigned char address, unsigned char value);
float temp();
void Display_char_no_decode(unsigned char position, char text);
void Display_temp();
void clear_text();
void Display_text_position(char* str, unsigned char position);
void Text_run(char* str,int speed,unsigned char mode);
int num_groups_of_8_chars(char* str);
void create_slave_strings(char* string, char slave_strings[][9], int *num_slave_strings);
void reverse_string(char *str, char *rev_str);
void Display_text(char* str, int speed, unsigned char mode);
void init(void);
