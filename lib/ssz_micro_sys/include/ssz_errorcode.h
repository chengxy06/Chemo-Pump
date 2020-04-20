#pragma once

/************************************************
* Declaration
************************************************/

typedef enum {
	kError = -1,
	kSuccess = 0,
	kOperationNotPermitted = 1,//#define EPERM			 1  /* Operation not permitted */
	kFileNotFound = 2, //#define ENOENT         2  /* No such file or directory */
	//#define	ESRCH		 3	/* No such process */
	//#define	EINTR		 4	/* Interrupted system call */
	//#define	EIO 		 5	/* I/O error */
	kNotExist = 6, //#define	ENXIO		 6	/* No such device or address */
	//#define	E2BIG		 7	/* Argument list too long */
	//#define	ENOEXEC		 8	/* exec format error */
	//#define	EBADF		 9	/* Bad file number */
	//#define	ECHILD		10	/* No child processes */
	//#define	EAGAIN		11	/* Try again */
	//#define	ENOMEM		12	/* Out of memory */
	//#define	EACCES		13	/* Permission denied */
	//#define	EFAULT		14	/* Bad address */
	//#define	ENOTBLK		15	/* Block device required */
	kBusy=16, //#define	EBUSY		16	/* Device or resource busy */
	//#define	EEXIST		17	/* File exists */
	//#define	EXDEV		18	/* Cross-device link */
	//#define	ENODEV		19	/* No such device */
	//#define	E_NOTDIR	20	/* Not a directory */
	//#define	EISDIR		21	/* Is a directory */
	kInvalidParam = 22,	//#define EINVAL      22/*Invalid argument */
	//#define	ENFILE		23	/* File table overflow */
	//#define	EMFILE		24	/* Too many open files */
	//#define	ENOTTY		25	/* Not a typewriter */
	//#define	ETXTBSY		26	/* Text file busy */
	//#define	EFBIG		27	/* File too large */
	//#define	ENOSPC		28	/* No space left on device */
	//#define	ESPIPE		29	/* Illegal seek */
	//#define	EROFS		30	/* Read-only file system */
	//#define	EMLINK		31	/* Too many links */
	//#define	EPIPE		32	/* Broken pipe */
	//#define	EDOM		33	/* Math argument out of domain of func */
	//#define	ERANGE		34	/* Math result not representable */
	kTimeout = 35,
	kChecksumWrong= 36,	//
	kDataNotInit=37,	//e.g. the flash data is all 0xFF
	KNotOpen=38,
	kNotSupport=39,
	kUserCustomErrorStart = 64,
}ErrorCode;