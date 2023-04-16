#include <iostream>
using namespace std;

class Page {
 public:
  int id;
  bool isLocal;
  bool isReferenced = false;
  Page() {}
  Page(int _id, bool _isLocal) : id(_id), isLocal(_isLocal) {}
};