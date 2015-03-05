#include<stdio.h>
#include<stdlib.h>
#ifndef WEXITSTATUS
#include <sys/wait.h>
#endif

char buf[32768];

#if (HAS_DEVENV == 0)
char * __dev_env;
#endif

static void __env(void)
{
 char * p=getenv("MENUETDEV");
 if(!p)
 {
  printf("MENUETDEV system variable not set !!!\n");
  exit(-1);
 }
#if (HAS_DEVENV == 0)
 __dev_env=p;
#endif
}

int main(int argc,char * argv[])
{
 int u;
 __env();
 if(argc<3)
 {
  fprintf(stderr,"Usage: %s infile.c outfile.o\n",argv[0]);
  return 1;
 }
#if (HAS_DEVENV==1)
 sprintf(buf,"gcc -c %s -o %s -Os -nostdinc -fno-builtin -fno-stack-protector -I/dev/env/MENUETDEV/include "
             "-fno-common -DMENUETDEV='\"/dev/env/MENUETDEV\"' "
	     "-D__DEV_CONFIG_H='</dev/env/MENUETDEV/config.h>' -D__MENUETOS__ ",argv[1],argv[2]);
#else
 sprintf(buf,"gcc -c %s -o %s -Os -nostdinc -fno-builtin -fno-stack-protector -I%s/include "
             "-fno-common -DMENUETDEV='\"%s\"' "
	     "-D__DEV_CONFIG_H=\"<%s/config.h>\" -D__MENUETOS__ ",argv[1],argv[2],__dev_env,__dev_env,__dev_env);
#endif
 if(argc>3)
  for(u=3;u<argc;u++)
  {
   strcat(buf,argv[u]);
   strcat(buf," ");
  }
 return WEXITSTATUS(system(buf));
}
