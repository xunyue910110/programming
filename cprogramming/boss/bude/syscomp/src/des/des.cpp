#include <stdio.h>
#include <string.h>

#include "des.h"

/*
unsigned char *byte2bit(unsigned char byte[64] , unsigned char bit[8]);
unsigned char *bit2byte(unsigned char bit[8] , unsigned char byte[64]);
void keychange(unsigned char oldkey[8] , unsigned char newkey[16][8]);
void s_replace(unsigned char s_bit[8]);
*/

int _desHex(char c);
int _desGetKey(unsigned char k_bit[8]);

void Encryptor(unsigned char m_bit[8] , unsigned char k_bit[8] , unsigned char e_bit[8]);
void Decryptor(unsigned char m_bit[8] , unsigned char k_bit[8] , unsigned char e_bit[8]);


/**
 *  Implementation of class DES
 */

/**
 *  加密
 *　return  0  success
 *         -1  open des-key file failure
 */
int DES::encrypt(const string& _mtext, string& _etext)
{
    unsigned char m_bit[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    unsigned char k_bit[8]; //={'[','Z','W','g','j','V','g','n'}; //key
    unsigned char e_bit[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

	char c_array[20] = {0};
	memset(c_array, '\0', sizeof(c_array));
	
	//! get des-key from config-key file    
	if (_desGetKey(k_bit) < 0)  return -1;
	
	///!
	int mlen = _mtext.length();
	for (int i=0; i<mlen; i++) {
		m_bit[i%8] = (_mtext.c_str())[i];

		if ((i+1)%8 == 0) {		
			Encryptor(m_bit, k_bit, e_bit);
			
			memset(c_array, '\0', sizeof(c_array));
			for (int j=0; j<8; j++) {
				sprintf(c_array+j*2, "%02X", e_bit[j]);
			}
			c_array[16] = '\0';
			
			_etext += c_array;
			memset(m_bit, '\0', sizeof(m_bit));
			memset(e_bit, '\0', sizeof(e_bit));
		}
	}

	//!
	{	
		Encryptor(m_bit, k_bit, e_bit);
		
		memset(c_array, '\0', sizeof(c_array));
		for (int i=0; i<8; i++) {
			sprintf(c_array+i*2, "%02X", e_bit[i]);
		}
		c_array[16] = '\0';
		
		_etext += c_array;
	}
	
	//
	return 0;			
}

/**
 *  解密
 *  return  0  succes
 *         -1  open des-key file failure
 *         -2  invalid charactor
 */
int DES::decrypt(const string& _etext, string& _mtext)
{
    unsigned char m_bit[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    unsigned char k_bit[8]; //={'[','Z','W','g','j','V','g','n'}; //key
    unsigned char e_bit[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

	char c_array[20] = {0};
	memset(c_array, '\0', sizeof(c_array));

	//! get des-key from config-key file    
	if (_desGetKey(k_bit) < 0)  return -1;
    
	///! encrypted-text must be 16bit per group.
	memset(c_array, '\0', sizeof(c_array));   
	int elen = _etext.length();
	for (int i = 0; i<elen; i++) {
		c_array[i%16] = _etext.c_str()[i];
		
		// 15, 31 for from 0
		if ((i+1)%16 == 0) {
			for (int k=0; k<8; k++) {
				char h = _desHex(c_array[2*k]);
				char l = _desHex(c_array[2*k+1]);
				if (h<0 || l<0) {  //invalid charactor
					return -2;
				}
				
				e_bit[k] = h*16 + l;
			}
			
			Decryptor(e_bit, k_bit, m_bit);
			
			char c_arr[10] = {0};
			memset(c_arr, '\0', sizeof(c_arr));
			for (int k=0; k<8; k++) {
				c_arr[k] = m_bit[k];
			}
			c_arr[8] = '\0';
			
			//
			_mtext += c_arr;    
			memset(m_bit, '\0', sizeof(m_bit));
			memset(e_bit, '\0', sizeof(e_bit));
			memset(c_array, '\0', sizeof(c_array));
		}
	}    
	
	//
	return 0;
}


/**
 *  base algorithm functions
 */
 
/*change 64byte to 64bit(8byte)*/
unsigned char *byte2bit(unsigned char byte[64] , unsigned char bit[8])
{
    int i = 0 ;

    /*byte1*/
    for (i = 0; i <= 7; i++)
    {
        if (byte[0] == 0x1) bit[0] = bit[0] | 0x80 ;
        else bit[0] = bit[0] & 0x7f ;
        if (byte[1] == 0x1) bit[0] = bit[0] | 0x40 ;
        else bit[0] = bit[0] & 0xbf ;
        if (byte[2] == 0x1) bit[0] = bit[0] | 0x20 ;
        else bit[0] = bit[0] & 0xdf ;
        if (byte[3] == 0x1) bit[0] = bit[0] | 0x10 ;
        else bit[0] = bit[0] & 0xef ;
        if (byte[4] == 0x1) bit[0] = bit[0] | 0x08 ;
        else bit[0] = bit[0] & 0xf7 ;
        if (byte[5] == 0x1) bit[0] = bit[0] | 0x04 ;
        else bit[0] = bit[0] & 0xfb ;
        if (byte[6] == 0x1) bit[0] = bit[0] | 0x02 ;
        else bit[0] = bit[0] & 0xfd ;
        if (byte[7] == 0x1) bit[0] = bit[0] | 0x01 ;
        else bit[0] = bit[0] & 0xfe ;
    }

    /*byte2*/
    for (i = 8;i <= 15;i++)
    {
        if (byte[8] == 0x1) bit[1] = bit[1] | 0x80 ;
        else bit[1] = bit[1] & 0x7f ;
        if (byte[9] == 0x1) bit[1] = bit[1] | 0x40 ;
        else bit[1] = bit[1] & 0xbf ;
        if (byte[10] == 0x1) bit[1] = bit[1] | 0x20 ;
        else bit[1] = bit[1] & 0xdf ;
        if (byte[11] == 0x1) bit[1] = bit[1] | 0x10 ;
        else bit[1] = bit[1] & 0xef ;
        if (byte[12] == 0x1) bit[1] = bit[1] | 0x08 ;
        else bit[1] = bit[1] & 0xf7 ;
        if (byte[13] == 0x1) bit[1] = bit[1] | 0x04 ;
        else bit[1] = bit[1] & 0xfb ;
        if (byte[14] == 0x1) bit[1] = bit[1] | 0x02 ;
        else bit[1] = bit[1] & 0xfd ;
        if (byte[15] == 0x1) bit[1] = bit[1] | 0x01 ;
        else bit[1] = bit[1] & 0xfe ;
    }

    /*byte3*/
    for (i = 16;i <= 23 ;i++)
    {
        if (byte[16] == 0x1) bit[2] = bit[2] | 0x80 ;
        else bit[2] = bit[2] & 0x7f ;
        if (byte[17] == 0x1) bit[2] = bit[2] | 0x40 ;
        else bit[2] = bit[2] & 0xbf ;
        if (byte[18] == 0x1) bit[2] = bit[2] | 0x20 ;
        else bit[2] = bit[2] & 0xdf ;
        if (byte[19] == 0x1) bit[2] = bit[2] | 0x10 ;
        else bit[2] = bit[2] & 0xef ;
        if (byte[20] == 0x1) bit[2] = bit[2] | 0x08 ;
        else bit[2] = bit[2] & 0xf7 ;
        if (byte[21] == 0x1) bit[2] = bit[2] | 0x04 ;
        else bit[2] = bit[2] & 0xfb ;
        if (byte[22] == 0x1) bit[2] = bit[2] | 0x02 ;
        else bit[2] = bit[2] & 0xfd ;
        if (byte[23] == 0x1) bit[2] = bit[2] | 0x01 ;
        else bit[2] = bit[2] & 0xfe ;
    }

    /*byte4*/
    for (i = 24;i <= 31 ;i++)
    {
        if (byte[24] == 0x1) bit[3] = bit[3] | 0x80 ;
        else bit[3] = bit[3] & 0x7f ;
        if (byte[25] == 0x1) bit[3] = bit[3] | 0x40 ;
        else bit[3] = bit[3] & 0xbf ;
        if (byte[26] == 0x1) bit[3] = bit[3] | 0x20 ;
        else bit[3] = bit[3] & 0xdf ;
        if (byte[27] == 0x1) bit[3] = bit[3] | 0x10 ;
        else bit[3] = bit[3] & 0xef ;
        if (byte[28] == 0x1) bit[3] = bit[3] | 0x08 ;
        else bit[3] = bit[3] & 0xf7 ;
        if (byte[29] == 0x1) bit[3] = bit[3] | 0x04 ;
        else bit[3] = bit[3] & 0xfb ;
        if (byte[30] == 0x1) bit[3] = bit[3] | 0x02 ;
        else bit[3] = bit[3] & 0xfd ;
        if (byte[31] == 0x1) bit[3] = bit[3] | 0x01 ;
        else bit[3] = bit[3] & 0xfe ;
    }

    /*byte5*/
    for (i = 32;i <= 39 ;i++)
    {
        if (byte[32] == 0x1) bit[4] = bit[4] | 0x80 ;
        else bit[4] = bit[4] & 0x7f ;
        if (byte[33] == 0x1) bit[4] = bit[4] | 0x40 ;
        else bit[4] = bit[4] & 0xbf ;
        if (byte[34] == 0x1) bit[4] = bit[4] | 0x20 ;
        else bit[4] = bit[4] & 0xdf ;
        if (byte[35] == 0x1) bit[4] = bit[4] | 0x10 ;
        else bit[4] = bit[4] & 0xef ;
        if (byte[36] == 0x1) bit[4] = bit[4] | 0x08 ;
        else bit[4] = bit[4] & 0xf7 ;
        if (byte[37] == 0x1) bit[4] = bit[4] | 0x04 ;
        else bit[4] = bit[4] & 0xfb ;
        if (byte[38] == 0x1) bit[4] = bit[4] | 0x02 ;
        else bit[4] = bit[4] & 0xfd ;
        if (byte[39] == 0x1) bit[4] = bit[4] | 0x01 ;
        else bit[4] = bit[4] & 0xfe ;
    }

    /*byte6*/
    for (i = 40;i <= 47 ;i++)
    {
        if (byte[40] == 0x1) bit[5] = bit[5] | 0x80 ;
        else bit[5] = bit[5] & 0x7f ;
        if (byte[41] == 0x1) bit[5] = bit[5] | 0x40 ;
        else bit[5] = bit[5] & 0xbf ;
        if (byte[42] == 0x1) bit[5] = bit[5] | 0x20 ;
        else bit[5] = bit[5] & 0xdf ;
        if (byte[43] == 0x1) bit[5] = bit[5] | 0x10 ;
        else bit[5] = bit[5] & 0xef ;
        if (byte[44] == 0x1) bit[5] = bit[5] | 0x08 ;
        else bit[5] = bit[5] & 0xf7 ;
        if (byte[45] == 0x1) bit[5] = bit[5] | 0x04 ;
        else bit[5] = bit[5] & 0xfb ;
        if (byte[46] == 0x1) bit[5] = bit[5] | 0x02 ;
        else bit[5] = bit[5] & 0xfd ;
        if (byte[47] == 0x1) bit[5] = bit[5] | 0x01 ;
        else bit[5] = bit[5] & 0xfe ;
    }

    /*byte7*/
    for (i = 48;i <= 55 ;i++)
    {
        if (byte[48] == 0x1) bit[6] = bit[6] | 0x80 ;
        else bit[6] = bit[6] & 0x7f ;
        if (byte[49] == 0x1) bit[6] = bit[6] | 0x40 ;
        else bit[6] = bit[6] & 0xbf ;
        if (byte[50] == 0x1) bit[6] = bit[6] | 0x20 ;
        else bit[6] = bit[6] & 0xdf ;
        if (byte[51] == 0x1) bit[6] = bit[6] | 0x10 ;
        else bit[6] = bit[6] & 0xef ;
        if (byte[52] == 0x1) bit[6] = bit[6] | 0x08 ;
        else bit[6] = bit[6] & 0xf7;
        if (byte[53] == 0x1) bit[6] = bit[6] | 0x04 ;
        else bit[6] = bit[6] & 0xfb ;
        if (byte[54] == 0x1) bit[6] = bit[6] | 0x02 ;
        else bit[6] = bit[6] & 0xfd ;
        if (byte[55] == 0x1) bit[6] = bit[6] | 0x01 ;
        else bit[6] = bit[6] & 0xfe ;
    }

    /*byte8*/
    for (i = 56;i <= 63 ;i++)
    {
        if (byte[56] == 0x1) bit[7] = bit[7] | 0x80 ;
        else bit[7] = bit[7] & 0x7f ;
        if (byte[57] == 0x1) bit[7] = bit[7] | 0x40 ;
        else bit[7] = bit[7] & 0xbf ;
        if (byte[58] == 0x1) bit[7] = bit[7] | 0x20 ;
        else bit[7] = bit[7] & 0xdf ;
        if (byte[59] == 0x1) bit[7] = bit[7] | 0x10 ;
        else bit[7] = bit[7] & 0xef ;
        if (byte[60] == 0x1) bit[7] = bit[7] | 0x08 ;
        else bit[7] = bit[7] & 0xf7 ;
        if (byte[61] == 0x1) bit[7] = bit[7] | 0x04 ;
        else bit[7] = bit[7] & 0xfb ;
        if (byte[62] == 0x1) bit[7] = bit[7] | 0x02 ;
        else bit[7] = bit[7] & 0xfd ;
        if (byte[63] == 0x1) bit[7] = bit[7] | 0x01 ;
        else bit[7] = bit[7] & 0xfe ;
    }

    return bit ;
}/*end of byte2bit*/


/*----------------------------------------------------*/
/* bit2byte */
/*----------------------------------------------------*/
/*change 64bit(8byte) to 64byte*/
unsigned char *bit2byte(unsigned char bit[8] , unsigned char byte[64])
{
    int i=0 ;

    for (i=0 ; i<=63 ; i++)
    {
        byte[i] = 0x0 ;
    }

    for (i=0 ; i<=7 ; i++)
    {
        if ((bit[i] & 0x80) == 0x80) byte[i*8+0] = 0x01 ;
        if ((bit[i] & 0x40) == 0x40) byte[i*8+1] = 0x01 ;
        if ((bit[i] & 0x20) == 0x20) byte[i*8+2] = 0x01 ;
        if ((bit[i] & 0x10) == 0x10) byte[i*8+3] = 0x01 ;
        if ((bit[i] & 0x08) == 0x08) byte[i*8+4] = 0x01 ;
        if ((bit[i] & 0x04) == 0x04) byte[i*8+5] = 0x01 ;
        if ((bit[i] & 0x02) == 0x02) byte[i*8+6] = 0x01 ;
        if ((bit[i] & 0x01) == 0x01) byte[i*8+7] = 0x01 ;
    }

    return byte;
}/* end of bit2byte */


/*--------------------------------------------------*/
/* change the key */
/*--------------------------------------------------*/
void keychange(unsigned char oldkey[8] , unsigned char newkey[16][8])
{
    int i=0,j=0,k=0 ;
    int pc_1[56] = {57,49,41,33,25,17,9,
        1,58,50,42,34,26,18,
        10,2,59,51,43,35,27,
        19,11,3,60,52,44,36,
        63,55,47,39,31,23,15,
        7,62,54,46,38,30,22,
        14,6,61,53,45,37,29,
        21,13,5,28,20,12,4} ;

    int pc_2[48] = {14,17,11,24,1,5,
        3,28,15,6,21,10,
        23,19,12,4,26,8,
        16,7,27,20,13,2,
        41,52,31,37,47,55,
        30,40,51,45,33,48,
        44,49,39,56,34,53,
        46,42,50,36,29,32} ;

    int ccmovebit[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1} ;
    unsigned char oldkey_byte[64] ;
    unsigned char oldkey_byte1[64] ;
    unsigned char oldkey_byte2[64] ;
    unsigned char oldkey_c[28] ;
    unsigned char oldkey_d[28] ;
    unsigned char cc_temp ;
    unsigned char newkey_byte[16][64] ;

    bit2byte(oldkey,oldkey_byte) ;/*change to byte*/

    for (i=0;i<=55;i++) oldkey_byte1[i] = oldkey_byte[pc_1[i] - 1] ;/*goto PC-1*/
    for (i=0;i<=27;i++) oldkey_c[i] = oldkey_byte1[i] ;/*move 28bit -> c0*/
    for (i=28;i<=55;i++) oldkey_d[i -28] = oldkey_byte1[i] ;/*move other 28bit -> d0*/

    /*cc_movebit : get c1-16,d1-16*/
    for (i=0;i<=15;i++)
    {
        for (j=1;j<=ccmovebit[i];j++)
        {
            cc_temp = oldkey_c[0] ; /*move out the first bit*/
            for (k=0;k<=26;k++)
            {
                oldkey_c[k] = oldkey_c[k+1] ;
            }
            oldkey_c[27] = cc_temp ; /*move the first bit to the last bit*/

            cc_temp = oldkey_d[0] ; /*move out the first bit*/
            for (k=0;k<=26;k++)
            {
                oldkey_d[k] = oldkey_d[k+1] ;
            }

            oldkey_d[27] = cc_temp ; /*move the first bit to the last bit*/
        } /*cc_movebit*/

        /*goto pc-2 change bit*/
        for (k=0;k<=27;k++) oldkey_byte2[k] = oldkey_c[k] ;
        for (k=28;k<=55;k++) oldkey_byte2[k] = oldkey_d[k-28] ;

        /*add c(i)+d(i) -> for pc-2 change 56bit to 48bit k(i)*/
        for (k=0;k<=47;k++) newkey_byte[i][k] = oldkey_byte2[pc_2[k] - 1] ;

        /*ready to next k(i)*/
    }/*end of one of change the 48bit key*/
    /*byte to bit for 48bit newkey*/

    for (i=0;i<=15;i++) byte2bit(newkey_byte[i],newkey[i]) ;
}/*end of keychange*/


/*--------------------------------------------------*/
/* S_replace */
/*--------------------------------------------------*/
void s_replace(unsigned char s_bit[8])
{
    int p[32] = { 16,7,20,21,
        29,12,28,17,
        1,15,23,26,
        5,18,31,10,
        2,8,24,14,
        32,27,3,9,
        19,13,30,6,
        22,11,4,25
    } ;
    unsigned char s1[4][16] = { 14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
        0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
        4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
        15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13
    } ;
    unsigned char s2[4][16] = { 15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
        3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
        0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
        13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9
    };
    unsigned char s3[4][16] = { 10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
        13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
        13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
        1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12
    };
    unsigned char s4[4][16] = { 7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
        13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
        10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
        3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14
    };
    unsigned char s5[4][16] = { 2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
        14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
        4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
        11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,
    };
    unsigned char s6[4][16] = { 12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
        10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
        9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
        4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13
    };
    unsigned char s7[4][16] = { 4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
        13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
        1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
        6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12
    };
    unsigned char s8[4][16] = { 13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
        1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
        7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
        2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
    };
    int i=0 , j=0 ;
    unsigned char s_byte[64] ;
    unsigned char s_byte1[64] ;
    unsigned char s_bit_temp[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0} ;
    unsigned char row = 0 , col = 0 ;
    unsigned char s_out_bit[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0} ;
    unsigned char s_out_bit1[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0} ;

    /*initialize*/
    for (i=0;i<=63;i++) s_byte[i] = 0x0 ;
    for (i=0;i<=63;i++) s_byte1[i] = 0x0 ;

    /*change 48bit(8bit x 6byte) to 48bit(6bit x 8byte)*/
    bit2byte(s_bit,s_byte) ;
    for (i=0;i<=7;i++)
    {
        for (j=0;j<=63;j++) s_byte1[j] = 0x0 ;/*clear temp*/

        s_byte1[6] = s_byte[i*6] ; /*get bit 0 in 6 bit*/
        s_byte1[7] = s_byte[i*6+5] ;/*get bit 5 in 6 bit*/
        byte2bit(s_byte1,s_bit_temp) ;/* 0000 00?? */
        row = s_bit_temp[0] ;/*get row[i]*/

        for (j=0;j<=63;j++) s_byte1[j] = 0x0 ;/*clear temp*/

        s_byte1[4] = s_byte[i*6+1] ;/*0000 ????*/
        s_byte1[5] = s_byte[i*6+2] ;
        s_byte1[6] = s_byte[i*6+3] ;
        s_byte1[7] = s_byte[i*6+4] ;
        byte2bit(s_byte1,s_bit_temp) ;
        col = s_bit_temp[0] ;/*get column in S table*/

        /*get number from S table with row and col*/
        switch (i)
        {
        case 0 :
            s_out_bit[i] = s1[row][col] ;
            break ;
        case 1 :
            s_out_bit[i] = s2[row][col] ;
            break ;
        case 2 :
            s_out_bit[i] = s3[row][col] ;
            break ;
        case 3 :
            s_out_bit[i] = s4[row][col] ;
            break ;
        case 4 :
            s_out_bit[i] = s5[row][col] ;
            break ;
        case 5 :
            s_out_bit[i] = s6[row][col] ;
            break ;
        case 6 :
            s_out_bit[i] = s7[row][col] ;
            break ;
        case 7 :
            s_out_bit[i] = s8[row][col] ;
            break ;
        } ;


    } /*s_out_bit[0-7]:0000???? 0000???? 0000???? 0000????...0000???? */

    /*change 64bit to 32bit : clean 0000(high 4bit)*/
    s_out_bit1[0] = (s_out_bit[0]<<4) + s_out_bit[1] ;
    s_out_bit1[1] = (s_out_bit[2]<<4) + s_out_bit[3] ;
    s_out_bit1[2] = (s_out_bit[4]<<4) + s_out_bit[5] ;
    s_out_bit1[3] = (s_out_bit[6]<<4) + s_out_bit[7] ;

    /*now s_out_bit1[0-7] = ???????? ???????? ???????? ???????? 0000..*/
    for (i=0;i<=63;i++) s_byte[i] = 0x0 ;
    for (i=0;i<=63;i++) s_byte1[i] = 0x0 ;
    bit2byte(s_out_bit1 , s_byte) ;/*change byte for P swap bit*/
    for (i=0;i<=31;i++) s_byte1[i] = s_byte[p[i] - 1] ;/*goto P swap bit*/
    for (i=0;i<=7;i++) s_bit[i] = 0x0 ;
    byte2bit(s_byte1 , s_bit) ;
    /*now ! we got 32bit f(R,K)*/
}/*end of S_replace*/

/*--------------------------------------------------*/
/* data encryption */
/*--------------------------------------------------*/
void Encryptor(unsigned char m_bit[8] , unsigned char k_bit[8] , unsigned char e_bit[8])
{
    int ip[64] = { 58,50,42,34,26,18,10,2,
        60,52,44,36,28,20,12,4,
        62,54,46,38,30,22,14,6,
        64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9,1,
        59,51,43,35,27,19,11,3,
        61,53,45,37,29,21,13,5,
        63,55,47,39,31,23,15,7
    } ;

    int ip_1[64] = {40,8,48,16,56,24,64,32,
        39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30,
        37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28,
        35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26,
        33,1,41,9,49,17,57,25
    } ;

    int e[48] = { 32,1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10,11,12,13,
        12,13,14,15,16,17,
        16,17,18,19,20,21,
        20,21,22,23,24,25,
        24,25,26,27,28,29,
        28,29,30,31,32,1
    } ;

    unsigned char m_bit1[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0} ;
    unsigned char m_byte[64] ;
    unsigned char m_byte1[64] ;
    unsigned char key_n[16][8] ;
    unsigned char l_bit[17][8] ;
    unsigned char r_bit[17][8] ;
    unsigned char e_byte[64] ;
    unsigned char e_byte1[64] ;
    unsigned char r_byte[64] ;
    unsigned char r_byte1[64] ;
    int i = 0 , j = 0;

    /*initialize*/
    for (i=0;i<=15;i++)
    {
        for (j=0;j<=7;j++)
        {
            l_bit[i][j] = 0x0 ;
            r_bit[i][j] = 0x0 ;
            key_n[i][j] = 0x0 ;
        }
    }

    for (i=0;i<=63;i++)
    {
        m_byte[i] = 0x0 ;
        m_byte1[i] = 0x0 ;
        r_byte[i] = 0x0 ;
        r_byte1[i] = 0x0 ;
        e_byte[i] = 0x0 ;
        e_byte1[i] = 0x0 ;
    }

    keychange(k_bit,key_n) ;/*get the 48bit key x 16 (16rows x 6byte in key_n)*/
    bit2byte(m_bit,m_byte) ;/*change to byte*/
    for (i=0;i<=63;i++) m_byte1[i] = m_byte[ip[i] - 1] ;/*goto IP swap bit*/
    byte2bit(m_byte1,m_bit1) ;/*re-change to bit*/
    for (i=0;i<=3;i++) l_bit[0][i] = m_bit1[i] ;/*move left 32bit -> l0*/
    for (i=4;i<=7;i++) r_bit[0][i - 4] = m_bit1[i] ;/*move right 32bit -> r0*/

    for (i=1;i<=16;i++) /*16 layer*/
    {
        for (j=0;j<=3;j++) l_bit[i][j] = r_bit[i-1][j] ;/*L(n) = R(n-1)*/

        /*comput f(R(n-1),k)*/
        bit2byte(r_bit[i-1],r_byte) ;
        for (j=0;j<=47;j++) r_byte1[j] = r_byte[e[j] - 1] ;/*goto E swap bit*/
        byte2bit(r_byte1,r_bit[i-1]) ;/*now r_bit is 48bit*/

        /*xor 48bit key*/
        for (j=0;j<=5;j++) r_bit[i-1][j] = r_bit[i-1][j] ^ key_n[i-1][j] ;

        /*goto <S replace> and <P swap bit>*/
        s_replace(r_bit[i-1]) ;/*change 48bit r_bit[i-1]->32bit r_bit[i-1]*/
        for (j=0;j<=3;j++)/*get next r_bit*/
        {
            r_bit[i][j] = l_bit[i-1][j] ^ r_bit[i-1][j] ;/*f(R(n-1),k)*/
        }
    }/*end of endes*/

    for (i=0;i<=3;i++) e_bit[i] = r_bit[16][i] ;
    for (i=4;i<=7;i++) e_bit[i] = l_bit[16][i - 4] ;
    /*r_bit + l_bit -> e_bit(64bit)*/

    bit2byte(e_bit,e_byte) ;/*change to byte for swap bit IP-1*/
    for (i=0;i<=63;i++) e_byte1[i] = e_byte[ip_1[i] - 1] ;/*goto IP-1 swap bit*/
    
    //
    byte2bit(e_byte1,e_bit) ;/*got e_bit*/

}/*end of data encryption*/


/*--------------------------------------------------*/
/* data uncryption */
/*--------------------------------------------------*/
void Decryptor(unsigned char m_bit[8] , unsigned char k_bit[8] , unsigned char e_bit[8])
/*NOTE: in fact , m_bit is encryption data , e_bit is uncryption*/
{
    int ip[64] = { 58,50,42,34,26,18,10,2,
        60,52,44,36,28,20,12,4,
        62,54,46,38,30,22,14,6,
        64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9,1,
        59,51,43,35,27,19,11,3,
        61,53,45,37,29,21,13,5,
        63,55,47,39,31,23,15,7
    } ;

    int ip_1[64] = {40,8,48,16,56,24,64,32,
        39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30,
        37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28,
        35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26,
        33,1,41,9,49,17,57,25
    } ;

    int e[48] = { 32,1, 2, 3, 4, 5,
        4, 5, 6, 7, 8, 9,
        8, 9, 10,11,12,13,
        12,13,14,15,16,17,
        16,17,18,19,20,21,
        20,21,22,23,24,25,
        24,25,26,27,28,29,
        28,29,30,31,32,1
    } ;

    unsigned char m_bit1[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0} ;
    unsigned char m_byte[64] ;
    unsigned char m_byte1[64] ;
    unsigned char key_n[16][8] ;
    unsigned char l_bit[17][8] ;
    unsigned char r_bit[17][8] ;
    unsigned char e_byte[64] ;
    unsigned char e_byte1[64] ;
    unsigned char l_byte[64] ;
    unsigned char l_byte1[64] ;
    int i = 0 , j = 0 ;

    /*initialize*/
    for (i=0;i<=15;i++)
    {
        for (j=0;j<=7;j++)
        {
            l_bit[i][j] = 0x0 ;
            r_bit[i][j] = 0x0 ;
            key_n[i][j] = 0x0 ;
        }
    }

    for (i=0;i<=63;i++)
    {
        m_byte[i] = 0x0 ;
        m_byte1[i] = 0x0 ;
        l_byte[i] = 0x0 ;
        l_byte1[i] = 0x0 ;
        e_byte[i] = 0x0 ;
        e_byte1[i] = 0x0 ;
    }

    keychange(k_bit,key_n) ;/*get the 48bit key x 16 (16rows x 6byte in key_n)*/

    bit2byte(m_bit,m_byte) ;/*change to byte*/
    for (i=0;i<=63;i++) m_byte1[i] = m_byte[ip[i] - 1] ;/*goto IP swap bit*/
    byte2bit(m_byte1,m_bit1) ;/*re-change to bit*/
    for (i=0;i<=3;i++) r_bit[16][i] = m_bit1[i] ;/*move left 32bit -> r16*/
    for (i=4;i<=7;i++) l_bit[16][i - 4] = m_bit1[i] ;/*move right 32bit -> l16*/

    for (i=16;i>=1;i--) /*->(16) layer from 16 -> 1*/
    {
        for (j=0;j<=3;j++) r_bit[i-1][j] = l_bit[i][j] ;/*R(n-1) = L(n)*/

        /*comput f(L(n),k)*/
        bit2byte(l_bit[i],l_byte) ;
        for (j=0;j<=47;j++) l_byte1[j] = l_byte[e[j] - 1] ;/*goto E swap bit*/
        byte2bit(l_byte1,l_bit[i]) ;/*now r_bit is 48bit*/
        /*xor 48bit key*/
        for (j=0;j<=5;j++) l_bit[i][j] = l_bit[i][j] ^ key_n[i-1][j] ;
        /*goto <S replace> and <P swap bit>*/
        s_replace(l_bit[i]) ;/*change 48bit l_bit[i]->32bit l_bit[i]*/
        for (j=0;j<=3;j++)/*get PREV l_bit*/
        {
            l_bit[i-1][j] = r_bit[i][j] ^ l_bit[i][j] ;/*f(L(n),k)*/
        }
    }/*end of undes*/

    for (i=0;i<=3;i++) e_bit[i] = l_bit[0][i] ;
    for (i=4;i<=7;i++) e_bit[i] = r_bit[0][i - 4] ;
    /*r_bit + l_bit -> e_bit(64bit)*/

    bit2byte(e_bit,e_byte) ;/*change to byte for swap bit IP-1*/
    for (i=0;i<=63;i++) e_byte1[i] = e_byte[ip_1[i] - 1] ;/*goto IP-1 swap bit*/
    byte2bit(e_byte1,e_bit) ;/*got e_bit*/
    /*now ! in fact , the e_bit is the uncryption data.*/

}/*end of uncryption data*/

/**
 * 
 */
int _desHex(char c)
{
	int x=-1;
	
	switch ( c ) {
		case '0': x = 0; break;
		case '1': x = 1; break;
		case '2': x = 2; break;
		case '3': x = 3; break;
		case '4': x = 4; break;
		case '5': x = 5; break;
		case '6': x = 6; break;
		case '7': x = 7; break;
		case '8': x = 8; break;
		case '9': x = 9; break;
		case 'A': x = 10; break;
		case 'B': x = 11; break;
		case 'C': x = 12; break;
		case 'D': x = 13; break;
		case 'E': x = 14; break;
		case 'F': x = 15; break;
		default:
			x = -1;
	}		
	
	return x;						
}

/**
 *
 */
int _desGetKey(unsigned char k_bit[8])
{
	int _open = 0;
	
    FILE  *keyfp = fopen("key.key","r");
    
    if (keyfp == NULL)
    {
		const char* _keyfile = getenv("DES_KEY");
		keyfp = fopen(_keyfile, "r");		
    }
    
    if (keyfp == NULL)
    {
    	char buffer[256];
		snprintf(buffer, sizeof(buffer), "%s/key.key", getenv("HOME"));
		keyfp = fopen(buffer, "r");	
	}		

	if (keyfp == NULL) return -1;

	//!
	char c_array[10] = {0};
	memset(c_array, '\0', sizeof(c_array));
	
    fgets(c_array, 8, keyfp);
    c_array[8] = '\0';
    fclose(keyfp);

    for (int i=0; i<8; i++) {
    	k_bit[i] = c_array[i];
    }
    
    //
    return 0;
}
