#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <vector>

using namespace std;

#define BOX_SIZE	4

#define MIN_WORD_LEN	4
#define MAX_WORD_LEN	16

#define NOT_SEEN	0
#define SEEN		1

#define DICT_SIZE       300000
#define FOUND_SIZE      1000

class boggleSolver
{
 public:
  char *dict[DICT_SIZE];
  char *found[FOUND_SIZE];

  // nb lettres     0 1 2 3 4 5 6 7 8  9  10 11 12 13 14 15
  int valeur[17] = {0,0,0,1,1,2,3,5,11,11,11,11,11,11,11,11,1000};
  int num_words, num_found = 0;
  string status;
  char letters[BOX_SIZE][BOX_SIZE];
  int graph[BOX_SIZE][BOX_SIZE];
  char choices[MAX_WORD_LEN+1];
  
  boggleSolver(string dict);
  boggleSolver(vector<char*> dict);
  ~boggleSolver(){};
  void try_print(char *word);
  void output(int depth);
  void dfs(int x, int y, int depth);
  string solve(string data);
};

int my_sort(const void *a, const void *b);
int st_sort_bylen(const void *a, const void *b);
