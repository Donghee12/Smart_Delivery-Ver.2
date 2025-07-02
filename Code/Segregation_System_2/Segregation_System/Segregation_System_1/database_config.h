// database_config.h
#ifndef DATABASE_CONFIG_H
#define DATABASE_CONFIG_H

#include <string>

struct DatabaseConfig {
    std::string db_host;
    std::string db_user;
    std::string db_password;
    std::string db_schema;
};

// DatabaseConfig 객체를 extern으로 선언 (다른 파일에서 사용할 수 있도록)
extern DatabaseConfig db_config;

#endif // DATABASE_CONFIG_H
