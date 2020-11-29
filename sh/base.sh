#!/bin/ksh
## File                         base.sh                        ## has state
## Parameters:  None
##              
## Remarks:   Basic Facilities
##
## History  09 Jun 1997  1.0     Initial Version
##

#####   Module:   Attributes save and restore (asr_)
##	   Purpose:  To allow save and restore of Shell flags/variables and stty attributes
##


asr_save()                                                       ## Pure
## Parameters:   None
##
## Return Code   None
## Result        A string giving the options
{ 
  echo $- | sed -e 's/i//'
  stty -g
  echo ${IFS}
}


asr_restore()
## Parameters:   p1	From save_set()
##
## Return Code   None
## Result        None
##
## To be invoked by IFS=# asr_restore "$xxx"
## where xxx is the result of asr_save
{ # echo R $1 R
  echo $1 | 
  ( read eny
#    echo .$eny.
    set +abCefhkmnpstuvx 
    [[ "X$eny" != "X  " ]] && set $eny
    read eny
#    echo ..$eny..
    stty $eny < /dev/tty
    read eny
#    echo ..$eny..
    IFS=$eny
  )    
}

#####    End module: asr_

alias const_integer='typeset -ir' # is required

#####   Module:   Condition_codes(CC_)
##	   Purpose:  To allow save and restore of Shell flags
##

const_integer CC_OK=0             	# Successful
const_integer CC_ESUCCESS=0          # Successful
const_integer CC_EPERM=1		# Not owner
const_integer CC_ENOENT=2		# No such file or directory
const_integer CC_ESRCH=3		# No such process
const_integer CC_EINTR=4		# Interrupted system call
const_integer CC_EIO=5		# I/O error
const_integer CC_ENXIO=6		# No such device or address
const_integer CC_E2BIG=7		# Arg list too long
const_integer CC_ENOEXEC=8		# Exec format error
const_integer CC_EBADF=9		# Bad file number
const_integer CC_ECHILD=10		# No children
const_integer CC_EDEADLK=11		# Operation would cause deadlock
const_integer CC_ENOMEM=12		# Not enough core
const_integer CC_EACCES=13		# Permission denied
const_integer CC_EFAULT=14		# Bad address
const_integer CC_ENOTBLK=15		# Block device required
const_integer CC_EBUSY=16		# Mount device busy
const_integer CC_EEXIST=17		# File exists
const_integer CC_EXDEV=18		# Cross-device link
const_integer CC_ENODEV=19		# No such device
const_integer CC_ENOTDIR=20		# Not a director
const_integer CC_EISDIR=21		# Is a directory
const_integer CC_EINVAL=22		# Invalid argument
const_integer CC_ENFILE=23		# File table overflow
const_integer CC_EMFILE=24		# Too many open files
const_integer CC_ENOTTY=25		# Not a typewriter
const_integer CC_ETXTBSY=26		# Text file busy
const_integer CC_EFBIG=27		# File too large
const_integer CC_ENOSPC=28		# No space left on device
const_integer CC_ESPIPE=29		# Illegal seek
const_integer CC_EROFS=30		# Read-only file system
const_integer CC_EMLINK=31		# Too many links
const_integer CC_EPIPE=32		# Broken pipe
const_integer CC_EDOM=33		# Argument too large
const_integer CC_ERANGE=34		# Result too large
const_integer CC_EWOULDBLOCK=35	# Operation would block
const_integer CC_EAGAIN=35		# ditto
const_integer CC_EINPROGRESS=36	# Operation now in progress
const_integer CC_EALREADY=37		# Operation already in progress
const_integer CC_ENOTSOCK=38		# Socket operation on non-socket
const_integer CC_EDESTADDRREQ=39	# Destination address required
const_integer CC_EMSGSIZE=40		# Message too long
const_integer CC_EPROTOTYPE=41	# Protocol wrong type for socket
const_integer CC_ENOPROTOOPT=42	# Protocol not available
const_integer CC_EPROTONOSUPPORT=43	# Protocol not supported
const_integer CC_ESOCKTNOSUPPORT=44	# Socket type not supported
const_integer CC_EOPNOTSUPP=45	# Operation not supported on socket
const_integer CC_EPFNOSUPPORT=46	# Protocol family not supported
const_integer CC_EAFNOSUPPORT=47	# Address family not supported by protocol family
const_integer CC_EADDRINUSE=48	# Address already in use
const_integer CC_EADDRNOTAVAIL=49	# Can't assign requested address
const_integer CC_ENETDOWN=50		# Network is down
const_integer CC_ENETUNREACH=51	# Network is unreachable
const_integer CC_ENETRESET=52	# Network dropped connection on reset
const_integer CC_ECONNABORTED=53	# Software caused connection abort
const_integer CC_ECONNRESET=54	# Connection reset by peer
const_integer CC_ENOBUFS=55		# No buffer space available
const_integer CC_EISCONN=56		# Socket is already connected
const_integer CC_ENOTCONN=57		# Socket is not connected
const_integer CC_ESHUTDOWN=58	# Can't send after socket shutdown
const_integer CC_ETOOMANYREFS=59	# Too many references: can't splice
const_integer CC_ETIMEDOUT=60	# Connection timed out
const_integer CC_ECONNREFUSED=61	# Connection refused
const_integer CC_ELOOP=62		# Too many levels of symbolic links
const_integer CC_ENAMETOOLONG=63	# File name too long
const_integer CC_EHOSTDOWN=64	# Host is down
const_integer CC_EHOSTUNREACH=65	# No route to host
const_integer CC_ENOTEMPTY=66	# Directory not empty
const_integer CC_EPROCLIM=67		# Too many processes
const_integer CC_EUSERS=68		# Too many users
const_integer CC_EDQUOT=69		# Disc quota exceeded
const_integer CC_ESTALE=70		# Stale NFS file handle
const_integer CC_EREMOTE=71		# Too many levels of remote in path
const_integer CC_EBADRPC=72		# RPC struct is bad
const_integer CC_ERPCMISMATCH=73	# RPC version wrong
const_integer CC_EPROGUNAVAIL=74	# RPC prog. not avail
const_integer CC_EPROGMISMATCH=75	# Program version wrong
const_integer CC_EPROCUNAVAIL=76	# Bad procedure for program
const_integer CC_ENOLCK=77		# No locks available
const_integer CC_ENOSYS=78		# Function not implemented
const_integer CC_EFTYPE=79		# Inappropriate file type or format
const_integer CC_ENOMSG=80		# No msg matches receive request
const_integer CC_EIDRM=81		# Msg queue id has been removed
const_integer CC_ENOSR=82		# Out of STREAMS resources
const_integer CC_ETIME=83		# System call timed out
const_integer CC_EBADMSG=84		# Next message has wrong type
const_integer CC_EPROTO=85		# STREAMS protocol error
const_integer CC_ENODATA=86		# No message on stream head read q
const_integer CC_ENOSTR=87		# fd not associated with a stream
const_integer CC_ECLONEME=88		# Tells open to clone the device
const_integer CC_EDIRTY=89		# Mounting a dirty fs w/o force
const_integer CC_EDUPPKG=90		# duplicate package name on install
const_integer CC_EVERSION=91		# version number mismatch
const_integer CC_ENOPKG=92		# unresolved package name
const_integer CC_ENOSYM=93		# unresolved symbol name
const_integer CC_ECANCELED=94	# operation canceled
const_integer CC_EFAIL=95		# cannot start operation
const_integer CC_EINPROG=97		# operation (now) in progress
const_integer CC_EMTIMERS=98		# too many timers
const_integer CC_ENOTSUP=99		# function not implemented
const_integer CC_EAIO=100		# internal AIO operation complete


#####    End module: CC_

