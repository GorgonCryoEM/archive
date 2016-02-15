#include <vector>
#include <iostream>

using namespace std;

template <int D, class T>
class Dim {
  public:
    Dim()
	    : vals(D, T())
      {}

    Dim(T v)
        : vals(D, v)
      {}

    vector<T> vals;
};

template <int D, class T>
ostream& operator<<(ostream& out, const Dim<D,T> &obj){
  for(typename vector<T>::const_iterator it=obj.vals.begin();
		  it!=obj.vals.end();
		  ++it
     )
    out<<" "<<*it;

  return out;
}
