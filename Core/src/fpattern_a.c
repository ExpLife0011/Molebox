
#include <windows.h>
#include <ctype.h>
#include <stddef.h>

#define FPAT_DEL  '/'   /* Path delimiter   */
#define FPAT_DEL2 '\\'    /* Path delimiter   */
#define FPAT_ANY  '?'   /* Any one char     */
#define FPAT_CLOS '*'   /* Zero or more chars   */

#ifndef DELIM
#define DELIM   0
#endif

#define DEL   FPAT_DEL
#define DEL2    FPAT_DEL2
#define lowercase(c)  tolower(c)


static int fpattern_submatch(const char *pat, const char *fname)
{
    int   fch;
    int   pch;
    int   i;

    /* Attempt to match subpattern against subfilename */
    while (*pat != '\0')
    {
        fch = *fname;
        pch = *pat;
        pat++;

        switch (pch)
        {
        case FPAT_ANY:
            /* Match a single char */
        #if DELIM
            if (fch == DEL  ||  fch == DEL2  ||  fch == '\0')
                return (FALSE);
        #else
            if (fch == '\0')
                return (FALSE);
        #endif
            fname++;
            break;

        case FPAT_CLOS:
            /* Match zero or more chars */
            i = 0;
        #if DELIM
            while (fname[i] != '\0'  &&
                    fname[i] != DEL  &&  fname[i] != DEL2)
                i++;
        #else
            while (fname[i] != '\0')
                i++;
        #endif
            while (i >= 0)
            {
                if (fpattern_submatch(pat, fname+i))
                    return (TRUE);
                i--;
            }
            return (FALSE);

#if DELIM
        case DEL:
    #if DEL2 != DEL
        case DEL2:
    #endif
            /* Match path delimiter char */
            if (fch != DEL  &&  fch != DEL2)
                return (FALSE);
            fname++;
            break;
#endif

        default:
            /* Match a (non-null) char exactly */
            if (lowercase(fch) != lowercase(pch))
                return (FALSE);
            fname++;
            break;
        }
    }

    /* Check for complete match */
    if (*fname != '\0')
        return (FALSE);

    /* Successful match */
    return (TRUE);
}


int fpattern_match(const char *pat, const char *fname)
{
    int   rc;

    /* Check args */
    if (fname == NULL)
        return (FALSE);

    if (pat == NULL)
        return (FALSE);

    /* Attempt to match pattern against filename */
    if (fname[0] == '\0')
        return (pat[0] == '\0');  /* Special case */
    if ( strcmp(pat,"*.*") == 0 ) pat = "*";
    rc = fpattern_submatch(pat, fname);

    return (rc);
}
