#include "bptree.hh"
#include <sys/time.h>
#include <random>
#include <vector>

#include "bptree_func.hh"
vector<int> btree_array;

int main(int argc, char *argv[])
{
  struct timeval begin, end;

  init_root();

  printf("-----Insert-----\n");
  begin = cur_time();

  std::random_device rnd;
  for (int i = 0; i <= MAX_OBJ; i++)
  {
    int key = rand() % MAX_OBJ; // random
    // int key = i * (-1); //降順
    // int key = i; // 昇順
    btree_array.push_back(key);
  }

  for (int i = 0; i <= MAX_OBJ; i++)
  {
    insert(btree_array[i], NULL);
  }

  print_tree(root);

  // int count = counting_search();

  // if (count == MAX_OBJ + 1) // if you could find all key
  //{
  //   cout << "you are succeed!" << endl;
  // }
  // else
  // {
  //   cout << "unfortunally,you are failed in searching." << endl;
  //}

  end = cur_time();
  double elapsed = (end.tv_sec - begin.tv_sec) * 1000.0; // sec to ms
  elapsed += (end.tv_usec - begin.tv_usec) / 1000.0;     // us to ms
  printf("Time elapsed: %.4f ms\n", elapsed);
  return 0;
}
