#pragma once

#include <vector>
#include <string_view>

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
inline char const* ast_type_name (AST_type type) {
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
