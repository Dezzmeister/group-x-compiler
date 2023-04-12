#include <vector>

#include "parseutils.h"

ASTNode * x::top = nullptr;

void x::parse_math_op_expr(MathExpr ** res, MathExpr * expr, char op, NumLiteral * num) {
    if (expr->get_kind() == MathOp::kind) {
        MathOp * math_op = (MathOp *) expr;

        if (math_op->op == op) {
            math_op->push_operand(num);
            *res = math_op;
            return;
        }
    }

    *res = new MathOp(op, {expr, num});
}
