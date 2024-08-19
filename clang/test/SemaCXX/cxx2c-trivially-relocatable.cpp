// RUN: %clang_cc1 -std=c++2c -verify %s

class Trivial {};
struct NonRelocatable {
    ~NonRelocatable();
};
static NonRelocatable NonRelocatable_g;

class A memberwise_trivially_relocatable {};
class B memberwise_trivially_relocatable : Trivial{};
class C memberwise_trivially_relocatable {
    int a;
    void* b;
    int c[3];
    Trivial d[3];
    NonRelocatable& e = NonRelocatable_g;
};
class D memberwise_trivially_relocatable : Trivial {};
class E memberwise_trivially_relocatable : virtual Trivial {};
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'E'}}
// expected-note@-2  {{because it has a virtual base class 'Trivial'}}

class F memberwise_trivially_relocatable : NonRelocatable {};
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'F'}}
// expected-note@-2  {{because it inherits from a non trivially-relocatable class 'NonRelocatable'}}

class I memberwise_trivially_relocatable {
    NonRelocatable a;
    NonRelocatable b[1];
    const NonRelocatable c;
    const NonRelocatable d[1];
};

class J memberwise_trivially_relocatable:  virtual Trivial, NonRelocatable {
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'J'}}
// expected-note@-2 {{because it has a virtual base class 'Trivial'}}
// expected-note@-3 {{because it inherits from a non trivially-relocatable class 'NonRelocatable'}}
    NonRelocatable a;
};

class G memberwise_trivially_relocatable {
    G(G&&);
};

class H memberwise_trivially_relocatable {
    ~H();
};

struct Incomplete; // expected-note {{forward declaration of 'Incomplete'}}
static_assert(__is_cpp_trivially_relocatable(Incomplete));  // expected-error {{incomplete type 'Incomplete' used in type trait expression}}
static_assert(__is_cpp_trivially_relocatable(Trivial));
static_assert(__is_cpp_trivially_relocatable(G));
static_assert(__is_cpp_trivially_relocatable(H));
static_assert(__is_cpp_trivially_relocatable(int));
static_assert(__is_cpp_trivially_relocatable(void*));
static_assert(!__is_cpp_trivially_relocatable(int&));
static_assert(!__is_cpp_trivially_relocatable(Trivial&));
static_assert(__is_cpp_trivially_relocatable(const Trivial));
static_assert(__is_cpp_trivially_relocatable(Trivial[1]));

struct UserDtr {
    ~UserDtr();
};

struct DefaultedDtr {
    ~DefaultedDtr() = default;
};
struct UserMoveWithDefaultCopy {
    UserMoveWithDefaultCopy(UserMoveWithDefaultCopy&&);
    UserMoveWithDefaultCopy(const UserMoveWithDefaultCopy&) = default;
};

struct UserMove{
    UserMove(UserMove&&);
};

struct UserMoveDefault{
    UserMoveDefault(UserMoveDefault&&) = default;
};

struct UserCopy{
    UserCopy(const UserCopy&);
};

struct UserCopyDefault{
    UserCopyDefault(const UserCopyDefault&) = default;
};


struct UserDeletedMove{
    UserDeletedMove(UserDeletedMove&) = delete;
    UserDeletedMove(const UserDeletedMove&) = default;
};

static_assert(!__is_cpp_trivially_relocatable(UserDtr));
static_assert(__is_cpp_trivially_relocatable(DefaultedDtr));
static_assert(!__is_cpp_trivially_relocatable(UserMoveWithDefaultCopy));
static_assert(!__is_cpp_trivially_relocatable(UserMove));
static_assert(!__is_cpp_trivially_relocatable(UserCopy));
static_assert(__is_cpp_trivially_relocatable(UserMoveDefault));
static_assert(__is_cpp_trivially_relocatable(UserCopyDefault));
static_assert(__is_cpp_trivially_relocatable(UserDeletedMove));

template <typename T>
class TestDependentErrors memberwise_trivially_relocatable : T {};
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'TestDependentErrors<NonRelocatable>'}}
// expected-note@-2 {{because it inherits from a non trivially-relocatable class 'NonRelocatable'}}
TestDependentErrors<Trivial> Ok;
TestDependentErrors<NonRelocatable> Err; // expected-note {{in instantiation of template class}}

struct DeletedMove {
    DeletedMove(DeletedMove&&) = delete;
};
struct DeletedCopy {
    DeletedCopy(const DeletedCopy&) = delete;
};
struct DeletedMoveAssign {
    DeletedMoveAssign& operator=(DeletedMoveAssign&&) = delete;
};

static_assert(!__is_cpp_trivially_relocatable(DeletedMove));
static_assert(!__is_cpp_trivially_relocatable(DeletedCopy));
static_assert(!__is_cpp_trivially_relocatable(DeletedMoveAssign));



namespace replaceable {

struct DeletedMove {
    DeletedMove(DeletedMove&&) = delete;
};
struct DeletedCopy {
    DeletedCopy(const DeletedCopy&) = delete;
};
struct DeletedMoveAssign {
    DeletedMoveAssign& operator=(DeletedMoveAssign&&) = delete;
};

struct DefaultedMove {
    DefaultedMove(DefaultedMove&&) = default;
    DefaultedMove& operator=(DefaultedMove&&) = default;
};
struct DefaultedCopy {
    DefaultedCopy(const DefaultedCopy&) = default;
    DefaultedCopy(DefaultedCopy&&) = default;
    DefaultedCopy& operator=(DefaultedCopy&&) = default;
};
struct DefaultedMoveAssign {
    DefaultedMoveAssign(DefaultedMoveAssign&&) = default;
    DefaultedMoveAssign& operator=(DefaultedMoveAssign&&) = default;
};

struct UserProvidedMove {
    UserProvidedMove(UserProvidedMove&&){};
};
struct UserProvidedCopy {
    UserProvidedCopy(const UserProvidedCopy&) {};
};
struct UserProvidedMoveAssign {
    UserProvidedMoveAssign& operator=(const UserProvidedMoveAssign&){return *this;};
};

struct Empty{};
static_assert(__builtin_is_replaceable(Empty));
struct S1 memberwise_replaceable{};
static_assert(__builtin_is_replaceable(S1));

static_assert(__builtin_is_replaceable(DefaultedMove));
static_assert(__builtin_is_replaceable(DefaultedCopy));
static_assert(__builtin_is_replaceable(DefaultedMoveAssign));

static_assert(!__builtin_is_replaceable(DeletedMove));
static_assert(!__builtin_is_replaceable(DeletedCopy));
static_assert(!__builtin_is_replaceable(DeletedMoveAssign));

static_assert(!__builtin_is_replaceable(UserProvidedMove));
static_assert(!__builtin_is_replaceable(UserProvidedCopy));
static_assert(!__builtin_is_replaceable(UserProvidedMoveAssign));

using NotReplaceable = DeletedMove;

template <typename T>
struct S {
    T t;
};

template <typename T>
struct WithBase : T{};

template <typename T>
struct WithVBase : virtual T{};

struct WithVirtual {
    virtual ~WithVirtual() = default;
    WithVirtual(WithVirtual&&) = default;
    WithVirtual& operator=(WithVirtual&&) = default;
};

static_assert(__builtin_is_replaceable(S<int>));
static_assert(__builtin_is_replaceable(S<volatile int>));
static_assert(!__builtin_is_replaceable(S<const int>));
static_assert(!__builtin_is_replaceable(S<const int&>));
static_assert(!__builtin_is_replaceable(S<int&>));
static_assert(__builtin_is_replaceable(S<int[2]>));
static_assert(!__builtin_is_replaceable(S<const int[2]>));
static_assert(__builtin_is_replaceable(WithBase<S<int>>));
static_assert(!__builtin_is_replaceable(WithBase<S<const int>>));
static_assert(!__builtin_is_replaceable(WithBase<UserProvidedMove>));
static_assert(!__builtin_is_replaceable(WithVBase<S<int>>));
static_assert(!__builtin_is_replaceable(WithVBase<S<const int>>));
static_assert(!__builtin_is_replaceable(WithVBase<UserProvidedMove>));
static_assert(__builtin_is_replaceable(WithVirtual));


struct U1 memberwise_replaceable {
    ~U1() = delete;
    U1(U1&&) = default;
    U1& operator=(U1&&) = default;

};
static_assert(__builtin_is_replaceable(U1));

struct U2 memberwise_replaceable {
// expected-error@-1 {{invalid 'memberwise_replaceable' specifier on non memberwise replaceable class 'U2'}}
// expected-note@-2 {{because it has a deleted move constructor or assignment operator}}
    U2(const U2&) = delete;
};
static_assert(!__builtin_is_replaceable(U2));


template <typename T>
struct WithVBaseExplicit memberwise_replaceable : virtual T{};
// expected-error@-1 {{invalid 'memberwise_replaceable' specifier on non memberwise replaceable class 'WithVBaseExplicit<replaceable::S<int>>'}} \
// expected-note@-1 {{because it has a virtual base class 'S<int>'}}
static_assert(__builtin_is_replaceable(WithVBaseExplicit<S<int>>)); // expected-error {{failed}} \
                                                                    // expected-note {{requested here}}

}
