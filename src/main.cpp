#include <vector>
#include <string_view>
#include <iostream>

#define LIST_OF_VALUES \
	X(UNKNOWN)\
	X(S_EXPR)\
	X(IDENTIFIER)\
	X(STRING)\
	X(NUMBER)

#define X(name) name,
enum class AST_type {
	LIST_OF_VALUES
};
#undef X

#define X(name) case AST_type::name: return #name;
char const* ast_type_name (AST_type type) {
	switch (type) {
		LIST_OF_VALUES
	}
}
#undef X
#undef LIST_OF_VALUES

struct AST {
	AST_type type;
	std::string_view content;
	std::vector<AST*> children;
};

struct parse_result {
	bool success;
	AST* ast;
	std::string_view rest;
};

auto parse_expression (std::string_view text) -> parse_result;
auto parse_s_expr (std::string_view text) -> parse_result;
auto parse_number (std::string_view text) -> parse_result;
auto parse_string (std::string_view text) -> parse_result;
auto parse_identifier (std::string_view text) -> parse_result;

auto parse_expression (std::string_view text) -> parse_result {
	{
		if(text.size() == 0) {
			std::cerr << "Could not parse empty program\n";
			return {false, nullptr, text};
		}

		while(isspace(text[0]))
			text = text.substr(1);

		if(text.size() == 0) {
			std::cerr << "Could not parse empty program\n";
			return {false, nullptr, text};
		}
	}

	if(auto result = parse_identifier(text); result.success) return result;
	if(auto result = parse_s_expr    (text); result.success) return result;
	if(auto result = parse_string    (text); result.success) return result;
	if(auto result = parse_number    (text); result.success) return result;

	std::cerr << "Could not parse program\n";
	return {false, nullptr, text};
}

auto parse_s_expr (std::string_view text) -> parse_result {
	{
		if(text.size() == 0) {
			std::cerr << "Could not parse empty program\n";
			return {false, nullptr, text};
		}

		if (text[0] != '(') {
			std::cerr << "Could not parse s expression\n";
			return {false, nullptr, text};
		}
	}

	// consume '('
	text = text.substr(1);

	// consume whitespace after '('
	while (isspace(text[0]))
		text = text.substr(1);

	AST* ast = new AST{AST_type::S_EXPR, {}, {}};

	while (text.size() != 0 && text[0] != ')') {
		auto result = parse_expression(text);

		if (not result.success) {
			std::cerr << "Could not parse s expression\n";
			return {false, ast, text};
		}

		text = result.rest;
		ast->children.push_back(result.ast);

		while (isspace(text[0]))
			text = text.substr(1);
	}

	if (text[0] != ')') {
		std::cerr << "Could not parse s expression\n";
		return {false, ast, text};
	}

	// consume ')'
	text = text.substr(1);

	return {true, ast, text};
}

auto parse_number (std::string_view text) -> parse_result {
	{
		if(text.size() == 0) {
			std::cerr << "Could not parse empty number\n";
			return {false, nullptr, text};
		}

		if (not isdigit(text[0])) {
			std::cerr << "Could not parse number\n";
			return {false, nullptr, text};
		}
	}

	AST* ast = new AST{AST_type::NUMBER, {}, {}};

	int i = 0;
	while(isdigit(text[i])){
		i++;
	}

	ast->content = text.substr(0, i);
	text = text.substr(i);

	return {true, ast, text};
}

auto parse_identifier (std::string_view text) -> parse_result {
	{
		if(text.size() == 0) {
			std::cerr << "Could not parse empty identifier\n";
			return {false, nullptr, text};
		}

		if (not isalpha(text[0])) {
			std::cerr << "Could not parse identifier\n";
			return {false, nullptr, text};
		}
	}

	AST* ast = new AST{AST_type::IDENTIFIER, {}, {}};

	int i = 0;
	while(isalpha(text[i]) || isdigit(text[i])){
		i++;
	}

	ast->content = text.substr(0, i);
	text = text.substr(i);

	return {true, ast, text};
}

auto parse_string (std::string_view text) -> parse_result {
	{
		if(text.size() == 0) {
			std::cerr << "Could not parse empty number\n";
			return {false, nullptr, text};
		}

		if (text[0] != '"') {
			std::cerr << "Could not parse string\n";
			return {false, nullptr, text};
		}
	}

	AST* ast = new AST{AST_type::STRING, {}, {}};

	// consume opening '"'
	text = text.substr(1);

	int i = 0;
	while(text[i] != '"'){
		i++;
	}

	ast->content = text.substr(0, i);
	text = text.substr(i);

	// consume closing '"'
	text = text.substr(1);

	return {true, ast, text};
}


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
