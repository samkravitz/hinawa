#pragma once

#include "../value.h"

namespace js
{
class BlockStmt;
class VariableStmt;
class EmptyStmt;
class IfStmt;
class ReturnStmt;
class ExpressionStmt;
class FunctionDecl;

class UnaryExpr;
class BinaryExpr;
class CallExpr;
class Literal;

class StmtVisitor
{
public:
	virtual void visit(const BlockStmt *) const = 0;
	virtual void visit(const VariableStmt *) const = 0;
	virtual void visit(const EmptyStmt *) const = 0;
	virtual void visit(const IfStmt *) const = 0;
	virtual void visit(const ReturnStmt *) const = 0;
	virtual void visit(const ExpressionStmt *) const = 0;
	virtual void visit(const FunctionDecl *) const = 0;
};

class ExprVisitor
{
public:
	virtual Value visit(const UnaryExpr *) const = 0;
	virtual Value visit(const BinaryExpr *) const = 0;
	virtual Value visit(const CallExpr *) const = 0;
	virtual Value visit(const Literal *) const = 0;
};

class PrintVisitor
{
public:
	virtual void visit(const BlockStmt *, std::string const & prefix) const = 0;
	virtual void visit(const VariableStmt *, std::string const & prefix) const = 0;
	virtual void visit(const EmptyStmt *, std::string const & prefix) const = 0;
	virtual void visit(const IfStmt *, std::string const & prefix) const = 0;
	virtual void visit(const ReturnStmt *, std::string const & prefix) const = 0;
	virtual void visit(const ExpressionStmt *, std::string const & prefix) const = 0;
	virtual void visit(const FunctionDecl *, std::string const & prefix) const = 0;
	virtual void visit(const UnaryExpr *, std::string const & prefix) const = 0;
	virtual void visit(const BinaryExpr *, std::string const & prefix) const = 0;
	virtual void visit(const CallExpr *, std::string const & prefix) const = 0;
	virtual void visit(const Literal *, std::string const & prefix) const = 0;
};
}
