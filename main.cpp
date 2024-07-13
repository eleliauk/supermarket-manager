#define _CRT_SECURE_NO_WARNINGS
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

char* hidden_input();
typedef struct {
    MYSQL* conn;
} Database;
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}
char* hidden_input() {
    char* p = (char*)malloc(20 * sizeof(char)); // 分配内存
    if (p == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    do {
        p[i] = getch();
        if (13 == (int)p[i]) { // 如果是回车
            p[i] = '\0';
            break;
        }
        if ((int)p[i] == 8) { // 如果是退格
            if (0 == i) {
                printf("\a");
                continue;
            }
            i = i - 1;
            printf("\b \b");
        }
        else {
            i = i + 1;
            if (20 != i) {
                printf("*");
            }
            else {
                i = i - 1;
            }
        }
    } while ('\n' != p[i] && i < 20);
    printf("\n");

    if ('\0' == p[0]) {
        printf("密码为空，请重新输入！\n");
    }
    return p;
}

void Database_init(Database* db) {
    db->conn = mysql_init(NULL);
    if (db->conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        exit(EXIT_FAILURE);
    }

    if (mysql_real_connect(db->conn, "localhost", "root", "Syj123450520!", "supermarket", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(db->conn);
        exit(EXIT_FAILURE);
    }
    if (mysql_query(db->conn, "set names gbk")) {
        printf("set names gbk失败,原因：%s", mysql_error(db->conn));
        exit(-1);
    }
}

void Database_close(Database* db) {
    mysql_close(db->conn);
}

typedef struct {
    Database* db;
} User;

void User_init(User* user, Database* db) {
    user->db = db;
}

int isUsernameExist(User* user, const char* username) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM users WHERE username = '%s'", username);
    if (mysql_query(user->db->conn, query)) {
        fprintf(stderr, "=========================================检查用户名失败:========================================= %s\n", mysql_error(user->db->conn));
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(user->db->conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() 失败: %s\n", mysql_error(user->db->conn));
        return 0;
    }

    int userExist = mysql_num_rows(res) > 0;
    mysql_free_result(res);
    return userExist;
}

int registerUser(User* user, const char* username, const char* password) {
    if (isUsernameExist(user, username)) {
        printf("=========================================================================\n");
        printf("=========================================用户名已存在，请选择其他用户名。===================================\n");
        printf("=============================================================================================================\n");
        return 0;
    }

    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO users(username, password) VALUES('%s', '%s')", username, password);
    if (mysql_query(user->db->conn, query)) {
        fprintf(stderr, "=========================================注册用户失败: =========================================%s\n", mysql_error(user->db->conn));
        return 0;
    }
    printf("================================================================================================\n");
    printf("=========================================用户注册成功！=========================================\n");
    printf("================================================================================================\n");
    return 1;
}

int loginUser(User* user, const char* username, const char* password) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM users WHERE username = '%s' AND password = '%s'", username, password);
    if (mysql_query(user->db->conn, query)) {
        fprintf(stderr, "=========================================登录用户失败:========================================= %s\n", mysql_error(user->db->conn));
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(user->db->conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() 失败: %s\n", mysql_error(user->db->conn));
        return 0;
    }

    int loginSuccess = mysql_num_rows(res) > 0;
    mysql_free_result(res);

    if (loginSuccess) {
        printf("====================================================================================================\n");
        printf("=========================================登录成功！=================================================\n");
        printf("====================================================================================================\n");
        return 1;
    }
    else {
        printf("====================================================================================================\n");
        printf("=========================================用户名或密码错误。=========================================\n");
        printf("====================================================================================================\n");
        return 0;
    }
}

typedef struct {
    Database* db;
} Product;

void Product_init(Product* product, Database* db) {
    product->db = db;
}

void addProduct(Product* product, int id, const char* name, double price, int quantity) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO products(id, name, price, quantity) VALUES(%d, '%s', %.2f, %d)", id, name, price, quantity);
    if (mysql_query(product->db->conn, query)) {
        fprintf(stderr, "添加商品失败: %s\n", mysql_error(product->db->conn));
    }
    else {
        printf("================================================================================================\n");
        printf("=========================================商品添加成功！=========================================\n");
        printf("================================================================================================\n");
    }
}

void queryAllProducts(Product* product) {
    char query[256] = "SELECT * FROM products";
    if (mysql_query(product->db->conn, query)) {
        fprintf(stderr, "=========================================查询商品失败: =========================================%s\n", mysql_error(product->db->conn));
        return;
    }

    MYSQL_RES* res = mysql_store_result(product->db->conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() 失败: %s\n", mysql_error(product->db->conn));
        return;
    }

    MYSQL_ROW row;
    printf("=================================================================================================================\n");
    while ((row = mysql_fetch_row(res))) {
        printf("商品ID: %s\n", row[0]);
        printf("商品名称: %s\n", row[1]);
        printf("商品价格: %s\n", row[2]);
        printf("商品数量: %s\n", row[3]);
        printf("--------------------\n");
    }
    printf("==================================================================================================================\n");
    mysql_free_result(res);
}

void deleteProduct(Product* product, int id) {
    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM products WHERE id = %d", id);
    if (mysql_query(product->db->conn, query)) {
        fprintf(stderr, "=========================================删除商品失败:========================================== %s\n", mysql_error(product->db->conn));
    }
    else {
        printf("==================================================================================================\n");
        printf("=========================================商品删除成功！===========================================\n");
        printf("==================================================================================================\n");
    }
}

void queryProduct(Product* product, int id) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM products WHERE id = %d", id);

    if (mysql_query(product->db->conn, query)) {
        fprintf(stderr, "=========================================查询商品失败:========================================= %s\n", mysql_error(product->db->conn));
        return;
    }

    MYSQL_RES* res = mysql_store_result(product->db->conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() 失败: %s\n", mysql_error(product->db->conn));
        return;
    }

    MYSQL_ROW row;
    if ((row = mysql_fetch_row(res))) {
        printf("===================================================================================================================\n");
        printf("商品ID: %s\n", row[0]);
        printf("商品名称: %s\n", row[1]);
        printf("商品价格: %s\n", row[2]);
        printf("商品数量: %s\n", row[3]);
        printf("====================================================================================================================\n");
    }
    else {
        printf("====================================================================================================================\n");
        printf("======================================================未找到商品。==================================================\n");
        printf("====================================================================================================================\n");
    }
    mysql_free_result(res);
}

typedef struct {
    Database* db;
} Supplier;

void Supplier_init(Supplier* supplier, Database* db) {
    supplier->db = db;
}

void addSupplier(Supplier* supplier, int id, const char* name, const char* contact) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO suppliers(id, name, contact) VALUES(%d, '%s', '%s')", id, name, contact);
    if (mysql_query(supplier->db->conn, query)) {
        fprintf(stderr, "=========================================添加供应商失败:========================================= %s\n", mysql_error(supplier->db->conn));
    }
    else {
        printf("==================================================================================================\n");
        printf("=========================================供应商添加成功！=========================================\n");
        printf("==================================================================================================\n");
    }
}

void queryAllSuppliers(Supplier* supplier) {
    char query[256] = "SELECT * FROM suppliers";
    if (mysql_query(supplier->db->conn, query)) {
        fprintf(stderr, "=========================================查询供应商失败: =========================================%s\n", mysql_error(supplier->db->conn));
        return;
    }

    MYSQL_RES* res = mysql_store_result(supplier->db->conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() 失败: %s\n", mysql_error(supplier->db->conn));
        return;
    }

    MYSQL_ROW row;
    printf("==================================================================================================\n");
    while ((row = mysql_fetch_row(res))) {
        printf("供应商ID: %s\n", row[0]);
        printf("供应商名称: %s\n", row[1]);
        printf("供应商联系方式: %s\n", row[2]);
        printf("--------------------\n");
    }
    printf("==================================================================================================\n");
    mysql_free_result(res);
}

void deleteSupplier(Supplier* supplier, int id) {
    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM suppliers WHERE id = %d", id);
    if (mysql_query(supplier->db->conn, query)) {
        fprintf(stderr, "删除供应商失败: %s\n", mysql_error(supplier->db->conn));
    }
    else {
        printf("==================================================================================================\n");
        printf("=========================================供应商删除成功！=========================================\n");
        printf("==================================================================================================\n");
    }
}

void editProduct(Database* db, int id, const char* name, double price, int quantity) {
    char query[256];
    snprintf(query, sizeof(query), "UPDATE products SET name='%s', price=%f, quantity=%d WHERE id=%d", name, price, quantity, id);
    if (mysql_query(db->conn, query)) {
        fprintf(stderr, "=====================================更新商品失败!==============================================%s\n", mysql_error(db->conn));
    }
    else {
        printf("======================================商品更新成功！===========================================\n");
    }
}

void setStockThreshold(Database* db, int product_id, int threshold) {
    char query[256];
    snprintf(query, sizeof(query), "UPDATE products SET stock_threshold = %d WHERE id = %d", threshold, product_id);
    if (mysql_query(db->conn, query)) {
        fprintf(stderr, "======================================设置库存阈值失败: =====================================%s\n", mysql_error(db->conn));
    }
    else {
        printf("=========================================库存阈值设置成功！=======================================\n");
    }
}

void checkStock(Database* db) {
    MYSQL_RES* res;
    MYSQL_ROW row;
    int quantity;

    // 提示用户输入数量阈值
    printf("请输入要查询的库存阈值：");
    scanf("%d", &quantity);

    char query[256];
    snprintf(query, sizeof(query), "SELECT id, name, quantity FROM products WHERE quantity < %d", quantity);

    if (mysql_query(db->conn, query)) {
        fprintf(stderr, "查询库存失败: %s\n", mysql_error(db->conn));
        return;
    }

    res = mysql_store_result(db->conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(db->conn));
        return;
    }

    // 遍历结果集并输出警告
    while ((row = mysql_fetch_row(res))) {
        printf("警告: 商品ID: %s, 名称: %s, 当前库存: %s\n", row[0], row[1], row[2]);
    }

    mysql_free_result(res);
}

void addSupplierProduct(Database* db, int supplier_id, int product_id) {
    char query[256];
    snprintf(query, sizeof(query), "INSERT INTO supplier_products (supplier_id, product_id) VALUES (%d, %d)", supplier_id, product_id);
    if (mysql_query(db->conn, query)) {
        fprintf(stderr, "=====================================添加供应商商品失败:=======================================%s\n", mysql_error(db->conn));
    }
    else {
        printf("==================================供应商商品添加成功！=====================================\n");
    }
}

void querySupplierProducts(Database* db, int supplier_id) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT products.id, products.name, products.price, products.quantity FROM products "
        "JOIN supplier_products ON products.id = supplier_products.product_id "
        "WHERE supplier_products.supplier_id = %d", supplier_id);
    if (mysql_query(db->conn, query)) {
        fprintf(stderr, "=======================================查询供应商商品失败:============================================ %s\n", mysql_error(db->conn));
        return;
    }

    MYSQL_RES* res = mysql_store_result(db->conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(db->conn));
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        printf("商品ID: %s, 名称: %s, 价格: %s, 数量: %s\n", row[0], row[1], row[2], row[3]);
    }

    mysql_free_result(res);
}

void querySupplier(Supplier* supplier, int id) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM suppliers WHERE id = %d", id);

    if (mysql_query(supplier->db->conn, query)) {
        fprintf(stderr, "=========================================查询供应商失败:=========================================== %s\n", mysql_error(supplier->db->conn));
        return;
    }

    MYSQL_RES* res = mysql_store_result(supplier->db->conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() 失败: %s\n", mysql_error(supplier->db->conn));
        return;
    }

    MYSQL_ROW row;
    if ((row = mysql_fetch_row(res))) {
        printf("======================================================================================================\n");
        printf("              供应商ID:%s\n          ", row[0]);
        printf("              供应商名称: %s\n       ", row[1]);
        printf("            供应商联系方式: %s\n      ", row[2]);
        printf("======================================================================================================\n");
    }
    else {
        printf("======================================================================================================\n");
        printf("========================================未找到供应商。================================================\n");
        printf("======================================================================================================\n");
    }
    mysql_free_result(res);
}

int main() {
    int choice1, choice2;
    Database db;
    User user;
    Product product;
    Supplier supplier;

    Database_init(&db);
    User_init(&user, &db);
    Product_init(&product, &db);
    Supplier_init(&supplier, &db);

    do {
        printf("=========================================\n");
        printf("1. 用户管理\n");
        printf("2. 商品管理\n");
        printf("3. 供应商管理\n");
        printf("0. 退出\n");
        printf("请输入选择：");
        scanf("%d", &choice1);

        switch (choice1) {
            case 1:
                do {
                    printf("=========================================\n");
                    printf("1. 登录\n");
                    printf("2.注册\n");
                    printf("0. 返回\n");
                    printf("请输入选择：");
                    scanf("%d", &choice2);

                    switch (choice2) {
                        case 1:
                            // 添加登录功能
                            char username[50], password[50];
                            printf("请输入用户名：");
                            scanf("%s", username);
                            printf("请输入密码：");
                            scanf("%s", password);
                            if (loginUser(&user, username, password)) {
                                printf("登录成功！\n");
                            } else {
                                printf("登录失败，请检查用户名和密码。\n");
                            }
                            break;
                        case 2:
                            printf("请输入用户名：");
                            scanf("%s", username);
                            printf("请输入密码：");
                            char* password1; // 初始化 password1
                            password1 = hidden_input();
                            registerUser(&user, username, password1);
                            break;
                        case 0:
                            break;
                        default:
                            printf("无效选择，请重新输入。\n");
                            break;
                    }
                } while (choice2 != 0);
                break;
            case 2:
                do {
                    printf("=========================================\n");
                    printf("1. 添加商品\n");
                    printf("2. 查询所有商品\n");
                    printf("3. 删除商品\n");
                    printf("4. 查询商品\n");
                    printf("5. 更新商品\n");
                    printf("6.设置商品数量阀值\n");
                    printf("7.查询库存\n");
                    printf("0. 返回\n");
                    printf("请输入选择：\n");
                    scanf("%d", &choice2);

                    switch (choice2) {
                        case 1:
                            // 添加商品功能
                            int id, quantity;
                            double price;
                            char name[50];
                            printf("请输入商品ID：");
                            scanf("%d", &id);
                            printf("请输入商品名称：");
                            scanf("%s", name);
                            printf("请输入商品价格：");
                            scanf("%lf", &price);
                            printf("请输入商品数量：");
                            scanf("%d", &quantity);
                            addProduct(&product, id, name, price, quantity);
                            break;
                        case 2:
                            queryAllProducts(&product);
                            break;
                             case 3:
                            // 删除商品功能
                            printf("请输入要删除的商品ID：");
                            scanf("%d", &id);
                            deleteProduct(&product, id);
                            break;
                        case 4:
                            // 查询商品功能
                            printf("请输入要查询的商品ID：");
                            scanf("%d", &id);
                            queryProduct(&product, id);
                            break;
                        case 5:
                        {
                            int id, quantity;
                            double price;
                            char name[50];
                            printf("请输入商品ID：");
                            scanf("%d", &id);
                            printf("请输入更改后商品名称：");
                            scanf("%s", name);
                            printf("请输入更改后商品价格：");
                            scanf("%lf", &price);
                            printf("请输入更改后商品数量：");
                            scanf("%d", &quantity);
                            editProduct(product.db, id, name, price, quantity);
                        }
                        break;
                         case 6:
                        {
                            int id,quantity;
                            printf("请输入商品ID：");
                            scanf("%d", &id);
                            printf("请输入商品数量阀值：");
                            scanf("%d", &quantity);
                            setStockThreshold(product.db, id, quantity);
                        }
                        break;
                        case 7:
                        {
                         checkStock(product.db);
                        }
                        break;
                        case 0:
                            break;
                        default:
                            printf("无效选择，请重新输入。\n");
                            break;
                    }
                } while (choice2 != 0);
                break;

            case 3:
                do {
                    printf("=========================================\n");
                    printf("1. 添加供应商\n");
                    printf("2. 查询所有供应商\n");
                    printf("3. 删除供应商\n");
                    printf("4. 查询供应商\n");
                    printf("5.添加供应商商品\n");
                    printf("6.查询供应商商品\n");
                    printf("0. 返回\n");
                     printf("请输入选择：");
                    scanf("%d", &choice2);

                    switch (choice2) {
                        case 1:
                            // 添加供应商功能
                            int supplier_id;
                            char supplier_name[50], contact[50];
                            printf("请输入供应商ID：");
                            scanf("%d", &supplier_id);
                            printf("请输入供应商名称：");
                            scanf("%s", supplier_name);
                            printf("请输入供应商联系方式：");
                            scanf("%s", contact);
                            addSupplier(&supplier, supplier_id, supplier_name, contact);
                            break;
                        case 2:
                            queryAllSuppliers(&supplier);
                            break;
                        case 3:
                            // 删除供应商功能
                            printf("请输入要删除的供应商ID：");
                            scanf("%d", &supplier_id);
                            deleteSupplier(&supplier, supplier_id);
                            break;
                        case 4:
                            // 查询供应商功能
                            printf("请输入要查询的供应商ID：");
                            scanf("%d", &supplier_id);
                            querySupplier(&supplier, supplier_id);
                            break;
                        case 5:
                        {
                            int id1, id2;
                            printf("请输入供应商ID：");
                            scanf("%d", &id1);
                            printf("请输入添加商品ID：");
                            scanf("%d", &id2);
                            addSupplierProduct(supplier.db, id1, id2);
                        }
                        break;
                        case 6:
                        {
                            int id;
                            printf("请输入供应商ID：");
                            scanf("%d", &id);
                            querySupplierProducts(supplier.db, id);
                        }
                        case 0:
                        break;
                        default:
                            printf("无效选择，请重新输入。\n");
                            break;
                    }
                } while (choice2 != 0);
                break;

            case 0:
                printf("退出程序。\n");
                break;

            default:
                printf("无效选择，请重新输入。\n");
                break;
        }
    } while (choice1 != 0);

    Database_close(&db);

    return 0;
    }
