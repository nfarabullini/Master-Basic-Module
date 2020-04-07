#include <iostream>
#include <vector>
#include <cstdint>

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
  // TODO
  return g;
}


struct Partitioning {
  std::vector<BinaryKey> bit0;
  std::vector<BinaryKey> bit1;
};


Partitioning psi_partition(std::vector<BinaryKey>& keys, Dimension d, size_t g) {
  Partitioning M;
  // TODO
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
  Node* n = nullptr;
  // TODO
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

  /* std::cout << "Disc P bit: " << dsc_inc(keys, Path, 0) << std::endl; */
  /* std::cout << "Disc V bit: " << dsc_inc(keys, Value, 0) << std::endl; */

  return 0;
}
