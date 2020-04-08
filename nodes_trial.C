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

  for(int i = 0; i < keys.size(); i++) {
    BinaryKey& first_key = keys[i];
    BinaryKey& second_key = keys[i+1];
    while(g <= first_key.Get(d).size()) {
      for(BinaryKey& second_key : keys){
	if(first_key.Get(d)[g] != second_key.Get(d)[g]) {
	  g;
	}
      }
      g++;
    }
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
    if(key.Get(d)[g] == 0) {
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
  // TODO
  size_t g_Pp = dsc_inc(keys, Path, g_P);
  size_t g_Vp = dsc_inc(keys, Value, g_V);
  //
  Node* n = nullptr;
  for(int i = 0; i < keys.size(); i++) {
    BinaryKey& key_i = keys[i];
    BinaryKey& key_j = keys[i+1];
    (*n).s_P.push_back(key_i.Get(Path)[g_P, g_Pp - 1]);
    n -> s_P;
    (*n).s_V.push_back(key_i.Get(Value)[g_P, g_Pp - 1]);
    n -> s_V;
    // TODO
    if(g_Pp > key_i.Get(Path).size() && g_Vp > key_i.Get(Value).size()) {
      n -> d;
      for(BinaryKey& key_j : keys) {
	// Append reference WHAT?
      }
    }
  
    if(g_Pp > key_i.Get(Path).size()) {
      d = Value;
      size_t g_D = g_Vp;
    } else if(g_Vp > key_i.Get(Value).size()) {
      d = Path;
      size_t g_D = g_Pp;
    }
  
    //Partitioning M = psi_partition(keys, d, g_D);
    //if(M.bit0.size() != 0) {
	//(*n).children[0] = ConstructRCAS(keys, d, g_Pp, g_Vp);
	//}
      //if(M.bit1.size() != 0) {
	//(*n).children[1] = ConstructRCAS(keys, d, g_Pp, g_Vp);
	//}
  }
  return n;
}



int main()
{
  std::vector<std::string> list_paths = {
    "/bom/item/canoe",
    "/bom/item/carabinier",
    "/bom/item/car/battery",
    "/bom/item/car/battery",
    "/bom/item/car/belt"
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
    bkey.path_  = path_to_binary(list_paths[i]);
    bkey.value_ = value_to_binary(list_values[i]);
    keys.push_back(bkey);
  }

  for (BinaryKey& key : keys) {
    key.Dump();
    std::cout << std::endl;
  }


  BinaryKey& first_key = keys[0];
  first_key.Get(Path);

   std::cout << "Disc P bit: " << dsc_inc(keys, Path, 0) << std::endl; 
   std::cout << "Disc V bit: " << dsc_inc(keys, Value, 0) << std::endl; 

  return 0;
}
