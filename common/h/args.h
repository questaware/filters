extern Short   argc_;
extern Short   argix_;
extern char ** argv_;
extern FILE * argip_;



#define set_args(argc, argv, argip) \
{  argix_ = 0; \
   argc_ = argc; \
   argv_ = argv; \
   argip_ = argip; \
}

extern Char * next_arg(Bool wh);


