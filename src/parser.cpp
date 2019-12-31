#include "parser.hpp"

#include <cctype>

auto parse_expression (std::string_view text) -> parse_result {
	{
		if(text.size() == 0) {
			return {false, nullptr, text};
		}

		while(isspace(text[0]))
			text = text.substr(1);

		if(text.size() == 0) {
			return {false, nullptr, text};
		}
	}

	if(auto result = parse_identifier(text); result.success) return result;
	if(auto result = parse_s_expr    (text); result.success) return result;
	if(auto result = parse_string    (text); result.success) return result;
	if(auto result = parse_number    (text); result.success) return result;

	return {false, nullptr, text};
}

auto parse_s_expr (std::string_view text) -> parse_result {
	{
		if(text.size() == 0) {
			return {false, nullptr, text};
		}

		if (text[0] != '(') {
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
			return {false, ast, text};
		}

		text = result.rest;
		ast->children.push_back(result.ast);

		while (isspace(text[0]))
			text = text.substr(1);
	}

	if (text[0] != ')') {
		return {false, ast, text};
	}

	// consume ')'
	text = text.substr(1);

	return {true, ast, text};
}

auto parse_number (std::string_view text) -> parse_result {
	{
		if(text.size() == 0) {
			return {false, nullptr, text};
		}

		if (not isdigit(text[0])) {
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
			return {false, nullptr, text};
		}

		if (not isalpha(text[0])) {
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
			return {false, nullptr, text};
		}

		if (text[0] != '"') {
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
