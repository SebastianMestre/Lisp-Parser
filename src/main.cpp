#include <iostream>

#include "ast.hpp"
#include "parser.hpp"

void print_ast (AST* ast, int nesting = 0) {
	for(int i = 0; i < nesting; ++i) std::cout << "  ";

	if(not ast) {
		std::cout << "(null)";
	} else {
		std::cout << "[" << ast_type_name(ast->type) << "]";

		if(ast->type == AST_type::S_EXPR){
			std::cout << "(\n";
			for(AST* child : ast->children){
				print_ast(child, nesting+1);
			}
			for(int i = 0; i < nesting; ++i) std::cout << "  ";
			std::cout << ")";
		} else {
			std::cout << ast->content;
		}
	}
	std::cout << '\n';
}

int main () {
	auto source = R"lisp(
		(
			(define val 58)
			val
			((id link) "hello" "hello.html")
		)
	)lisp";

	auto result = parse_expression(source);

	print_ast(result.ast);
}
