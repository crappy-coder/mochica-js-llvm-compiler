#ifndef MOENJIN_AST_ASTUTILS_H
#define MOENJIN_AST_ASTUTILS_H

#include "Platform/Configuration.h"
#include "AST/Node.h"
#include "AST/NodeType.h"

#define AST_IS_LOCATION_NODE(node) (node->GetType() == NODE_RESOLVE || node->GetType() == NODE_BRACKET_ACCESSOR || node->GetType() == NODE_DOT_ACCESSOR)
#define AST_IS_RESOLVE_NODE(node) (node->GetType() == NODE_RESOLVE)
#define AST_IS_BRACKET_ACCESSOR_NODE(node) (node->GetType() == NODE_BRACKET_ACCESSOR)
#define AST_IS_DOT_ACCESSOR_NODE(node) (node->GetType() == NODE_DOT_ACCESSOR)
#define AST_IS_FUNCTION_EXPRESSION_NODE(node) (node->GetType() == NODE_FUNCTION_EXPR)
#define AST_IS_NULL_NODE(node) (node->GetType() == NODE_NULL)
#define AST_IS_NUMBER_NODE(node) (node->GetType() == NODE_NUMBER)
#define AST_IS_BOOLEAN_NODE(node) (node->GetType() == NODE_BOOLEAN)
#define AST_IS_STRING_NODE(node) (node->GetType() == NODE_STRING)
#define AST_IS_PRIMITIVE_NODE(node) (node->GetType() == NODE_BOOLEAN || node->GetType() == NODE_NUMBER || node->GetType() == NODE_STRING)
#define AST_IS_PRIMITIVE_OR_NULL_NODE(node) (node->GetType() == NODE_BOOLEAN || node->GetType() == NODE_NUMBER || node->GetType() == NODE_STRING || node->GetType() == NODE_NULL)
#define AST_IS_COMMA_NODE(node) (node->GetType() == NODE_COMMA)
#define AST_IS_UNARY_PLUS_NODE(node) (node->GetType() == NODE_UNARY_PLUS)

namespace MoEnjin
{
	class Expression;

	extern mo_bool CanExpressionFoldToBoolean(Expression* expr, mo_bool& constBoolValue);
}

#endif