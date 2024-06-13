#pragma once

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include <iostream>

#include "debug.hh"

using namespace std;
#define MAX_OBJ (1000 * 1000)
#define N 4

class Data
{
public:
  int key;
  int val;
  Data *next;
};

class Node
{
public:
  bool isLeaf;
  struct Node *chi[N];
  int key[N - 1];
  int nkey;
  Node *parent;
};

class Temp
{
public:
  bool isLeaf;
  Node *chi[N + 1]; // for internal split (for leaf, only N is enough)
  int key[N];       // for leaf split
  int nkey;
};

Node *root = NULL;
