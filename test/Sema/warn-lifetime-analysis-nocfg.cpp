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
  MyPointer release();
  int *release2();
  int *c_str();
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

MyPointer g2() {
  T t;
  return t.release(); // ok
}

int *g3() {
  T t;
  return t.release2(); // ok
}

int *g4() {
  T t;
  return t.c_str(); // expected-warning {{address of stack memory associated with local variable 't' returned}}
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
  S(int i) : p(&i) {} // expected-warning {{initializing pointer member 'p' with the stack address of parameter 'i'}}
  S() : p(T{}) {} // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
  S(double) : p(MyOwner{}) {} // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
};

MyPointer i() {
  int j;
  return &j; // expected-warning {{address of stack memory associated with local variable 'j' returned}}
}

MyPointer i2() {
  T t;
  return t; // expected-warning {{address of stack memory associated with local variable 't' returned}}
            // TODO: other warning message, the address of the object pointed by 't' is returned.
}

MyPointer i3() {
  return T{}; // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
              // expected-warning@-1 {{returning address of local temporary object}}
}

MyPointer global;

void j() {
  MyPointer p = MyOwner{}; // expected-warning {{object backing the pointer will be destroyed at the end of the full-expression}}
  p = MyOwner{}; // TODO ?
  global = MyOwner{}; // TODO ?
  p = T{}; // TODO ?
  global = T{}; // TODO ?
}

struct IntVector {
  int *begin();
  int *end();
};

void future_work() {
  int *it = IntVector{}.begin(); // TODO ?
  (void)it;
}
