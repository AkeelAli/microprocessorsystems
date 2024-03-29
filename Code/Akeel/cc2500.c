#include "cc2500.h"

void RF_Config()
{
	RF_Reset();
	SPI_Write(SETTING_A_IOCFG0, SETTING_IOCFG0);
	SPI_Write(SETTING_A_MCSM1, SETTING_MCSM1);
	SPI_Write(SETTING_A_MCSM0, SETTING_MCSM0);
 	SPI_Write(SETTING_A_PKTLEN, SETTING_PKTLEN);
	SPI_Write(SETTING_A_PKTCTRL0, SETTING_PKTCTRL0);
	SPI_Write(SMARTRF_SETTING_A_FSCTRL1, SMARTRF_SETTING_FSCTRL1);
	SPI_Write(SMARTRF_SETTING_A_FSCTRL0, SMARTRF_SETTING_FSCTRL0);
	SPI_Write(SMARTRF_SETTING_A_FREQ2, SMARTRF_SETTING_FREQ2);
	SPI_Write(SMARTRF_SETTING_A_FREQ1, SMARTRF_SETTING_FREQ1);
	SPI_Write(SMARTRF_SETTING_A_FREQ0, SMARTRF_SETTING_FREQ0);
	SPI_Write(SMARTRF_SETTING_A_MDMCFG4, SMARTRF_SETTING_MDMCFG4);
	SPI_Write(SMARTRF_SETTING_A_MDMCFG3, SMARTRF_SETTING_MDMCFG3);
	SPI_Write(SMARTRF_SETTING_A_MDMCFG2, SMARTRF_SETTING_MDMCFG2);
	SPI_Write(SMARTRF_SETTING_A_MDMCFG1, SMARTRF_SETTING_MDMCFG1);
	SPI_Write(SMARTRF_SETTING_A_MDMCFG0, SMARTRF_SETTING_MDMCFG0);
	SPI_Write(SMARTRF_SETTING_A_CHANNR, SMARTRF_SETTING_CHANNR);
	SPI_Write(SMARTRF_SETTING_A_DEVIATN, SMARTRF_SETTING_DEVIATN);
	SPI_Write(SMARTRF_SETTING_A_FREND1, SMARTRF_SETTING_FREND1);
	SPI_Write(SMARTRF_SETTING_A_FREND0, SMARTRF_SETTING_FREND0);
	SPI_Write(SMARTRF_SETTING_A_MCSM0, SMARTRF_SETTING_MCSM0);
	SPI_Write(SMARTRF_SETTING_A_FOCCFG, SMARTRF_SETTING_FOCCFG);
	SPI_Write(SMARTRF_SETTING_A_BSCFG, SMARTRF_SETTING_BSCFG);
	SPI_Write(SMARTRF_SETTING_A_AGCCTRL2, SMARTRF_SETTING_AGCCTRL2);
	SPI_Write(SMARTRF_SETTING_A_AGCCTRL1, SMARTRF_SETTING_AGCCTRL1);
	SPI_Write(SMARTRF_SETTING_A_AGCCTRL0, SMARTRF_SETTING_AGCCTRL0);
	SPI_Write(SMARTRF_SETTING_A_FSCAL3, SMARTRF_SETTING_FSCAL3);
	SPI_Write(SMARTRF_SETTING_A_FSCAL2, SMARTRF_SETTING_FSCAL2);
	SPI_Write(SMARTRF_SETTING_A_FSCAL1, SMARTRF_SETTING_FSCAL1);
	SPI_Write(SMARTRF_SETTING_A_FSCAL0, SMARTRF_SETTING_FSCAL0);
	SPI_Write(SMARTRF_SETTING_A_FSTEST, SMARTRF_SETTING_FSTEST);
	SPI_Write(SMARTRF_SETTING_A_TEST2, SMARTRF_SETTING_TEST2);
	SPI_Write(SMARTRF_SETTING_A_TEST1, SMARTRF_SETTING_TEST1);
	SPI_Write(SMARTRF_SETTING_A_TEST0, SMARTRF_SETTING_TEST0);
	SPI_Write(SMARTRF_SETTING_A_FIFOTHR, SMARTRF_SETTING_FIFOTHR);
	SPI_Write(SMARTRF_SETTING_A_IOCFG2, SMARTRF_SETTING_IOCFG2);
	SPI_Write(SMARTRF_SETTING_A_PKTCTRL1, SMARTRF_SETTING_PKTCTRL1);
	SPI_Write(SMARTRF_SETTING_A_PKTCTRL0, SMARTRF_SETTING_PKTCTRL0);
	SPI_Write(SMARTRF_SETTING_A_ADDR, SMARTRF_SETTING_ADDR);
}

void RF_Verify_Config(uint16_t* buffer)
{
	buffer[0] = SPI_Read(SETTING_A_IOCFG0);
	buffer[1] = SPI_Read(SETTING_A_MCSM1);
	buffer[2] = SPI_Read(SETTING_A_MCSM0);
 	buffer[3] = SPI_Read(SETTING_A_PKTLEN);
	buffer[4] = SPI_Read(SETTING_A_PKTCTRL0);
	buffer[5] = SPI_Read(SMARTRF_SETTING_A_FSCTRL1);
	buffer[6] = SPI_Read(SMARTRF_SETTING_A_FSCTRL0);
	buffer[7] = SPI_Read(SMARTRF_SETTING_A_FREQ2);
	buffer[8] = SPI_Read(SMARTRF_SETTING_A_FREQ1);
	buffer[9] = SPI_Read(SMARTRF_SETTING_A_FREQ0);
	buffer[10] = SPI_Read(SMARTRF_SETTING_A_MDMCFG4);
	buffer[11] = SPI_Read(SMARTRF_SETTING_A_MDMCFG3);
	buffer[12] = SPI_Read(SMARTRF_SETTING_A_MDMCFG2);
	buffer[13] = SPI_Read(SMARTRF_SETTING_A_MDMCFG1);
	buffer[14] = SPI_Read(SMARTRF_SETTING_A_MDMCFG0);
	buffer[15] = SPI_Read(SMARTRF_SETTING_A_CHANNR);
	buffer[16] = SPI_Read(SMARTRF_SETTING_A_DEVIATN);
	buffer[17] = SPI_Read(SMARTRF_SETTING_A_FREND1);
	buffer[18] = SPI_Read(SMARTRF_SETTING_A_FREND0);
	buffer[19] = SPI_Read(SMARTRF_SETTING_A_MCSM0);
	buffer[20] = SPI_Read(SMARTRF_SETTING_A_FOCCFG);
	buffer[21] = SPI_Read(SMARTRF_SETTING_A_BSCFG);
	buffer[22] = SPI_Read(SMARTRF_SETTING_A_AGCCTRL2);
	buffer[23] = SPI_Read(SMARTRF_SETTING_A_AGCCTRL1);
	buffer[24] = SPI_Read(SMARTRF_SETTING_A_AGCCTRL0);
	buffer[25] = SPI_Read(SMARTRF_SETTING_A_FSCAL3);
	buffer[26] = SPI_Read(SMARTRF_SETTING_A_FSCAL2);
	buffer[27] = SPI_Read(SMARTRF_SETTING_A_FSCAL1);
	buffer[28] = SPI_Read(SMARTRF_SETTING_A_FSCAL0);
	buffer[29] = SPI_Read(SMARTRF_SETTING_A_FSTEST);
	buffer[30] = SPI_Read(SMARTRF_SETTING_A_TEST2);
	buffer[31] = SPI_Read(SMARTRF_SETTING_A_TEST1);
	buffer[32] = SPI_Read(SMARTRF_SETTING_A_TEST0);
	buffer[33] = SPI_Read(SMARTRF_SETTING_A_FIFOTHR);
	buffer[34] = SPI_Read(SMARTRF_SETTING_A_IOCFG2);
	buffer[35] = SPI_Read(SMARTRF_SETTING_A_PKTCTRL1);
	buffer[36] = SPI_Read(SMARTRF_SETTING_A_PKTCTRL0);
	buffer[37] = SPI_Read(SMARTRF_SETTING_A_ADDR);
}

uint16_t RF_GetChipStatus()
{
	// Get chip status byte
	return SPI_Read(0x003D);
}

void RF_Reset()
{
	SPI_Command(RF_COMMAND_RESET);
}

void RF_SetToRXMode()
{
	SPI_Command(RF_COMMAND_SRX);
}

void RF_SetToTXMode()
{
	SPI_Command(RF_COMMAND_STX);
}

void RF_FlushRXMode()
{
	SPI_Command(RF_COMMAND_SFRX);
}

void RF_FlushTXMode()
{
	SPI_Command(RF_COMMAND_SFTX);
}

void RF_SetToIdleMode()
{
	SPI_Command(RF_COMMAND_SIDLE);
}


void RF_Transmit(uint16_t* data)
{
	int l;

//	uint16_t bob, joe;
   
   	RF_FlushRXMode();
	RF_FlushTXMode();

	for(l = 0; l < SETTING_PKTLEN; l++)
	{
		SPI_Write(0x3F, data[l]);
	}

	//bob = SPI_Read(0xFA);

	RF_SetToTXMode();

	//joe = SPI_Read(0xF5);

	while((SPI_Read(0xF5) & 0xFF) == 0x13);

	//bob = SPI_Read(0xFA);

	RF_SetToIdleMode();
}
uint16_t bob;
void RF_Receive(uint16_t* data)
{
	int l, i = 0;
	

	RF_FlushRXMode();
	RF_SetToRXMode();

	bob = SPI_Read(0xF5);

	//while((SPI_Read(0xF5) & 0xFF) != 0x0D);

	//receiving data
	while((SPI_Read(0xF8) & 0x01) == 0);
	while((SPI_Read(0xF8) & 0x01) == 1);

	for(i = 0; i < 200; i++);

	RF_SetToIdleMode();

	for(l = 0; l < SETTING_PKTLEN; l++)
	{
		data[l] = SPI_Read(0xBF);
	}
}
