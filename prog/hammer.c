 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "build.h"


Char buff[200];


void explain()

{ fprintf(stderr, "hammer {-a #} {-t #} {-i #} {-f #} {-F #} {-d #}\n"
									"     -a #   -- Area\n"
									"     -t #   -- Tide height\n"
									"			-i #   -- input rate\n"
									"			-f #   -- out flow rate per head\n"
									"			-F #   -- in flow rate per head\n"
									"			-p #	 -- Max pound\n"
									"			-d #	 -- days\n"
         );
  exit(0);
}




int main(Vint argc, Char **argv)

{ Int argsleft = argc - 1;
  Char ** argv_ = &argv[1];

  Set opts = 0;
  Bool u_opt = true;
  double tide_height = 4.6;
  double max_pound = 3.5;
  double input_rate = 150.0;
  double area = 1000000;
  double flow_in_head = input_rate / 3;
  double max_flow_out = input_rate * 9;
  int mins = 2 * 24 * 60;
	int lead_in = 3;
  
  for (; argsleft > 0 and argv_[0][0] == '-'; --argsleft)
  { Char * flgs;
    for (flgs = &argv_[0][1]; *flgs != 0; ++flgs)
      if      (*flgs == 'a')
      { ++argv_;
      	--argsleft;
      	area = atof(argv_[0]);
      }
      else if (*flgs == 'i')
      { ++argv_;
      	--argsleft;
      	tide_height = atof(argv_[0]);
      }
      else if (*flgs == 't')
      { ++argv_;
      	--argsleft;
      	input_rate = atof(argv_[0]) * 60;
      }
      else if (*flgs == 'f')
      { ++argv_;
      	--argsleft;
      	max_flow_out = atoi(argv_[0]);
      }
      else if (*flgs == 'F')
      { ++argv_;
      	--argsleft;
      	flow_in_head = atoi(argv_[0]);
      }
      else if (*flgs == 'p')
      { ++argv_;
      	--argsleft;
      	max_pound = atof(argv_[0]);
      }
      else if (*flgs == 'd')
      { ++argv_;
      	--argsleft;
      	mins = atoi(argv_[0]) * 24* 60;
      }
			else
      { explain();
      }
    ++argv_;
  }

	printf("Days %d\n", mins / (24 * 60));
	printf("Flow per head (in) %9.4f  (out) %9.4f\n\n", 
					flow_in_head, max_flow_out);

  input_rate *= 60;				// per minute
	flow_in_head *= 60;
	max_flow_out *= 60;
	max_pound -= input_rate / area;

	mins += lead_in * 24 * 60;

  printf("Pound        Tide    Head   Flow/sec       Power\n");

{ const double flood = 3.7;
  const double head_loss = 0.9;
  const double drawdown = 0.6;
  double pound = 0;
	double prev_pound = -0.25;
	double prev_tide = 0;
	double prev_flow = 0;
	double tot_energy = 0.0;
	double tot_back = 0;
	double tot_back_energy = 0;
	double period_out = 0;
	double period_energy = 0;
	double t = 0;
	const float pi = 3.147;
	int hours = 0;

  while (--mins > 0)
  { ++t;
	{ double progress = fmod(pi * 2 * t / (12.5 * 60), 2 * pi);
		double next_low = 1.5 * pi;
		double time_to_low = next_low - progress;
		double max_tgt = max_pound - (time_to_low < 0        ? drawdown :
																	time_to_low > 0.7 * pi ? drawdown : 0);
		double tide = sin(progress) * tide_height / 2.0;
		double rate = -sin(progress);
		int rising = tide > prev_tide;
		double head = pound - tide;
		double out = head < 1.5									  ? 0						 :  // per minute
								 rate > 0.75 								  ? rate * max_flow_out :
								 pound >= max_pound						? input_rate   :
								 pound >= max_tgt && rising   ? input_rate   : 0;

		double gain = input_rate / area + prev_tide - tide;

		prev_pound = pound;
		prev_tide = tide;
		prev_flow = out;

		if (t < lead_in * 24 * 60)
			continue;

  { double power = out * (head * head_loss) / 60 * 9.81;    // unit KW
    if (power < 0)
		{ power = -power;
			tot_back_energy += power / 60;
		} 
 		tot_energy += power / 60;						// unit KWh
    pound += (input_rate - out) / area;

		period_out += out;
		period_energy += power;							// unit KWh

    if (mins % 15 != 0)
    	continue;

    printf("%c %6.2f   %6.2f  %6.3f %9.4f   %12.4f    @ %7.4f\n", 
    			rising ? '-' : '+',
    			pound, tide, head,
					period_out / (60 * 15),
					period_energy / 15, rate);
    if (mins % 60 == 0)
    { ++ hours;
    	if (hours > 24)
    		hours -= 24;
    	printf("%2d:00\n", hours);
    }
		period_out = 0.0;
		period_energy = 0.0;
  }}}
	
	printf("\nTotal Gross MWh  %12.3f\n", tot_energy / 1000);
	printf("\nTotal Net   MWh  %12.3f\n", tot_energy / 1000 * 0.75);
	printf("\nTotal Back Energy  %13.3f\n", tot_back_energy);
	return 0;
}}
