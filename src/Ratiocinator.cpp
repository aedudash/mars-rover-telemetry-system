// Ratiocinator.cpp
// Author: Aidan Dudash
// Project: CSIS3700 Project 4
// Purpose: Implementation of argument parsing and 3-valued evaluation
//          using lab syntax: ~, *, +, :-

#include "Ratiocinator.h"

#include <cctype>
#include <stack>
#include <sstream>
#include <ostream>

// Trim leading and trailing whitespace from a string.
std::string Ratiocinator::trim(const std::string& s) {
    std::size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return std::string();
    std::size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

void Ratiocinator::setTruth(const std::string& label, TruthValue tv) {
    tau_[label] = tv;
}

TruthValue Ratiocinator::truthOf(const std::string& label) const {
    auto it = tau_.find(label);
    if (it == tau_.end()) return TruthValue::Unknown;
    return it->second;
}

// Load named arguments of the form `X = expr` from a stream.
// Validates tokens and stores both infix and postfix forms.
bool Ratiocinator::loadArguments(std::istream& in) {
    args_.clear();

    std::string line;
    while (std::getline(in, line)) {
        std::string t = trim(line);
        if (t.empty()) continue;
        if (t[0] == '#') continue;  // comment line

        // Expect "X = expr"
        std::size_t eqPos = t.find('=');
        if (eqPos == std::string::npos) return false;

        std::string namePart = trim(t.substr(0, eqPos));
        std::string exprPart = trim(t.substr(eqPos + 1));
        if (namePart.empty() || exprPart.empty()) return false;

        Argument arg;
        arg.name  = namePart;
        arg.infix = exprPart;

        auto tokens = tokenizeExpression(exprPart);

        // Validate tokens: only P..T, parentheses, and operators ~ * + :-
        for (const auto& tok : tokens) {
            if (tok == "(" || tok == ")" ||
                tok == "~" || tok == "*" || tok == "+" || tok == ":-") {
                continue;
            }
            if (!isPropositionLabel(tok)) {
                return false;
            }
        }

        arg.postfix = infixToPostfix(tokens);
        arg.value   = TruthValue::Unknown;
        args_.push_back(arg);
    }

    return true;
}

// Evaluate all loaded arguments using the current τ mapping.
void Ratiocinator::evaluateArguments() {
    for (auto& arg : args_) {
        std::stack<TruthValue> st;
        arg.value = TruthValue::Unknown;

        for (const auto& tok : arg.postfix) {
            if (isPropositionLabel(tok)) {
                st.push(truthOf(tok));
            } else if (tok == "~") {
                if (st.empty()) { arg.value = TruthValue::Unknown; break; }
                TruthValue a = st.top(); st.pop();
                st.push(tvNot(a));
            } else if (tok == "*" || tok == "+" || tok == ":-") {
                if (st.size() < 2) { arg.value = TruthValue::Unknown; break; }
                TruthValue b = st.top(); st.pop();
                TruthValue a = st.top(); st.pop();
                TruthValue r = TruthValue::Unknown;
                if (tok == "*")      r = tvAnd(a, b);
                else if (tok == "+") r = tvOr(a, b);
                else                 r = tvImplies(a, b);
                st.push(r);
            } else {
                // Unknown token should not happen if loadArguments validated tokens.
                arg.value = TruthValue::Unknown;
                break;
            }
        }

        if (!st.empty()) {
            arg.value = st.top();
        }
        // If stack is empty and we never set value, it stays Unknown.
    }
}

TruthValue Ratiocinator::truthOfArgument(const std::string& name) const {
    for (const auto& a : args_) {
        if (a.name == name) return a.value;
    }
    return TruthValue::Unknown;
}

// Break an expression string into tokens: proposition labels, parentheses,
// and operators (~, *, +, :-).
std::vector<std::string> Ratiocinator::tokenizeExpression(const std::string& expr) {
    std::vector<std::string> tokens;
    std::string current;

    auto flushCurrent = [&]() {
        if (!current.empty()) {
            tokens.push_back(current);
            current.clear();
        }
    };

    for (std::size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];

        if (std::isspace(static_cast<unsigned char>(c))) {
            flushCurrent();
            continue;
        }

        // Parentheses and 1-char operators
        if (c == '(' || c == ')' || c == '~' || c == '*' || c == '+') {
            flushCurrent();
            tokens.push_back(std::string(1, c));
            continue;
        }

        // ":-" operator
        if (c == ':' && i + 1 < expr.size() && expr[i + 1] == '-') {
            flushCurrent();
            tokens.push_back(":-");
            ++i; // skip '-'
            continue;
        }

        // Part of an identifier (e.g., P, Q, R...).
        current.push_back(c);
    }

    flushCurrent();
    return tokens;
}

// Convert from infix to postfix using a simple stack-based
// operator-precedence pass, with our custom precedence/associativity rules.
std::vector<std::string> Ratiocinator::infixToPostfix(const std::vector<std::string>& tokens) {
    std::vector<std::string> output;
    std::stack<std::string>  ops;

    for (const auto& tok : tokens) {
        if (isPropositionLabel(tok)) {
            output.push_back(tok);
        } else if (tok == "(") {
            ops.push(tok);
        } else if (tok == ")") {
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top());
                ops.pop();
            }
            if (!ops.empty() && ops.top() == "(") {
                ops.pop();
            }
        } else if (tok == "~" || tok == "*" || tok == "+" || tok == ":-") {
            while (!ops.empty() && ops.top() != "(") {
                int pTop = precedence(ops.top());
                int pTok = precedence(tok);
                if (pTop > pTok || (pTop == pTok && !isRightAssociative(tok))) {
                    output.push_back(ops.top());
                    ops.pop();
                } else {
                    break;
                }
            }
            ops.push(tok);
        }
    }

    while (!ops.empty()) {
        if (ops.top() != "(" && ops.top() != ")") {
            output.push_back(ops.top());
        }
        ops.pop();
    }

    return output;
}

// Our syntax only allows single-letter labels P..T.
bool Ratiocinator::isPropositionLabel(const std::string& tok) {
    if (tok.size() != 1) return false;
    char c = tok[0];
    return c >= 'P' && c <= 'T';  // P, Q, R, S, T only
}

// Operator precedence: highest to lowest: ~, *, +, :-
int Ratiocinator::precedence(const std::string& op) {
    if (op == "~")   return 3;
    if (op == "*")   return 2;
    if (op == "+")   return 1;
    if (op == ":-")  return 0;
    return -1;
}

// NOT and IMPLIES are right-associative; AND/OR are left-associative.
bool Ratiocinator::isRightAssociative(const std::string& op) {
    return (op == "~" || op == ":-");
}

// 3-valued NOT.
TruthValue Ratiocinator::tvNot(TruthValue a) {
    switch (a) {
    case TruthValue::True:    return TruthValue::False;
    case TruthValue::False:   return TruthValue::True;
    case TruthValue::Unknown: return TruthValue::Unknown;
    }
    return TruthValue::Unknown;
}

// 3-valued AND: False dominates, Unknown propagates, otherwise True.
TruthValue Ratiocinator::tvAnd(TruthValue a, TruthValue b) {
    if (a == TruthValue::False || b == TruthValue::False) return TruthValue::False;
    if (a == TruthValue::Unknown || b == TruthValue::Unknown) return TruthValue::Unknown;
    return TruthValue::True;
}

// 3-valued OR: True dominates, Unknown propagates, otherwise False.
TruthValue Ratiocinator::tvOr(TruthValue a, TruthValue b) {
    if (a == TruthValue::True || b == TruthValue::True) return TruthValue::True;
    if (a == TruthValue::Unknown || b == TruthValue::Unknown) return TruthValue::Unknown;
    return TruthValue::False;
}

// 3-valued IMPLIES using the identity a -> b ≡ ¬a ∨ b.
TruthValue Ratiocinator::tvImplies(TruthValue a, TruthValue b) {
    return tvOr(tvNot(a), b);
}

// --- stream inserters ---

std::ostream& operator<<(std::ostream& os, TruthValue tv) {
    switch (tv) {
    case TruthValue::True:    os << "T"; break;
    case TruthValue::False:   os << "F"; break;
    case TruthValue::Unknown: os << "?"; break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Argument& arg) {
    os << arg.name << " = " << arg.infix << "  => " << arg.value;
    return os;
}
