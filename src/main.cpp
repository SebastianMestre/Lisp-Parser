#include <vector>
#include <string>
#include <string_view>
#include <iostream>

struct AST {
	std::string type;
	std::string content;
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

	AST* ast = new AST{"s_expr", "", {}};

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

	AST* ast = new AST{"number", "", {}};

	while(isdigit(text[0])){
		ast->content.push_back(text[0]);
		text = text.substr(1);
	}

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

	AST* ast = new AST{"identifier", "", {}};

	while(isalpha(text[0]) || isdigit(text[0])){
		ast->content.push_back(text[0]);
		text = text.substr(1);
	}

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

	AST* ast = new AST{"string", "", {}};

	// consume opening '"'
	text = text.substr(1);

	while(text[0] != '"'){
		ast->content.push_back(text[0]);
		text = text.substr(1);
	}

	// consume closing '"'
	text = text.substr(1);

	return {true, ast, text};
}

void print_ast (AST* ast, int nesting = 0) {
	for(int i = 0; i < nesting; ++i) std::cout << "  ";

	if(not ast) {
		std::cout << "(null)";
	} else if(ast->type == "s_expr"){
		std::cout << "[s_expr](\n";
		for(AST* child : ast->children){
			print_ast(child, nesting+1);
		}
		for(int i = 0; i < nesting; ++i) std::cout << "  ";
		std::cout << ")";
	} else {
		std::cout << "[" << ast->type << "]" << ast->content;
	}
	std::cout << '\n';
}

int main () {
#if 1
	auto source = R"lisp(
		(
			(define val 58)
			val
			((id link) "hello" "hello.html")
		)
	)lisp";
#endif

	auto result = parse_expression(source);
	print_ast(result.ast);
}
