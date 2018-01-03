//
//  condition.cpp
//  
//
//  Created by Β¬Πρ»Τ on 16/7/1.
//
//

#include "condition.h"

bool Condition::ifRight(int content){
    stringstream ss;
    ss << right_Value;
    int tContent;
    ss >> tContent;
    
    switch (operator_Type) {
        case CONDITION_EQUAL:
            return content == tContent;
            break;
        case CONDITION_NOT_EQUAL:
            return content != tContent;
            break;
        case CONDITION_LESS:
            return content < tContent;
            break;
        case CONDITION_MORE:
            return content > tContent;
            break;
        case CONDITION_EQUAL_LESS:
            return content <= tContent;
            break;
        case CONDITION_EQUAL_MORE:
            return content >= tContent;
            break;
        default:
            return true;
            break;
    }
}


bool Condition::ifRight(float content){
    stringstream ss;
    ss << right_Value;
    float tContent;
    ss >> tContent;
    
    switch (operator_Type) {
        case CONDITION_EQUAL:
            return content == tContent;
            break;
        case CONDITION_NOT_EQUAL:
            return content != tContent;
            break;
        case CONDITION_LESS:
            return content < tContent;
            break;
        case CONDITION_MORE:
            return content > tContent;
            break;
        case CONDITION_EQUAL_LESS:
            return content <= tContent;
            break;
        case CONDITION_EQUAL_MORE:
            return content >= tContent;
            break;
        default:
            return true;
            break;
    }

    
}

bool Condition::ifRight(string content){
    stringstream ss;
    ss << right_Value;
    string tContent;
    ss >> tContent;
    
    switch (operator_Type) {
        case CONDITION_EQUAL:
            return content == tContent;
            break;
        case CONDITION_NOT_EQUAL:
            return content != tContent;
            break;
        case CONDITION_LESS:
            return content < tContent;
            break;
        case CONDITION_MORE:
            return content > tContent;
            break;
        case CONDITION_EQUAL_LESS:
            return content <= tContent;
            break;
        case CONDITION_EQUAL_MORE:
            return content >= tContent;
            break;
        default:
            return true;
            break;
    }

    
}