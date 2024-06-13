#pragma once
#include "bptree.hh"

struct timeval cur_time(void)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return t;
}

void print_tree_core(Node *n)
{
  printf("[");
  for (int i = 0; i < n->nkey; i++)
  {
    if (!n->isLeaf)
      print_tree_core(n->chi[i]);
    printf("%d", n->key[i]);
    if (i != n->nkey - 1 && n->isLeaf)
      putchar(' ');
  }
  if (!n->isLeaf)
    print_tree_core(n->chi[n->nkey]);
  printf("]");
}

void print_tree(Node *node)
{
  print_tree_core(node);
  printf("\n");
  fflush(stdout);
}

Node *find_leaf(Node *node, int key)
{
  int kid;

  if (node->isLeaf)
    return node;
  for (kid = 0; kid < node->nkey; kid++)
  {
    if (key < node->key[kid])
      break;
  }

  return find_leaf(node->chi[kid], key);
}

Node *insert_in_leaf(Node *leaf, int key, Data *data)
{
  int i;
  if (key < leaf->key[0])
  {
    for (i = leaf->nkey; i > 0; i--)
    {
      leaf->chi[i] = leaf->chi[i - 1];
      leaf->key[i] = leaf->key[i - 1];
    }
    leaf->key[0] = key;
    leaf->chi[0] = (Node *)data;
  }
  else
  {
    for (i = 0; i < leaf->nkey; i++)
    {
      if (key < leaf->key[i])
        break;
    }
    for (int j = leaf->nkey; j > i; j--)
    {
      leaf->chi[j] = leaf->chi[j - 1];
      leaf->key[j] = leaf->key[j - 1];
    }

    /* CodeQuiz */
    leaf->key[i] = key;
    leaf->chi[i] = (Node *)data;
  }
  leaf->nkey++;

  return leaf;
}

int search_b_plus_tree(int search_key)
{
  Node *search = find_leaf(root, search_key); // use find_leaf 関数 to search
  for (int i = 0; i < search->nkey; i++)
  {
    if (search->key[i] == search_key)
    {
      return search_key;
    }
  }
  return -1;
}

int counting_search()
{
  int count = 0;
  for (int i = 0; i <= MAX_OBJ; i++)
  {
    int search_result = search_b_plus_tree(i);
    if (search_result != i)
    {
      break;
    }
    count++;
  }
  return count;
}

Node *alloc_root(Node *left, int rs_key, Node *right) // node for root
{
  Node *node;
  if (!(node = (Node *)calloc(1, sizeof(Node)))) // メモリ確保に失敗した場合
    ERR;
  node->isLeaf = false;
  node->parent = NULL;
  node->nkey = 1;
  node->key[0] = rs_key;
  node->chi[0] = left;
  node->chi[1] = right;

  return node;
}

Node *alloc_parent(Node *parent) // node for parent but not root
{
  Node *node;
  if (!(node = (Node *)calloc(1, sizeof(Node))))
    ERR;
  node->isLeaf = false;
  node->parent = parent;
  node->nkey = 0;

  return node;
}

Node *alloc_leaf(Node *parent) // node for leaf
{
  Node *node;
  if (!(node = (Node *)calloc(1, sizeof(Node))))
    ERR;
  node->isLeaf = true;
  node->parent = parent;
  node->nkey = 0;

  return node;
}

void copy_from_target_to_temp(Node *node, Temp *temp)
{
  // target: [1, 2, 3, _], tmp: [_, _, _, _]
  int i;
  bzero(temp, sizeof(Temp)); // erase memory from temp
  for (i = 0; i < (N - 1); i++)
  {
    temp->key[i] = node->key[i];
    temp->chi[i] = node->chi[i];
  }
  temp->nkey = N - 1;
  temp->chi[i] = node->chi[i];
  // target: [1, 2, 3, _], tmp: [1, 2, 3, _]
}

void insert_key_in_temp(int key, Temp *temp, void *ptr)
{
  // tmp: [100, 200, 300, _]

  // case 1: key = 0
  // tmp: [0, 100, 200, 300]

  // case 2:
  // key = 150
  // tmp: [100, 150, 200, 300]
  // key = 400
  // tmp: [100, 150, 200, 400]

  if (temp->key[0] > key)
  {
    for (int i = temp->nkey; i > 0; i--)
    {
      temp->key[i] = temp->key[i - 1];
      temp->chi[i] = temp->chi[i - 1];
    }
    temp->key[0] = key;
    temp->chi[0] = (Node *)ptr; // arrayとleaf nodeをポインタで繋ぐ
  }
  else
  {
    int findInsertPosition = 0;
    while (findInsertPosition < temp->nkey &&
           temp->key[findInsertPosition] <= key)
    {
      findInsertPosition++;
    }
    for (int j = temp->nkey; j > findInsertPosition; j--)
    {
      temp->key[j] = temp->key[j - 1];
      temp->chi[j] = temp->chi[j - 1];
    }
    temp->key[findInsertPosition] = key;
    temp->chi[findInsertPosition] = (Node *)ptr;
  }
  temp->nkey++;
}

void erase_node(Node *node)
{
  for (int i = 0; i < N - 1; i++)
  {
    node->key[i] = 0;
    node->chi[i + 1] = NULL;
  }
  node->nkey = 0;
}

void copy_from_temp_to_parent(Node *split_parent, Node *parent, Temp *temp) // for parent node(not root)
{
  // parent[7,_,_,_]
  // parent[3,5,_,_]split_parent[9,_,_,_]
  // leaf: target[1,2,_,_] target/split[3,4,_,_] target/split[5,6,_,_]....
  int i;
  for (i = 3; i < N; i++)
  {
    split_parent->key[i - 3] = temp->key[i];
    split_parent->chi[i - 3] = temp->chi[i];
    split_parent->nkey++;
  }
  split_parent->chi[i - 3] = temp->chi[i];

  for (int j = 0; j < split_parent->nkey + 1; j++)
  {
    split_parent->chi[j]->parent = split_parent;
  }

  for (int a = 0; a < 2; a++)
  {
    parent->key[a] = temp->key[a];
    parent->chi[a] = temp->chi[a];
    parent->nkey++;
  }
  parent->chi[2] = temp->chi[2];
}

void copy_from_temp_to_leaf(Node *split, Node *target, Temp temp)
{
  for (int i = 0; i < (N + 1) / 2; i++)
  {
    target->key[i] = temp.key[i];
    target->chi[i] = temp.chi[i];
    target->nkey++;
  }
  for (int i = (N + 1) / 2; i < N; i++)
  {
    split->key[i - ((N + 1) / 2)] = temp.key[i];
    split->chi[i - ((N + 1) / 2)] = temp.chi[i];
    split->nkey++;
  }
}

void insert_in_current_parent(Node *parent, Node *target, int rs_key, Node *split)
{
  int left, right, findPosition = 0;
  while (findPosition < parent->nkey + 1)
  {
    if (parent->chi[findPosition] == target)
    {
      left = findPosition;
      right = left + 1;
      break;
    }
    findPosition++;
  }
  for (findPosition = parent->nkey + 1; findPosition > right; findPosition--)
  {
    parent->chi[findPosition] = parent->chi[findPosition - 1];
  }
  for (findPosition = parent->nkey; findPosition > left; findPosition--)
  {
    parent->key[findPosition] = parent->key[findPosition - 1];
  }
  parent->key[left] = rs_key;
  parent->chi[right] = split;
  parent->nkey++;
}

void insert_in_current_parent_from_temp(Temp *temp, Node *target, int rs_key, Node *split)
{
  int left, right, findPosition = 0;
  while (findPosition < temp->nkey + 1)
  {
    if (temp->chi[findPosition] == target)
    {
      left = findPosition;
      right = left + 1;
      break;
    }
    findPosition++;
  }

  for (findPosition = temp->nkey + 1; findPosition > right; findPosition--)
  {
    temp->chi[findPosition] = temp->chi[findPosition - 1];
  }
  for (findPosition = temp->nkey; findPosition > left; findPosition--)
  {
    temp->key[findPosition] = temp->key[findPosition - 1];
  }
  temp->key[left] = rs_key;
  temp->chi[right] = split;
  temp->nkey++;
}

void insert_in_parent(int rs_key, Node *target, Node *split)
{
  Node *parent = new Node();
  Node *split_parent = new Node();

  // step C: insert ptr of target and split and rs_key in parent

  // case C-1: if parent is not exist
  // target: [100, 200, _, _], split: [300, 400, _, _]
  if (target == root)
  {
    root = alloc_root(target, rs_key, split);
    target->parent = split->parent = root;
    // Root: [300, _, _, _]
    // target: [100, 200, _, _], split: [300, 400, _, _]
    return;
  }

  // case C-1: if parent is exist
  // case Ca: insert in current parent
  // Root(parent): [300, 500, _, _]
  // target: [100, 200, _, _], split/target: [300, 400, _, _],(new) split: [500,
  // 600, _, _](perhaps)
  parent = target->parent;
  if (parent->nkey < (N - 1))
  {
    insert_in_current_parent(parent, target, rs_key, split);
  }
  // case Cb: split parent node
  // Root(parent)[]
  // parent[], split_parent[]
  // target: [100, 200, _, _], split/target: [300, 400, _, _],(new) split: [500,
  // 600, _, _]
  else
  {
    Temp temp;
    copy_from_target_to_temp(parent, &temp);
    insert_in_current_parent_from_temp(&temp, target, rs_key, split);
    erase_node(parent);
    split_parent = alloc_parent(parent->parent); // making parent(not root)
    copy_from_temp_to_parent(split_parent, parent, &temp);
    int right_key_parent = temp.key[(N + 1) / 2]; // right_parent's [0] key
    insert_in_parent(right_key_parent, parent, split_parent);
  }
}

void insert(int key, Data *data)
{
  Node *leaf;

  // step A. find insert leaf node
  // case A-1: if root is not exist

  if (root == NULL)
  {
    leaf = alloc_leaf(NULL);
    root = leaf;
  }

  // case A-2: find leaf node
  else
  {
    leaf = find_leaf(root, key);
  }

  // step B. insert in leaf node
  // case B-1: insert in current leaf node
  if (leaf->nkey < (N - 1))
  {
    insert_in_leaf(leaf, key, data);
  }
  // case B-2: insert in split leaf node
  else
  {
    // target: [100, 200, 300, _]
    Node *target = leaf;
    // split: [_, _, _, _]
    Node *split = alloc_leaf(leaf->parent);
    Temp temp;

    // target: [100, 200, 300, _], tmp: [100, 200, 300, _]
    copy_from_target_to_temp(target, &temp);

    // step Ba: insert key in temp
    // case Ba-1: if key is smaller than temp->key[0]
    // case Ba-2: if key is bigger than temp->key[0]

    insert_key_in_temp(key, &temp, data);
    split->chi[N - 1] = target->chi[N - 1]; // connect target and split pointer
    target->chi[N - 1] = split;
    erase_node(target);
    copy_from_temp_to_leaf(split, target, temp);
    int parent_key = split->key[0];
    insert_in_parent(parent_key, target, split);
  }
}

void init_root(void) { root = NULL; }
