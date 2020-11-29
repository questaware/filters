/*
	SPRINTF.C:	Reduced functionality, fast sprintf
*/
#define IN_ESPRINTF
#include "build.h"

#if S_MSDOS == 0 && 0
#include <standards.h>
#endif

#if S_MSDOS == 0
#include    <stdarg.h>
#endif

#include "h/eprintf.h"

#define SUPPORT_IO 1

#if SUPPORT_IO
# include <stdio.h>
#endif

#ifndef ALL_32BIT
#define ALL_32BIT (sizeof(int) == 4)
#endif


#define FMTCH '%'


Int spare_sprintf_arg;

static unsigned int the_radix;
static Vint mlwidth;
static unsigned long sign_bit;
static char pad_str[2] = " ";

				/* output integer as radix r */
char * mlputli(char * t, unsigned long l)
{
  static char hexdigits[] = "0123456789abcdef";

    if (l & sign_bit)
    { l = -l;
      *t++ = --mlwidth == 0 ? '*' : '-';
    }
    
{   register unsigned long q;
    if (sign_bit & 0x8000)
      q = (unsigned short)l / the_radix;
    else
      q = l / the_radix;
    
    if (--mlwidth == 0)
    { if (q == 0)
        mlwidth = -1;
    }
    else
    { if (q == 0)
      { while (--mlwidth >= 0)
          *t++ = pad_str[0];
      }
      else
        t = mlputli(t, q);
    }
    *t++ = mlwidth == 0 ? '!' : hexdigits[(int)(l-q*the_radix)];
    *t = 0;
    return t;
}}


				/* tgt == null => stdout */
#if STACK_GROWS_UP
#define ADJUST(p, size)	p -= size
#else
#define ADJUST(p, size)	p += size
#endif



#if S_MSDOS 
int eprintf(Char * tgt, const Char * __format, ...)
#define get_arg(type,ap) (*(type*)ap)
#else
 int eprintf( char * tgt, ... )
#define get_arg(type,ap) va_arg(ap, type)
#undef ADJUST
#define ADJUST(p, type)
#endif
{
#if S_MSDOS 
 register char * ap = ((char *)&__format);
 ADJUST(ap, sizeof(tgt));
#else
 Char * __format;
 va_list ap;
 va_start(ap, tgt);
 __format = va_arg(ap, char *);
#endif
{
          char buff[14];
 register int ch;
 	  char * fmt = __format;
 register char * t = tgt;
#if SUPPORT_IO == 0
 #define std_o false
 #define fputs(a,b)
#else
 Bool std_o = t == null or t == (char*)stdout
			or t == (char *)stderr;
 FILE * std_o_ = (FILE*)tgt;
 if (std_o_ == null)
   std_o_ = stdout;
#endif

  while ((buff[0] = *fmt++) != 0)
  { if (buff[0] == FMTCH)
    { pad_str[0] = ' ';
      the_radix = 10;
      sign_bit = ALL_32BIT ? 0x80000000 : 0x8000;
    { Vint stack_sz = ALL_32BIT ? sizeof(long) : sizeof(int);
      Vint mlwidth_ = 0;
      while (mlwidth_ >= 0 and (ch = *fmt++) != 0)
      { if (in_range(ch, '0', '9'))
        { if (mlwidth_ == 0 and ch == '0')
            pad_str[0] = '0';
	  mlwidth_ = mlwidth_ * 10 + ch - '0';
	}
	else
	switch (ch)
	{ char * s;
	  case 'c': 
	  case '#': buff[0] = ALL_32BIT ? get_arg(int, ap) : get_arg(char, ap);
	            ADJUST(ap, stack_sz);
		    if (ch == 'c')
		      goto fpb;

          when 'l': stack_sz = sizeof(long);
                    sign_bit = 0x80000000;
	  when 'u': sign_bit = 0;
          when 'x': the_radix = 18;
          case 'o': the_radix -= 2;
		    sign_bit = 0;
	  case 'i':
	  case 'd': if (std_o and SUPPORT_IO)
                    { while (mlwidth_ > sizeof(buff) - 1)
		      { fputs(pad_str, std_o_);
		        --mlwidth_;
		      }
		      t = &buff[0];
                    }
		    mlwidth = mlwidth_;
		    t = mlputli(t, ch == 'i'              ? spare_sprintf_arg     :
		                   stack_sz==sizeof(long) ? get_arg(long, ap)     :
		    	           sign_bit        	  ? (long)get_arg(int,ap) : (Nat2)get_arg(int,ap) );
  		    ADJUST(ap, stack_sz);

  		    mlwidth_ = -1;

  		    if (std_o)
  		      goto fpbuff;
          when 's':
#ifdef USE_T
	  case 't':
	  case 'b':
#endif
	            s  = get_arg(char*, ap);
#ifdef CHK_S
		    if (not prob_ptr(s))
		      s = "Not a pointer";
#endif
	            ADJUST(ap, sizeof(char *));
	 	    while (--mlwidth_ != 0)
  		    { buff[0] = *s;
  		      if      (buff[0] != 0)
  		        s++;
  		      else if (mlwidth_ >= 0)
  		        buff[0] = ' ';
  		      else
  		        break;
  		      if (std_o)
	              { buff[1] = 0;
  		        fputs(buff, std_o_);
  		      }
  		      else
  		        *t++ = buff[0];
  		    }
        }
      }
    }}
    else
    {
fpb:
      if (not std_o)
        *t++ = buff[0];
      else
      { buff[1] = 0;
fpbuff:
        fputs(buff, std_o_);
      }
    }
  }

  if (std_o)
    return 0;    
  *t = 0;
#if S_MSDOS 
#else
	va_end(ap);
#endif
  return t - tgt;
}}




#if SUPPORT_IO

#include <stdio.h>

 void fputserr(char * s)

 { fputs(s, stderr);
 }


 void fputsout(char * s)

 { fputs(s, stdout);
 }

 void msg_str(const char * msg, const char * str)

 { fputsout(msg);
   fputsout(str);
   fputsout("\n");
 }

#endif

