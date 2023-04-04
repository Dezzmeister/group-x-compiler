#ifndef SRC_AST_H
#define SRC_AST_H

class ASTNode {
    public:
        virtual int get_kind() = 0;
};

class IntLiteral : public ASTNode {
    public:
        const static int kind = 1;

        const int value;
        
        IntLiteral(const int value);

        int get_kind() {
            return kind;
        }
};

class FloatLiteral : public ASTNode {
    public:
        const static int kind = 2;
        
        const float value;

        FloatLiteral(const float value);

        int get_kind() {
            return kind;
        }
};

#endif
