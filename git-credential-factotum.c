#include <u.h>
#include <libc.h>
#include <9pclient.h>
#include <auth.h>
#include <thread.h>
#include <bio.h>

char *svc;
char *srv;

void
parserequest()
{
	Biobuf buf;
	char *line, *args[6];
	int i, n;

	Binit(&buf, 0, OREAD);
	for(;;){
		line = Brdline(&buf, '\n');
		if(!line)
			break;
		n = gettokens(line, args, 6, "=\n");
		for(i = 0; i < n; i = i+2){
			if(!strcmp(args[i], "protocol")){
				svc = args[i+1];
			}else if(!strcmp(args[i], "host")){
				srv = args[i+1];
			}
		}
		if(svc == nil || srv == nil)
			threadexits(nil);
	}
	Bterm(&buf);
}

void
threadmain(int argc, char *argv[])
{
	UserPasswd *up;

	if(argc == 1)
		threadexits("args");
	if(strcmp(argv[1], "get") != 0)
		threadexits(nil);
	svc = nil;
	srv = nil;
	parserequest();	
	up = auth_getuserpasswd(auth_getkey, "proto=pass role=client service=%s server=%s", svc, srv);
	if(up == nil){
		sysfatal("getuserpasswd: %r");
	}else{
		print("username=%s\n", up->user);
		print("password=%s\n", up->passwd);
	}
}
