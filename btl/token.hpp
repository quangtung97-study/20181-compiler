enum class Token {
    Ident, Number,

    Begin, Call, Const, Do, Else, End, For, 
    If, Odd, Procedure, Function,
    Program, Then, To, 
    Var, While, 

    Plus, Minus, Times, Divide, Remainder,

    EQ, NE, LT, GT, LE, GE,

    LParent, RParent, 
    LBracket, RBracket,

    Period, Comma, Semicolon, Assign,
};

#define MAX_DIGIT_COUNT 9
#define IDENT_NAME_SIZE 10

static const char *token_to_string(Token token) {
    switch (token) {
        case Token::Ident: return "Ident";
        case Token::Number: return "Number";

        case Token::Begin:  return "Begin";
        case Token::Call:   return "Call";
        case Token::Const:  return "Const";
        case Token::Do:     return "Do";
        case Token::Else:   return "Else";
        case Token::End:    return "End";
        case Token::For:    return "For";
        case Token::If:     return "If";
        case Token::Odd:    return "Odd";
        case Token::Procedure: return "Procedure";
        case Token::Function: return "Function";
        case Token::Program: return "Program";
        case Token::Then:   return "Then";
        case Token::To:     return "To";
        case Token::Var:    return "Var";
        case Token::While:  return "While";

        case Token::Plus: return "+";
        case Token::Minus: return "-";
        case Token::Times: return "*";
        case Token::Divide: return "/";
        case Token::Remainder: return "%";

        case Token::EQ: return "=";
        case Token::NE: return "<>";
        case Token::LT: return "<";
        case Token::GT: return ">";
        case Token::LE: return "<=";
        case Token::GE: return ">=";

        case Token::LParent: return "(";
        case Token::RParent: return ")";
        case Token::LBracket: return "[";
        case Token::RBracket: return "]";

        case Token::Period: return ".";
        case Token::Comma: return ",";
        case Token::Semicolon: return ";";
        case Token::Assign: return ":=";
    }
}
