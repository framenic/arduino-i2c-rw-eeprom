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

const char* type1[] = {
	"24XX1026",
	"CAT24M01",
	"AT24CM01",
	"M24M01",
	"...",
	NULL
};

const char* type2[] = {
	"24XX1025",
	"...",
	NULL
};
	
const char* type3[] = {
	"M24M02",
	"AT24CM02",
	"...",
	NULL
};

inline void print_eeprom()
{
	int i;
	printf("\n-----\nRead the datasheet of the EEPROM and identify the addressing type compatible with one implemented.\n");
	printf("\n1 | 0 | 1 | 0 | A2 | A1 | B0/A16  =>  -d 1\n");
	printf("	Devices:\n");	
	for(i = 0; type1[i]; i++)
		printf("		%s\n", type1[i]);
	printf("\n1 | 0 | 1 | 0 | B0/A16 | A1 | A0  =>  -d 2\n");
	printf("	Devices:\n");
	for(i = 0; type2[i]; i++)
		printf("		%s\n", type2[i]);
	printf("\n1 | 0 | 1 | 0 | A2 | A17 | A16    =>  -d 3\n");
	printf("	Devices:\n");
	for(i = 0; type3[i]; i++)
		printf("		%s\n", type3[i]);
	printf("\n-----");
}
