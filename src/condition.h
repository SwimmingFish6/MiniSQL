#ifndef ____condition__
#define ____condition__

#include <string>
#include <sstream>

#define CONDITION_EQUAL         (0)    //=
#define CONDITION_NOT_EQUAL     (1)    //!=
#define CONDITION_LESS          (2)    //<
#define CONDITION_MORE          (3)    //>
#define CONDITION_EQUAL_LESS    (4)    //<=
#define CONDITION_EQUAL_MORE    (5)    //>=

using namespace std;

class Condition{
private:
    string left_Value;
    string right_Value;
    int operator_Type;
    
public:
    bool ifRight(int content);
    bool ifRight(float content);
    bool ifRight(string content);
    string getAttributeName() { return left_Value; }
    int getOp() { return operator_Type; }
    string getValue() { return right_Value; }
    Condition(string a, string v, int op):left_Value(a),right_Value(v), operator_Type(op){};
};

#endif /* defined(____condition__) */
