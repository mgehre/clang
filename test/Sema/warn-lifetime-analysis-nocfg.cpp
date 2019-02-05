// RUN: %clang_cc1 -fsyntax-only -verify -Wdangling-lifetime-pointer -Waddress-of-temporary %s
struct [[gsl::Owner]] MyOwner {
  int &operator*();
};

struct [[gsl::Pointer]] MyPointer {
  MyPointer(int *p = 0);
  MyPointer(const MyOwner &);
  int &operator*();
};

struct T {
  operator MyPointer(); 
};

void f() {
  new MyPointer(T{}); // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
  new MyPointer(MyOwner{}); // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
}

void g() {
  int i;
  MyPointer p{&i}; // no-warning
  new MyPointer(MyPointer{p}); // no-warning
}

struct Y {
  int a[4];
};

void h() {
  MyPointer p = Y{}.a; // expected-warning{{pointer is initialized by a temporary array}}
  (void)p;
}

struct S {
  MyPointer p; // expected-note {{pointer member declared here}}
  S(int i)
    : p(&i) {} // expected-warning {{initializing pointer member 'p' with the stack address of parameter 'i'}}
  S() : p(T{}) {}
  S(double) : p(MyOwner{}) {} // TODO
};

MyPointer i() {
  int j;
  return &j; // TODO
}

MyPointer global;

void j() {
  MyPointer p = MyOwner{}; // TODO
  p = MyOwner{}; // TODO ?
  global = MyOwner{}; // TODO ?
}
