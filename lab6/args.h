#pragma once
#ifndef _USER_FD_H_
#define _USER_FD_H_ 1

#include "types.h"
#include "fs.h"

#define _ARGSET(x) (x)=0
#define _ARGUSED(x) if(x){}else

#define ARGBEGIN    for((argv?0:(argv=(void*)&argc)),\
                argv++,argc--;\
                argv[0] && argv[0][0]=='-' && argv[0][1];\
                argc--, argv++) {\
                char *_args, *_argt;\
                char _argc;\
                _args = &argv[0][1];\
                if(_args[0]=='-' && _args[1]==0){\
                    argc--; argv++; break;\
                }\
                _argc = 0;\
                while(*_args && (_argc = *_args++))\
                switch(_argc)
#define ARGEND      _ARGSET(_argt);_ARGUSED(_argt);_ARGUSED(_argc);_ARGUSED(_args);}_ARGUSED(argv);_ARGUSED(argc);
#define ARGF()      (_argt=_args, _args="",\
                (*_argt? _argt: argv[1]? (argc--, *++argv): 0))
#define EARGF(x)    (_argt=_args, _args="",\
                (*_argt? _argt: argv[1]? (argc--, *++argv): ((x), abort(), (char*)0)))

#define ARGC()      _argc


// pre-declare for forward references
struct Fd;
struct Stat;
struct Dev;

struct Dev {
	int dev_id;
	char *dev_name;
	int(*dev_read)(struct Fd *, void *, u_int, u_int);
	int(*dev_write)(struct Fd *, const void *, u_int, u_int);
	int(*dev_close)(struct Fd *);
	int(*dev_stat)(struct Fd *, struct Stat *);
	int(*dev_seek)(struct Fd *, u_int);
};

struct Fd {
	u_int fd_dev_id;
	u_int fd_offset;
	u_int fd_omode;
};

struct Stat {
	char st_name[MAXNAMELEN];
	u_int st_size;
	u_int st_isdir;
	struct Dev *st_dev;
};

struct Filefd {
	struct Fd f_fd;
	u_int f_fileid;
	struct File f_file;
};

int fd_alloc(struct Fd **fd);
int fd_lookup(int fdnum, struct Fd **fd);
u_int fd2data(struct Fd *);
int fd2num(struct Fd *);
int dev_lookup(int dev_id, struct Dev **dev);
int
num2fd(int fd);
extern struct Dev devcons;
extern struct Dev devfile;
extern struct Dev devpipe;


#endif
