#include<iostream>
#include<string>
#include<cstdlib>

const unsigned long int con[16] = //constants c
{
	0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344,
	0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89,
	0x452821e6, 0x38d01377, 0xbe5466cf, 0x34e90c6c,
	0xc0ac29b7, 0xc97c50dd, 0x3f84d5b5, 0xb5470917
};

const int permutation[10][16] =
{
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
	{ 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 },
	{ 11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4 },
	{ 7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8 },
	{ 9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13 },
	{ 2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9 },
	{ 12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11 },
	{ 13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10 },
	{ 6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5 },
	{ 10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13 , 0 }
};

const unsigned long int s[4] = {}; //salt

unsigned long int rot(unsigned long int a, int n) //rotation of n bits towards less significant bits
{
	return ( (a << (32 - n)) | (a >> n) );
}

void loadMsg(unsigned long int m[], char *ptr, unsigned long long int len)
{
	char tempchar = 0;

	for (int i = 0; i < (long double)len / 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (i * 8 + j > len - 1)
				break;

			tempchar = ptr[i * 8 + j];
			m[i] += strtoul(&tempchar, NULL, 16);
			if (j != 7 && i * 8 + j <= len - 1) m[i] = m[i] << 4;
		}
	}
}

void initialization(unsigned long int v[], unsigned long int h[], unsigned long int t[])
{
	for (int i = 0; i <= 7; i++)
		v[i] = h[i];
	for (int i = 0; i <= 3; i++)
		v[8 + i] = s[i] ^ con[i];
	v[12] = t[0] ^ con[4];
	v[13] = t[0] ^ con[5];
	v[14] = t[1] ^ con[6];
	v[15] = t[1] ^ con[7];
}

void G(unsigned long int &a, unsigned long int &b, unsigned long int &c, unsigned long int &d, int r, int i, unsigned long int m[])
{
	a = ( a + b + (m[ permutation[r % 10][2 * i] ] ^ con[ permutation[r % 10][2 * i + 1] ]) ) % 4294967296; //mod 2^32
	d = rot( (d ^ a), 16 );
	c = (c + d) % 4294967296;
	b = rot( (b ^ c), 12 );
	a = (a + b + (m[permutation[r % 10][2 * i + 1]] ^ con[permutation[r % 10][2 * i]])) % 4294967296;
	d = rot((d ^ a), 8);
	c = (c + d) % 4294967296;
	b = rot((b ^ c), 7);
}

void rounds(unsigned long int v[], unsigned long int m[])
{
	for (int r = 0; r < 14; r++)
	{
		//columns
		G(v[0], v[4], v[8], v[12], r, 0, m);
		G(v[1], v[5], v[9], v[13], r, 1, m);
		G(v[2], v[6], v[10], v[14], r, 2, m);
		G(v[3], v[7], v[11], v[15], r, 3, m);
		//diagonals
		G(v[0], v[5], v[10], v[15], r, 4, m);
		G(v[1], v[6], v[11], v[12], r, 5, m);
		G(v[2], v[7], v[8], v[13], r, 6, m);
		G(v[3], v[4], v[9], v[14], r, 7, m);
	}
}

void finalization(unsigned long int v[], unsigned long int h[])
{
	for (int i = 0; i <= 7; i++)
		h[i] = h[i] ^ s[i%4] ^ v[i] ^ v[i + 8];
}

void showResult(unsigned long int h[])
{
	std::cout << std::hex << std::uppercase;

	//add zeros before number if it's too small
	for (int i = 0; i <= 7; i++)
	{
		if (h[i] >= 0x10000000)
			std::cout << h[i];
		else if (h[i] >= 0x1000000)
			std::cout << "0" << h[i];
		else if (h[i] >= 0x100000)
			std::cout << "00" << h[i];
		else if (h[i] >= 0x10000)
			std::cout << "000" << h[i];
		else if (h[i] >= 0x1000)
			std::cout << "0000" << h[i];
		else if (h[i] >= 0x100)
			std::cout << "00000" << h[i];
		else if (h[i] >= 0x10)
			std::cout << "000000" << h[i];
		else if (h[i] >= 0x1)
			std::cout << "0000000" << h[i];
		else
			std::cout << "00000000";
	}
}

int main(int argc, char *argv[])
{
	unsigned long int h[8] =
	{
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	};

	unsigned long int t[2] = {};
	unsigned long int v[16];
	unsigned long int m[16] = {};

	if (argc != 2)
	{
		std::cout << "ERROR! Enter only one argument." << std::endl;
		return 0;
	}

	char *ptr = argv[1];

	//check message length (number of chars)
	unsigned long long int len = 0;
	while (ptr[len] != '\0')
		len++;
	//

	if (len * 4 > 0xffffffff) //if message is longer than (2^32)-1 bits
	{
		std::cout << "ERROR! Message is too long." << std::endl;
		return 0;
	}

	if (len <= 111) //for messages that are not longer than 111 chars - one iteration
	{
		t[0] = len * 4; //set counter

		loadMsg(m, ptr, len);

		//padding
		m[len / 8] << 4;
		if (len < 111)
		{
			m[len / 8] += 8;
			m[len / 8] = m[len / 8] << ((len / 8 + 1) * 8 - len - 1) * 4;
			m[13] += 1;
		}
		else
			m[len / 8] += 9;
		//
	
		m[14] = t[1];
		m[15] = t[0];

		initialization(v, h, t);
		rounds(v, m);
		finalization(v, h);
		showResult(h);
	}

	if (len > 111) //for messages that longer than 111 chars
	{
		unsigned long rlen = len; //remaining length
		unsigned long t0prev; //previous counter
		int itr = 1; // iteration
		bool sflag = 0; //if length of message is multiple of 128 - special case

		//set counter
		if (len <= 128)
			t[0] = len * 4;
		else
			t[0] = 128 * 4;

		while (rlen > 0)
		{
			loadMsg(m, ptr, rlen<=128 ? rlen : 128);

			if (rlen <= 111)
			{
				//padding
				m[rlen / 8] << 4;
				if (rlen < 111)
				{
					m[rlen / 8] += 8;
					m[rlen / 8] = m[rlen / 8] << ((rlen / 8 + 1) * 8 - rlen - 1) * 4;
					m[13] += 1;
				}
				else
					m[rlen / 8] += 9;
				//
				
				m[14] = t[1];
				m[15] = len*4;

				//possible conter reset
				if (itr >= 2)
				{
					if (t0prev == t[0])
						t[0] = 0;
				}

				t0prev = t[0];

				initialization(v, h, t);
				rounds(v, m);
				finalization(v, h);
				showResult(h);
				return 0;
			}

			else if (rlen < 128)
			{
				//padding
				m[rlen / 8] << 4;
				m[rlen / 8] += 8;
				m[rlen / 8] = m[rlen / 8] << ((rlen / 8 + 1) * 8 - rlen - 1) * 4;

				for (int i = 1; rlen / 8 + i <= 15; i++)
					m[rlen / 8 + i] = 0;					
			}

			if (rlen == 128)
				sflag = 1;
			
			//possible counter reset
			if (itr >= 2)
			{
				if (t0prev == t[0])
					t[0] = 0;
			}

			t0prev = t[0];

			initialization(v, h, t);
			rounds(v, m);
			finalization(v, h);

			//reset m before next iteration
			for (int i = 0; i <= 15; i++)
				m[i] = 0;
			itr++;

			//change rlen to remaining chars number and move pointer ptr
			if (rlen >= 128)
				rlen -= 128;
			else
				rlen = 0;
			ptr += 128;

			//increase the counter
			if (rlen >= 128)
				t[0] += 128 * 4;
			else
				t[0] = len * 4;
		}
		
		//padding countinued
		if (sflag == 1)
			m[0] = 0x80000000;

		m[13] = 1;
		m[14] = t[1];
		m[15] = len*4;

		//possible counter reset
		if (itr >= 2)
		{
			if (t0prev == t[0])
				t[0] = 0;
		}

		initialization(v, h, t);
		rounds(v, m);
		finalization(v, h);
		showResult(h);
	}

	return 0;
}