#pragma once

#include <string_view>

#include "ast.hpp"

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
