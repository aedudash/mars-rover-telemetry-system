// Ratiocinator.h
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Parse and evaluate arguments (X, Y, Z) over P–T using 3-valued logic.
//
// Syntax (from argument.txt):
//   ~  : NOT
//   *  : AND
//   +  : OR
//   :- : IMPLIES

#pragma once

#include <map>
#include <string>
#include <vector>
#include <iosfwd>

enum class TruthValue {
    False,
    True,
    Unknown
};

struct Argument {
    std::string              name;     // "X", "Y", "Z", etc.
    std::string              infix;    // original expression text
    std::vector<std::string> postfix;  // tokens in postfix order
    TruthValue               value = TruthValue::Unknown;
};

class Ratiocinator {
private:
    std::map<std::string, TruthValue> tau_;   // τ(P..T)
    std::vector<Argument>             args_;  // X, Y, Z ...

public:
    Ratiocinator() = default;

    // PRE: label in {P,Q,R,S,T} ideally, but any string allowed
    // POST: sets τ(label) = tv
    void setTruth(const std::string& label, TruthValue tv);

    // PRE: none
    // POST: returns τ(label) if present, otherwise Unknown
    TruthValue truthOf(const std::string& label) const;

    // PRE: 'in' is an open stream for the argument file (X, Y, Z)
    // POST: parses all arguments; returns false on syntax error
    bool loadArguments(std::istream& in);

    // PRE: loadArguments has succeeded
    // POST: evaluates all arguments using τ; stores result in Argument::value
    void evaluateArguments();

    // PRE: none
    // POST: returns evaluated truth of named argument (e.g., "X"), or Unknown if not found
    TruthValue truthOfArgument(const std::string& name) const;

    // PRE: none
    // POST: returns const reference to internal argument list for reporting
    const std::vector<Argument>& arguments() const { return args_; }

private:
    // helpers
    static std::string trim(const std::string& s);

    // Tokenize an infix expression using ~, *, +, :-, parentheses, and labels P..T
    static std::vector<std::string> tokenizeExpression(const std::string& expr);

    // Shunting-yard: infix → postfix
    static std::vector<std::string> infixToPostfix(const std::vector<std::string>& tokens);

    // P..T only
    static bool isPropositionLabel(const std::string& tok);

    // Operator helpers for *, +, :-, ~
    static int  precedence(const std::string& op);
    static bool isRightAssociative(const std::string& op);

    // 3-valued connectives
    static TruthValue tvNot(TruthValue a);
    static TruthValue tvAnd(TruthValue a, TruthValue b);
    static TruthValue tvOr(TruthValue a, TruthValue b);
    static TruthValue tvImplies(TruthValue a, TruthValue b);
};

// PRE: none
// POST: streams a compact representation of a TruthValue ("T", "F", "?")
std::ostream& operator<<(std::ostream& os, TruthValue tv);

// PRE: none
// POST: streams a human-readable description of the argument and its truth value
std::ostream& operator<<(std::ostream& os, const Argument& arg);
