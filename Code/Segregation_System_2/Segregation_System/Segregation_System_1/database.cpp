// database.cpp
//#include "pch.h"
#include "database.h"
#include <iostream>

using namespace std;
using namespace sql;

// GLOBAL VARIABLE
OrderDetails orderData;

// 데이터베이스 연결 함수 (예시)
Connection* db_connect_to(const DatabaseConfig& db_config)
{
    try {
        sql::Driver* driver;
        sql::Connection* conn;

        driver = get_driver_instance();
        conn = driver->connect(db_config.db_host, db_config.db_user, db_config.db_password);
        conn->setSchema(db_config.db_schema);

        return conn; // 연결 객체 반환
    }
    catch (sql::SQLException& e) {
        cout << u8"DB 연결 오류: " << e.what() << endl;
        return nullptr;
    }
}

// 데이터베이스에 고객 데이터 삽입하는 함수
string db_insertCustomerData(Connection* conn, const string& customer_name, const string& address, const string& phone)
{
    try
    {
        // 고객 데이터 삽입
        sql::PreparedStatement* db_stmt = conn->prepareStatement(
            "INSERT INTO customer (customer_name, address, phone) VALUES (?, ?, ?)");

        // 데이터 바인딩
        db_stmt->setString(1, customer_name);
        db_stmt->setString(2, address);
        db_stmt->setString(3, phone);
       

        // 쿼리 실행
        db_stmt->executeUpdate();

        delete db_stmt;
        return "SUCESS INSERT CUSTOMER";
    }
    catch (sql::SQLException& e)
    {
        return string(e.what());
    }
}

// product 테이블에 제품 정보를 저장하는 함수
string db_insertProductData(sql::Connection* conn, const string& product, const string& product_brand, int price, int customer_id)
{
    string product_name = product + "_" + product_brand;

    try {
        sql::PreparedStatement* db_stmt = conn->prepareStatement(
            "INSERT INTO test_db.order (product_name, price, customer_id) VALUES (?, ?, ?)");

        // 데이터 바인딩
        db_stmt->setString(1, product_name);
        db_stmt->setInt(2, price);
        db_stmt->setInt(3, customer_id);

        // 쿼리 실행
        db_stmt->executeUpdate();


        delete db_stmt;

        //return 1;
        return "Success INSERT PRODUCT";
    }
    catch (sql::SQLException& e) {
        //return -1;
        return string(e.what());
    }
}

// 기존 가입 고객 확인용 함수
int db_checkCustomerExists(sql::Connection* conn, const string& customer_name, const string& phone_number)
{
    sql::PreparedStatement* pstmt = conn->prepareStatement(
        "SELECT COUNT(*) AS count FROM customer WHERE customer_name = ? AND phone = ?");
    pstmt->setString(1, customer_name);
    pstmt->setString(2, phone_number);

    sql::ResultSet* res = pstmt->executeQuery();
    int exists = 0;

    if (res && res->next()) {
        int count = res->getInt("count");
        exists = (count > 0);
    }

    delete res;
    delete pstmt;

    return exists;
}


// 기존 주문번호 확인용 함수
int db_checkOrderExists(sql::Connection* conn, const int order_num)
{
    sql::PreparedStatement* pstmt = conn->prepareStatement(
        "SELECT COUNT(*) AS count FROM test_db.order WHERE order_id = ?");
    pstmt->setInt(1, order_num);


    sql::ResultSet* res = pstmt->executeQuery();
    int exists = 0;

    if (res && res->next()) {
        int count = res->getInt("count");
        exists = (count > 0);
    }

    delete res;
    delete pstmt;

    return exists;
}



// 주문 고유 번호 검색 함수

int db_getOrderId(sql::Connection* conn, const string& product, const string& brand, const int customer_id)
{
    sql::PreparedStatement* pstmt = nullptr;
    sql::ResultSet* res = nullptr;
    int order_id = -1;  // Default to -1 if no order found

    string product_name = product + "_" + brand;

    try {
        // Use a prepared statement to prevent SQL injection
        string query = "SELECT order_id FROM test_db.order WHERE product_name = ? AND customer_id = ?";
        pstmt = conn->prepareStatement(query);

        // Bind the parameters
        pstmt->setString(1, product_name);  // First parameter for product_name
        pstmt->setInt(2, customer_id);      // Second parameter for customer_id

        // Execute the query
        res = pstmt->executeQuery();

        // Check if the result exists
        if (res && res->next()) {
            order_id = res->getInt("order_id");
        }
    }
    catch (sql::SQLException& e) {
        cout << "SQL Error: " << e.what() << endl;
        order_id = -1;  // Return -1 in case of an error
    }

    // Clean up
    delete res;
    delete pstmt;

    return order_id;
}




// 고객 고유 번호 검색 함수

int db_getCustomerId(sql::Connection* conn, const string& customer_name, const string& addr, const string& phone_number)
{

    sql::Statement* stmt = conn->createStatement();
    sql::ResultSet* res = nullptr;
    int customer_id = -1;

    try {

        string query = "SELECT customer_id FROM customer WHERE customer_name = '" + customer_name + "' "
            "AND address = '" + addr + "' AND phone = '" + phone_number + "'";

        // Execute the query
        res = stmt->executeQuery(query);

        // Check if the result exists
        if (res && res->next()) {
            customer_id = res->getInt("customer_id");
        }
    }
    catch (sql::SQLException& e) {
        cout << "SQL Error: " << e.what() << endl;
        customer_id = -1;  // Return -1 in case of an error
    }

    // Clean up
    delete res;
    delete stmt;

    return customer_id;
}



void db_getOrderDetails(sql::Connection* conn, int order_num) {
    sql::PreparedStatement* pstmt = nullptr;
    sql::ResultSet* res = nullptr;

    try {
        string query = "SELECT o.order_id, o.product_name, c.customer_id, c.customer_name, c.address, c.phone "
            "FROM test_db.order o "
            "JOIN test_db.customer c ON o.customer_id = c.customer_id "
            "WHERE o.order_id = ?";

        pstmt = conn->prepareStatement(query);
        pstmt->setInt(1, order_num);

        res = pstmt->executeQuery();

        while (res && res->next()) 
        {
           orderData.order_id = res->getInt("order_id");
           orderData.product_name = res->getString("product_name");
           orderData.customer_id = res->getInt("customer_id");
           orderData.customer_name = res->getString("customer_name");
           orderData.addr = res->getString("address");
           orderData.ph = res->getString("phone");
   
        }
    }
    catch (sql::SQLException& e) {
        cout << "SQL Error: " << e.what() << endl;
    }

    delete res;
    delete pstmt;
}

//반품된 상품 DB에 반환하는 코드
void db_updateProdOk(sql::Connection* conn, int order_id) {
    try {
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "UPDATE `order` SET prod_ok = 1 WHERE order_id = ?"
        );
        pstmt->setInt(1, order_id);
        pstmt->executeUpdate();
        delete pstmt;
        cout << "주문 번호 " << order_id << "의 prod_ok 값이 1로 업데이트되었습니다." << endl;
    }
    catch (sql::SQLException& e) {
        cout << "SQL Error: " << e.what() << endl;
    }
}


void printOrderData(sql::Connection* conn, int order_num)
{
    db_getOrderDetails(conn, order_num);
    cout << "Order Num: " << orderData.order_id << endl;
    cout << "Product Name: " << orderData.product_name << endl;
    cout << "Customer ID: " << orderData.customer_id << endl;
    cout << "Customer Name: " << orderData.customer_name << endl;
    cout << "Address: " << orderData.addr << endl;
    cout << "Phone: " << orderData.ph << endl;
}