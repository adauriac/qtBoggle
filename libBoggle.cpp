#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "libBoggle.h"

boggleSolver::boggleSolver(string dictSt)
{
    unsigned int i;
    char buf[128];
    FILE *f;
    num_words = 0;

    if (!(f = fopen((char*)dictSt.c_str(), "r")))
    {
        status = "error opening dictionary";
        return;
    }

    while (fgets(buf, 128, f)) {
        buf[strlen(buf)-1] = '\0';
        for (i = 0; i < strlen(buf); i++)
            buf[i] = tolower(buf[i]);
        dict[num_words++] = strdup(buf);
        if (num_words >= DICT_SIZE) {
            status = "dictionary too large\n";
            return;
        }
    }
    qsort(dict, num_words, sizeof(char *), my_sort);
    status = "OK";
}     // FIN boggleSolver::boggleSolver(string dict)
// ***********************************************************************

boggleSolver::boggleSolver(vector<char*> dictCont)
// flag==0 : c'est le NOM du fichier, flag=1 : c'est le contenu du fichier
{
    unsigned int i;
    num_words = 0;
    for(i=0;i<dictCont.size();i++)
    {
        char *buf = dictCont[i],*mot;
        mot = (char*)malloc(strlen(buf)-1);
        for (unsigned int j = 0; j < strlen(buf)-1; j++)
            mot[j] = tolower(buf[j]);
        mot[strlen(buf)-1]= '\0';
        dict[num_words++] = mot;
        if (num_words >= DICT_SIZE) {
            status = "dictionary too large \n";
            return;
        }
    }
    qsort(dict, num_words, sizeof(char *), my_sort);
    status = "OK";
}     // FIN boggleSolver::boggleSolver(char* dictCont)
// ***********************************************************************

string boggleSolver::solve(string data)
{
    int i,j;
    int len_min_affiche=4;
    for(i=0;i<4;i++)
    {
        // char st[6];
        //printf("%d ",i);fflush(stdout);
        //if (fgets(st,6,stdin)==NULL)
        //exit(0);
        for(j=0;j<4;j++)
        {
            //letters[i][j] = tolower(st[j]);
            letters[i][j] = tolower(data[4*i+j]);
            graph[i][j] = NOT_SEEN;
        }
    }
    // printf("Working...\n");
    // fflush(stdout);
    for (j = 0; j < BOX_SIZE; j++)
        for (i = 0; i < BOX_SIZE; i++)
            dfs(i, j, 0);
    qsort( found, num_found, sizeof(char *), st_sort_bylen);

    for( i=0;i< num_found;i++)
        if (strlen(found[i])>=(unsigned int) len_min_affiche)
            break;
    int z = 0;
    string ans = "";
    char aux[1024];
    for(i=0;i< num_found;i++)
    {
        sprintf(aux,"%s   (%d)\n",found[i],valeur[strlen(found[i])]);
        ans += string(aux);
        z += valeur[strlen(found[i])];
    }
    sprintf(aux,"gain= %d\n",z);
    ans += string(aux);
    return ans;
}     // FIN string boggleSolver::solve(string data)
// ***********************************************************************

void boggleSolver::try_print(char *word)
{
  int i;

  if (num_found >= FOUND_SIZE) {
    printf("too many words found\n");
    return;
  }

  for (i = 0; i < num_found; i++)
    if (!strcmp(word, found[i]))
      return;

  found[num_found++] = strdup(word);
  //  printf("bog %s\n", word);
}     // FIN  void boggleSolver::try_print()
// ***************************************************************

void boggleSolver::output(int depth)
{
  static int bot, top, cen, res;

  choices[depth] = '\0';
  bot = 0;
  top = num_words-1;

  for(;;) {
    cen = (bot+top)/2;
    res = strcmp(choices, dict[cen]);
    if (!res) {
      try_print(choices);
      return;
    } else if (bot >= top) {
      return;
    } else if (res > 0) {
      bot = ++cen;
    } else
      top = --cen;
  }
}     // FIN  void boggleSolver::output()
// ***************************************************************

void boggleSolver::dfs(int x, int y, int depth)
{
  int i, j;

  if (x < 0 || y < 0 || x >= BOX_SIZE || y >= BOX_SIZE)
    return;
  if (graph[x][y] == SEEN) {
    return;
  }

  graph[x][y] = SEEN;

  choices[depth++] = letters[x][y];

  if (depth >= MIN_WORD_LEN)
    output(depth);

  if (depth < MAX_WORD_LEN) {
    for (j = -1; j <= 1; j++)
      for(i = -1; i <= 1; i++)
	if (i || j)
      dfs(x+i, y+j, depth);
  }

  graph[x][y] = NOT_SEEN;
}     // FIN  void boggleSolver::dfs()
// ***************************************************************

int my_sort(const void *a, const void *b)
{
  return (strcmp(*(char **)a, *(char **)b));
}     // FIN  my_sort()
// ***************************************************************

int st_sort_bylen(const void *a, const void *b)
{
  return strlen(*(char **)a) > strlen(*(char **)b);
}     // FIN  st_sort_bylen()
// ***************************************************************
