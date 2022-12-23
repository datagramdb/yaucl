//
// Created by giacomo on 22/12/22.
//

#ifndef AIRLINE_ANTLR4_HELPER_H
#define AIRLINE_ANTLR4_HELPER_H

#ifdef _MSC_VER
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#endif

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#ifdef ERROR
#undef ERROR
#endif

#define ANTLR4_HELPER_BEGIN(classname, original) class classname : public original ## Visitor { public: void parse( std::istream& query); private:

#define ANTLR4_HELPER_END  };

#define ANTLR_HELPER_IMPLEMENTATION(classname, original, initialiser) void classname ##:: parse( std::istream& query) {\
        antlr4::ANTLRInputStream input(query);\
        original ## Lexer lexer(&input);\
        antlr4::CommonTokenStream tokens(&lexer);\
        original ## Parser parser(&tokens);\
        (parser.  initialiser ) ->accept(this);\
    }\

#include <any>

template<class...Ts>
bool is_any_of_types( std::any const& a ) {
    return (( a.type() == typeid(Ts) ) || ... );
}

#endif //AIRLINE_ANTLR4_HELPER_H
