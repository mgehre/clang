// RUN: %clang_cc1 -fsyntax-only -Wdangling-lifetime-pointer -Waddress-of-temporary -verify %s
struct [[gsl::Owner]] MyOwner {
  int &operator*();
};

struct [[gsl::Pointer]] MyPointer {
  MyPointer(int *p = 0);
  MyPointer(const MyOwner &);
  int &operator*();
};

struct [[gsl::Owner]] T {
  operator MyPointer(); 
  int &operator*();
};

void f() {
  new MyPointer(T{}); // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
  new MyPointer(MyOwner{}); // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
}

void g() {
  int i;
  MyPointer p{&i}; // ok
  new MyPointer(MyPointer{p}); // ok
}

struct Y {
  int a[4];
};

void h() {
  MyPointer p = Y{}.a; // expected-warning {{pointer is initialized by a temporary array}}
  (void)p;
}

struct S {
  MyPointer p; // expected-note {{pointer member declared here}}
  S(int i)
    : p(&i) {} // expected-warning {{initializing pointer member 'p' with the stack address of parameter 'i'}}
  S() : p(T{}) {} // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
  S(double) : p(MyOwner{}) {} // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
};

MyPointer i() {
  int j;
  return &j; // TODO
}

MyPointer global;

void j() {
  MyPointer p = MyOwner{}; // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
  p = MyOwner{}; // TODO ?
  global = MyOwner{}; // TODO ?
}
