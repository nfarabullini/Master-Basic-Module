#include <bits/stdc++.h>
#include <limits>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream> 
#include <vector>
#include <string>

using namespace std;

// function to convert path to binary
string binary_path_fun(string myString) {
  
  string binary_path;
  
  for (std::size_t i = 0; i < myString.size(); ++i) {
     bitset<8> b_1(myString.c_str()[i]);
     binary_path+= b_1.to_string();
  }
  return binary_path;
}

// function to convert value to binary
string binary_value_fun(const string& s)
{
    stringstream ss;
    ss << hex << s;
    unsigned n;
    ss >> n;
    bitset<32> b_1(n);

    //unsigned x = 0;
    //if (boost::starts_with(s, "0x") || boost::starts_with(s, "0X")) x = 2;
    //return b_2.to_string().substr(32 - 4*(s.length() - 2));

    return b_1.to_string();
}

// function to find position of last mutual bit - Alg 1
int position_stop (string b_path_1, string b_path_2) {

  // define two new strings for loops
    string item_1;
    string item_2;

    vector<string> list_paths = {b_path_1, b_path_2};
    int lst_size_paths;
    lst_size_paths = list_paths.size();
    int j_pass;

    // loop over paths to find differing position - Alg 1
    for (int i = 1; i < lst_size_paths; i++) {
        item_1 = list_paths[i - 1];
        item_2 = list_paths[i];
        int item_1_size = item_1.size();
	int j = 0;
        while (j < item_1_size) {
            if (item_1[j] == item_2[j]) {
	      j_pass = j;
	    } else {
	      break;
	    }
	    j++;
	}
    }
    return j_pass;
}

// Binary Tree Node 
struct Node { 
    int key; 
    Node *left, *right; 
}; 
  
/* utility that allocates a new Node  
   with the given key */
Node* newNode(int key) 
{ 
    Node* node = new Node; 
    node->key = key; 
    node->left = node->right = nullptr; 
    return (node); 
}

Node* create_tree(Node*& root, int key, vector<string> index_list) {
    int size = index_list.size();
    int i = 0;
    // start with root node
    Node *curr = root;

    // pointer to store parent node of current node
    Node *parent = nullptr;

    // if tree is empty, create a new node and set root
    if (root == nullptr){
      root = newNode(key);
      return root;
    }

    vector<int> j_pass_values = {};

for (int i = 1; i < index_list.size(); i++) {
     j_pass_values.push_back(position_stop(index_list[0], index_list[i]));
   }

 string b_value_1 = binary_value_fun(index_list[0]);
 
 // find minimum value for all discriminative positions
   int min_val = *min_element(j_pass_values.begin(), j_pass_values.end());

// create lists to differentiate between discriminatory bits and not
  string item;
  vector<int> m_same = {};
  vector<int> m_other = {};
  vector<int> index_list_same = {};
  vector<int> index_list_other = {};

// find discriminative bits and fill lists with relative position - Alg 2
  for (int i = 0; i < index_list.size(); i++) {
      item = index_list[i];
      if (item[min_val + 1] == b_value_1[1]) {
	cout << i << endl;
	cout << "n_3" << endl;
	m_same.push_back(item[min_val + 1]);
	index_list_same.push_back(i);
	curr = curr -> left;
      } else {
	cout << i << endl;
	cout << "n_2" << endl;
	m_other.push_back(item[min_val + 1]);
	index_list_other.push_back(i);
	curr = curr -> right;
      }
  }
  

    // construct a new node and assign to appropriate parent pointer
    if (key < parent->key){
      parent->left = newNode(key);
    } else {
      parent->right = newNode(key);
    }
    
    //return nullptr;
  }
  
  

int main() 
{ 

  //initialize strings for conversion
  vector<string> list_str_paths = {"/bom/item/canoe$", "/bom/item/carabinier$", "/bom/item/car/battery$", "/bom/item/car/battery$", "/bom/item/car/belt$"};

  vector<string> list_str_values = {"0x00010E50", "0x000000F1", "0x0003D35A", "0x0003D3B0", "0x00000B4A"};

   // convert to binary
   for (int i = 0; i < list_str_paths.size(); i++) {
     list_str_paths[i] = binary_path_fun(list_str_paths[i]);
   }

   for (int i = 0; i < list_str_values.size(); i++) {
     list_str_values[i] = binary_value_fun(list_str_values[i]);
   }

   string b_value_1 = binary_value_fun(list_str_values[0]);

   // find last mutual position
   vector<int> j_pass_values = {};
   vector<int> j_pass_paths = {};

   for (int i = 1; i < list_str_values.size(); i++) {
     j_pass_values.push_back(position_stop(list_str_values[0], list_str_values[i]));
   }
   
   for (int i = 1; i < list_str_paths.size(); i++) {
     j_pass_paths.push_back(position_stop(list_str_paths[0], list_str_paths[i]));
   }

   // find minimum value for all discriminative positions
   int min_path = *min_element(j_pass_paths.begin(), j_pass_paths.end());
   int min_val = *min_element(j_pass_values.begin(), j_pass_values.end());

  // create lists to differentiate between discriminatory bits and not
  string item;
  vector<int> m_same = {};
  vector<int> m_other = {};
  vector<int> index_list_same = {};
  vector<int> index_list_other = {};
  
    // find discriminative bits and fill lists with relative position - Alg 2
  for (int i = 0; i < list_str_values.size(); i++) {
      item = list_str_values[i];
      if (item[min_val + 1] == b_value_1[1]) {
	cout << i << endl;
	cout << "n_3" << endl;
	m_same.push_back(item[min_val + 1]);
	index_list_same.push_back(i);
      } else {
	cout << i << endl;
	cout << "n_2" << endl;
	m_other.push_back(item[min_val + 1]);
	index_list_other.push_back(i);
      }
  }

  Node* root = nullptr;
  int key = 0;
  create_tree(root, key, index_list_other);
  return 0;
  
}  

