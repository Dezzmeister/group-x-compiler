#include <queue>

#include "ast.h"

struct UniqueNode {
    ASTNode * node;
    int id;
};

static struct UniqueNode with_id(ASTNode * node, int * id) {
    struct UniqueNode out = {
        .node = node,
        .id = *id
    };

    (*id)++;

    return out;
}

static std::string label(ASTNode * node) {
    if (node->get_kind() == Ident::kind) {
        Ident * ident = (Ident *) node;

        return ident->id;
    }

    if (node->get_kind() == TypeIdent::kind) {
        TypeIdent * ident = (TypeIdent *) node;

        return ident->id;
    }

    if (node->get_kind() == IntLiteral::kind) {
        IntLiteral * lit = (IntLiteral *) node;

        return std::to_string(lit->value);
    }

    if (node->get_kind() == FloatLiteral::kind) {
        FloatLiteral * lit = (FloatLiteral *) node;

        return std::to_string(lit->value);
    }

    return x::kind_map[node->get_kind()];
}

void x::tree_dotfile(std::ostream &out, ProgramSource * prog) {
    std::queue<struct UniqueNode> nodes;
    int id = 0;

    out << "digraph ExpressionTree {\n";

    nodes.push(with_id(prog, &id));

    while (! nodes.empty()) {
        struct UniqueNode node = nodes.front();
        nodes.pop();

        out << "    node" << node.id << " [label=\"" << label(node.node);
        out << "\"]" << std::endl;

        std::vector<ASTNode *> children = node.node->children();

        for (auto &item : children) {
            struct UniqueNode child = with_id(item, &id);

            out << "    node" << node.id << " -> node" << child.id << std::endl;

            nodes.push(child);
        }
    }

    out << "}" << std::endl;
}
