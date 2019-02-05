// RUN: %clang_cc1 -fsyntax-only -verify -Wdangling-lifetime-pointer %s
struct [[gsl::Pointer]] MyPointer {
  MyPointer(int *p = 0);
  int &operator*();
};

struct T {
  operator MyPointer(); 
};

void f() {
  new MyPointer(T{}); // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
}

void g() {
  int i;
  MyPointer p{&i};
  new MyPointer(MyPointer{p});
}

