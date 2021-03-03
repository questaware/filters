#include <stdio.h>
#include <string.h>


// #define in_range(a, b, c) ((unsigned)((a)-(b))<=((c)-(b)) && ((c)-(b)) >= 0)

typedef int bool;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

bool EncodeBase64
				( const uint8_t* const input_data, 
				  const uint32_t input_len, 
				  uint8_t* output_data, 
				  uint32_t output_len 
				)
{ const uint8_t* s = input_data;
	int sz = input_len;
  uint8_t* t = output_data;
	int lim = output_len;
	int wh = 3;
	
	unsigned int val = 0;
	
	const char * trans = 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+1";
	
	for (s = input_data - 1; --sz >= 0;)
	{ val = (val << 8) | *++s;
		if (--wh <= 0)
		{ lim -= 4;
			if (lim < 0)
				break;

			wh = 3;
			t[3] = trans[val & 0x3f];
			val = val >> 6;
		  t[2] = trans[val & 0x3f];
			val = val >> 6;
		  t[1] = trans[val & 0x3f];
			val = val >> 6;
		  t[0] = trans[val & 0x3f];
		  t += 4;
		}
	}

{	int got = 3 - wh;			// got is 0, 1 or 2

	if (lim - (got == 0 ? 0 : 4) < 0)
	  return 0;
	
	val = val << (got * 6);

	switch (got)
	{ case 1:
			t[3] = '=';
			t[2] = '=';
			val = val >> 12;
		  t[1] = trans[val & 0x3f];
			val = val >> 6;
		  t[0] = trans[val & 0x3f];
			break;						
		case 2:
			t[3] = '=';
			val = val >> 6;
		  t[2] = trans[val & 0x3f];
			val = val >> 6;
		  t[1] = trans[val & 0x3f];
			val = val >> 6;
		  t[0] = trans[val & 0x3f];
			break;
	}
	return 1;
}}

int main()
{ char buf[1024];

	memset(buf, 0, sizeof(buf));
{ const char * t1 = "";
	bool rc = EncodeBase64(t1, strlen(t1), buf, sizeof(buf));
	
	printf(rc == 0 ? "No space\n" : "T1 %s : %s\n", t1, buf);
	
	memset(buf, 0, sizeof(buf));
  t1 = "M";
	rc = EncodeBase64(t1, strlen(t1), buf, sizeof(buf));

	printf(rc == 0 ? "No space\n" : "T2 %s : %s\n", t1, buf);

	memset(buf, 0, sizeof(buf));
  t1 = "M1a";
	rc = EncodeBase64(t1, strlen(t1), buf, sizeof(buf));
	
	printf(rc == 0 ? "No space\n" : "T3 %s : %s\n", t1, buf);

	memset(buf, 0, sizeof(buf));
  t1 = "M1a/";
	rc = EncodeBase64(t1, strlen(t1), buf, sizeof(buf));
	
	printf(rc == 0 ? "No space\n" : "T4 %s : %s\n", t1, buf);

	memset(buf, 0, sizeof(buf));
  t1 = "M1a/+";
	rc = EncodeBase64(t1, strlen(t1), buf, sizeof(buf));
	
	printf(rc == 0 ? "No space\n" : "T5 %s : %s\n", t1, buf);

	rc = EncodeBase64(t1, strlen(t1), buf, 7);
	
	printf(rc == 0 ? "No space YES\n" : "T5 No %s : %s\n", t1, buf);

	memset(buf, 0, sizeof(buf));
  t1 = "Man is distinguished,";
	rc = EncodeBase64(t1, strlen(t1), buf, sizeof(buf));
	
	printf(rc == 0 ? "No space\n" : "T6 %s : %s\n", t1, buf);
	


	printf("Done\n");
}}
