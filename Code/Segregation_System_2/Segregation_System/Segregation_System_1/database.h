#pragma once
// database.h

#ifndef DATABASE_H
#define DATABASE_H
// database.h (헤더 파일)
// 데이터베이스 연결 함수 선언 및 구조체 정의
#include <string>
#include <mysql/jdbc.h>
#include "database_config.h"

using namespace std;
using namespace sql;

// 데이터베이스 연결 함수 선언
Connection* db_connect_to(const DatabaseConfig& db_config);

// moble_customers 테이블 생성 함수
void db_create_mobletable(Connection* conn);

// 고객 정보 삽입 함수
string db_insertCustomerData(Connection* conn, const string& customer_name, const string& address, const string& phone);

// 데이터베이스에서 moble_customers 테이블 삭제하는 함수
void db_drop_moble_customers_table(Connection* conn);


// 제품명으로 product_id를 조회하는 함수
int db_getOrderId(sql::Connection* conn, const string& product, const string& brand, const int customer_id);

// product 테이블에 제품 정보를 저장하는 함수
string db_insertProductData(sql::Connection* conn, const string& product, const string& product_brand, int price, int customer_id);
int db_checkCustomerExists(sql::Connection* conn, const string& customer_name, const string& phone_number);

int db_checkOrderExists(sql::Connection* conn, const int order_num);

//반품된 상품 DB에 반환하는 코드
void db_updateProdOk(sql::Connection* conn, int order_id);

int db_getCustomerId(sql::Connection* conn, const string& customer_name, const string& addr, const string& phone_number);

struct OrderDetails {
    int order_id;
    string product_name;
    int customer_id;
    string customer_name;
    string addr;
    string ph;
};



void db_getOrderDetails(sql::Connection* conn, int order_num);
void printOrderData(sql::Connection* conn, int order_num);

#endif // DATABASE_H
