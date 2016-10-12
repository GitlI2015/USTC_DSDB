#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct node
{
    double coef;
    double exp;
    struct node *next;
};
struct polynomial
{
    struct node *entrance;
    double c;
    int n;
};
int pFloat(double);
int isZero(double);
int isEmpty(struct polynomial *p)
{
    return p->entrance == NULL;
}
void destroyP(struct polynomial *);
void printPoly(struct polynomial *);
struct node *createN(double, double);
struct polynomial *createP(void);
struct polynomial *createPN(struct node *);
void insert(struct node *n, struct polynomial *p);
void input(struct polynomial *);
void add(struct polynomial *, struct polynomial *, struct polynomial *);
void add_to(struct polynomial *, struct polynomial *);
void sub(struct polynomial *, struct polynomial *, struct polynomial *);
void mult(struct polynomial *, struct polynomial *, struct polynomial *);
void divide(int, struct polynomial *, struct polynomial *, struct polynomial *);
void integrate(struct polynomial *, struct polynomial *);
void differentiate(struct polynomial *, struct polynomial *);
struct polynomial *parse(char *);
struct polynomial *operate(struct polynomial *, char, struct polynomial *);
double evaluate(struct polynomial *, double);
double dintegrate(struct polynomial *, double , double);
char precede(char, char);
int HMI();
void Edit();
void Calculate();
void FundamentalCalculate();
void AdvancedCalculate();
int saveto(struct polynomial*, int);

char poly[256];
char tmp;
struct polynomial *pol[10];

int main()
{
    for(int i = 0; i < 10; i++)
        pol[i] = NULL;

    struct polynomial *result;

    while(1)
    {
        int choice = HMI();

        switch(choice)
        {
        case 0:
            return 0;
            break;

        case 1:
            for(int i = 0; i < 10; i++)
            {
                printf("[%d]:\t", i);

                if(pol[i] == NULL)
                    printf("NULL");
                else
                    printPoly(pol[i]);

                putchar('\n');
            }

            putchar('\n');
            break;

        case 2:
            Edit();
            break;

        case 3:
            Calculate();
            break;
        }

//        gets(poly);
//        result = parse(poly);
//        printPoly(result);
//        putchar('\n');
    }

    return 0;
}
void differentiate(struct polynomial *res, struct polynomial *in)
{
    struct node *t = in->entrance;
    struct node *t1;

    while(t != NULL)
    {
        t1 = createN((t->coef) * (t->exp), t->exp - 1);
        insert(t1, res);
        free(t1);
        t = t->next;
    }

}
void integrate(struct polynomial *res, struct polynomial *origin)
{
    struct node *t = origin->entrance;
    struct node *t1;
    insert(createN(origin->c, 1), res);

    while(t != NULL)
    {
        t1 = createN(t->coef / (t->exp + 1), t->exp + 1);
        insert(t1, res);
        free(t1);
        t = t->next;
    }
}
double dintegrate(struct polynomial *in, double low, double up)
{
    struct polynomial *t = createP();
    integrate(t, in);
    double result = evaluate(t, up) - evaluate(t, low);
    destroyP(t);
    return result;
}
void AdvancedCalculate()
{
    char ch1 = '0';
    printf("Possible advanced calculations  \n");
    printf("--------------------------------\n");
    printf("    1. Derivative               \n");
    printf("    2. Indefinite integral      \n");
    printf("    3. definite integral        \n");
    printf(" Else. Return back              \n");
    printf("--------------------------------\n");
    printf("Select an operation you want to execute");
    scanf("%c", &ch1);
    getchar();

    while((ch1 > '0') && (ch1 < '6'))
    {
        char ch2 = '0';
        double x1 = 0, x2 = 0;
        struct polynomial *temp1 = NULL, *temp2, *temp3;

        switch(ch1 - '0')
        {
        case 1:
            printf("-----------Derivative-----------\n");
            printf("Enter subscript number to choose original polynomial from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            while(temp1 == NULL)
            {
                if((ch2 < '0') || (ch2 > '9'))
                {
                    puts("Input: ");
                    gets(poly);
                    temp1 = parse(poly);
                }
                else
                    temp1 = pol[ch2 - '0'];

                if(temp1 == NULL)
                    printf("Target position is empty now, please choose another one.\n");
            }

            printf("f(x) = ");
            printPoly(temp1);
            putchar('\n');
            temp2 = createP();
            differentiate(temp2, temp1);
            printf("f'(x) = ");
            printPoly(temp2);
            putchar('\n');
            printf("Enter subscript number to save the result to, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            while(1)
            {
                if((ch2 >= '0') && (ch2 <= '9'))
                {
                    if(saveto(temp2, ch2 - '0'))
                        break;
                }
                else
                {
                    destroyP(temp2);
                    break;
                }

                printf("Enter subscript number to save the result to, otherwise, type anything else.\n");
                scanf("%c", &ch2);
                getchar();
            }

            temp1 = NULL;
            break;

        case 2:
            printf("-----------Indefinite integral-----------\n");

            while(temp1 == NULL)
            {

                printf("Enter subscript number to choose original polynomial from memory, otherwise, type anything else.\n");
                scanf("%c", &ch2);
                getchar();

                if((ch2 < '0') || (ch2 > '9'))
                {
                    puts("Input: ");
                    gets(poly);
                    temp1 = parse(poly);
                }
                else
                    temp1 = pol[ch2 - '0'];

                if(temp1 == NULL)
                    printf("Target position is empty now, please choose another one.\n");
            }

            printf("f(x) = ");
            printPoly(temp1);
            putchar('\n');
            temp2 = createP();
            integrate(temp2, temp1);
            printf("F(x) = ");
            printPoly(temp2);
            putchar('\n');
            printf("Enter subscript number to save the result to, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            while(1)
            {
                if((ch2 >= '0') && (ch2 <= '9'))
                {
                    if(saveto(temp2, ch2 - '0'))
                        break;
                }
                else
                {
                    destroyP(temp2);
                    break;
                }

                printf("Enter subscript number to save the result to, otherwise, type anything else.\n");
                scanf("%c", &ch2);
                getchar();
            }

            break;
            temp1 = NULL;

        case 3:
            printf("-----------Indefinite integral-----------\n");
            printf("Enter subscript number to choose original polynomial from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();


            while(temp1 == NULL)
            {
                if((ch2 < '0') || (ch2 > '9'))
                {
                    puts("Input: ");
                    gets(poly);
                    temp1 = parse(poly);
                }
                else
                    temp1 = pol[ch2 - '0'];

                if(temp1 == NULL)
                    printf("Target position is empty now, please choose another one.\n");
            }

            printf("Please enter the boundary for integration.\n");
            printf("Lower bound: ");
            scanf("%lf", &x1);
            printf("Upper bound: ");
            scanf("%lf", &x2);
            printf("f(x) = ");
            printPoly(temp1);
            putchar('\n');
            printf("Integrate f(x) from ");
            pFloat(x1);
            printf(" to ");
            pFloat(x2);
            printf(" = ");
            pFloat(dintegrate(temp1, x1, x2));
            putchar('\n');
            temp1 = NULL;
            break;

        default:
            return;
        }

        printf("Possible advanced calculations  \n");
        printf("--------------------------------\n");
        printf("    1. Derivative               \n");
        printf("    2. Indefinite integral      \n");
        printf("    3. definite integral        \n");
        printf(" Else. Return back              \n");
        printf("--------------------------------\n");
        printf("Select an operation you want to execute:");
        scanf("%c", &ch1);
        getchar();
    }
}
double evaluate(struct polynomial *p, double v)
{
    double res = p->c;
    struct node *t;
    t = p->entrance;

    while(t != NULL)
    {
        res += t->coef * pow(v, t->exp);
        t = t->next;
    }

    return res;
}
int saveto(struct polynomial *p, int i)
{
    char ch;

    if(pol[i] != NULL)
    {
        printf("Target position isn't empty, if you want to cover current value, please enter y/Y, else enter other characters:");
        scanf("%c", &ch);
        getchar();

        if((ch == 'y') || (ch == 'Y'))
        {
            destroyP(pol[i]);
            pol[i] == NULL;
            return 1;
        }
        else
            return 0;
    }
    else
    {
        pol[i] = p;
        return 1;
    }
}
void FundamentalCalculate()
{
    char ch1 = '0';
    printf("Possible Fundamental Calculations: \n");
    printf("--------------------------\n");
    printf("    1. Add                \n");
    printf("    2. Subtract            \n");
    printf("    3. Multiply            \n");
    printf("    4. Divide              \n");
    printf("    5. Modulo              \n");
    printf("    6. Evaluate            \n");
    printf(" Else. Return back         \n");
    printf("--------------------------\n");
    printf("Select an operation you want to execute:");
    scanf("%c", &ch1);
    getchar();

    while((ch1 > '0') && (ch1 < '8'))
    {
        char ch2 = '0';
        double x = 0;
        int n = 0;
        struct polynomial *temp1, *temp2, *temp3, *temp4;

        switch(ch1 - '0')
        {
        case 1:
            printf("-----------Add-----------\n");
            printf("Enter subscript number to choose Augend from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("Augend: \n");
                gets(poly);
                temp1 = createP();
                temp1 = parse(poly);
            }
            else
                temp1 = pol[ch2 - '0'];

            printf("Enter subscript number to choose Addend from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("Addend\n");
                gets(poly);
                temp2 = createP();
                temp2 = parse(poly);
            }
            else
                temp2 = pol[ch2 - '0'];

            printf("Addend: \n");
            printPoly(temp1);
            putchar('\n');
            printf("Augend: \n");
            printPoly(temp2);
            putchar('\n');
            temp3 = createP();
            add(temp3, temp1, temp2);
            printf("Result: \n");
            printPoly(temp3);
            putchar('\n');
            printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
            scanf("%c", &ch2);
            getchar();

            while(1)
            {
                if((ch2 >= '0') && (ch2 <= '9'))
                {
                    if(saveto(temp3, ch2 - '0'))
                        break;
                }
                else
                {
                    destroyP(temp3);
                    break;
                }

                printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
                scanf("%c", &ch2);
                getchar();
            }

            break;

        case 2:
            printf("---------Subtract-----------\n");
            printf("Enter subscript number to choose Minuend from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("Minuend: \n");
                gets(poly);
                temp1 = createP();
                temp1 = parse(poly);
            }
            else
                temp1 = pol[ch2 - '0'];

            printf("Enter subscript number to choose Subtractor from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("Subtractor:\n");
                gets(poly);
                temp2 = createP();
                temp2 = parse(poly);
            }
            else
                temp2 = pol[ch2 - '0'];

            printf("Minuend: \n");
            printPoly(temp1);
            putchar('\n');
            printf("Subtractor: \n");
            printPoly(temp2);
            putchar('\n');
            temp3 = createP();
            sub(temp3, temp1, temp2);
            printf("Result: \n");
            printPoly(temp3);
            putchar('\n');
            printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
            scanf("%c", &ch2);
            getchar();

            while(1)
            {
                if((ch2 >= '0') && (ch2 <= '9'))
                {
                    if(saveto(temp3, ch2 - '0'))
                        break;
                }
                else
                {
                    destroyP(temp3);
                    break;
                }

                printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
                scanf("%c", &ch2);
                getchar();
            }

            break;

        case 3:
            printf("---------Multiply-----------\n");
            printf("Enter subscript number to choose Multiplicand from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("Multiplicand: \n");
                gets(poly);
                temp1 = createP();
                temp1 = parse(poly);
            }
            else
                temp1 = pol[ch2 - '0'];

            printf("Enter subscript number to choose Multiplier from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("Multiplier:\n");
                gets(poly);
                temp2 = createP();
                temp2 = parse(poly);
            }
            else
                temp2 = pol[ch2 - '0'];

            printf("Multiplicand: \n");
            printPoly(temp1);
            putchar('\n');
            printf("Multiplier: \n");
            printPoly(temp2);
            putchar('\n');
            temp3 = createP();
            mult(temp3, temp1, temp2);
            printf("Result: \n");
            printPoly(temp3);
            putchar('\n');
            printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
            scanf("%c", &ch2);
            getchar();

            while(1)
            {
                if((ch2 >= '0') && (ch2 <= '9'))
                {
                    if(saveto(temp3, ch2 - '0'))
                        break;
                }
                else
                {
                    destroyP(temp3);
                    break;
                }

                printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
                scanf("%c", &ch2);
                getchar();
            }

            break;

        case 4:
            printf("---------Divide-----------\n");
            printf("Enter subscript number to choose Dividend from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("Dividend: \n");
                gets(poly);
                temp1 = createP();
                temp1 = parse(poly);
            }
            else
                temp1 = pol[ch2 - '0'];

            printf("Enter subscript number to choose divisor from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("divisor:\n");
                gets(poly);
                temp2 = createP();
                temp2 = parse(poly);
            }
            else
                temp2 = pol[ch2 - '0'];

            printf("Dividend: \n");
            printPoly(temp1);
            putchar('\n');
            printf("Divisor: \n");
            printPoly(temp2);
            putchar('\n');
            temp3 = createP();
            divide(1, temp3, temp1, temp2);
            temp4 = createP();
            divide(2, temp4, temp1, temp2);
            printf("Result: \n");
            printPoly(temp3);
            putchar('\n');
            printf("Remainder: \n");
            printPoly(temp4);
            putchar('\n');
            printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
            scanf("%c", &ch2);
            getchar();
            destroyP(temp4);

            while(1)
            {
                if((ch2 >= '0') && (ch2 <= '9'))
                {
                    if(saveto(temp3, ch2 - '0'))
                        break;
                }
                else
                {
                    destroyP(temp3);
                    break;
                }

                printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
                scanf("%c", &ch2);
                getchar();
            }

            break;


        case 5:
            printf("---------Modulo-----------\n");
            printf("Enter subscript number to choose left operand from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("Left operand: \n");
                gets(poly);
                temp1 = createP();
                temp1 = parse(poly);
            }
            else
                temp1 = pol[ch2 - '0'];

            printf("Enter subscript number to choose right operand from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                printf("Right operand:\n");
                gets(poly);
                temp2 = createP();
                temp2 = parse(poly);
            }
            else
                temp2 = pol[ch2 - '0'];

            printf("Left operand: \n");
            printPoly(temp1);
            putchar('\n');
            printf("Right operand: \n");
            printPoly(temp2);
            putchar('\n');
            temp3 = createP();
            divide(2, temp3, temp1, temp2);
            printf("Result: \n");
            printPoly(temp3);
            putchar('\n');
            printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
            scanf("%c", &ch2);
            getchar();

            while(1)
            {
                if((ch2 >= '0') && (ch2 <= '9'))
                {
                    if(saveto(temp3, ch2 - '0'))
                        break;
                }
                else
                {
                    destroyP(temp3);
                    break;
                }

                printf("Enter a subscript number if you want to save the result, otherwise, type anything else\n");
                scanf("%c", &ch2);
                getchar();
            }

            break;

        case 6:
            printf("-----------Evaluate-----------\n");
            printf("Enter subscript number to choose from memory, otherwise, type anything else.\n");
            scanf("%c", &ch2);
            getchar();

            if((ch2 < '0') || (ch2 > '9'))
            {
                gets(poly);
                temp1 = parse(poly);
            }
            else
                temp1 = pol[ch2 - '0'];

            printf("Enter the value of x:");
            scanf("%lf", &x);
            printf("f(x) = ");
            printPoly(temp1);
            putchar('\n');
            printf("f(");
            pFloat(x);
            printf(") = ");
            pFloat(evaluate(temp1, x));
            putchar('\n');
            break;

        default:
            return;
        }

        printf("Possible Fundamental Calculations: \n");
        printf("--------------------------\n");
        printf("    1. Add                \n");
        printf("    2. Subtract            \n");
        printf("    3. Multiply            \n");
        printf("    4. Divide              \n");
        printf("    5. Modulo              \n");
        printf("    6. Evaluate            \n");
        printf(" Else. Return back           \n");
        printf("--------------------------\n");
        printf("Select an operation you want to execute:");
        scanf("%c", &ch1);
        getchar();
    }
}

void Calculate()
{
    char ch1 = '0';
    printf("Possible calculating operations   \n");
    printf("--------------------------\n");
    printf("    1. Fundamental             \n");
    printf("    2. Advanced             \n");
    printf(" Else. Return back           \n");
    printf("--------------------------\n");
    printf("Choose an operation you want to execute:");
    scanf("%c", &ch1);
    getchar();

    while((ch1 == '1') || (ch1 == '2'))
    {
        switch(ch1 - '0')
        {
        case 1:
            FundamentalCalculate();
            break;

        case 2:
            AdvancedCalculate();
            break;

        default:
            return;
        }

        printf("Possible calculating operations   \n");
        printf("--------------------------\n");
        printf("    1. Fundamental             \n");
        printf("    2. Advanced            \n");
        printf(" else. Return back           \n");
        printf("--------------------------\n");
        printf("Choose an operation you want to execute:");
        scanf("%c", &ch1);
        getchar();
    }
}
void Edit()
{
    char ch1 = '0', ch2 = 0;
    int x, y;
    printf("--------------------------\n");
    printf("  Possible operation       \n");
    printf("--------------------------\n");
    printf("    1. Modify                \n");
    printf("    2. Delete                \n");
    printf(" Else. return to top level   \n");
    printf("--------------------------\n");
    printf("Select an operation you want to execute:");
    scanf("%c", &ch1);
    getchar();

    while((ch1 > '0') && (ch1 < '3'))
    {
        switch(ch1 - '0')
        {
        case 1:
            while(ch2 < '0' || ch2 > '9')
            {
                printf("Enter the position you want to save to:");
                scanf("%c", &ch2);
                getchar();

                if(ch2 < '0' || ch2 > '9')
                    printf("position should be a number between 0 and 9\n");
            }

            x = ch2 - '0';
            ch2 = 0;

            if(pol[x] != NULL)
            {
                printf("Target position isn't empty, if you want to cover current value, please enter y/Y, else enter other characters:\n");
                scanf("%c", &ch2);
                getchar();

                if((ch2 == 'y') || (ch2 == 'Y'))
                {
                    destroyP(pol[x]);
                    pol[x] == NULL;
                }
                else
                    break;
            }
            printf("Input:\n");
            gets(poly);
            pol[x] = parse(poly);
            break;

        case 2:
            while(ch2 < '0' || ch2 > '9')
            {
                printf("Enter the position you want delete:");
                scanf("%c", &ch2);
                getchar();

                if(ch2 < '0' || ch2 > '9')
                    printf("position should be a number between 0 and 9\n");
            }

            x = ch2 - '0';

            if(pol[x] == NULL)
            {
                printf("Target position empty!\n\n");
                break;
            }

            printf("The polynomial at %d is:\n", x);
            printPoly(pol[x]);
            putchar('\n');
            printf("Enter y/Y to confirm:");
            scanf("%c", &ch2);
            getchar();

            if((ch2 == 'y') || (ch2 == 'Y'))
            {
                destroyP(pol[x]);
                pol[x] = NULL;
                printf("%d is empty now.\n", x);
            }

            break;

        default:
            return;
        }

        printf("--------------------------\n");
        printf("  Possible operation       \n");
        printf("--------------------------\n");
        printf("    1. Modify                \n");
        printf("    2. Delete                \n");
        printf(" Else. Return back   \n");
        printf("--------------------------\n");
        printf("Select an operation you want to execute:");
        scanf("%c", &ch1);
        getchar();
    }
}
int HMI()
{
    char ch;
    printf("----------------------------\n");
    printf("Sparse polynomial calculator\n");
    printf("----------------------------\n");
    printf("    1: Display memory             \n");
    printf("    2: Edit memory           \n");
    printf("    3: Calculate                 \n");
    printf(" Else: Exit                \n");
    printf("----------------------------\n");
    printf("Select an operation you want to execute:");
    scanf("%c", &ch);
    getchar();
    ch = (ch > '3') || (ch < '1') ? '0' : ch;
    return ch - '0';
}
struct polynomial *createP(void)
{
    struct polynomial *np =
        (struct polynomial *)malloc(sizeof(struct polynomial));
    np->entrance = NULL;
    np->n        = 0;
    np->c        = 0;
    return np;
}
void destroyP(struct polynomial *p)
{
    struct node *j;

    for(struct node *i = p->entrance; i != NULL; i = j)
    {
        j = i->next;
        free(i);
    }

    p->n        = 0;
    p->entrance = NULL;
    free(p);
}
struct node *createN(double c, double e)
{
    struct node *t = (struct node *)malloc(sizeof(struct node));
    t->coef        = c;
    t->exp         = e;
    t->next        = NULL;
    return t;
}
void insert(struct node *tn, struct polynomial *p)
{
    struct node *n = createN(tn->coef, tn->exp);

    if(isZero(n->exp))
    {
        p->c += n->coef;
        free(n);
        return;
    }

    if(isZero(n->coef) || n->exp < 0)
        return;
    else if(isEmpty(p))
    {
        p->entrance = n;
        ++p->n;
    }
    else
    {
        struct node *i = p->entrance;
        struct node *t = i;

        if(n->exp > i->exp)
        {
            p->entrance = n;
            n->next     = i;
            ++p->n;
            return;
        }

        for(;; i = i->next)
        {
            if(n->exp == i->exp)
            {
                i->coef += n->coef;

                if(isZero(i->coef))
                {
                    if(i == t)
                    {
                        p->entrance = i->next;
                        free(i);
                    }
                    else
                    {
                        t->next = i->next;
                        free(i);
                    }
                }

                --p->n;

                return;
            }
            else if(i->next == NULL)
            {
                i->next = n;
                ++p->n;
                return;
            }
            else if(n->exp > i->exp && n->exp < i->next->exp)
            {
                n->next = i->next;
                i->next = n;
                ++p->n;
                return;
            }

            t = i;
        }
    }
}
int pFloat(double i)
{
    char a[256];
    sprintf(a, "%.12f", i);

    for(int acc = strlen(a) - 1; acc > 0; acc--)
        if(a[acc] != '0')
        {
            if(a[acc] == '.')
            {
                a[acc] = '\0';
                printf("%s", a);
                return acc;
            }

            a[acc + 1] = '\0';
            printf("%s", a);
            return acc + 1;
        }
}
inline int isZero(double i)
{
    return fabs(i) < 0.000000001;
}
void input(struct polynomial *p)
{
    double c, e;
    scanf("%lf%lf", &c, &e);
    struct node *np = createN(c, e);
    insert(np, p);
}
void printPoly(struct polynomial *p)
{
    int t = 1;
    struct node *i;
    i = p->entrance;

    while(i!=NULL)
    {
        if(t != 1)
        {
            if(i->coef < 0)
                putchar('-');
            else
                putchar('+');

            putchar(' ');
        }
        else
        {
            if(i->coef < 0)
            {
                putchar('-');
                putchar(' ');
            }

            t = 0;
        }

        if(fabs(i->coef) != 1)
        {
            pFloat(fabs(i->coef));

            putchar(' ');
            putchar('*');
            putchar(' ');
        }

        t = 0;
        putchar('x');

        if(i->exp != 1)
        {
            putchar('^');
            pFloat(i->exp);
        }

        putchar(' ');
        i = i->next;
    }
    if(p->entrance == NULL)
        pFloat(p->c);
    else
    {
        if(isZero(p->c))
            return;
        else if(p->c<0)
        {
            putchar('-');
            putchar(' ');
            pFloat(fabs(p->c));
        }
        else
        {
            putchar('+');
            putchar(' ');
            pFloat(p->c);
        }
    }
}
void add(struct polynomial *res, struct polynomial *add1,
         struct polynomial *add2)
{
    struct node *en;
    res->c = add1->c + add2->c;
    en     = add1->entrance;

    while(en != NULL)
    {
        insert(en, res);
        en = en->next;
    }

    en = add2->entrance;

    while(en != NULL)
    {
        insert(en, res);
        en = en->next;
    }
}
void add_to(struct polynomial *res, struct polynomial *add1)
{
    struct node *en;
    res->c += add1->c;
    en = add1->entrance;

    while(en != NULL)
    {
        insert(en, res);
        en = en->next;
    }
}

void sub(struct polynomial *res, struct polynomial *left,
         struct polynomial *right)
{
    struct node *en;
    res->c = left->c - right->c;
    en     = left->entrance;

    while(en != NULL)
    {
        insert(en, res);
        en = en->next;
    }

    en = right->entrance;

    while(en != NULL)
    {
        struct node *temp = createN(-en->coef, en->exp);
        insert(temp, res);
        free(temp);
        en = en->next;
    }
}

void mult(struct polynomial *res, struct polynomial *lp, struct polynomial *rp)
{
    res->c = lp->c * rp->c;
    struct node *tn;

    double t = lp->c;

    if(rp->entrance != NULL)
        for(struct node *i = rp->entrance; i != NULL; i = i->next)
        {
            tn = createN(t * i->coef, i->exp);
            insert(tn, res);
            free(tn);
        }

    t = rp->c;

    if(lp->entrance != NULL)
        for(struct node *i = lp->entrance; i != NULL; i = i->next)
        {
            tn = createN(t * i->coef, i->exp);
            insert(tn, res);
            free(tn);
        }

    for(struct node *i = lp->entrance; i != NULL; i = i->next)
        for(struct node *j = rp->entrance; j != NULL; j = j->next)
        {
            tn = createN((i->coef) * (j->coef), i->exp + j->exp);
            insert(tn, res);
            free(tn);
        }

    return;
}

void divide(int mode, struct polynomial *res, struct polynomial *lp,
            struct polynomial *rp)
{
    struct node *tn;

    if(lp->entrance == NULL)
    {
        if(rp->entrance == NULL)
        {
            if(rp->c != 0)
                tn = createN(lp->c / rp->c, 0);
            else
            {
                printf("divisor can't be zero\n");
                exit(-1);
            }

            insert(tn, res);
            free(tn);
            return;
        }
        else
            return;
    }

    struct node *tl = lp->entrance;

    if(rp->entrance == NULL)
    {
        if(isZero(rp->c))
        {
            printf("divisor can't be zero\n");
            exit(-1);
        }

        res->c = lp->c / rp->c;

        if(mode == 1)
            while(tl != NULL)
            {
                tn = createN(tl->coef / rp->c, tl->exp);
                insert(tn, res);
                free(tn);
                tl = tl->next;
            }
        else if(mode == 2)
        {
            printf("no mod method for a polynomial over a number\n");
            exit(-1);
        }

        return;
    }

    struct polynomial *tpres;

    struct polynomial *tp;

    while(tl != NULL && tl->exp - rp->entrance->exp >= 0)
    {
        double rc  = tl->coef / rp->entrance->coef;
        double ret = tl->exp;
        double re  = ret - rp->entrance->exp;
        tp         = createP();
        tpres      = createP();
        tn         = createN(rc, re);

        if(mode == 1) insert(tn, res);

        free(tn);
        tn = createN(-rc, re);
        tp = createPN(tn);
        free(tn);
        mult(tpres, tp, rp);
        add_to(lp, tpres);
        destroyP(tpres);
        destroyP(tp);
        tl = lp->entrance;
        //for (tl = lp->entrance; tl != NULL; tl=tl->next)
        //   if (tl->exp < ret) break;
    }

    if(mode == 2)
    {
        res->c = lp->c;
        tn     = lp->entrance;

        while(tn != NULL)
        {
            insert(tn, res);
            tn = tn->next;
        }
    }
}

struct polynomial *parse(char *input)
{
    int i = -1, j = 0, k = 0;
    int length = strlen(input);
    struct polynomial *poly_stack[128];
    char optr[32];
    double coef, exp;
    int flag = 0, x = 0;
    int minus_flag = 1;
    int coef_flag  = 0;
    int sinm_flag  = 0;
    char c;

    optr[k++]       = '#';
    poly_stack[j++] = createP();

    for(int i = 1; i < 128; i++) poly_stack[i] = NULL;

    for(int i = 1; i < 32; i++) optr[i] = '0';

    c = '+';

    while(i < length)
    {
        if(sinm_flag == 0)
        {
            if(c == '-')
            {
                minus_flag = -1;
                sinm_flag  = 1;
                i++;
                c = input[i];
                continue;
            }

            else if(isdigit(c) || c == '*')
            {
                sinm_flag = 1;
                continue;
            }
        }

        if(flag == 1 || isdigit(c))
        {
            if(flag == 0)
            {
                sscanf(input + i, "%lf", &coef);
                coef_flag = 1;
            }

            else
                sscanf(input + i, "%lf", &exp);

            while(isdigit(input[i]) || input[i] == '.') i++;

            i--;

            if(flag == 1)
            {
                struct node *t        = createN(minus_flag * coef, exp);
                struct polynomial *tp = createPN(t);
                coef_flag             = 0;
                minus_flag            = 1;
                poly_stack[j++]       = tp;
                free(t);
                flag = 0;
                x    = 0;
                coef = exp = 0;
            }
        }
        else if(c == ' ')
        {
            i++;
            c = input[i];
            continue;
        }

        else if(c == '^' && x == 1)
            flag = 1;

        else if(c == 'x')
        {
            if(coef_flag == 0) coef = 1;

            coef_flag = 1;
            x         = 1;
            sinm_flag = 1;
        }

        else if(c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
                c == '(' || c == ')')
        {
            if(c == '(') sinm_flag = 0;

            if(c == ')') sinm_flag = 1;

            if(x == 1 && flag == 0)
            {
                struct node *t        = createN(minus_flag * coef, 1);
                struct polynomial *tp = createPN(t);
                coef_flag             = 0;
                minus_flag            = 1;
                x                     = 0;
                poly_stack[j++]       = tp;
                sinm_flag             = 1;
                free(t);
            }
            else if(x == 0 && coef_flag == 1)
            {
                struct polynomial *t = createP();
                t->c                 = minus_flag * coef;
                coef                 = 0;
                coef_flag            = 0;
                minus_flag           = 1;
                poly_stack[j++]      = t;
                sinm_flag            = 1;
            }

            if(c == '-')
            {
                minus_flag = -1;
                c          = '+';
            }

            struct polynomial *t1 = poly_stack[j - 2];

            struct polynomial *t2 = poly_stack[j - 1];

            switch(precede(optr[k - 1], c))
            {
            case '<': optr[k++] = c; break;

            case '=': k--; break;

            case '>':

                poly_stack[j - 2] = operate(t1, optr[k - 1], t2);
                sinm_flag         = 1;
                free(t1);
                free(t2);
                k--;
                j--;
                continue;
            }
        }

        ++i;
        c = input[i];
    }

    if(x == 1)
    {
        struct node *t        = createN(minus_flag * coef, 1);
        struct polynomial *tp = createPN(t);
        coef_flag             = 0;
        minus_flag            = 1;
        poly_stack[j++]       = tp;
        free(t);
        flag = 0;
        x    = 0;
        coef = 0;
    }

    if(coef_flag == 1)
    {
        struct polynomial *t = createP();
        t->c                 = minus_flag * coef;
        minus_flag           = 1;
        coef                 = 0;
        poly_stack[j++]      = t;
    }

    j = j - 2;

    while(j >= 0)
    {
        struct polynomial *t1 = poly_stack[j];
        struct polynomial *t2 = poly_stack[j + 1];
        poly_stack[j]         = operate(t1, optr[j + 1], t2);
        free(t1);
        free(t2);
        j--;
    }

    return poly_stack[0];
}
struct polynomial *operate(struct polynomial *lp, char op,
                           struct polynomial *rp)
{
    struct polynomial *t = createP();

    if(op == '+')
        add(t, lp, rp);
    else if(op == '*')
        mult(t, lp, rp);
    else if(op == '/')
        divide(1, t, lp, rp);
    else if(op == '%')
        divide(2, t, lp, rp);

    return t;
}
struct polynomial *createPN(struct node *tn)
{
    struct polynomial *np =
        (struct polynomial *)malloc(sizeof(struct polynomial));
    np->entrance = NULL;
    np->n        = 0;
    np->c        = 0;
    insert(tn, np);
    return np;
}
char precede(char lp, char rp)
{
    if(lp == '+' || lp == '-')
        if(rp == '+' || rp == '-') return '>';

    if(lp == ')') return '>';

    if(lp == '(' && rp == ')') return '=';

    if(rp == ')') return '>';

    if(lp == '*' || lp == '/' || lp == '%')
        if(rp == '*' || rp == '/' || rp == '%' || rp == '+') return '>';

    return '<';
}
