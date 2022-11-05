#include "ast.h"
// #include <iostream>

Node::Node()
{
    // std::cout << "Node::Node()" << std::endl;
}

Start::Start(std::vector<class Decl *> *DeclList, std::vector<class AssgnStmt *> *AssgnStmtList, std::vector<class GradStmt *> *GradStmtList)
{
    this->DeclList = DeclList;
    this->AssgnStmtList = AssgnStmtList;
    this->GradStmtList = GradStmtList;
}

llvm::Value *Start::codegen()
{
    std::cout << "Start::codegen()" << std::endl;

    llvm::Value *v = nullptr;

    for (auto &decl : *DeclList)
    {
        v = decl->codegen();
    }

    // for (auto &assgn : *AssgnStmtList)
    // {
    //     v = assgn->codegen();
    // }

    // for (auto &grad : *GradStmtList)
    // {
    //     v = grad->codegen();
    // }

    return v;
}


Decl::Decl(GradSpecifier GradType, TypeSpecifier DataType, InitDeclarator *InitDeclaratorList)
{
    this->GradType = GradType;
    this->DataType = DataType;
    this->InitDeclaratorList = InitDeclaratorList;
}

llvm::Value *Decl::codegen()
{
    std::cout << "Decl::codegen()" << std::endl;

    llvm::Value *v = nullptr;

    v = InitDeclaratorList->codegen();

    return v;
}

InitDeclarator::InitDeclarator(Declarator *declarator, Initializer *initializer = NULL)
{
    this->declarator = declarator;
    this->initializer = initializer;
}

llvm::Value *InitDeclarator::codegen()
{
    std::cout << "InitDeclarator::codegen()" << std::endl;

    llvm::Value *v = nullptr;

    v = declarator->codegen();

    if (initializer != NULL)
    {
        v = initializer->codegen();
    }

    return v;
}

Declarator::Declarator(std::string name)
{
    this->name = name;
    // this->Dimensions = Dimensions;
}

llvm::Value *Declarator::codegen()
{
    return nullptr;
}

ConstValue::ConstValue(int value)
{
    this->isInt = true;
    this->value.int_val = value;
}

ConstValue::ConstValue(float value)
{
    this->isInt = false;
    this->value.float_val = value;
}

llvm::Value *ConstValue::codegen()
{
    if(this->isInt)
    {
        return llvm::ConstantInt::get(TheContext, llvm::APInt(32, this->value.int_val, true));
    }
    else
    {
        return llvm::ConstantFP::get(TheContext, llvm::APFloat(this->value.float_val));
    }
}

Initializer::Initializer(ConstValue *value)
{
    this->val.cvalue = value;
    this->isScalar = true;
}

Initializer::Initializer(std::vector<Initializer *> *InitializerList)
{
    this->val.InitializerList = InitializerList;
    this->isScalar = false;
}

llvm::Value *Initializer::codegen()
{
    if(this->isScalar)
    {
        return this->val.cvalue->codegen();
    }
    else
    {
        return nullptr;
    }
}

void Initializer::printInitializerList()
{
    // std::cout << "InitializerList" << std::endl;
    if (this->isScalar)
    {
        std::cout << "Scalar ";
        if (this->val.cvalue->isInt)
        {
            std::cout << this->val.cvalue->value.int_val << std::endl;
        }
        else
        {
            std::cout << this->val.cvalue->value.float_val << std::endl;
        }
    }
    else
    {
        std::cout << "Array" << std::endl;
        for (auto i : *this->val.InitializerList)
        {
            i->printInitializerList();
        }
    }
}

// void print_init_list_tree(Initializer *obj)
// {
// }

AssgnStmt::AssgnStmt(std::string name, std::optional<AssignmentOperator> op, Expr *expr)
{
    this->name = name;
    this->op = op;
    this->expr = expr;
}

llvm::Value *AssgnStmt::codegen()
{
    std::cout << "AssgnStmt::codegen()" << std::endl;

    llvm::Value *v = nullptr;

    v = expr->codegen();

    return v;
}

Expr::Expr()
{
}

void Expr::printExpression() {}

BinaryExpr::BinaryExpr(Expr *lhs, Expr *rhs, char op)
{
    this->lhs = lhs;
    this->rhs = rhs;
    this->op = op;
}

llvm::Value *BinaryExpr::codegen()
{
    return nullptr;
}

void BinaryExpr::printExpression()
{
    std::cout << "(";
    this->lhs->printExpression();
    std::cout << " " << this->op << " ";
    this->rhs->printExpression();
    std::cout << ")";
}


UnaryExpr::UnaryExpr(Expr *expr, std::optional<LibFuncs> libfunc, std::string identifier, ConstValue *cvalue)
{
    this->expr = expr;
    this->libfunc = libfunc;
    this->identifier = identifier;
    this->cvalue = cvalue;
}

void UnaryExpr::printExpression()
{
    if (this->identifier != "")
    {
        std::cout << this->identifier;
    }
    else if (this->cvalue != nullptr)
    {
        if (this->cvalue->isInt)
        {
            std::cout << this->cvalue->value.int_val;
        }
        else
        {
            std::cout << this->cvalue->value.float_val;
        }
    }
    else
    {
        std::cout << "(";
        switch (this->libfunc.value())
        {
            {
            case LibFuncs::SIN:
                std::cout << "sin(";
                break;
            case LibFuncs::COS:
                std::cout << "cos(";
                break;

            default:
                std::cout << "Invalid libfunc";
                break;
            }
        }
        this->expr->printExpression();
        std::cout << ")";
    }
}

llvm::Value *UnaryExpr::codegen()
{
    return nullptr;
}


GradStmt::GradStmt(GradType grad_type, std::string name)
{
    this->grad_type = grad_type;
    this->name = name;
}

llvm::Value *GradStmt::codegen()
{
    std::cout << "GradStmt::codegen()" << std::endl;

    llvm::Value *v = nullptr;

    return v;
}

// int main()
// {
//     return 0;
// }