/**********************************************************
**   PARSER_C.FRM
**   Coco/R C Support Frames.
**   Author: Frankie Arzu <farzu@uvg.edu.gt>
**
**   Jun 12, 1996   Version 1.06
**      Many fixes and suggestions thanks to
**      Pat Terry <p.terry@ru.ac.za>
**********************************************************/

#include "proxycs.h"
#include "proxycp.h"

#include "conf.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* This is a simple expression calculator */

/*----------------- Scanner Specifications ----------------------*/



Error_Func Custom_Error = 0L;

static int Sym;
static int errors = 0;                /*number of detected errors*/
static int ErrDist = MinErrDist;

#define MAXSYM		1


/* Production prototypes */

static void ProxyConf(void);
static void Address(void);


#define NSETBITS        16

static unsigned short int SymSet[][MAXSYM] = {
  /*EOF_Sym */
  {0x1},
  {0x0}
};

void GenError(int errno)
{ if (ErrDist >= MinErrDist) {
    if (Custom_Error != 0L)
      (*Custom_Error) (errno, S_NextLine, S_NextCol, S_NextPos);
    errors++;
  }
  ErrDist = 0;
}

void SynError(int errno)
{ if (errno <= MAXERROR) errno = MAXERROR;
  if (ErrDist >= MinErrDist) {
    if (Custom_Error != 0L)
      (*Custom_Error) (errno, S_NextLine, S_NextCol, S_NextPos);
    errors++;
  }
  ErrDist = 0;
}

void SemError(int errno)
{ if (errno <= MAXERROR) errno = MAXERROR;
  if (ErrDist >= MinErrDist) {
    if (Custom_Error != 0L)
      (*Custom_Error) (errno, S_Line, S_Col, S_Pos);
    errors++;
  }
  ErrDist = 0;
}

static void Get(void)
{ do {
    Sym = S_Get();
    if (Sym <= MAXT) ErrDist ++;
    else {
      /* Empty Stmt */ ;
      S_NextPos  = S_Pos;  S_NextCol = S_Col;
      S_NextLine = S_Line; S_NextLen = S_Len;
    }
  } while (Sym > MAXT);
}

static int In (unsigned short int *SymbolSet, int i)
{ return SymbolSet[i / NSETBITS] & (1 << (i % NSETBITS)); }

static void Expect (int n)
{ if (Sym == n) Get(); else GenError(n); }

static void ExpectWeak (int n, int follow)
{ if (Sym == n) Get();
  else {
    GenError(n);
    while (!(In(SymSet[follow], Sym) || In(SymSet[0], Sym))) Get();
  }
}

static int WeakSeparator (int n, int syFol, int repFol)
{ unsigned short int s[MAXSYM];
  int i;

  if (Sym == n) { Get(); return 1; }
  if (In(SymSet[repFol], Sym)) return 0;
  for (i = 0; i < MAXSYM; i++)
    s[i] = SymSet[0][i] | SymSet[syFol][i] | SymSet[repFol][i];
  GenError(n);
  while (!In(s, Sym)) Get();
  return In(SymSet[syFol], Sym);
}

int Successful(void)
{ return errors == 0; }

/* Productions */

static void ProxyConf(void)
{
	total=0;
	Expect(LessproxyGreaterSym);
	Address();
	while (Sym == LessipaddressSym) {
		Address();
	}
	Expect(LessSlashproxyGreaterSym);
}

static void Address(void)
{
	Expect(LessipaddressSym);
	
	char buffip[20];
	char buffport[10];
	unsigned int portnum;
	struct proxybind *tmp; 
	
	Expect(EqualSym);
	Expect(DquoteSym);
	Expect(ipSym);
	
	S_GetString(S_Pos,S_Len,buffip,sizeof(buffip)-1);
	
	Expect(DquoteSym);
	Expect(portSym);
	Expect(EqualSym);
	Expect(DquoteSym);
	Expect(numberSym);
	
	S_GetString(S_Pos,S_Len,buffport,sizeof(buffport)-1);
	portnum = atoi(buffport);
	
	Expect(DquoteSym);
	Expect(SlashGreaterSym);
	tmp = allocProxy();
	if ( (inet_pton(AF_INET,buffip,&tmp->ipaddress)) < 1)
	     {
	     free(tmp);
	     freeAll(&head);
	     fprintf(stderr,"Error parsing IP Address");
	     exit(-1);       
	     }
	tmp->port = portnum;
	head = insertProxy(head,tmp);
	total++;
	
}



void Parse(void)
{ S_Reset(); Get();
  ProxyConf();
}


