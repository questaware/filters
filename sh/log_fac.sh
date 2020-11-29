#!/bin/ksh
## File                         log_fac.sh                        ## has state
## Parameters:  filenamep:      Default name of log file
##              anti_repeatsp:  Default number of repeat messages to allow
##              srcp:		Default message source flag
## Remarks:
## This module maintains the log file.  The module has one instance in each process using it.
##
## History  09 Jun 1997  1.0       Initial version
##

#####   Module:   incident_logging(log_)
##	   Purpose:  To permanently record in a concise fashion the incidents occuring in a process.
##      
set -x
        log_filename=
integer log_anti_repeats
        log_src=

         log_last_code=
         log_last_text=
 integer log_last_ct=0
 integer log_last_time=0

#####   Module Initialisation:  log_

log_filename=${filenamep:-/tmp/log$$}
log_anti_repeats=${anti_repeatsp:-20}
log_src=${srcp:-ERR_}

log_set_log()
## Parameters:   p1:	File name
##
## Return Code:  CC_EACCES:   
## Result:       None
{ 
  ## Purpose: Sets the name of the log file.  The file is opened anew for each log entry.
  if [[ ! -w $1 ]] ; then
    local dir=dirname $1
    if [[ ( -d ${dir} ) && ( -w ${dir} ) ]] ; then
      :
    else
      return CC_EACCES
    fi
  fi
  log_filename=$1
}


log_set_anti_repeat()
## Parameters:   p1:	seconds
##
## Return Code:  None   
## Result:       None
{ 
  ## Purpose: Sets this attribute.
  log_anti_repeats=$1
}


log_set_src()
## Parameters:   p1:	four chars
##
## Return Code:  None   
## Result:       None
{ 
  ## Purpose: Sets the source identifier to the first four characters of source.
  log_src=$(echo $1 | sed -e 's/\(....\).*/\1)/')
}


log_msg()
## Parameters:   p1:	error code
## 		 p2:    error text
##
## Return Code:  None
## Result:       None
{ 
  ## Purpose: Output the error message unless the last was the same ...
  if [[ ( x${log_last_code} = x$1 ) && ( x${log_last_text} = x$2 ) ]] ; then
    ## we cannot easily manipulate times in ksh so we dont!!
     log_last_ct=${log_last_ct}+1
     
     echo '*'${log_last_ct} >> ${log_filename}
  else
     log_last_ct=1
     log_last_code=$1
     log_last_text=$2
     local tstr=$(date | sed -e 's/ BST / /;s/ GMT / /;s/[12][0-9]\([0-9][0-9]\)$/\1/')
     msg=$(echo $tstr " " ${log_src} " " ${log_last_code} " " ${log_last_text})
     echo ${msg} >> ${log_filename}
  fi
}

#####    End module: log_
							

