#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#define MAXSIZE 100

int heapSize=0;

typedef union
{
    float fData;
    char cData;
} Item;

typedef struct   //Variable's data store in the heap (array)
{
    char key[10];
    float value;
} Variable;

typedef struct
{
    int top;
    Item items[MAXSIZE];
} Stack;

typedef struct Node
{
    struct Node *right;
    struct Node *left;
    float data;
    char *key;
} Node;

Node *newNode(float value,char key[])
{
    Node *node=malloc(sizeof(Node));
    node->left=NULL;
    node->right=NULL;
    node->key=malloc(strlen(key)+1);
    strcpy(node->key,key);
    node->data=value;
    return node;
}

Node* insert(Node* root,float value,char key[])
{
    if(root == NULL)
        return newNode(value,key);
    if(strcmp(key,root->key) < 0)
        root->left = insert(root->left,value,key);
    else if(strcmp(key,root->key) > 0)
        root->right = insert(root->right,value,key);
    else   //To avoid insert duplicates if they exist
        return root;
    return root;
}

Node* search(Node* root,char* key)
{
    if(root == NULL)
        return NULL;
    if(strcmp(key,root->key) == 0)
        return root;
    else if(strcmp(key,root->key) < 0)
        return search(root->left, key);
    else
        return search(root->right, key);
}

Stack* initialize ()
{
    Stack*s=malloc(sizeof(Stack));
    s->top=0;
    return s;
}

void push(Stack*s,Item value)
{
    s->items[s->top++]=value;
}

Item pop (Stack*s)
{
    return s->items[--s->top];
}

Item top(Stack*s)
{
    return s->items[s->top-1];
}

int isEmpty (Stack*s)
{
    if(s->top==0)
        return 1;
    else
        return 0;
}

int checkStack(Stack*s)  //To check whether the stack contains more than one element or not
{
    int count=0;
    Stack*temp=initialize();
    while(!isEmpty(s))
    {
        push(temp,pop(s));
        count++;
    }
    while(!isEmpty(temp))
        push(s,pop(temp));
    return count;
}

void error()
{
    printf("Error in Expression\n");
    exit(-1);
}

int prec(char op)
{
    switch(op)
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '^':
        return 3;
    default:
        error();
        return -1;
    }
}

int isLowerPrec(char op1,char op2)
{
    return prec(op1)<=prec(op2);
}

void infixToPostfix(char* infix, char* postfix)  //Stack will contain operators
{
    Stack*s=initialize();
    int i,index=0;
    Item temp;
    for(i=0; i<strlen(infix); i++)
    {
        if(infix[i]==' ')
            continue;
        if(isdigit(infix[i]))
        {
            postfix[index++]=infix[i];  //Any number put it directly in the postfix
            if(infix[i+1]==' '|| infix[i+1]=='+' || infix[i+1]=='-' || infix[i+1]=='*' || infix[i+1]=='/' || infix[i+1]=='^' || infix[i+1]==')' || infix[i+1]=='\0')   //To add space after the last digit or check for multiple digits
                postfix[index++]=' ';   //To add space in all cases except multiple digits or float numbers '.'
        }
        else if(infix[i]=='-' && infix[i+1]!=' ' && !isdigit(infix[i-1]))  //To handle negative numbers
            postfix[index++]=infix[i];
        else if(infix[i]=='.')
            postfix[index++]=infix[i];
        else if(infix[i]=='(')
        {
            temp.cData=infix[i];
            push(s,temp);
        }
        else if(infix[i]==')')
        {
            while(!isEmpty(s) && top(s).cData!='(')  //pop from stack until open bracket '('
            {
                postfix[index++]=pop(s).cData;
                postfix[index++]=' ';  //To add space after the operator
            }
            if(isEmpty(s) || top(s).cData!='(')
                error();
            pop(s);  //Get rid of open bracket '('
        }
        else  //operator
        {
            while(!isEmpty(s) && top(s).cData!='(' && isLowerPrec(infix[i],top(s).cData))  //Enters the loop if the operator has lower or equal precedence with the top operator in stack
            {
                temp.cData=pop(s).cData;
                if(temp.cData!='(')
                {
                    postfix[index++]=temp.cData;
                    postfix[index++]=' ';
                }
            }
            temp.cData=infix[i];
            push(s,temp);
        }
    }
    while(!isEmpty(s))  //remaining
    {
        if(top(s).cData=='(')
            error();
        postfix[index++]=pop(s).cData;
        postfix[index++]=' ';
    }
    postfix[index]=0;  // NULL character to indicate the end of the string (postfix expression)
}

float evaluatePostfix(char*postfix)  //Stack will contain numbers
{
    Stack*s=initialize();
    int i,flag=0;
    Item op1,op2,res,temp;
    for(i=0; i<strlen(postfix); i++)
    {
        int pointCount = 0;
        if(postfix[i]==' ')
            continue;
        else if(isdigit(postfix[i]) || (postfix[i]=='-' && postfix[i+1]!=' '))
        {
            temp.fData=0;
            int power=-1,checkPoint=1;
            if(postfix[i]=='-')
            {
                flag=1;
                continue;
            }
            while(isdigit(postfix[i]) || postfix[i]=='.')
            {
                if(pointCount>1)
                    error();
                if(postfix[i]=='.')
                {
                    i++;
                    pointCount++;
                    continue;
                }
                if(postfix[i-checkPoint]=='.')
                {
                    temp.fData+=(postfix[i]-'0')*pow(10.0,power);  //To convert a string of a single or multiple digits to a numerical value
                    power--;
                    checkPoint++;  // i shifts then checkPoint must also shift
                }
                else
                    temp.fData=temp.fData*10.0+(postfix[i]-'0');  //for single and multiple digits
                i++;
            }
            i--;
            if(flag==0)
                push(s,temp);
            else
            {
                temp.fData*=-1.0;
                push(s,temp);
                flag=0;
            }
        }
        else  //operator
        {
            if(!isEmpty(s))
                op1=pop(s);
            else
            {
                error();
                break;
            }
            if(!isEmpty(s))
                op2=pop(s);
            else
            {
                error();
                break;
            }
            switch(postfix[i])
            {
            case '+':
                res.fData=op1.fData+op2.fData;
                push(s,res);
                break;
            case '-':
                res.fData=op2.fData-op1.fData;
                push(s,res);
                break;
            case '*':
                res.fData=op1.fData*op2.fData;
                push(s,res);
                break;
            case '/':
                if(op1.fData == 0)  //To avoid run time error on dividing by zero
                    error();
                res.fData=op2.fData/op1.fData;
                push(s,res);
                break;
            case '^':
                res.fData=pow(op2.fData,op1.fData);
                push(s,res);
                break;
            default:
                error();
                break;
            }

        }
    }
    int count=checkStack(s);
    if(count>1)
    {
        error();  //Because the stack should contain only one element at the end
        exit(-1);
    }
    else
        return pop(s).fData;
}

void insertInHeap(Variable heap[],float data,char *key)
{
    heapSize++;
    int i = heapSize - 1;
    heap[i].value = data;
    strcpy(heap[i].key,key);
}

int searchInHeap(Variable heap[],char *key)
{
    int index=0;
    for(int i=0; i<heapSize; i++)
    {
        if(strcmp(heap[i].key,key)==0)
        {
            index=i;
            break;
        }
    }
    return index;
}

void swap(Variable* heapVariable1, Variable* heapVariable2)
{
    Variable tempVariable = *heapVariable1;
    *heapVariable1 = *heapVariable2;
    *heapVariable2 = tempVariable;
}

void heapify(Variable heap[],int heapSize,int i)
{
    int largest=i;
    int left=2*i+1;
    int right=2*i+2;
    if(left<heapSize && heap[left].value>heap[largest].value)
    {
        largest=left;
    }
    if(right<heapSize && heap[right].value>heap[largest].value)
    {
        largest=right;
    }
    if(largest!=i)
    {
        swap(&heap[i],&heap[largest]);
        heapify(heap,heapSize,largest);
    }
}

void sortHeap(Variable heap[])
{
    for(int i=(heapSize-1)/2; i>=0; i--)
    {
        heapify(heap,heapSize,i);
    }
    for(int i=heapSize-1; i>=0; i--)
    {
        swap(&heap[i],&heap[0]);
        heapify(heap,i,0);
    }

}

Node* put(Node* root, Variable heap[], char* key, float value)
{
    Node *node=search(root,key);
    if(node)
    {
        node->data=value;
        int index=searchInHeap(heap,key);
        heap[index].value=value;
    }
    else
    {
        root=insert(root,value,key);
        insertInHeap(heap,value,key);
    }
    return root;
}

void removeSpaces(char* str)
{
    int count=0;
    for(int i=0; str[i]!='\0'; i++)
    {
        if(str[i]!=' ')
        {
            str[count++]=str[i];
        }
    }
    str[count]='\0';
}

int getFileLinesCount()
{
    FILE* fp = fopen("Src.txt","r");
    int lines=0;
    char line[50];
    while(fgets(line,50, fp))
    {
        lines++;
    }
    fclose(fp);
    return lines;
}

char* getLine(FILE *fp)
{
    char* temp = malloc(50);
    fgets(temp,50,fp);
    strtok(temp,"\n");
    return temp;
}

int checkEquation(char *str)
{
    char* p = malloc(strlen(str)+1);
    strcpy(p,str);
    removeSpaces(p);
    int i,count=0;
    for(i=0; p[i]!='\0'; i++)
    {
        if(p[i]=='=')
            count++;
    }
    if(count!=1)
        return 0;
    if(p[0]=='=')  //Checks if the left side is empty
        return 0;
    if(p[strlen(p)-1]=='=')  //Checks if the right side is empty
        return 0;
    return 1;
}

void checkFile(FILE *fp)
{
    if(!fp)
    {
        printf("File not found !! Please rename the text file to Src.txt");
        exit(-1);
    }
    fseek (fp, 0, SEEK_END);
    int size = ftell(fp);
    if(size==0)
    {
        printf("Empty File !!");
        exit(-1);
    }
}

Node* readFile(Variable heap[])
{
    Node* root = NULL;
    FILE* fp = fopen("Src.txt","r");
    checkFile(fp);
    fseek(fp,0,SEEK_SET);
    for(int i=1; i<=getFileLinesCount(); i++)
    {
        char temp[50];
        strcpy(temp,getLine(fp));
        removeSpaces(temp);
        if(temp[0] == '\n' || temp[0] == '\0')  //To handle if there was an empty line (having white spaces or not) between other file lines
            continue;
        if(strstr(temp,"=") && checkEquation(temp))
        {
            char *token = strtok(temp,"=");
            char leftHandSide[strlen(token)+1];
            strcpy(leftHandSide,token);
            if(strstr(leftHandSide,"+")||strstr(leftHandSide,"-")||strstr(leftHandSide,"*")||strstr(leftHandSide,"/")||strstr(leftHandSide,"^")||strstr(leftHandSide,"(")||strstr(leftHandSide,")"))
            {
                error();
            }
            token=strtok(NULL,"\0");
            char rightHandSide[256];
            strcpy(rightHandSide,token);
            char array[256]="";
            for(int j=0; j<strlen(rightHandSide); j++)
            {
                if((rightHandSide[j]>='a'&&rightHandSide[j]<='z')||(rightHandSide[j]>='A'&&rightHandSide[j]<='Z'))
                {
                    int index=0;
                    char variableName[50];
                    variableName[index++]=rightHandSide[j++];
                    while(((rightHandSide[j]>='a'&&rightHandSide[j]<='z')||(rightHandSide[j]>='A'&&rightHandSide[j]<='Z')) && (j<strlen(rightHandSide)))
                    {
                        variableName[index++]=rightHandSide[j++];
                    }
                    variableName[index]='\0';
                    Node* node=search(root,variableName);
                    if(node)
                    {
                        sprintf(variableName,"%f",node->data);
                        strcat(array,variableName);
                    }
                    else
                    {
                        error();
                    }
                    j--;
                }
                else
                {
                    char temporary[2];
                    temporary[0]=rightHandSide[j];
                    temporary[1]='\0';
                    strcat(array,temporary);
                }
            }
            char postfix[50]= "";
            infixToPostfix(array,postfix);
            root = put(root,heap,leftHandSide,evaluatePostfix(postfix));
        }
        else
        {
            error();
        }
    }
    fclose(fp);
    return root;
}

void inOrderTraversal(Node *root)
{
    if(root)
    {
        inOrderTraversal(root->left);
        printf("%s = %f\n",root->key,root->data);
        inOrderTraversal(root->right);
    }
}

void printHeap(Variable heap[])
{
    printf("\n");
    int i;
    for(i=0; i< heapSize; i++)
    {
        printf("%s = %f\n",heap[i].key,heap[i].value);
    }
}

int main()
{
    Variable heap[100];
    Node* root;
    root=readFile(heap);
    inOrderTraversal(root);
    sortHeap(heap);
    printHeap(heap);
    return 0;
}
