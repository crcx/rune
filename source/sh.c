#include <stdio.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0
#define OKAY 1
#define ERROR 0
#define MAXLINE 200		/* Maximum length of input line */
#define MAXARG 20		/* Max number of args for each simple command */
#define PIPELINE 5		/* Max number of simple commands in a pipeline */
#define MAXNAME 100		/* Maximum length of i/o redirection filename */
#define OPEN_MAX 100

char line[MAXLINE + 1];		/* User typed input line */
char *lineptr;			/* Pointer to current position in line[] */
char avline[MAXLINE + 1];	/* Argv strings taken from line[] */
char *avptr;			/* Pointer to current position in avline[] */
char infile[MAXNAME + 1];	/* Input redirection filename */
char outfile[MAXNAME + 1];	/* Ouput redirection filename */

int backgnd;			/* TRUE if & ends pipeline else FALSE */
int lastpid;			/* PID of last simple command in
				   pipeline */
int append;			/* TRUE for append redirection (») else FALSE */

struct cmd {
    char *av[MAXARG];
    int infd;
    int outfd;
} cmdlin[PIPELINE];		/* Argvs and fds, one per simple command */


main(void)
{
    int i;

    initcold();

    for (;;) {
	initwarm();

	if (getline())
	    if (i = parse())
		execute(i);
    }
}


initcold(void)
{
    /*
       signal(SIGINT, SIG_IGN); 
       signal (SIGOUIT, SIG_IGN);
     */
}



initwarm(void)
{
    int i;

    backgnd = FALSE;
    lineptr = line;
    avptr = avline;
    infile[0] = '\0';
    outfile[0] = '\0';
    append = FALSE;

    for (i = 0; i < PIPELINE; ++i) {
	cmdlin[i].infd = 0;
	cmdlin[i].outfd = 1;
    }

    for (i = 3; i < OPEN_MAX; ++i)
	close(i);

    printf("sh: ");
    fflush(stdout);
}


getline(void)
{
    int i;

    for (i = 0; (line[i] = getchar()) != '\n' && i < MAXLINE; ++i);

    if (i == MAXLINE) {
	fprintf(stderr, "Command line too long\n");
	return (ERROR);
    }
    line[i + 1] = '\0';
    return (OKAY);
}


parse(void)
{
    int i;

    /* 1 */
    command(0);

    /* 2 */
    if (check("<"))
	getname(infile);

    /* 3 */
    for (i = 1; i < PIPELINE; ++i)
	if (check("|"))
	    command(i);
	else
	    break;

    /* 4 */
    if (check(">")) {
	if (check(">"))
	    append = TRUE;

	getname(outfile);
    }

    /* 5 */
    if (check("&"))
	backgnd = TRUE;

    /* 6 */
    if (check("\n"))
	return (i);
    else {
	fprintf(stderr, "Command line syntax error\n");
	return (ERROR);
    }
}


command(int i)
{
    int j, flag, inword;

    for (j = 0; j < MAXARG - 1; ++j) {
	while (*lineptr == ' ' || *lineptr == '\t')
	    ++lineptr;

	cmdlin[i].av[j] = avptr;
	cmdlin[i].av[j + 1] = NULL;

	for (flag = 0; flag == 0;) {
	    switch (*lineptr) {
	    case '>':
	    case '<':
	    case '|':
	    case '&':
	    case '\n':
		if (inword == FALSE)
		    cmdlin[i].av[j] = NULL;

		*avptr++ = '\0';
		return;
	    case ' ':
	    case '\t':
		inword = FALSE;
		*avptr++ = '\0';
		flag = 1;
		break;
	    default:
		inword = TRUE;
		*avptr++ = *lineptr++;
		break;
	    }
	}
    }
}



execute(int j)
{
    int i, fd, fds[2];

    /* 1 */
    if (infile[0] != '\0')
	cmdlin[0].infd = open(infile, O_RDONLY);

    /* 2 */
    if (outfile[0] != '\0')
	if (append == FALSE)
	    cmdlin[j - 1].outfd =
		open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	else
	    cmdlin[j - 1].outfd =
		open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);

    /* 3 */
    if (backgnd == TRUE)
	signal(SIGCHLD, SIG_IGN);
    else
	signal(SIGCHLD, SIG_DFL);

    /* 4 */
    for (i = 0; i < j; ++i) {
	/* 5 */
	if (i < j - 1) {
	    pipe(fds);
	    cmdlin[i].outfd = fds[1];
	    cmdlin[i + 1].infd = fds[0];
	}

	/* 6 */
	forkexec(&cmdlin[i]);

	/* 7 */
	if ((fd = cmdlin[i].infd) != 0)
	    close(fd);

	if ((fd = cmdlin[i].outfd) != 1)
	    close(fd);
    }

    /* 8 */
    if (backgnd == FALSE)
	while (wait(NULL) != lastpid);
}

forkexec(struct cmd *ptr)
{
    int i, pid;

    /* 1 */
    if (pid = fork()) {
	/* 2 */
	if (backgnd == TRUE)
	    printf("%d\n", pid);
	lastpid = pid;
    } else {
	/* 3 */
	if (ptr->infd == 0 && backgnd == TRUE)
	    ptr->infd = open("/dev/null", O_RDONLY);

	/* 4 */
	if (ptr->infd != 0) {
	    close(0);
	    dup(ptr->infd);
	}

	if (ptr->outfd != 1) {
	    close(1);
	    dup(ptr->outfd);
	}

	/* 5 */
	if (backgnd == FALSE) {
	    signal(SIGINT, SIG_DFL);
	    signal(SIGQUIT, SIG_DFL);
	}

	/* 6 */
	for (i = 3; i < OPEN_MAX; ++i)
	    close(i);

	/* 7 */
	execvp(ptr->av[0], ptr->av);
	exit(1);
    }
}

check(char *ptr)
{
    char *tptr;

    while (*lineptr == ' ')
	lineptr++;

    tptr = lineptr;

    while (*ptr != '\0' && *ptr == *tptr) {
	ptr++;
	tptr++;
    }
    if (*ptr != '\0')
	return (FALSE);
    else {
	lineptr = tptr;
	return (TRUE);
    }
}


getname(char *name)
{
    int i;

    for (i = 0; i < MAXNAME; ++i) {
	switch (*lineptr) {
	case '>':
	case '<':
	case '|':
	case '&':
	case ' ':
	case '\n':
	case '\t':
	    *name = '\0';
	    return;

	default:
	    *name++ = *lineptr++;
	    break;
	}
    }
    *name = '\0';
}
