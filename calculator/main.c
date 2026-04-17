#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPR 256

char *expr;
int pos;

typedef struct {
    long long num;
    long long den;
} Fraction;

long long gcd(long long a, long long b){
    if(b == 0) return llabs(a);
    return gcd(b, a % b);
}

Fraction simplify(Fraction f){
    long long g = gcd(f.num, f.den);
    f.num /= g;
    f.den /= g;

    if(f.den < 0)
    {
        f.den *= -1;
        f.num *= -1;
    }
    return f;
}

void skip_spaces(){
    while(expr[pos] == ' ') pos++;
}

Fraction toFraction(double x){
    Fraction f;
    f.num = (long long)(x * 1000000);
    f.den = 1000000;
    return simplify(f);
}

double toDouble(Fraction f){
    return (double)f.num / f.den;
}

Fraction add_fraction(Fraction a, Fraction b){
    Fraction result;
    result.num = a.num * b.den + b.num * a.den;
    result.den = a.den * b.den;
    return simplify(result);
}

Fraction sub_fraction(Fraction a, Fraction b){
    Fraction result;
    result.num = a.num * b.den - b.num * a.den;
    result.den = a.den * b.den;
    return simplify(result);
}

Fraction mul_fraction(Fraction a, Fraction b){
    Fraction result;
    result.num = a.num * b.num;
    result.den = a.den * b.den;
    return simplify(result);
}

Fraction div_fraction(Fraction a, Fraction b){
    Fraction result;
    result.num = a.num * b.den;
    result.den = a.den * b.num;
    return simplify(result);
}
double parse_expression();
double parse_term();
double parse_factor();
double parse_number();
Fraction parse_frac_expression();
Fraction parse_frac_term();
Fraction parse_frac_factor();

double parse_expression(){
    double r = parse_term();
    skip_spaces();

    while(expr[pos] == '+' || expr[pos] == '-'){
        char op = expr[pos++];
        double t = parse_term();

        if(op == '+') r = r + t;
        else r = r - t;
        skip_spaces();
    }
    return r;
}

double parse_term(){
    double r = parse_factor();
    skip_spaces();

    while(expr[pos] == '*' || expr[pos] == '/'){
        char op = expr[pos++];
        double v = parse_factor();

        if(op == '*') r = r * v;
        else r = r / v;
        skip_spaces();
    }
    return r;
}

double parse_factor(){
    skip_spaces();

    if(strncmp(&expr[pos], "frac", 4) == 0){
        pos += 4;

        if(expr[pos] != '(') return NAN;
        pos++;

        double a = parse_expression();

        if(expr[pos] != ',') return NAN;
        pos++;

        double b = parse_expression();

        if(expr[pos] != ')') return NAN;
        pos++;

        Fraction f;
        f.num = (long long)a;
        f.den = (long long)b;
        f = simplify(f);

        return toDouble(f);
    }

    if(expr[pos] == '-'){
        pos++;
        return -parse_factor();
    }

    if(expr[pos] == '('){
        pos++;
        double v = parse_expression();
        if(expr[pos] == ')') pos++;
        return v;
    }

    return parse_number();
}

double parse_number(){
    char *end;
    double v = strtod(&expr[pos], &end);
    pos = end - expr;
    return v;
}

Fraction parse_frac_expression(){
    Fraction r = parse_frac_term();
    skip_spaces();

    while(expr[pos] == '+' || expr[pos] == '-'){
        char op = expr[pos++];
        Fraction t = parse_frac_term();

        if(op == '+') r = add_fraction(r, t);
        else r = sub_fraction(r, t);
        skip_spaces();
    }
    return r;
}

Fraction parse_frac_term(){
    Fraction r = parse_frac_factor();
    skip_spaces();

    while(expr[pos] == '*' || expr[pos] == '/'){
        char op = expr[pos++];
        Fraction v = parse_frac_factor();

        if(op == '*') r = mul_fraction(r, v);
        else r = div_fraction(r, v);
        skip_spaces();
    }
    return r;
}

Fraction parse_frac_factor(){
    skip_spaces();

    if(strncmp(&expr[pos], "frac", 4) == 0)
    {
        pos += 4;

        if(expr[pos] != '(')
        {
            Fraction f = {0, 1};
            return f;
        }
        pos++;

        Fraction a = parse_frac_expression();

        if(expr[pos] != ',')
        {
            Fraction f = {0, 1};
            return f;
        }
        pos++;

        Fraction b = parse_frac_expression();

        if(expr[pos] != ')')
        {
            Fraction f = {0, 1};
            return f;
        }
        pos++;

        Fraction f;
        f.num = a.num * b.den;
        f.den = a.den * b.num;
        return simplify(f);
    }

    if(expr[pos] == '-')
    {
        pos++;
        Fraction f = parse_frac_factor();
        f.num = -f.num;
        return f;
    }

    if(expr[pos] == '(')
    {
        pos++;
        Fraction v = parse_frac_expression();
        if(expr[pos] == ')') pos++;
        return v;
    }

    char *end;
    double v = strtod(&expr[pos], &end);
    pos = end - expr;

    Fraction f;
    f.num = (long long)(v * 1000000);
    f.den = 1000000;
    return simplify(f);
}

void print_fraction(Fraction f){
    if(f.den == 1){
        printf("%lld", f.num);
    } else {
        printf("%lld/%lld", f.num, f.den);
    }
}


int main()
{
    char input[MAX_EXPR];
    int mode;

    printf("========================================\n");
    printf("   SCIENTIFIC CALCULATOR (HYBRID MODE)  \n");
    printf("========================================\n\n");

    printf("Select mode:\n");
    printf("1. Decimal mode (default)\n");
    printf("2. Fraction mode (exact rational results)\n");
    printf("Choice (1/2): ");
    scanf("%d", &mode);
    getchar();

    printf("\nCommands:\n");
    printf("- Use 'frac(a,b)' for fractions\n");
    printf("- Type 'exit' to quit\n");
    printf("- Type 'mode' to switch mode\n\n");

    while(1){
        printf("> ");
        fflush(stdout);

        if(!fgets(input, MAX_EXPR, stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if(strcmp(input, "exit") == 0) break;

        if(strcmp(input, "mode") == 0){
            mode = (mode == 1) ? 2 : 1;
            printf("Switched to %s mode\n\n", mode == 1 ? "Decimal" : "Fraction");
            continue;
        }

        if(strlen(input) == 0) continue;

        expr = input;
        pos = 0;

        if(mode == 1)
        {

            double result = parse_expression();

            if(expr[pos] != '\0' && expr[pos] != '\n')
            {
                printf("Error near: %c\n", expr[pos]);
            }
        else
            {
                printf("= %.10lf\n", result);
            }
        }
        else
            {

            Fraction result = parse_frac_expression();

            if(expr[pos] != '\0' && expr[pos] != '\n')
            {
                printf("Error near: %c\n", expr[pos]);
            }
        else

            {
                printf("= ");
                print_fraction(result);
                printf(" (≈ %.10lf)\n", toDouble(result));
            }
        }
    }

    printf("\nGoodbye!\n");
    return 0;
}
