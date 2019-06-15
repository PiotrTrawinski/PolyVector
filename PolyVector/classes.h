#pragma once
#include <iostream>

struct Base {
    Base(int data) : data(data) {}
    //virtual ~Base() { std::cout << "Base destructor\n"; }
    virtual int getData() { return data; }
    virtual void setData(int value) { data = value; }
protected:
    int data;
};
struct Derived : public Base {
    Derived(int data, int asdf) : Base(data), asdf(asdf) {}
    //virtual ~Derived() { std::cout << "Derived destructor\n"; }
    int getData() override { return data + asdf; }
    void setData(int value) override { asdf = value; }
    int asdf;
};
struct Derived2 : public Base {
    Derived2(int data, int asdf) : Base(data), asdf(asdf) {}
    //virtual ~Derived2() { std::cout << "Derived2 destructor\n"; }
    int getData() override { return data + asdf; }
    void setData(int value) override { asdf = value; }
    int asdf;
};
struct Derived3 : public Base {
    Derived3(int data, int asdf) : Base(data), asdf(asdf) {}
    //virtual ~Derived3() { std::cout << "Derived3 destructor\n"; }
    int getData() override { return data + asdf; }
    void setData(int value) override { asdf = value; }
    int asdf;
};
struct Derived4 : public Base {
    Derived4(int data, int asdf) : Base(data), asdf(asdf) {}
    //virtual ~Derived4() { std::cout << "Derived4 destructor\n"; }
    int getData() override { return data + asdf; }
    void setData(int value) override { asdf = value; }
    int asdf;
};