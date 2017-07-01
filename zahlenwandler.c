#include <stdio.h>
#include <string.h>
#include <ctype.h>

int MAX_INPUT_SIZE = 32;
int MAX_OUT_SIZE = 8; 	// Sollte nicht größer sein, da weiter unten 16^MAX_OUT_SIZE gerechnet wird -> Integeroverflow
char *PRE_BIN = "0b";
char *PRE_HEX = "0x";
char *PRE_ROM = "0r";
char *HEX = "0123456789abcdef";

void readn(char *dest, int n) {
	int i = 0;
	char c;
	while ( ((c = getchar()) != '\n') && (i < (n-1))) {
		dest[i] = c;
		i++;
	}
	dest[i] = '\0';
}

void description() {
	printf("Dieses Programm ließt Zahlen verschiedener Schreibweisen\n");
	printf("und wandelt diese in andere Schreibweisen um.\n\n");
	printf("Eingabe : \n");
	printf("\t0b00101\t - binäre Zahl\n");
	printf("\t0x1ff \t - hexadezimale Zahl\n");
	printf("\t0rMIV \t - römische Zahl\n");
	printf("\t42 \t - normale Zahl\n\n");
	printf("Ihre Eingabe : ");
}

void output_menu() {
	printf("In welchem Format möchten Sie die Zahl ausgegeben haben?\n\n");
	printf("\t - 0 : binäre Zahl\n");
	printf("\t - 1 : hexadezimale Zahl\n");
	printf("\t - 2 : römische Zahl\n");
	printf("\t - 3 : normale Zahl\n\n");
	printf("Ihre Auswahl : ");
}

int ipow(int base, int exponent) {
	int result = base;

	if (exponent == 0) return 1;
	else if (exponent < 0) return ipow(base, -1 * exponent);

	for (int i=1; i<exponent; i++) {
		result *= base;
	}

	return result;
}

int bin_to_dec(char *bin_number) {
	bin_number += 2; // Ignore the first two characters "0b"
	int i, dec=0, len = i = strlen(bin_number);

	while (i > 0) {
		if (bin_number[i-1] == '1') dec += ipow(2, len-i);
		i--;
	}
	return dec;
}

int hex_to_dec(char *hex_number) {
	hex_number += 2; // Ignore the first two characters "0x"
	int len = strlen(hex_number);
	int dec = 0;
	int digit;
	char c;

	for (int i = len; i > 0; i--) {
		c = hex_number[i-1];	//Get character
		digit = 0;
		while(HEX[digit] != c) digit++; // Convert character to decimal
		dec += digit * ipow(16, len-i);
	}

	return dec;
}

int rom_to_dec(char *rom_number) {
	rom_number += 2; // Skip first two characers
	int i=0;
	int numbers[MAX_INPUT_SIZE];
	memset(numbers, 0, sizeof(int)*MAX_INPUT_SIZE);
	int len = strlen(rom_number);

	while (rom_number[i]) {
		rom_number[i] = toupper(rom_number[i]);
		i++;
	}

	for (i=len; i>0; i--) {
		int n;
		switch(rom_number[i-1]) {
			case 'I': n=1;    break;
			case 'V': n=5;    break;
			case 'X': n=10;   break;
			case 'L': n=50;   break;
			case 'C': n=100;  break;
			case 'D': n=500;  break;
			case 'M': n=1000; break;
		}
		numbers[i] = n;
	}

	int dec = 0;
	i = MAX_INPUT_SIZE -1;

	while (i>=0) {
		dec += numbers[i];
		if ((numbers[i] == 5) || (numbers[i] == 10)) {
			if ((i>0) && (numbers[i-1] == 1)) {
				dec -= 1;
				i--;
			} 
		} else if ((numbers[i] == 50) || (numbers[i] == 100)) {
			if ((i>0) && (numbers[i-1] == 10)) {
				dec -= 10;
				i--;
			} 
		} else if ((numbers[i] == 500) || (numbers[i] == 1000)) {
			if ((i>0) && (numbers[i-1] == 100)) {
				dec -= 100;
				i--;
			} 
		}
		i--;
	}
	return dec;
}

int decimal_int(char *number) {
	int dec;
	if (strncmp(PRE_BIN, number, 2) == 0) {
		return bin_to_dec(number);
	} else if (strncmp(PRE_HEX, number, 2) == 0) {
		return hex_to_dec(number);
	} else if (strncmp(PRE_ROM, number, 2) == 0) {
		return rom_to_dec(number);
	} else { // It's a decimal number
		sscanf(number, "%d", &dec);
		return dec;
	}
}

void dec_to_bin(int dec, char* out) {
	for (int i=MAX_OUT_SIZE; i>0; i--) {
		if (dec >= ipow(2, i-1)) {
			dec -= ipow(2, i-1);
			out[MAX_OUT_SIZE-i] = '1';
		}
	}

}

void dec_to_hex(int dec, char* out) {
	for (int i=MAX_OUT_SIZE; i>0; i--) {
		if ((dec / ipow(16, i-1)) > 0) {
			out[MAX_OUT_SIZE-i] = HEX[dec / ipow(16, i-1)];
			dec = dec % ipow(16, i-1);
		}
	} 
}

void dec_to_rom(int dec, char* out) {
	int rom_numbers[] = { 1000, 900, 500, 400, 100, 90, 50, 40,  10, 9, 5, 4, 1 };
	char rom_chars[] = { 'M', 0 ,'D', -1, 'C', 0, 'L', -1, 'X', 0, 'V', -1, 'I' };
	int i = 0, j, k=0;

	while (dec > 0) {
		j = dec / rom_numbers[i];
		if (j > 0) {
			dec = dec % rom_numbers[i];
			if (rom_chars[i] < 1) {
				if (rom_chars[i] == 0) {
					out[k] = rom_chars[i+3]; // Nächstkleinere Zehnerpotenz
					out[k+1] = rom_chars[i-1]; // Nächstgrößere Zehnerpotenz
				} else {
					out[k] = rom_chars[i+1];
					out[k+1] = rom_chars[i-1];
				}
				k += 2;
				i += 3; // Muss um 4 Stellen erhöht werden, aber wird unten noch mal inkrementiert.
			} else {
				for (int l=0; l<j; l++) {
					out[k+l] = rom_chars[i];
				}
				k += j;
			}
		}
		i++;
	}
}

void remove_rom_zeros(char* out) {
	int l = strlen(out);
	for(int i=0; i<l; i++) {
		if (out[i] == '0') out[i]=0;
	}
}

int main(int argc, char* argv[]) {
	char input[MAX_INPUT_SIZE];

	description();
	readn(input, MAX_INPUT_SIZE);
	int decimal = decimal_int(input);
	output_menu();
	int choice;
	scanf("%d", &choice);	

	char out[MAX_OUT_SIZE+1];
	memset(out, '0', MAX_OUT_SIZE);
	out[MAX_OUT_SIZE] = 0;
	switch (choice) {
		case 0:
			dec_to_bin(decimal, out);
			printf("Die Zahl ist in binärer Schreibweise : 0b%s \n", out);
			break;
		case 1:
			dec_to_hex(decimal, out);
			printf("Die Zahl ist in hexadezimaler Schreibweise : 0x%s \n", out);
			break;
		case 2:
			dec_to_rom(decimal, out);
			remove_rom_zeros(out);
			printf("Die Zahl ist in römischer Schreibweise : %s \n", out);
			break;
		case 3:
			printf("Die Zahl ist in dezimaler Schreibweise : %d \n", decimal);
			break;
		default: break;
	}
	return 0;
}
