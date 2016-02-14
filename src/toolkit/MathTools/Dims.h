#include <vector>
#include <iostream>
#include <stdexcept>

using namespace std;

template <int D, class T>
class DimBase {
  public:
    DimBase()    : dim(T()), dims()  {}
    DimBase(T v) : dim(v),   dims(v-1) {
      cout<<"DimBase<"<<D<<">::ctor("<<v<<")"<<endl;
    }

    T& operator[](int d) {
      //cout<<"operator[]<"<<D<<">(int "<<d<<")"<<endl;
      return const_cast<T &>(static_cast<const DimBase<D,T>& >(*this)[d]);
    }

    const T& operator[](int d) const {
      //cout<<"operator[]<"<<D<<">(int "<<d<<")"<<endl;
      return d==D ? dim : dims[d];
    }

    T dim;
    DimBase<D-1,T> dims;
};

template <int D, class T>
ostream& operator<<(ostream& out, const DimBase<D,T> &obj){
    out<<obj.dims
	<<" "<<obj.dim;

  return out;
}

template <class T>
class DimBase<1,T> {
  public:
    DimBase()    : dim(T()) {}
    DimBase(T v) : dim(v)   {
      cout<<"DimBase<1>::ctor("<<v<<")"<<endl;
    }

    T& operator[](int d) {
      //cout<<"operator[]<"<<1<<">(int "<<d<<")"<<endl;
      //return dim;
      return const_cast<T &>(static_cast<const DimBase<1,T>& >(*this)[d]);
    }

    const T& operator[](int d) const {
      return d==1 ? dim : throw out_of_range("Out of range");
    }

    T dim;
};

template <class T>
ostream& operator<<(ostream& out, const DimBase<1,T> &obj){
    out<<obj.dim;

  return out;
}

template <int D, class T>
class Dim : public DimBase<D,T>{
    public:
        Dim()    : DimBase<D,T>() {}
        Dim(T v) : DimBase<D,T>(v)   {}
};

template <class T>
class Dim<1,T> : public DimBase<1,T>{
    public:
        Dim()    : DimBase<1,T>() {}
        Dim(T v) : DimBase<1,T>(v)   {}
};

//template<class T>
//class DimBase<1,T> {
//  public:
//    T Y() {
//      return this->operator[](1);
//    }

