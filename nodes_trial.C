#include <iostream>
#include <vector>
#include <cstdint>
#include <bits/stdc++.h>

using namespace std;

enum Dimension {
  Path,
  Value,
  Leaf
};


struct BinaryKey {
  std::vector<bool> path_;
  std::vector<bool> value_;
  uint64_t r;

  void Dump() {
    std::cout << "path:  ";
    PrintBitVector(path_);
    std::cout << std::endl;
    std::cout << "value: ";
    PrintBitVector(value_);
    std::cout << std::endl;
  }

  void PrintBitVector(const std::vector<bool>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
      std::cout << (v[i] ? "1" : "0");
      if ((i+1) % 8 == 0) {
        std::cout << " ";
      }
    }
  }

  const std::vector<bool>& Get(Dimension d) {
    if (d == Path) {
      return path_;
    } else if (d == Value) {
      return value_;
    } else {
      throw std::invalid_argument("BinaryKey does not have a value for dimension Leaf");
    }
  }
};



std::vector<bool> path_to_binary(const std::string& path) {
  std::vector<bool> bpath(8*(path.size()+1), false);
  const char *path_c = path.c_str();
  for (std::size_t i = 0; i < path.size()+1; ++i) {
    std::bitset<8> bitset(path_c[i]);
    for (int j = 0; j < 8; j++) {
      bpath[i*8+(8-j-1)] = bitset[j];
    }
  }
  return bpath;
}


std::vector<bool> value_to_binary(uint32_t value) {
  std::vector<bool> bvalue(32, false);
  for (int i = 0; i < 32; ++i) {
    bvalue[32-i-1] = value & 1;
    value >>= 1;
  }
  return bvalue;
}


size_t dsc_inc(std::vector<BinaryKey>& keys, Dimension d, size_t g) {

    BinaryKey& first_key = keys[0];

    while(g <= first_key.Get(d).size()) {
      for(BinaryKey& second_key : keys){
	if(first_key.Get(d)[g] != second_key.Get(d)[g]) {
	  return g;
	}
      }
      g++;
    }
  
  return g;
  
}


struct Partitioning {
  std::vector<BinaryKey> bit0;
  std::vector<BinaryKey> bit1;
};


Partitioning psi_partition(std::vector<BinaryKey>& keys, Dimension d, size_t g) {
  Partitioning M;

  for (BinaryKey& key : keys) {
    if(key.Get(d)[g] == false) {
      M.bit0.push_back(key); 
    } else {
      M.bit1.push_back(key);
    }
  }
  return M;
}


// Binary Tree Node
struct Node {
  std::vector<bool> s_P;
  std::vector<bool> s_V;
  Dimension d;
  Node *left, *right;
};


Node* ConstructRCAS(std::vector<BinaryKey>& keys, Dimension d, size_t g_P, size_t g_V) {

  size_t g_Pp = dsc_inc(keys, Path, g_P);
  size_t g_Vp = dsc_inc(keys, Value, g_V);

  Node* n = new Node();  
  
    BinaryKey& key_i = keys[0];

    // s_P should contain all of the bits between g_P and g_Pp (e.g. loop over the key)
    for (int i = 0; i < key_i.Get(Path).size(); i++) { 
      if (i >= g_P || i < g_Pp) {
	n -> s_P.push_back(key_i.Get(Path)[i]);
      }
    }

    for (int i = 0; i < key_i.Get(Value).size(); i++) {
      if (i >= g_V || i < g_Vp) {
        n -> s_V.push_back(key_i.Get(Value)[i]);
      }
    }

    if(g_Pp >= key_i.Get(Path).size() && g_Vp >= key_i.Get(Value).size()) {
      n -> d = Leaf;
      for(BinaryKey& key_j : keys) {
        // Add reference
      }
      return n;
    }

    size_t g_D;
    if(key_i.Get(d).size() == key_i.Get(Path).size() && g_Pp >= key_i.Get(Path).size()) {
      d = Value;
      g_D = g_Vp;
    } else if(key_i.Get(d).size() == key_i.Get(Value).size() && g_Vp >= key_i.Get(Value).size()) {
      d = Path;
      g_D = g_Pp;
    } else if (key_i.Get(d).size() == key_i.Get(Value).size()) {
      g_D = g_Vp;
    } else if (key_i.Get(d).size() == key_i.Get(Path).size()) {
      g_D = g_Pp;
    }

    n -> d = d;
    Partitioning M = psi_partition(keys, d, g_D);
    // switch dimension for next iteration
    if (d == Path) {
      d = Value;
    } else if (d == Value) {
      d = Path;
    }

    cout << "Disc P bit: " << g_Pp << endl;
    cout << "Disc V bit: " << g_Vp << endl;
    cout << "Partitioning wrt:" << g_D << endl;

    n -> left = ConstructRCAS(M.bit0, d, g_Pp, g_Vp);
    n -> right = ConstructRCAS(M.bit1, d, g_Pp, g_Vp);

  return n;
}



int main()
{
  std::vector<std::string> list_paths = {
    "/bom/item/canoe$",
    "/bom/item/carabinier$",
    "/bom/item/car/battery$",
    "/bom/item/car/battery$",
    "/bom/item/car/belt$"
  };

  std::vector<uint32_t> list_values = {
    0x00010E50,
    0x000000F1,
    0x0003D35A,
    0x0003D3B0,
    0x00000B4A
  };

  std::vector<BinaryKey> keys;
  for (size_t i = 0; i < list_paths.size(); ++i) {
    BinaryKey bkey;
    bkey.path_ = path_to_binary(list_paths[i]);
    bkey.value_ = value_to_binary(list_values[i]);
    keys.push_back(bkey);
  }
   size_t g_P = dsc_inc(keys, Path, 0);
   size_t g_V = dsc_inc(keys, Value, 0);

   ConstructRCAS(keys, Path, g_P, g_V); 

  return 0;
}
