#include <stdio.h>
#include <stdlib.h>
#include <direct.h>                      
#include <string.h>
#include "/work/in/support/stand/prjstd.h" 			 /* changed to TARGET=PC */

#define MAXLINE 2000	/* 80 chars x 25 lines 								*/
#define MAXVAR	2000	/* 1 line 											*/
#define EMALLOC (-3)	/* NULL pointer returned by mallloc					*/
#define EOL 	(-2)	/* unexpected end of line, char isequal to '\0' 	*/ 
/*      EOF 	(-1)   	   end of file, already defined						*/
/*		FALSE     0		   false											*/
/*		TRUE      1        true                                             */
/*		OK		  1		   OK												*/
#define TOOLONG	  2		/* line too long, over MAXLINE						*/
#define NOVAR	  3		/* no variable is set in current line				*/
#define NOTSET	  4		/* the variable is not in the dictionary			*/
#define CONFLICT  5		/* the variable is already set						*/
#define NOTPATH	  6		/* the line does not define a path					*/
#define IN		  7		/* when you step into a !if block					*/
#define OUT		  8		/* when you step out of a !if block					*/

#define MAX_DIRENY_LEN 20                                 
                                 
/* -------------------------------------------------------------------------------------*/			   
/* TYPES                                                                                */					 
/* -------------------------------------------------------------------------------------*/
                
typedef struct dict_s { struct dict_s *next;
						char *code;
					   	char *desc;
					 } *Dict, Dict_t;
					 
typedef struct { Dict head;
				 Dict tail;
			   } *Dict_hdr, Dict_hdr_t;
			   
typedef struct dir_s { char name[MAX_DIRENY_LEN+1];
					   struct dir_s *parent;
					  } *Dir, Dir_t;

typedef struct list_s {	struct list_s *next;
						char *name;
						char *path;
						char *desc;
					  } *List, List_t;
					  
typedef struct { List head;
				 List tail;
			   } *List_hdr, List_hdr_t;                
                
List_hdr_t make; 
List_hdr_t load;
List_hdr_t obj;


Bool a_opt = false;
Bool dos_opt = false;
Bool h_opt = false;
Bool l_opt = false;
Bool m_opt = false;

/* -------------------------------------------------------------------------------------*/
/* FUNCTIONS                                                                            */
/* -------------------------------------------------------------------------------------*/
 
Cc read_next_line(FILE *stream, char *line)                       

/* the returned line is of one set and finishes by \0
 * links between consecutive lines, where there's a "\", are taken into account
 * returns 	OK when no problem occured
 *			TOOLONG when the line exeeds MAXLINE in length
 *			EOF when there's no more line to read off the stream
 */

{	int c;                                                        
	int i = 0;                                                    
	Cc  res = OK;
	Cc again= TRUE;  

	while (again==TRUE)
	{	i=0;
		line[i]='\0';
		while ( (((c=getc(stream))!='\n')||(line[(i>0?i-1:0)]=='\\'))
			  	&&((res=(i==MAXLINE)?TOOLONG:OK)!=TOOLONG)
			  	&&((res=(c==EOF)?EOF:OK)!=EOF) 
			  )	if (c!='\n') line[i++]=c;
			  	else i--;
		line[i]='\0'; 

		if 		(*line=='#' || *line=='.' ) 
		{	again=TRUE;
		}                
		else if (strstr(line,"echo")!=NULL)
		{	again=TRUE;
		}
		else again=FALSE;
	}
	return res;
}
/* -------------------------------------------------------------------------------------*/
void show_dict(Dict_hdr dh)

/* prints out the content of the dictionary
 */

{	Dict d;

	d=dh->head;
	printf("\fDICTIONARY\\n");
	printf("-------------------------------------------------\n"); 
	printf("CODES                DESC\n");
	printf("-------------------------------------------------\n");
	while (d!=NULL)
	{	printf("%-20s%s\n",d->code,d->desc);
		d=d->next;
	}             
}	
		
/* -------------------------------------------------------------------------------------*/
void free_dict(Dict_hdr dh)

/* deletes a dictionary and frees its occupied memory
 * then the header points to NULL
 */
{	Dict d=dh->head;
	Dict next=d->next;
 
	while (d!=NULL) 
	{	next=d->next;
		free(d);
		d=next;
	}          
	dh->head=NULL;
	dh->tail=NULL;
}
/* -------------------------------------------------------------------------------------*/
Cc check_for_conflict(Dict_hdr dh, char *newcode)

/* returns OK or CONFLICT if the variable is already set								
 */

{	Dict d=dh->head;

	while (d!=NULL)
		if (strcmp(d->code,newcode)==0) return CONFLICT;
	    else d=d->next;
	return OK;
}	
/* -------------------------------------------------------------------------------------*/
Cc add_to_dict(Dict_hdr dh, char *c, char *d)

/* adds a new cell to the dictionary list with code=c and desc=d
 * changes header's tail to the new cell.
 * returns	OK or EMALLOC
 */

{   if (check_for_conflict(dh,c)==CONFLICT)	return CONFLICT;
	
{ 	Dict dict=malloc(sizeof(Dict_t));
    if (dict != null)
 	{ dict->next=null;
 	  dict->code=malloc(strlen(c)+1);
  	  dict->desc=malloc(strlen(d)+1);   
 	  if (dict->code != null && dict->desc != null)
 	  {	strcpy(dict->code,c);
		strcpy(dict->desc,d);
		if (dh->head==NULL) dh->head=dict;
		else dh->tail->next=dict;
		dh->tail=dict;
    	return OK;
      }
    }
    return ENOSPACE;
}}
/* -------------------------------------------------------------------------------------*/
Cc set_var(char *line, Dict_hdr dh)

/* extracts *code and *desc from *line when *line sets a new variable : [#]CODE=DESC
 * such lines are recognized by a '=' sign. therefore, lines like "!if VAR1==VAR2" must
 * not be sent to this procedure and be processed elsewhere.
 * adds new variable to the dictionary
 * returns	OK
 *			NOVAR if the line sets no variable
 */

{	Cc res = OK;
	char c[MAXVAR],d[MAXVAR];
	char *p;
	int i,j,pos;
	    
	if ((p=strchr(line,'='))!=NULL)
	{	pos=p-line;  
   	 	i=0;									  			/* code <- var name */
		j=0;
		while ((line[i]!=32)&&(i<pos)) c[j++]=line[i++];
		c[j]='\0';
		i=pos;												 /* desc <- value 	*/
		j=0;
		while (line[++i]==32);
		while (line[i]!='\0') d[j++]=line[i++];
		d[j]='\0'; 
		res=add_to_dict(dh,c,d);
	}
   	else res=NOVAR;
    return res;
}
/* -------------------------------------------------------------------------------------*/
Cc code_to_desc(Dict_hdr dh, char *var)

/* scan the dictionary for var and replaces var by its description/value
 * returns either OK or NOTSET when the variable can't be found
 */

{	Dict d; 
	char code[MAXVAR];
                      
	strcpy(code,var);                      
	d=dh->head;
	while (strcmp(d->code,var)!=0)
		if((d=d->next)==NULL) 
		{	*var='\0';
			/*strcat(var,"$(NOTSET)"); */
			printf("%s not defined\n",code);
			return NOTSET;  
		}
	strcpy(var,d->desc);
	return OK;
}
/* -------------------------------------------------------------------------------------*/
Cc expand_var(char *line, Dict_hdr dh)

/* takes *line and expands the variables using the dictionary defined by hd
 * returns	OK
 * 			EOL when line finishes in the middle of a variable i.g. "$(VARIAB\n"
 *			NOTSET var is not in dictionary
 */

{ 	Cc res = OK;
	char newline[MAXLINE];
	char var[MAXVAR];
    int i,j,k;
    
    j=0;
    for (i=0;line[i]!='\0';i++) 
       	if ((line[i]=='$')&&(line[i+1]=='('))
    	{	k=0; 
    		i++;
    		while (line[++i]!=')')
    			if((var[k++]=line[i])=='\0') res=EOL;
    		var[k]='\0';
    		res=code_to_desc(dh,var);
    		for (k=0;var[k]!='\0';k++) newline[j++]=var[k];
    	}
    	else newline[j++]=line[i];
    newline[j]='\0';       
	strcpy(line,newline);
    return res;
} 
/* -------------------------------------------------------------------------------------*/			   
void show_dir(Dir d);
/* -------------------------------------------------------------------------------------*/			   
void free_dir(Dir d)

{ 	Dir p=d->parent;
	while (d!=NULL)
	{	p=d->parent;
		free(d);
		d=p;
	}
}
/* -------------------------------------------------------------------------------------*/			   
Dir get_dir(char *path)

/* split the path string and builds the direcotry tree
 * string should be of the form root/dir1/dir2/.../dirn
 * returns NULL if string is empty
 */

{	int i,j;
	char name[50];
	Dir p,d;

	p=NULL;
	name[j=0]='\0';      
	for (i=0;;i++) 
    {   if (path[i]=='/' || path[i]=='\\' || path[i]=='\0')
		{	name[j]='\0';
			if (j>0)
			{	if((d=malloc(sizeof(Dir_t)))==NULL) 
				{	printf("EMALLOC get_dir \n");
					return NULL;
				}
				strcpy(d->name,name);
				d->parent=p;
				p=d; 
				j=0;
				name[j]='\0';
			}
		}
		else 
		{	name[j++]=path[i]; 
		    if (j >= MAX_DIRENY_LEN)
		      j = MAX_DIRENY_LEN;
		}
		if (path[i] == '\0')
		  break;
    }
	return d;
}
/* -------------------------------------------------------------------------------------*/			   
Cc get_path(Dir d, char *path)

/* build a path string from the passed directory
 */

{	char p[MAXLINE];
	*path='\0';
	while (d!=NULL)
	{	strcpy(p,path);
		strcpy(path,d->name);
		strcat(path,"/");
		strcat(path,p);
		d=d->parent;
	}   
    return OK;
}
/* -------------------------------------------------------------------------------------*/			   
void format_line(char *line, char c)

{	int i=0;
	while (line[i]!='\0')
	{	if (line[i]=='/' || line[i]=='\\') line[i]=c;
		i++;
	} 
	strlwr(line);
}      
/* -------------------------------------------------------------------------------------*/
void relative_path(char *path)

/* from an absolut path string and the current dir, returns a relative path string
 * c:\dir1\dir2\dir3\dir4 is processed as ..\dir3\dir4 
 */
                              
{	char currp[MAXLINE];
	char oldp[MAXLINE];
	int i=1;
	int j=0;
	int k=0;
	Cc res;
	strcpy(oldp,path);
	_getcwd(currp,MAXLINE);
	strcat(currp,"/");
	format_line(currp,'/');
	while ((res=strncmp(currp,oldp,i))==0) 
	{	if (oldp[i-1]=='/') j=i;
		i++;
	}                 
	while (currp[i]!='\0') if (currp[i++]=='/') k++;
	*path='\0';
	for (i=0;i<k;i++) strcat(path,"../");
	strcat(path,oldp+j);
}
/* -------------------------------------------------------------------------------------*/
void reduce_path(char *line)

/* from a line of characters, reduces paths
 * 			from 	c:/dir1/dir2/../dir3/dir4
 *			to	 	c:/dir1/dir3/dir4/ 
 */

{	int i=0;
	int j=0;
	int p=0;
	while (line[i]!='\0')
		if ((line[i]=='/')||(line[i]=='\\'))                          
		{	if ((line[i+1]=='.')&&(line[i+2]=='.')&&(line[i+3]!='.'))
			{	j=i;       
				i+=3;
				while ((line[--j]!='/')&&(line[j]!='\\')) if (j==0) return;
				p=j;
				while (line[i]!='\0') line[j++]=line[i++];
				line[j]='\0';
				i=p;
			}
			else if ((line[i+1]=='.')&&(line[i+2]!='.'))
			{	j=i-1;
				i+=2;
				p=j;
				while (line[i]!='\0') line[j++]=line[i++]; 
				line[j]='\0';
				i=j;
			}
			else i++;
		}
		else i++;
	line[i]='\0';
}
/* -------------------------------------------------------------------------------------*/			   
Cc expand_dir(char *line, Dir current)

/* given the current directory and the path architecture that comes with it, the
 * returned line sees the paths expanded. the sets of char ..\ ../ .\ ./ are replaced
 * returns OK
 */

{	int i,j,k;
	Cc chgdir = FALSE;  
	Cc res = OK;
	char newline[MAXLINE], path[MAXLINE];
	Dir cdir;
	
	i=0;   
	j=0;           
	k=0;
	*path='\0';
   	*newline='\0';  
   	cdir=current;
   	reduce_path(line);
	while((line[i]!='\0')||(chgdir==TRUE))
	{	if  (line[i]=='.')
		{	if 		((line[i+1]=='.')&&((line[i+2]=='/')||(line[i+2]=='\\')))
			{	cdir=cdir->parent;
				chgdir=TRUE;
				i+=3;
			}	
			else if ((line[i+1]=='/')||(line[i+1]=='\\'))
			{	chgdir=TRUE;
				i+=2;
			}
			else 
				newline[j++]=line[i++];
		}
		else if (chgdir==TRUE)
		{	get_path(cdir,path);
		    while (path[k]!='\0') newline[j++]=path[k++];
		    chgdir=FALSE;
			cdir=current; 	
			*path='\0';	
			k=0;
		}
		else 
			newline[j++]=line[i++];
	}
	newline[j]='\0';
	strcpy(line,newline); 
	return res;
}
/* -------------------------------------------------------------------------------------*/
Cc get_word(char *line, char *word)

/* returns the first word of the line and removes it from the begining of the line 
 * word separator is 'space'
 * returns 	OK
 * 			EOL if line is null
 */

{	int i=-1;
	int j=0;
	*word='\0';             
	while (line[++i]==' '||line[i]=='\t');
	while ((line[i]!=' ')&&(line[i]!='\0'))	word[j++]=line[i++];
	if   (j==0) return EOL;
	else word[j]='\0';
	while ((line[i]==' ')&&(line[i]!='\0')) i++;
	j=0;
	while (line[i]!='\0') line[j++]=line[i++]; 
	line[j]='\0';
	return OK;
}
/* -------------------------------------------------------------------------------------*/			   
Cc get_file_dir(char *line, char *filename, char *path)

/* from a given line, returns a filename and its path
 * returns 	OK
 */

{	if (*line=='\\' || *line=='/')
	{	char temp[MAXLINE];
		_getcwd(temp,MAXLINE);
		temp[2]='\0';
		strcpy(path,temp);
	}
	else *path='\0';
	*filename='\0';
	
{	char *p=(strrchr(line,'/')==NULL?strrchr(line,'\\'):strrchr(line,'/'));
	if (p==NULL) strcpy(filename,line);
	else     
	{
		strcpy(filename,p+1);
		*p='\0';
		strcat(path,line);
		*p='/';
	}
{	int len=strlen(filename);
	if ((len>0)&&(len<9))	return OK;
	else return NOTPATH;
}}}
/* -------------------------------------------------------------------------------------*/			   
void init_list(List_hdr lh)

/* initialise a list header
 */

{	lh->head=NULL;
	lh->tail=NULL;   
}
/* -------------------------------------------------------------------------------------*/			   
Cc add_to_list(List_hdr lh, char *n, char *p, char *d)

{	Cc res = OK;
	List list=malloc(sizeof(List_t));
	if (list != null)
	{ list->name=malloc(strlen(n)+1);
	  list->path=malloc(strlen(p)+1);
	  list->desc=malloc(strlen(d)+1);
	
	  if (list->name != null && list->path!=null) 
	  {	list->next=NULL;
		strcpy(list->name,n);
		strcpy(list->path,p);
		strcpy(list->desc,d);
		if (lh->head==NULL)	lh->head=list;
		else lh->tail->next=list;
		lh->tail=list;
	    return OK;
	  }
	}              
	printf("run out of space");
	return EMALLOC;		
}
/* -------------------------------------------------------------------------------------*/
Cc cmppath(char *p1, char *p2)

/* compare 2 path strings independently from '/' or '\', and from case
 */

{	Cc res = TRUE;
	int i=0;          
	
	strlwr(p1);
	strlwr(p2);
	while ((p1[i]!='\0')&&(p2[i]!='\0'))
	{	if (p1[i]!=p2[i])
			if (((p1[i]!='/')&&(p1[i]!='\\'))||((p2[i]!='/')&&(p2[i]!='\\'))) 
				res=FALSE;
		i++;
	}       
	if ((p1[i]!='\0')||(p2[i]!='\0')) res=FALSE;
	return res;
}
/* -------------------------------------------------------------------------------------*/			   
Cc isthere(List_hdr lh, char *name, char *path)
{	Cc res = FALSE;
	List list=lh->head;
	while ((list!=NULL)&&(res==FALSE))
	{	if (stricmp(list->name,name)==0)
			if (path=='\0') res=TRUE;
			else res=cmppath(list->path,path);
		list=list->next;
	}
	return res;
}
/* -------------------------------------------------------------------------------------*/			   
void format(char *name)
{	if 			(strcmp(name+strlen(name)-5,".obj+")==0) 
		*(name+strlen(name)-1)=':';
	else if 	(strcmp(name+strlen(name)-4,".lib")==0) 
		/* leave it */ ;
	else 	
		strcat(name,".obj:");
}
/* -------------------------------------------------------------------------------------*/			   
void update_list(char *line)
{	char word[MAXLINE]; 
	char path[MAXLINE];
	char name[MAXLINE];
                  
	if(get_word(line,word)==OK)
	{	if (strcmp(word,"load")==0)
		{	get_word(line,word);						/* if path="" there's no need to*/ 
			get_file_dir(word,name,path);				/* append the make list since   */
			if (*path=='\0') _getcwd(path,MAXLINE);		/* we know that we're working on*/
			else										/* it 							*/
				if (isthere(&make,"makefile",path)==FALSE)
					add_to_list(&make,"makefile",path,"");
			format(name);
			if (isthere(&load,name,path)==FALSE)
				add_to_list(&load,name,path,"");
		}
		else if (  _stricmp(word+strlen(word)-5,".obj:")==0
				|| _stricmp(word+strlen(word)-6,".obj :")==0)
		{	if(isthere(&obj,word,"")==FALSE) 
			{	_getcwd(path,MAXLINE);
				strcpy(name,word);
				add_to_list(&obj,name,path,line);
			}
		}                                                                
	}
}
/* -------------------------------------------------------------------------------------*/			   
void format_list(List_hdr lh, char c)

{	List list=lh->head;
	while (list!=NULL)
	{	format_line(list->path,c);
		format_line(list->desc,c);
		strlwr(list->name);
		list=list->next;
	} 
}
/* -------------------------------------------------------------------------------------*/			   
void show_list(List_hdr lh,char *title) 
 
/* format the path to dir/dir/dir.... and convert to lower case
 */
 
{	List list=lh->head;
	if (h_opt=false) 
		printf("\f");
	else
	{	printf("\f-------------------------------------------------------------\n"); 
		printf("%s\n",title);
		printf("-------------------------------------------------------------\n"); 
		printf("Names                Paths\n");
		printf("-------------------------------------------------------------\n");
	}
	while (list!=NULL)
	{	printf("%-15s | %s\n",list->name,list->path);
		list=list->next;
	} 
}
/* -------------------------------------------------------------------------------------*/			   
Cc evaluate_exp(char *line)

/* string has to be of the type : "string1" (op) "string2" where op: == != > <
 * returns 	TRUE
 * 			FALSE
 *			NOTSET if string is of the wrong type
 */
 
{	Cc res=OK;
	char str1[MAXVAR];
	char str2[MAXVAR];
	char op[MAXVAR];

	if((res=get_word(line,str1))!=OK) res=NOTSET;
	if((res=get_word(line,op))!=OK) res=NOTSET;
	if((res=get_word(line,str2))!=OK) res=NOTSET;

	if (res==OK)
		if 		(strcmp(op,"==")==0) res=(strcmp(str1,str2)==0?TRUE:FALSE);
		else if (strcmp(op,"!=")==0) res=(strcmp(str1,str2)==0?FALSE:TRUE);
		else if (strcmp(op,">") ==0) res=(strcmp(str1,str2)>0 ?TRUE:FALSE);
		else if (strcmp(op,"<") ==0) res=(strcmp(str1,str2)<0 ?TRUE:FALSE);
		else res=NOTSET;

	return res;
}
/* -------------------------------------------------------------------------------------*/
Cc skip(FILE *stream, Cc exp)

/* skip lines of a file until it reaches a statement !else or !endif according to exp
 * returns 	OK
 * 			EOF if no statement found
 */

{	Cc res = OK;
	char line[MAXLINE];
	char word[MAXLINE];
	
	while ((res=read_next_line(stream,line))==OK)
		if (get_word(line,word)==OK)
		{	if ((exp==TRUE) &&(strcmp(word,"!endif")==0)) break;
			if ((exp==FALSE)&&(strcmp(word,"!else" )==0)) break;
			if (strcmp(word,"!if" )==0)	skip(stream,TRUE);
		} 
	return res;
}
/* -------------------------------------------------------------------------------------*/			   
Cc scan_file(char *path, Dict_hdr hd);
/* -------------------------------------------------------------------------------------*/			   
Cc step_in(FILE *stream, Dict_hdr dh, Dir dir, Cc exp)

/* expand variables and directories
 * look for !include files and scan them
 * takes care of !if, !else, !endif
 * set variables
 * send selected lines to ouptut
 * returns EOF
 */   
 
{   Cc res=OK;
	static char line[MAXLINE];
	static char word[MAXLINE];

   	while (read_next_line(stream,line)==OK)
	{	expand_var(line,dh);
		expand_dir(line,dir);               
		if (*line=='!') 
		{	if ((res=get_word(line+1,word))==OK)
			{	if 		(strcmp(word,"include")==0) 
	    		{	if (get_word(line+1,word)==OK) scan_file(word,dh);
				}
				else if (strcmp(word,"if")==0) 	
				{	exp=evaluate_exp(line+1);
					if (exp==TRUE) res=step_in(stream,dh,dir,exp);
					else if (exp==FALSE) res=skip(stream,exp);
				}
				else if (strcmp(word,"else")==0)
				{	if  (exp==FALSE) res=step_in(stream,dh,dir,exp);
					else if (exp==TRUE)  res=skip(stream,exp);
				}
				else if	(strcmp(word,"endif")==0)
				{	return OUT; 
				}
			}
		}
		else
		{	set_var(line,dh);
			update_list(line); 
		}
	}
	return res;	
}
/* -------------------------------------------------------------------------------------*/			   
Cc scan_file(char *linepath, Dict_hdr dh)

/* 	gest dir and filename from path
 * 	scans the file using step in
 * 	sets the lists
 *
 */  

{	Cc res = OK;
	char cpath[MAXLINE];
	char filename[MAXLINE];
	char path[MAXLINE];
	FILE *stream=fopen(linepath,"r");
	if (stream == null)
	  printf("Cannot find file %s\n", linepath);
	else
	{ _getcwd(cpath,MAXLINE);
	  get_file_dir(linepath,filename,path);
	  _chdir(path);
	
    { Dir dir=get_dir(path);
      if (dir != null)	
	  { step_in(stream,dh,dir,TRUE);
	    _chdir(cpath);                                     
	
	    free_dir(dir);
	  }
	  fclose(stream);
	}}
	return res;
}                 
/* -------------------------------------------------------------------------------------*/			   
void scan_all(Dict_hdr dh)

/* look through the makefile list and scan them all. 
 * everytime the 3 list are updated and if new makefiles are found they're added to
 * the list so that they're scan too.
 */
 
{	List file=make.head;
	while (file!=NULL)
	{	char *name=file->name;
		char *path=file->path;
		char filepath[MAXLINE];
		*filepath='\0';
		strcat(filepath,path);
		strcat(filepath,"/");
		strcat(filepath,name);
		scan_file(filepath,dh);
		free_dict(dh);
		file=file->next;
	}
}
/* -------------------------------------------------------------------------------------*/			   
void get_source_line(char *nmatch, char *pmatch, char *line)

{	List list=obj.head;
	*line='\0';
	while (list!=NULL) 
	{	if (strcmp(nmatch,list->name)==0 && strcmp(list->path,pmatch)==0) 
		{	strcat(line,list->desc);
			list=NULL;			
		}
		else list=list->next;
	}
	if (*line=='\0')
	{	strcat(line,".NOTFOUND:");
		strcat(line,nmatch);
	}
}
/* -------------------------------------------------------------------------------------*/			   
void append_source(List_hdr sh)

{	List loaded=load.head;     
	char line[MAXLINE];     
	char word[MAXLINE];
	while (loaded!=NULL)   
	{	get_source_line(loaded->name,loaded->path,line);
		while (get_word(line,word)==OK)
			add_to_list(sh,word,"","");
		loaded=loaded->next;
	}
}
/* -------------------------------------------------------------------------------------*/			   
void explain()

{ fprintf(stderr,
	  "smake {-a} {-dos} {-h} {-l} {-m} makefile {var=value}\n"
	  "   -a       -- Output absolute file names\n"
	  "   -dos     -- Output backslash path separators\n"
	  "   -h       -- Inhibit report headers\n"
	  "   -l       -- Produce the loaded list\n"
	  "   -m       -- Produce the makefile list\n"
	  "    Version 1.01\n");
  exit(0);
}
/* -------------------------------------------------------------------------------------*/			   
/* MAIN                                                                                 */					 
/* -------------------------------------------------------------------------------------*/

void main(argc, argv)
        Short   argc;
        Char ** argv;

{ 	if (argc <= 1)
		explain();

/*	sets the option variables	*/
{	fast Vint argix;
	for (argix = 0; ++argix < argc; )
	{ 	Char * s = argv[argix];
		Char ch;
    	if (*s != '-')
    	{ 
    		break;
    	}
    	while ((ch = *++s) != 0)
    	{	switch (ch)
    	  	{	when 'a':  a_opt = true;
    	    	when 'd':  if (s[1] == 'o' and s[2] == 's')
    	    	           { s += 2;
    	    	             dos_opt = true;
    	    	           }
    	    	when 'h':  h_opt = true;
       		 	when 'l':  l_opt = true;
       		 	when 'm':  m_opt = true;
      		}
    	}
  	}

/*	get the filepath */
{	Char * fn = argv[argix];
 	if (fn == null || strchr(fn,'=') != NULL)
    	explain();                     

/* 	Initialize the lists */                    
{	char name[MAXLINE];
	char path1[MAXLINE];
	char path2[MAXLINE];
	get_file_dir(fn,name,path1);
	if 		(*path1=='\0') 
		_getcwd(path1,MAXLINE);
	else if (path1[1]!=':')
	{	_getcwd(path2,MAXLINE);
		strcat(path2,"/");
		strcat(path2,path1);
		reduce_path(path2);
		strcpy(path1,path2);
	}
	init_list(&make);               
	add_to_list(&make,name,path1,"");
	init_list(&load);
	init_list(&obj);
    
/*	Initialize the dictionary */
{	Dict_hdr dh=malloc(sizeof(Dict_hdr_t));
	dh->head=NULL;
	dh->tail=NULL;
  	while (++argix < argc)
	{	char *line = argv[argix];
		if (strchr(line,'=') == NULL)
			explain();
		set_var(line,dh);
	}

/*	Call the scanning procedures */
	scan_all(dh);            

/*	format the list before continuing */	    
    format_list(&make,'/');
    format_list(&load,'/');
    format_list(&obj,'/');

/*	extract the sources */
{	List_hdr source;
	init_list(source);
	append_source(source);
	
/* 	print reports - absolute directories*/    
	if (m_opt==true) show_list(&make,"MAKEFILE LIST");
	if (l_opt==true) show_list(&load,"LOADED OBJECT LIST");
	
/*	send output */
{	List out=source->head;
	FILE *cfile=fopen("output.c","w");
	FILE *asmfile=fopen("output.asm","w"); 
	if (h_opt=false)
		printf("\f");
	else
	{	printf("\f------------------------------------------------------------\n"); 
		printf("ALL SOURCE FILE LIST\n");
		printf("-------------------------------------------------------------\n"); 
		printf("Names                Paths\n");
		printf("-------------------------------------------------------------\n");
	}
	while (out!=NULL)     
	{	char name[MAXLINE];
		strcpy(name,out->name);
		if (a_opt==false) 
		{	_getcwd(path1,MAXLINE);
			relative_path(name);
		}
		if (dos_opt==true) 
		{	format_line(name,'\\');
		}
		if (strcmp(name+strlen(name)-2,".c")==0) fprintf(cfile,"%s\n",name);
		if (strcmp(name+strlen(name)-4,".asm")==0) fprintf(asmfile,"%s\n",name);
		printf("%s\n",name);
		out=out->next;
	}

	fcloseall();

}}}}}}}
   
