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
class ForStmt;

class UnaryExpr;
struct UpdateExpr;
class BinaryExpr;
class CallExpr;
struct MemberExpr;
class Literal;
class Variable;

class PrintVisitor
{
public:
	virtual void visit(const BlockStmt *, int indent) const = 0;
	virtual void visit(const VariableStmt *, int indent) const = 0;
	virtual void visit(const EmptyStmt *, int indent) const = 0;
	virtual void visit(const IfStmt *, int indent) const = 0;
	virtual void visit(const ReturnStmt *, int indent) const = 0;
	virtual void visit(const ExpressionStmt *, int indent) const = 0;
	virtual void visit(const FunctionDecl *, int indent) const = 0;
	virtual void visit(const ForStmt *, int indent) const = 0;
	virtual void visit(const UnaryExpr *, int indent) const = 0;
	virtual void visit(const UpdateExpr *, int indent) const = 0;
	virtual void visit(const BinaryExpr *, int indent) const = 0;
	virtual void visit(const CallExpr *, int indent) const = 0;
	virtual void visit(const MemberExpr *, int indent) const = 0;
	virtual void visit(const Literal *, int indent) const = 0;
	virtual void visit(const Variable *, int indent) const = 0;
};
}
