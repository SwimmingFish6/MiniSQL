#include "Interpreter.h"
#include <fstream>
int main(void)
{
    Interpreter inter;
    char input[1024];
    string line;
    vector<Result> resultList;
    int resultNumber;
    while (1) {
        line = "";
        cin.getline(input, sizeof(input));
        line += input;
        while (input[strlen(input) - 1] != ';') {
            cin.getline(input, sizeof(input));
            line += input;
        }
        inter.getPStmt(line);
        resultList.clear();
        inter.runPStmt(&resultList);
        resultNumber = 0;
        while (resultNumber < resultList.size()) {
            if (resultList[resultNumber].operationType == EXECFILE) {
                ifstream in(resultList[resultNumber].fileName);
                if (!in.is_open()) {
                    cout << "EXECFILE      :FILE_OPEN_FAIL" << endl;
                }
                else {
                    cout << "EXECFILE      :FILE_OPEN_SUCCESS" << endl;
                    vector<Result> resultList1;
                    while (!in.eof()) {
                        in.getline(input, sizeof(input));
                        inter.getPStmt(input);
                        resultList1.clear();
                        inter.runPStmt(&resultList1);
                        int resultNumber1 = 0;
                        while (resultNumber1 < resultList1.size()) {
                            switch (resultList1[resultNumber1].operationType) {
                            case CREATE_TABLE:
                                cout << "CREATE_TABLE  :";
                                break;
                            case CREATE_INDEX:
                                cout << "CREATE_INDEX  :";
                                break;
                            case SELECT:
                                cout << "SELECT        :";
                                break;
                            case INSERT:
                                cout << "INSERT        :";
                                break;
                            case DELETE:
                                cout << "DELETE        :";
                                break;
                            case DROP_TABLE:
                                cout << "DROP_TABLE    :";
                                break;
                            case DROP_INDEX:
                                cout << "DROP_INDEX    :";
                                break;
                            case EXECFILE:
                                cout << "EXECFILE      :";
                                break;
                            case QUIT:
                                cout << "QUIT          :";
                                break;
                            default:
                                cout << "ILLEGAL       :";
                                break;
                            }
                            switch (resultList1[resultNumber1].result) {
                            case SUCCESS:
                                cout << "SUCCESS" << endl;
                                break;
                            case TABLE_NAME_EXIST:
                                cout << "TABLE_NAME_EXIST" << endl;
                                break;
                            case TABLE_NAME_NOT_EXIST:
                                cout << "TABLE_NAME_NOT_EXIST" << endl;
                                break;
                            case FIELD_NAME_TOO_LONG:
                                cout << "FIELD_NAME_TOO_LONG" << endl;
                                break;
                            case INDEX_NAME_EXIST:
                                cout << "INDEX_NAME_EXIST" << endl;
                                break;
                            case FIELD_NAME_NOT_EXIST:
                                cout << "FIELD_NAME_NOT_EXIST" << endl;
                                break;
                            case FIELD_INEDX_EXIST:
                                cout << "FIELD_INEDX_EXIST" << endl;
                                break;
                            case INDEX_NAME_NOT_EXIST:
                                cout << "INDEX_NAME_NOT_EXIST" << endl;
                                break;
                            case ATTRIBUTE_COUNT_ERROR:
                                cout << "ATTRIBUTE_COUNT_ERROR" << endl;
                                break;
                            case ATTRIBUTE_TYPE_ERROR:
                                cout << "ATTRIBUTE_TYPE_ERROR" << endl;
                                break;
                            case UNIQUE_ERROR:
                                cout << "UNIQUE_ERROR" << endl;
                                break;
                            case SELECT_NOT_FIND:
                                cout << "SELECT_NOT_FIND" << endl;
                                break;
                            case VALUE_TYPE_ERROR:
                                cout << "VALUE_TYPE_ERROR" << endl;
                                break;
                            case DELETE_NOT_EXIST:
                                cout << "DELETE_NOT_EXIST" << endl;
                                break;
                            case ILLEGAL:
                                cout << "SQL_STATEMENT_ILLEGAL" << endl;
                                break;
                            case ATTRIBUTEERROR:
                                cout << "ATTRIBUTEERROR" << endl;
                                break;
                            }
                            if (resultList1[resultNumber1].operationType == QUIT)
                                return 0;
                            if (resultList1[resultNumber1].operationType == SELECT && resultList1[resultNumber1].result == SUCCESS) {
                                for (int j = 0; j < resultList1[resultNumber1].attributeList.size(); j++) {
                                    cout << resultList1[resultNumber1].attributeList[j] << '\t';
                                }
                                cout << endl;
                                for (int i = 0; i < resultList1[resultNumber1].recordList.size(); i++) {
                                    for (int j = 0; j < resultList1[resultNumber1].attributeList.size(); j++)
                                        if (resultList1[resultNumber1].typeList[j] <= 255) {
                                            resultList1[resultNumber1].recordList[i].record[j][resultList1[resultNumber1].typeList[j]] = '\0';
                                            cout << resultList1[resultNumber1].recordList[i].record[j] << '\t';
                                        }
                                        else if (resultList1[resultNumber1].typeList[j] == TYPE_INT) {
                                            int num;
                                            memcpy(&num, resultList1[resultNumber1].recordList[i].record[j], 4);
                                            cout << num << '\t';
                                        }
                                        else if (resultList1[resultNumber1].typeList[j] == TYPE_FLOAT) {
                                            float num;
                                            memcpy(&num, resultList1[resultNumber1].recordList[i].record[j], 4);
                                            cout << num << '\t';
                                        }
                                        cout << endl;
                                }
                            }
                            resultNumber1++;
                        }
                    }
                    in.close();
                }
                resultNumber++;
                continue;
            }
            switch (resultList[resultNumber].operationType) {
            case CREATE_TABLE:
                cout << "CREATE_TABLE  :";
                break;
            case CREATE_INDEX:
                cout << "CREATE_INDEX  :";
                break;
            case SELECT:
                cout << "SELECT        :";
                break;
            case INSERT:
                cout << "INSERT        :";
                break;
            case DELETE:
                cout << "DELETE        :";
                break;
            case DROP_TABLE:
                cout << "DROP_TABLE    :";
                break;
            case DROP_INDEX:
                cout << "DROP_INDEX    :";
                break;
            case EXECFILE:
                cout << "EXECFILE      :";
                break;
            case QUIT:
                cout << "QUIT          :";
                break;
            default:
                cout << "ILLEGAL       :";
                break;
            }
            switch (resultList[resultNumber].result) {
            case SUCCESS:
                cout << "SUCCESS" << endl;
                break;
            case TABLE_NAME_EXIST:
                cout << "TABLE_NAME_EXIST" << endl;
                break;
            case TABLE_NAME_NOT_EXIST:
                cout << "TABLE_NAME_NOT_EXIST" << endl;
                break;
            case FIELD_NAME_TOO_LONG:
                cout << "FIELD_NAME_TOO_LONG" << endl;
                break;
            case INDEX_NAME_EXIST:
                cout << "INDEX_NAME_EXIST" << endl;
                break;
            case FIELD_NAME_NOT_EXIST:
                cout << "FIELD_NAME_NOT_EXIST" << endl;
                break;
            case FIELD_INEDX_EXIST:
                cout << "FIELD_INEDX_EXIST" << endl;
                break;
            case INDEX_NAME_NOT_EXIST:
                cout << "INDEX_NAME_NOT_EXIST" << endl;
                break;
            case ATTRIBUTE_COUNT_ERROR:
                cout << "ATTRIBUTE_COUNT_ERROR" << endl;
                break;
            case ATTRIBUTE_TYPE_ERROR:
                cout << "ATTRIBUTE_TYPE_ERROR" << endl;
                break;
            case UNIQUE_ERROR:
                cout << "UNIQUE_ERROR" << endl;
                break;
            case SELECT_NOT_FIND:
                cout << "SELECT_NOT_FIND" << endl;
                break;
            case VALUE_TYPE_ERROR:
                cout << "VALUE_TYPE_ERROR" << endl;
                break;
            case DELETE_NOT_EXIST:
                cout << "DELETE_NOT_EXIST" << endl;
                break;
            case ILLEGAL:
                cout << "SQL_STATEMENT_ILLEGAL" << endl;
                break;
            case ATTRIBUTEERROR:
                cout << "ATTRIBUTEERROR" << endl;
                break;
            }
            if (resultList[resultNumber].operationType == QUIT)
                return 0;
            if (resultList[resultNumber].operationType == SELECT && resultList[resultNumber].result == SUCCESS) {
                for (int j = 0; j < resultList[resultNumber].attributeList.size(); j++) {
                    cout << resultList[resultNumber].attributeList[j] << '\t';
                }
                cout << endl;
                for (int i = 0; i < resultList[resultNumber].recordList.size(); i++) {
                    for (int j = 0; j < resultList[resultNumber].attributeList.size(); j++)
                        if (resultList[resultNumber].typeList[j] <= 255) {
                            resultList[resultNumber].recordList[i].record[j][resultList[resultNumber].typeList[j]] = '\0';
                            cout << resultList[resultNumber].recordList[i].record[j] << '\t';
                        }
                        else if (resultList[resultNumber].typeList[j] == TYPE_INT) {
                            int num;
                            memcpy(&num, resultList[resultNumber].recordList[i].record[j], 4);
                            cout << num << '\t';
                        }
                        else if (resultList[resultNumber].typeList[j] == TYPE_FLOAT) {
                            float num;
                            memcpy(&num, resultList[resultNumber].recordList[i].record[j], 4);
                            cout << num << '\t';
                        }
                        cout << endl;
                }
            }
            resultNumber++;
        }
    }
    return 0;
}