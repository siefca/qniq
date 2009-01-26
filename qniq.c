/************************************************************************/
/* Quick Uniq for Ispell - initial revision - v 0.01			*/
/* (C) 1999 by Pawel Wilk <siefca@gnu.org>				*/
/*  									*/
/*  This program is free software; you can redistribute it and/or 	*/
/*  modify it under the terms of the GNU Library General Public 	*/
/*  License as published by the Free Software Foundation; either 	*/
/*  version 2 of the License, or (at your option) any later version.	*/
/************************************************************************/  

// to compile:	gcc -O3 qniq.c -oqniq && strip qniq
// usage (with filename):	./qniq filename	   
// usage (with pipeline):	|./qniq

/************************************************************* customizations */
/* flags sorting - defaultly disabled, U can uncomment it to make 
		   some order in yah file :) */
	     
// #define	SORT_FLAGS		// comment it to NOT sort flags
// #define	ALWAYS_SORT_FLAGS	// normally flags are sorted only when diff.
// #define	UPPERCASE_FIRST		// self-explanatory ;>

/************************************************** enumerators, constants... */

#define	LINE_LEN	1024
#define	NEVER_HAPPEND	'+'	// character which never will be flag-marker
#define	OMMIT_RANGE	"abcde"	// some flags cannot be joined :P

/******************************************************************* includes */

#include <stdio.h>
#ifdef SORT_FLAGS
#include <stdlib.h>
#include <ctype.h>
#endif
#include <errno.h>
#include <string.h>		//try asm/string.h if you like speed

/********************************************************* comparing function */

#ifdef SORT_FLAGS
int por (const void *first, const void *last)
    {
    char *First = (char*) first;
    char *Last = (char*) last;
    
    if (isupper(*First))
	{
	if (isupper(*Last))
	    {
	    if (*First > *Last) return (1);
	    if (*First == *Last) return (0);
	    return (-1);
	    }
	else
	    {
#ifdef UPPERCASE_FIRST
	    return (-1); 
#else
	    return (1);
#endif
	    }
	}
    else
	{
#ifdef UPPERCASE_FIRST
	return (1);
#else
	return (-1);
#endif
	}
    }
#endif

/************************************************************** main function */

int main (int argc, char *argv[])
    {
    int znakkreski;
    char *kreska1, *kreska2, *temptr;
    char buf[LINE_LEN], buf2[LINE_LEN], bufzapasowy[LINE_LEN];
    char *ptr1 = buf, *ptr2 = buf2;
    FILE *plik;
    
    if (argc > 2)
	{
	fprintf (stderr, "Usage: %s file\n", argv[0]);
	exit (0);
	}
	
    if (argc == 1) plik = stdin;
    else 
	{
	plik = fopen (argv[1], "r");
	if (!plik)
	    {
	    perror ("qniq");
	    exit (1);
	    }
	}
    
    znakkreski = (int) '/';
    
    if (!fgets(ptr1, LINE_LEN-1, plik))
	{
	perror ("qniq");
	exit (1);
	}
    if (temptr = strrchr (ptr1, '\n')) *temptr = '\0';
    
    kreska1 = strchr (ptr1, znakkreski);
    if (kreska1) 
        {
        *kreska1 = '\0';
        kreska1++;
        }
    else kreska1 = NULL;
	
    while (!feof(plik) && !ferror(plik))
	{
	if (!fgets(ptr2, LINE_LEN-1, plik)) 
	    {
	    if (kreska1) 
		{
#ifdef	SORT_FLAGS
#ifdef	ALWAYS_SORT_FLAGS
		qsort (kreska1, strlen(kreska1), sizeof(char), por);
#endif
#endif
		printf ("%s/%s\n", ptr1, kreska1);
		}
	    else printf ("%s\n", ptr1);
	    continue;
	    }
	    
	if (temptr = strrchr (ptr2, '\n')) *temptr = '\0';
	kreska2 = strchr (ptr2, znakkreski);
	if (kreska2) 
	    {
	    *kreska2 = '\0';
	    kreska2++;
	    }
	    
	if (strcmp(ptr1, ptr2) || (kreska2 && strpbrk (kreska2, OMMIT_RANGE))) 
	    {
	    if (kreska1 && *kreska1)
		{
#ifdef	SORT_FLAGS
#ifdef	ALWAYS_SORT_FLAGS
		qsort (kreska1, strlen(kreska1), sizeof(char), por);
#endif
#endif
		printf ("%s/%s\n", ptr1, kreska1);
		kreska1 = NULL;
		}
	    else printf ("%s\n", ptr1);
	    
	    temptr = ptr1;
	    ptr1 = ptr2;	// faster, cheeper, without moving :)
	    ptr2 = temptr;	// needed, according to memory
	    temptr = kreska1;
	    kreska1 = kreska2;
	    kreska2 = NULL;
	    continue;
	    }
	else 
	    {
	    if (!kreska1)
		{
		if (kreska2) 
		    {
		    kreska1 = bufzapasowy;
		    strcpy (kreska1, kreska2);
		    }
		continue;
		}
	    else
		{
		if (!kreska2) continue;
		}
	    	    
	    if (kreska1 == kreska2)	// when the addresses are same,
		{			// strpbrk wants it...
		kreska1 = bufzapasowy;
		strcpy (kreska1, kreska2);
		}
	    else
		{
		if (!strcmp (kreska1, kreska2)) continue;
		} // acceleration
		
	    while ((temptr = (char*) strpbrk (kreska1, kreska2))) 
		*temptr = NEVER_HAPPEND;

	    strcat (kreska1, kreska2);
	    temptr = kreska1;
	    while ((temptr = strchr (temptr, NEVER_HAPPEND)))
		strcpy (temptr, temptr+1);
#ifndef ALWAYS_SORT_FLAGS
#ifdef SORT_FLAGS 
	    qsort (kreska1, strlen(kreska1), sizeof(char), por);
#endif
#endif
	    }
	}
	
    close (plik);
    }
