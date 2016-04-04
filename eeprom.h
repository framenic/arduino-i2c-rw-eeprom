/*
 * All the required information for I2C EEPROM
 * Can be extended
 *
 * =============================================================================
 *
 * Original version:
 *    Diogo Rodrigues, https://github.com/DMRodrigues
 *
 */
 
#include <stdio.h>

const char* type[] = {
	"24XX32",
	"...",
	"24XX1026",
	"24X02",
	"...",
	NULL
};

const char* type1[] = {
	"24XX00",
	"...",
	"24XX16",
	"...",
	NULL
};

const char* type2[] = {
	"24XX1025",
	"...",
	NULL
};


inline void print_eeprom()
{
	int i;
	printf("\n-----\nRead the datasheet of the EEPROM and identify the addressing type compatible with one implemented.\n");
	printf("\nDefault, EEPROMs bigger than 4Kbytes(32 Kbits)");
	printf("\n1 | 0 | 1 | 0 | A2 | A1/B1/P1/A17 | A0/P0/B0/A16\n");
	printf("	Devices:\n		");	
	for(i = 0; type[i]; i++)
		printf("%s, ", type[i]);
	printf("\n\nEEPROMs up to 2Kbytes(16Kbits)\n1 | 0 | 1 | 0 | A2/B2/P2/A10 | A1/B1/P1/A9 | A0/B0/P0/A8  =>  -d 1\n");
	printf("	Devices:\n		");
	for(i = 0; type1[i]; i++)
		printf("%s,", type1[i]);
	printf("\n\nOther type of addressing\n1 | 0 | 1 | 0 | P0/B0/A16 | A1 | A0  =>  -d 2\n");
	printf("	Devices:\n		");
	for(i = 0; type2[i]; i++)
		printf("%s,", type2[i]);
	printf("\n-----");
}
