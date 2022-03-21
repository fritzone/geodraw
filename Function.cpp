#include "Function.h"
#include "util.h"
#include "RuntimeProvider.h"
#include <QDebug>
#include <math.h>
#include <string.h>
#include <memory>
#include <set>

Function::Function(const char *expr) : funBody(expr)
{
//    qDebug() << "Creating fun:" << expr;

//    QString as(expr);
//    if(as.indexOf('e') >= 0)
//    {
//        qDebug() << "Weird";
//    }

    const char* eqp_chr = strchr(expr, '=');
    if(!eqp_chr)
    {
        throw syntax_error_exception("Invalid function defintion: %s. Missing assignment in body.", expr);
    }
    int eqpos = (strchr(expr, '=') - expr);
    char* expr_p1 = before(eqpos, expr);
    char* ppar = strchr(expr_p1, '(');
    if(!ppar)
    {
        throw syntax_error_exception("Improper parametrization of the given function: %s", expr_p1);
    }
    expr_p1[ppar - expr_p1] = 0;
    m_name = expr_p1;
    m_name = strim(m_name);
    ppar++;

    ppar[strchr(ppar, ')') - ppar] = 0;
    while (ppar)
    {
        char* pve = strchr(ppar, ',');
        if (pve != NULL)
        {
            ppar[pve - ppar] = 0;
            std::string vn = ppar;
            vars[vn] = std::numeric_limits<double>::quiet_NaN();
            ppar = pve + 1;
        }
        else
        {
            std::string ujv = ppar;
            vars[ujv] = std::numeric_limits<double>::quiet_NaN();
            ppar = NULL;
        }
    }

    // now begin translating the part two of the formula
    char* expr_p2 = after(eqpos, expr);
    std::string sc = preverify_formula(expr_p2);
    char* cexpr_p2 = strdup(sc.c_str());
    // first: a new tree will be created
    root = new tree;
    root->left = nullptr;
    root->right = nullptr;

    doit(cexpr_p2, root);
    free(reinterpret_cast<void*>(cexpr_p2));
    delete[] expr_p1;
    delete[] expr_p2;

}

Function::~Function()
{
    free_tree(root);
}

void Function::SetVariable(const std::string& varn, double valu)
{
    vars[varn] = valu;
//    qDebug() << "varn:" << varn.c_str() << "set to:" << valu;
}

double Function::Calculate(RuntimeProvider *rp)
{
    return calc(root, rp);
}

const std::string &Function::get_name() const
{
    return m_name;
}

std::vector<std::string> Function::get_domain_variables() const
{
    std::vector<std::string> result;

    std::transform(vars.begin(), vars.end(), std::back_inserter(result), [](std::pair<std::string, double> p) -> std::string { return p.first; });

    return result;
}

const QString &Function::get_funBody() const
{
    return funBody;
}

std::string Function::extract_proper_expression(const char *&p, std::set<char> seps)
{
    std::string res = "";
    bool done = false;
    int current_par_level = 1;
    while(!done)
    {
        bool added = false;
        if(*p == '(')
        {
            current_par_level ++;
            res += *p;
            added = true;
        }
        else
        if(*p == ')')
        {
            current_par_level--;
            if(current_par_level >= 1)
            {
                res += *p;
                added = true;
            }
        }


        // see if we can leave: no more parentheses and the current one is separator
        if(seps.count(*p) > 0 and current_par_level <= 1)
        {
            if(current_par_level == 0)
            {
                done = true;
            }

            // except if the current one is a closing parenthesys and the separator is also a parenthesis
            if(  !(*p == ')' && seps.count(*p) > 0))
            {
                done = true;
            }
        }
        else
        {
            if(!added)
            {
                res += *p;
            }
        }

        p++;

        if(!*p) done = true;

    }

    return res;
}

std::string Function::preverify_formula(char* expr)
{
    std::string s;
    for (int i = 0; i < strlen(expr); i++) {
        if (expr[i] == ',' || expr[i] == '$' || isalnum(expr[i]) || isoperator(expr[i]) || isparanthesis(expr[i])) {
            if (expr[i] != '-')
            {
                s += expr[i];
            }
            else
            {
                if(s.empty() || s[s.length() - 1] == '(')
                {
                    s += "(0-1)*";
                }
                else
                {
                    s += "+(0-1)*";
                }
            }
        }
    }
    return s;
}

void Function::doit(const char* expr, tree* node)
{
    int zladd = l0add(expr), zlmlt = l0mlt(expr),
        zlop = zladd == -1 ? zlmlt == -1 ? -1 : zlmlt : zladd;

    if (zlop != -1) {
        char* beforer = before(zlop, expr);
        if (strlen(beforer) == 0) {
            throw syntax_error_exception("Possible error in statement: %s. No data before position %i (%c)", expr, zlop, expr[zlop]);
        }
        char* afterer = after(zlop, expr);
        if (strlen(afterer) == 0) {
            throw syntax_error_exception("Possible error in statement: %s. No data after position %i (%c)", expr, zlop, expr[zlop]);
        }

        node->info = c2str(expr[zlop]);
        node->left = new tree;
        doit(beforer, node->left);
        delete[] beforer;

        node->right = new tree;
        doit(afterer, node->right);
        delete[] afterer;
    }
    else
    {
        if (expr[0] == '(')
        {
            if (expr[strlen(expr) - 1] == ')')
            {
                char* expr2 = new char[strlen(expr)];
                const char* ext = expr + 1;
                strcpy(expr2, ext);
                expr2[strlen(expr2) - 1] = 0;
                if (strlen(expr2) == 0)
                {
                    delete[] expr2;
                    throw syntax_error_exception("Syntax error in statement: %s. Unmatched empty paranthesis", expr);
                }
                doit(expr2, node);
                delete[] expr2;
            }
            else
            {
                throw syntax_error_exception("Possible error in statement: %s. Not found enclosing paranthesis", expr);
            }

        }
        else
        {
            char* iffunc = isfunc(expr);
            std::unique_ptr<char[]> wrapper;
            wrapper.reset(iffunc);

            if (iffunc) {
                std::string siffunc (iffunc);
                node->info = iffunc;

                if(siffunc == "pow")
                {
                    // the power function has 2 parameters

                    // getting tjh first one
                    const char* expr3 = expr + strlen(iffunc);

                    if (!strcmp(expr3, "()"))
                    {
                        throw syntax_error_exception("Possible error in statement: %s No parameters for a function (%s)", expr, iffunc);
                    }
                    if (strlen(expr3) == 0)
                    {
                        throw syntax_error_exception("Possible error in statement: %s Meaningless use of a function (%s)", expr, iffunc);
                    }

                    // skipping the parentheses
                    if(*expr3 == '(')
                    {
                        expr3 ++;
                    }

                    std::string p1 = extract_proper_expression(expr3, {','});
                    std::string p2 =extract_proper_expression(expr3, {')'});

                    node->left = new tree;
                    doit(p1.c_str(), node->left);

                    node->right = new tree;
                    doit(p2.c_str(), node->right);
                }
                else
                {
                    node->right = nullptr;

                    const char* expr3 = expr + strlen(iffunc);

                    if (!strcmp(expr3, "()")) {
                        throw syntax_error_exception("Possible error in statement: %s No parameters for a function (%s)", expr, iffunc);
                    }
                    if (strlen(expr3) == 0) {
                        throw syntax_error_exception("Possible error in statement: %s Meaningless use of a function (%s)", expr, iffunc);
                    }
                    node->left = new tree;
                    doit(expr3, node->left);
                }

            }
            else if (expr[strlen(expr) - 1] == ')')
            {
                throw syntax_error_exception("Possible error in formula: %s No open paranthesis for a closed one.", expr);
            }
            else
            {
                node->info = expr;
                node->left = nullptr;
                node->right = nullptr;
            }
        }
    }
}

int Function::l0ops(const char* expr, char op1, char op2)
{
    unsigned int i = 0, level = 0;
    while (i < strlen(expr))
    {
        if (expr[i] == '(')
            level++;
        if (expr[i] == ')')
            level--;
        if ((expr[i] == op1 || expr[i] == op2) && level == 0)
            return i;
        i++;
    }
    return -1;
}

int Function::l0add(const char* expr)
{
    return l0ops(expr, '-', '+');
}

int Function::l0mlt(const char* expr)
{
    return l0ops(expr, '*', '/');
}

double Function::calc(tree* node, RuntimeProvider* rp)
{
    if (node->left)
    {
        if (node->right)
        {
            return op(node->info, calc(node->left, rp), calc(node->right, rp));
        }
        else
        {
            return op(node->info, calc(node->left, rp), 0);
        }
    }
    else
    {
        if (defd(node->info, rp))
        {
            return value(node->info, rp);
        }
        else
        {
            throw syntax_error_exception("[E001] Possible error in formula statement: %s is not understood.", node->info);
        }
    }
}

void Function::free_tree(tree *node)
{
    if (node->left)
    {
        free_tree(node->left);
    }

    if(node->right)
    {
        free_tree(node->right);
    }

    delete node;
}

double Function::op(const std::string& s, double op1, double op2)
{
    if (s == "+")
    {
        return op1 + op2;
    }

    if (s == "-")
    {
        return op1 - op2;
    }

    if (s == "*")
    {
        return op1 * op2;
    }

    if (s == "/")
    {
        if (op2 != 0)
        {
            return op1 / op2;
        }
        else
        {
            return  sgn(op1) * std::numeric_limits<double>::quiet_NaN();
        }
    }

    auto it = std::find_if(supported_functions.begin(), supported_functions.end(), [s](fun_desc_solve fds){ return fds.name == s;});
    if(it != supported_functions.end())
    {
        return it->solver(op1, op2);
    }

    return -1;
}

int Function::defd(const std::string& s, RuntimeProvider* rp)
{
    if (vars.count(s))
    {
        return 1;
    }
    if (isnumber(s.c_str()))
    {
        return 1;
    }

    if(rp->defd(s))
    {
        return 1;
    }

    return 0;
}

double Function::value(const std::string& s, RuntimeProvider* rp)
{
//    qDebug() << "Getting value of:" << s.c_str();

    if (isnumber(s.c_str()))
    {
        return atof(s.c_str());
    }

    if (vars.count(s))
    {
//        qDebug() << "is:" << vars[s];
        return vars[s];
    }

    if(rp->defd(s))
    {
        return rp->value(s);
    }

    return std::numeric_limits<double>::quiet_NaN();
}
