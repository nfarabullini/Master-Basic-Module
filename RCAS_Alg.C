#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

enum Dimension {
  Path,
  Value,
  Leaf
};

enum Matches {
  MATCH,
  MISMATCH,
  INCOMPLETE
};

struct RangeValues {
  std::vector<bool> v_l;
  std::vector<bool> v_h;
};

struct BinaryKey {
  std::vector<bool> path_;
  std::vector<bool> value_;
  string r;

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
      for(BinaryKey& second_key : keys) {
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
  std::vector<string> reference;
  Dimension d;
  Node *left, *right;
};

void PrintTree(Node *root, string space) 
{
  if (root != NULL) { 
    cout << space << endl;
   
    cout << space << "value[";
    if (root -> s_V.size() != 0) {
      for (int i = 0; i <= root -> s_V.size(); i++) {
	cout << root -> s_V[i];
      }
    }
    cout << "]" << endl;
    
    cout << space << "path[";
    if (root -> s_P.size() != 0) {
      for (int i = 0; i <= root -> s_P.size(); i++) {
	cout << root -> s_P[i];
      }
    }
    cout << "]" << endl;

    if (root -> d == Path) {
      cout << "Dimension: Path" << endl;
    } else if (root -> d == Value) {
      cout << "Dimension: Value" << endl;
    }
    
    PrintTree(root->left, space + " "); 
    PrintTree(root->right, space + " ");
  }
	
} 

Node* ConstructRCAS(std::vector<BinaryKey>& keys, Dimension d, size_t g_P, size_t g_V) {

  size_t g_Pp = dsc_inc(keys, Path, g_P);
  size_t g_Vp = dsc_inc(keys, Value, g_V);

  Node* n = new Node();
  
    BinaryKey& key_i = keys[0];
    
     // s_P and s_V should contain all of the bits between g_P and g_Pp (e.g. loop over the key)
    for (int i = g_P; i < g_Pp && i < key_i.Get(Path).size(); i++) {
	n -> s_P.push_back(key_i.Get(Path)[i]);
    }
    
    for (int i = g_V; i < g_Vp && i < key_i.Get(Value).size(); i++) { 
	n -> s_V.push_back(key_i.Get(Value)[i]);
    }
   
    if(g_Pp >= key_i.Get(Path).size() && g_Vp >= key_i.Get(Value).size()) {
      n -> d = Leaf;
      for(BinaryKey& key_j : keys) {
	n -> reference.push_back(key_j.r);
      }
      return n;
    }

    size_t g_D;
    if(d == Path && g_Pp >= key_i.Get(Path).size()) {
      d = Value;
    } else if(d == Value && g_Vp >= key_i.Get(Value).size()) {
      d = Path;
    }
    
    if (d == Value) {
      g_D = g_Vp;
    } else if (d == Path) {
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
    
    n -> left = ConstructRCAS(M.bit0, d, g_Pp, g_Vp);
    n -> right = ConstructRCAS(M.bit1, d, g_Pp, g_Vp);

  return n;
}

void UpdateBuffers(Node *n, std::vector<bool>& buff_V, std::vector<bool>& buff_P) {
  buff_P = n -> s_P;
  buff_V = n -> s_V;
}

Matches MatchValue(std::vector<bool>& buff_V, RangeValues& range, Node *n) {
  //lo, hi = longest common prefix between bff_V and (v_l and v_h)
  // lo <- discriminative bit between buff_V and v_l (similar approach to dsc_inc)

  size_t lo = 0;
  size_t hi = 0;

  while (lo < buff_V.size() && buff_V[lo] != range.v_l[lo]) {
    lo++;
  }

  while (hi < buff_V.size() && buff_V[hi] != range.v_h[hi]) {
    hi++;
  }

  string m_V;
   
  if (lo < buff_V.size() && lo < range.v_l.size() && buff_V[lo] < range.v_l[lo]) {
    return MISMATCH;
  } else if ((hi < buff_V.size() && hi < range.v_l.size()) && buff_V[hi] > range.v_h[hi]) {
    return MISMATCH;
  }
  
  string outcome = "outside";
  int i = 0;
    if ((lo < buff_V.size() && lo < range.v_l.size() && buff_V[lo] >= range.v_l[lo]) && (buff_V[hi] <= range.v_h[hi])) {
      outcome = "inside";
      i = i;
    }
   
  if (n -> d == Leaf && outcome == "inside") {
    //buff_V has to present bit 1 sooner than v_l and later than v_h or be identical to one of them: v_l <= buff_V <= v_h
    return MATCH;
  } else if (n -> d != Leaf && ((lo < buff_V.size() && lo < range.v_l.size()) && buff_V[lo] > range.v_l[lo]) && (buff_V[hi] < range.v_h[hi])) {
    return MATCH;
  } else {
    return INCOMPLETE;
  }
}

Matches MatchPath(std::vector<bool>& buff_P, std::vector<bool> q, Node *n) {

  string m_P;
  int i = 0;
  while (i < buff_P.size() && i < q.size()) {
   if(q[i] != buff_P[i]) {
     return MISMATCH;
    }
    i++;
  }

  if (n -> d == Leaf) {
    return MATCH;
  } else {
    return INCOMPLETE;
  }
}

void CasQuery(Node *n, std::vector<bool> q, RangeValues& range, std::vector<bool>& buff_V, std::vector<bool>& buff_P) {
  string m_V;
  string m_P;
  m_V = MatchValue(buff_V, range, n);
  m_P = MatchPath(buff_P, q, n);
  std::vector<string> ref_match;

  if (m_V == "MATCH" && m_P == "MATCH") {
    ref_match = n -> reference;
    for (int i = 0; i < ref_match.size(); i++) {
      cout << ref_match[i];
    }
    cout << "" << endl;
  } else if (m_V != "MISMATCH" && m_P != "MISMATCH") {
    CasQuery(n -> right, q, range, buff_V, buff_P);
    CasQuery(n -> left, q, range, buff_V, buff_P);
  }
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

  // finish list for all entries
  std::vector<string> list_ref = {
    "r_1",
    "r_2",
    "r_3",
    "r_4",
    "r_5"
  };

  std::vector<BinaryKey> keys;
  for (size_t i = 0; i < list_paths.size(); ++i) {
    BinaryKey bkey;
    bkey.path_ = path_to_binary(list_paths[i]);
    bkey.value_ = value_to_binary(list_values[i]);
    bkey.r = list_ref[i];
    keys.push_back(bkey);
  }

  size_t g_P = 0;
  size_t g_V = 0;   

   ConstructRCAS(keys, Path, g_P, g_V);
   // set root of node for printing equal to function above
   Node* root = ConstructRCAS(keys, Path, g_P, g_V);
   PrintTree(root, " ");

   // path predicate
   string query_path = "/bom/item/car/engine$";
   std::vector<bool> q = path_to_binary(query_path);
   
   // value predicate
   std::vector<uint32_t> v_32 = {
     0X00050151,
     0X010067E0
   };
   
     RangeValues range;
     range.v_l = value_to_binary(v_32[0]);
     range.v_h = value_to_binary(v_32[1]);

   std::vector<bool> buff_P;
   std::vector<bool> buff_V;
   //CasQuery(root, q, range, buff_V, buff_P)

  return 0;
}
