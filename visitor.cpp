#include <iostream>
#include "exp.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

void type_error(const string& msg) {
    cerr << "Error de tipo: " << msg << endl;
    exit(1);
}
///////////////////////////////////////////////////////////////////////////////////
ImpValue BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
ImpValue IFExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
ImpValue NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
ImpValue BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
ImpValue IdentifierExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int AssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int WhileStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int ForStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int StatementList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int Body::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////

ImpValue PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return ImpValue();
}

ImpValue PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return ImpValue();
}

ImpValue PrintVisitor::visit(BoolExp* exp) {
    if(exp->value) cout << "true";
    else cout << "false";
    return ImpValue();
}

ImpValue PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return ImpValue();
}

void PrintVisitor::visit(AssignStatement* stm) {
    cout << stm->id << " = ";
    stm->rhs->accept(this);
    cout << ";";
}

void PrintVisitor::visit(PrintStatement* stm) {
    cout << "print(";
    stm->e->accept(this);
    cout << ");";
}

void PrintVisitor::visit(IfStatement* stm) {
    cout << "if ";
    stm->condition->accept(this);
    cout << " then" << endl;
    stm->then->accept(this);
    if(stm->els){
        cout << "else" << endl;
        stm->els->accept(this);
    }
    cout << "endif";
}

void PrintVisitor::imprimir(Program* program){
    program->body->accept(this);
};


ImpValue PrintVisitor::visit(IFExp* pepito) {
    cout<< "ifexp(";
    pepito->cond->accept(this);
    cout<< ",";
    pepito->left->accept(this);
    cout<< ",";
    pepito->right->accept(this);
    cout << ")" ;
    return ImpValue();
}

void PrintVisitor::visit(WhileStatement* stm){
    cout << "while ";
    stm->condition->accept(this);
    cout << " do" << endl;
    stm->b->accept(this);
    cout << "endwhile";
}

void PrintVisitor::visit(ForStatement* stm){
    cout << "for ";
    cout<<stm->lex;
    cout<<" in range ";
    stm->start->accept(this);
    cout << "  ";
    stm->end->accept(this);
    cout << "  ";
    stm->step->accept(this);
    cout << " body " << endl;
    stm->b->accept(this);
    cout << "endfor";
}

void PrintVisitor::visit(VarDec* stm){
    cout << "var ";
    cout << stm->type;
    cout << " ";
    for(auto i: stm->vars){
        cout << i;
        if(i != stm->vars.back()) cout << ", ";
    }
    cout << ";";
}

void PrintVisitor::visit(VarDecList* stm){
    for(auto i: stm->vardecs){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(StatementList* stm){
    for(auto i: stm->stms){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Body* stm){
    stm->vardecs->accept(this);
    cout << endl;
    stm->slist->accept(this);
}

///////////////////////////////////////////////////////////////////////////////////

ImpValue EVALVisitor::visit(NumberExp* exp) {
    return ImpValue("int", exp->value, false);
}

ImpValue EVALVisitor::visit(BoolExp* exp) {
    return ImpValue("bool", 0, exp->value != 0);
}

ImpValue EVALVisitor::visit(IdentifierExp* exp) {
    if (!env.check(exp->name)) {
        cerr << "Variable no declarada: " << exp->name << endl;
        exit(1);
    }
    string type = env.lookup_type(exp->name);
    if (type == "int") {
        int val = env.lookup(exp->name);
        return ImpValue("int", val, false);
    } else if (type == "bool") {
        int val = env.lookup(exp->name);
        return ImpValue("bool", 0, val != 0);
    } else {
        cerr << "Tipo desconocido para variable " << exp->name << endl;
        exit(1);
    }
}

ImpValue EVALVisitor::visit(BinaryExp* exp) {
    ImpValue left = exp->left->accept(this);
    ImpValue right = exp->right->accept(this);

    if (exp->op == PLUS_OP || exp->op == MINUS_OP || exp->op == MUL_OP || exp->op == DIV_OP) {
        if (left.type != "int" || right.type != "int") {
            cerr << "Operación aritmética con operandos no enteros" << endl;
            exit(1);
        }
        int lval = left.int_value;
        int rval = right.int_value;
        int res = 0;
        switch (exp->op) {
            case PLUS_OP: res = lval + rval; break;
            case MINUS_OP: res = lval - rval; break;
            case MUL_OP: res = lval * rval; break;
            case DIV_OP:
                if (rval == 0) {
                    cerr << "Error: división por cero" << endl;
                    exit(1);
                }
                res = lval / rval;
                break;
            default: break;
        }
        return ImpValue("int", res, false);
    }
    else if (exp->op == LT_OP || exp->op == LE_OP || exp->op == EQ_OP) {
        // Comparación solo con operandos enteros o booleanos compatibles
        if (left.type != right.type) {
            cerr << "Comparación con operandos de tipos incompatibles" << endl;
            exit(1);
        }
        if (left.type == "int") {
            int lval = left.int_value;
            int rval = right.int_value;
            bool res = false;
            switch (exp->op) {
                case LT_OP: res = (lval < rval); break;
                case LE_OP: res = (lval <= rval); break;
                case EQ_OP: res = (lval == rval); break;
                default: break;
            }
            return ImpValue("bool", 0, res);
        } else if (left.type == "bool") {
            bool lval = left.bool_value;
            bool rval = right.bool_value;
            bool res = false;
            switch (exp->op) {
                case EQ_OP: res = (lval == rval); break;
                default:
                    cerr << "Solo == es válido para booleanos" << endl;
                    exit(1);
            }
            return ImpValue("bool", 0, res);
        }
        else {
            cerr << "Tipo desconocido en comparación" << endl;
            exit(1);
        }
    }
    cerr << "Operación desconocida" << endl;
    exit(1);
}

ImpValue EVALVisitor::visit(IFExp* exp) {
    ImpValue cond = exp->cond->accept(this);
    if (cond.type != "bool") {
        cerr << "Condición en ifexp no es booleana" << endl;
        exit(1);
    }
    if (cond.bool_value) {
        return exp->left->accept(this);
    } else {
        return exp->right->accept(this);
    }
}


void EVALVisitor::visit(AssignStatement* stm) {
    ImpValue val = stm->rhs->accept(this);
    if (!env.check(stm->id)) {
        cerr << "Variable no declarada: " << stm->id << endl;
        exit(1);
    }
    if (!env.typecheck(stm->id, val.type)) {
        cerr << "Error: asignación con tipo incompatible para variable " << stm->id << endl;
        exit(1);
    }
    if (val.type == "int") {
        env.update(stm->id, val.int_value);
    } else if (val.type == "bool") {
        env.update(stm->id, val.bool_value ? 1 : 0);
    }
}

void EVALVisitor::visit(PrintStatement* stm) {
    ImpValue val = stm->e->accept(this);
    if (val.type == "int") {
        cout << val.int_value << endl;
    } else if (val.type == "bool") {
        cout << (val.bool_value ? "true" : "false") << endl;
    } else {
        cout << "<unknown>" << endl;
    }
}

void EVALVisitor::visit(IfStatement* stm) {
    ImpValue cond = stm->condition->accept(this);
    if (cond.type != "bool") {
        cerr << "Error: condición en if debe ser booleana" << endl;
        exit(1);
    }
    if (cond.bool_value) {
        env.add_level();
        stm->then->accept(this);
        env.remove_level();
    } else if (stm->els != nullptr) {
        env.add_level();
        stm->els->accept(this);
        env.remove_level();
    }
}

void EVALVisitor::visit(WhileStatement* stm) {
    ImpValue cond = stm->condition->accept(this);
    if (cond.type != "bool") {
        cerr << "Error: condición en while debe ser booleana" << endl;
        exit(1);
    }
    env.add_level();
    while (true) {
        cond = stm->condition->accept(this);
        if (cond.type != "bool") {
            cerr << "Error: condición en while debe ser booleana" << endl;
            exit(1);
        }
        if (!cond.bool_value) break;
        stm->b->accept(this);
    }
    env.remove_level();
}



void EVALVisitor::visit(ForStatement* stm) {
    // Evaluar start, end, step
    ImpValue startVal = stm->start->accept(this);
    ImpValue endVal = stm->end->accept(this);
    ImpValue stepVal = stm->step->accept(this);
    if (startVal.type != "int" || endVal.type != "int" || stepVal.type != "int") {
        cerr << "Error: valores de inicio, fin y paso en for deben ser enteros" << endl;
        exit(1);
    }

    env.add_level();
    // Crear variable del for
    env.add_var("i", "int"); // Temporal, se debe usar id de la variable (mejorado abajo)
    // NOTE: En la gramática no se define variable explícita para for, ajustar para variable del for
    // Aquí supongo que la variable es "i", pero debería venir del parser. Por simplificar, pongo "i".
    int i = startVal.int_value;
    while ((stepVal.int_value > 0 && i <= endVal.int_value) ||
           (stepVal.int_value < 0 && i >= endVal.int_value)) {
        env.update("i", i);
        stm->b->accept(this);
        i += stepVal.int_value;
           }
    env.remove_level();
}

void EVALVisitor::visit(VarDec* stm) {
    for (auto varname : stm->vars) {
        if (env.check(varname)) {
            cerr << "Error: variable " << varname << " ya declarada" << endl;
            exit(1);
        }
        env.add_var(varname, stm->type);
    }
}

void EVALVisitor::visit(VarDecList* stm) {
    for (auto v : stm->vardecs) {
        v->accept(this);
    }
}

void EVALVisitor::visit(StatementList* stm) {
    for (auto s : stm->stms) {
        s->accept(this);
    }
}

void EVALVisitor::visit(Body* b) {
    env.add_level();
    b->vardecs->accept(this);
    b->slist->accept(this);
    env.remove_level();
}

void EVALVisitor::ejecutar(Program* program) {
    env.clear();
    env.add_level();
    program->body->accept(this);
    env.remove_level();
}

///////////////////////////////////////////////////////////////////////////////////

//0 = undefined
//1 = int
//2 = bool
void TypeVisitor::check(Program* program) {
    env.clear();
    env.add_level();
    program->body->accept(this);
    env.remove_level();
}

ImpValue TypeVisitor::visit(NumberExp* exp) {
    return ImpValue("int", exp->value, false);
}

ImpValue TypeVisitor::visit(BoolExp* exp) {
    return ImpValue("bool", 0, exp->value != 0);
}

ImpValue TypeVisitor::visit(IdentifierExp* exp) {
    if (!env.check(exp->name)) {
        type_error("Variable no declarada: " + exp->name);
    }
    string tipo = env.lookup_type(exp->name);
    if (tipo == "") {
        type_error("Tipo desconocido para variable " + exp->name);
    }
    if (tipo == "int") return ImpValue("int", 0, false);
    else if (tipo == "bool") return ImpValue("bool", 0, false);
    else type_error("Tipo desconocido: " + tipo);
    return ImpValue();
}
ImpValue TypeVisitor::visit(BinaryExp* exp) {
    ImpValue left = exp->left ? exp->left->accept(this) : ImpValue();
    ImpValue right = exp->right->accept(this);

    if (exp->op == PLUS_OP || exp->op == MINUS_OP || exp->op == MUL_OP || exp->op == DIV_OP) {
        if (left.type != "int" || right.type != "int") {
            type_error("unsupported operand type");  // 4 + true
        }
        return ImpValue("int", 0, false);
    }

    if (exp->op == LT_OP || exp->op == LE_OP || exp->op == EQ_OP) {
        if (left.type != right.type) {
            type_error("unsupported operand type");
        }
        return ImpValue("bool", 0, false);
    }

    if (exp->op == AND_OP || exp->op == OR_OP) {
        if (left.type != "bool" || right.type != "bool") {
            type_error("unsupported operand type");  // 4 and 5
        }
        return ImpValue("bool", 0, false);
    }

    if (exp->op == NOT_OP) {
        if (right.type != "bool") {
            type_error("unsupported operand type");  // not 5
        }
        return ImpValue("bool", 0, false);
    }

    type_error("unsupported operand type");
    return ImpValue();
}


ImpValue TypeVisitor::visit(IFExp* exp) {
    ImpValue cond = exp->cond->accept(this);
    if (cond.type != "bool") {
        type_error("Condición en ifexp no es booleana");
    }
    // El tipo resultante es el tipo de left o right, deben coincidir
    ImpValue left = exp->left->accept(this);
    ImpValue right = exp->right->accept(this);
    if (left.type != right.type) {
        type_error("Los tipos de then y else en ifexp deben coincidir");
    }
    return ImpValue(left.type, 0, false);
}

void TypeVisitor::visit(AssignStatement* stm) {
    if (!env.check(stm->id)) {
        type_error("Variable no declarada: " + stm->id);
    }
    ImpValue val = stm->rhs->accept(this);
    string var_type = env.lookup_type(stm->id);
    if (val.type != var_type) {
        type_error("Asignación con tipo incompatible para variable " + stm->id);
    }
}

void TypeVisitor::visit(PrintStatement* stm) {
    stm->e->accept(this);
}

void TypeVisitor::visit(IfStatement* stm) {
    ImpValue cond = stm->condition->accept(this);
    if (cond.type != "bool") {
        type_error("Condición en if debe ser booleana");
    }
    env.add_level();
    stm->then->accept(this);
    env.remove_level();

    if (stm->els != nullptr) {
        env.add_level();
        stm->els->accept(this);
        env.remove_level();
    }
}

void TypeVisitor::visit(WhileStatement* stm) {
    ImpValue cond = stm->condition->accept(this);
    if (cond.type != "bool") {
        type_error("Condición en while debe ser booleana");
    }
    env.add_level();
    stm->b->accept(this);
    env.remove_level();
}

void TypeVisitor::visit(ForStatement* stm) {
    ImpValue startVal = stm->start->accept(this);
    ImpValue endVal = stm->end->accept(this);
    ImpValue stepVal = stm->step->accept(this);

    if (startVal.type != "int" || endVal.type != "int" || stepVal.type != "int") {
        type_error("Valores de inicio, fin y paso en for deben ser enteros");
    }

    env.add_level();
    // La variable del for debería estar declarada, aquí asumimos "i"
    env.add_var("i", "int"); // Aquí se debería usar el id real, según el parser
    stm->b->accept(this);
    env.remove_level();
}

void TypeVisitor::visit(VarDec* stm) {
    if (stm->type != "int" && stm->type != "bool") {
        if (stm->type == "string") {
            type_error("type error");  // var string x,y
        } else {
            type_error("type error");
        }
    }
    for (auto varname : stm->vars) {
        if (env.check(varname)) {
            type_error("Variable " + varname + " ya declarada");
        }
        env.add_var(varname, stm->type);
    }
}

void TypeVisitor::visit(VarDecList* stm) {
    for (auto v : stm->vardecs) {
        v->accept(this);
    }
}

void TypeVisitor::visit(StatementList* stm) {
    for (auto s : stm->stms) {
        s->accept(this);
    }
}

void TypeVisitor::visit(Body* b) {
    env.add_level();
    b->vardecs->accept(this);
    b->slist->accept(this);
    env.remove_level();
}
