#include <u.h>
#include <libc.h>
#include <9pclient.h>
#include <auth.h>
#include <thread.h>
#include <bio.h>

typedef struct Request Request;

struct Request
{
	char *service;
	char *server;
	char *user;
	char *passwd;
};

Request*
parserequest()
{
	Request *req;
	Biobuf buf;
	char *line, *args[6];
	int i, n;

	req = malloc(sizeof(Request));
	if(!req)
		sysfatal("malloc");
	Binit(&buf, 0, OREAD);
	while((line = Brdstr(&buf, '\n', 1))){
		n = gettokens(line, args, 6, "=\n");
		for(i = 0; i < n; i = i+2){
			if(!strcmp(args[i], "protocol")){
				req->service = strdup(args[i+1]);
			}else if(!strcmp(args[i], "host")){
				req->server = strdup(args[i+1]);
			}else if(!strcmp(args[i], "username")){
				req->user = strdup(args[i+1]);
			}else if(!strcmp(args[i], "password")){
				req->passwd = strdup(args[i+1]);
			}
		}
		free(line);
	}
	Bterm(&buf);
	return req;
}

void
get(Request *r)
{
	UserPasswd *up;

	if(!r->service || !r->server)
		return;
	
	up = auth_getuserpasswd(auth_getkey, "proto=pass role=client service=%s server=%s", r->service, r->server);
	if(!up)
		return;

	print("username=%s\n", up->user);
	print("password=%s\n", up->passwd);
}

void
store(Request *r)
{
	CFid *fid;
	char *key;

	if(!r->service || !r->server || !r->user || !r->passwd)
		return;

	key = smprint("key proto=pass service=%s server=%s user=%s !password=%s\n",
					r->service, r->server, r->user, r->passwd);
	fid = nsopen("factotum", nil, "ctl", OWRITE);
	if(!fid)
		sysfatal("opening factotum/ctl: %r");
	fswrite(fid, key, strlen(key));
	fsclose(fid);
}

void
threadmain(int argc, char *argv[])
{
	Request *r;

	if(argc == 1)
		threadexits("args");
	
	r = parserequest();
	if(!strcmp(argv[1], "get"))
		get(r);
	else if(!strcmp(argv[1], "store"))
		store(r);
}
