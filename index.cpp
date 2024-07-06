#include <mysql.h>
#include <iostream>
#include <string>

using namespace std;

class Database {
private:
    MYSQL* conn;

public:
    Database() {
        conn = mysql_init(NULL);
        if (conn == NULL) {
            cerr << "mysql_init() failed\n";
            exit(EXIT_FAILURE);
        }

        if (mysql_real_connect(conn, "localhost", "root", "Syj123450520!", "supermarket", 0, NULL, 0) == NULL) {
            cerr << "mysql_real_connect() failed\n";
            mysql_close(conn);
            exit(EXIT_FAILURE);
        }
    }

    ~Database() {
        mysql_close(conn);
    }

    MYSQL* getConnection() {
        return conn;
    }
};
void setConsoleEncoding() {
    SetConsoleOutputCP(CP_UTF8);  // 设置控制台输出编码为 UTF-8
    SetConsoleCP(CP_UTF8);        // 设置控制台输入编码为 UTF-8
    // 这行代码是为了确保 C++ 的 cout 可以正确处理 UTF-8
    std::ios::sync_with_stdio(false);
    // 取消 cin 的同步以处理 UTF-8 输入
    std::cin.tie(nullptr);
}

class User {
private:
    Database* db;

public:
    User(Database* database) : db(database) {}

    bool isUsernameExist(const string& username) {
        string query = "SELECT * FROM users WHERE username = '" + username + "'";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "检查用户名失败: " << mysql_error(db->getConnection()) << endl;
            return false;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() 失败: " << mysql_error(db->getConnection()) << endl;
            return false;
        }

        bool userExist = mysql_num_rows(res) > 0;
        mysql_free_result(res);

        return userExist;
    }

    bool registerUser(const string& username, const string& password) {
        if (isUsernameExist(username)) {
            cout << "====================" << endl;
            cout << "用户名已存在，请选择其他用户名。" << endl;
            cout << "====================" << endl;
            return false;
        }

        string query = "INSERT INTO users(username, password) VALUES('" + username + "', '" + password + "')";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "注册用户失败: " << mysql_error(db->getConnection()) << endl;
            return false;
        }
        cout << "====================" << endl;
        cout << "用户注册成功！" << endl;
        cout << "====================" << endl;
        return true;
    }

    bool loginUser(const string& username, const string& password) {
        string query = "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "'";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "登录用户失败: " << mysql_error(db->getConnection()) << endl;
            return false;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() 失败: " << mysql_error(db->getConnection()) << endl;
            return false;
        }

        bool loginSuccess = mysql_num_rows(res) > 0;
        mysql_free_result(res);

        if (loginSuccess) {
            cout << "====================" << endl;
            cout << "登录成功！" << endl;
            cout << "====================" << endl;
            return true;
        }
        else {
            cout << "====================" << endl;
            cout << "用户名或密码错误。" << endl;
            cout << "====================" << endl;
            return false;
        }
    }
};

class Product {
private:
    Database* db;

public:
    Product(Database* database) : db(database) {}

    void addProduct(int id, const string& name, double price, int quantity) {
        string query = "INSERT INTO products(id, name, price, quantity) VALUES(" + to_string(id) + ", '" + name + "', " + to_string(price) + ", " + to_string(quantity) + ")";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "添加商品失败: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "商品添加成功！" << endl;
            cout << "====================" << endl;
        }
    }

    void updateProduct(int id, const string& name, double price, int quantity) {
        string query = "UPDATE products SET name = '" + name + "', price = " + to_string(price) + ", quantity = " + to_string(quantity) + " WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "更新商品失败: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "商品更新成功！" << endl;
            cout << "====================" << endl;
        }
    }

    void deleteProduct(int id) {
        string query = "DELETE FROM products WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "删除商品失败: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "商品删除成功！" << endl;
            cout << "====================" << endl;
        }
    }

    void queryProduct(int id) {
        string query = "SELECT * FROM products WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "查询商品失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() 失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        if ((row = mysql_fetch_row(res))) {
            cout << "====================" << endl;
            cout << "商品ID: " << row[0] << endl;
            cout << "商品名称: " << row[1] << endl;
            cout << "商品价格: " << row[2] << endl;
            cout << "商品数量: " << row[3] << endl;
            cout << "====================" << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "未找到商品。" << endl;
            cout << "====================" << endl;
        }
        mysql_free_result(res);
    }

    void queryAllProducts() {
        string query = "SELECT * FROM products";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "查询商品失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() 失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        cout << "====================" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "商品ID: " << row[0] << endl;
            cout << "商品名称: " << row[1] << endl;
            cout << "商品价格: " << row[2] << endl;
            cout << "商品数量: " << row[3] << endl;
            cout << "--------------------" << endl;
        }
        cout << "====================" << endl;
        mysql_free_result(res);
    }

    void checkLowStock(int threshold) {
        string query = "SELECT * FROM products WHERE quantity < " + to_string(threshold);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "查询库存失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() 失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        cout << "====================" << endl;
        cout << "低库存商品：" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "商品ID: " << row[0] << endl;
            cout << "商品名称: " << row[1] << endl;
            cout << "商品价格: " << row[2] << endl;
            cout << "商品数量: " << row[3] << endl;
            cout << "--------------------" << endl;
        }
        cout << "====================" << endl;
        mysql_free_result(res);
    }
};


class Supplier {
private:
    Database* db;

public:
    Supplier(Database* database) : db(database) {}

    void addSupplier(int id, const string& name, const string& contact) {
        string query = "INSERT INTO suppliers(id, name, contact) VALUES(" + to_string(id) + ", '" + name + "', '" + contact + "')";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "添加供货商失败: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "供货商添加成功！" << endl;
            cout << "====================" << endl;
        }
    }

    void updateSupplier(int id, const string& name, const string& contact) {
        string query = "UPDATE suppliers SET name = '" + name + "', contact = '" + contact + "' WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "更新供货商失败: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "供货商更新成功！" << endl;
            cout << "====================" << endl;
        }
    }

    void deleteSupplier(int id) {
        string query = "DELETE FROM suppliers WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "删除供货商失败: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "供货商删除成功！" << endl;
            cout << "====================" << endl;
        }
    }

    void querySupplier(int id) {
        string query = "SELECT * FROM suppliers WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "查询供货商失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() 失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        if ((row = mysql_fetch_row(res))) {
            cout << "====================" << endl;
            cout << "供货商ID: " << row[0] << endl;
            cout << "供货商名称: " << row[1] << endl;
            cout << "供货商联系方式: " << row[2] << endl;
            cout << "====================" << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "未找到供货商。" << endl;
            cout << "====================" << endl;
        }
        mysql_free_result(res);
    }

    void generateSupplierReport() {
        string query = "SELECT * FROM suppliers";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "查询供货商失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() 失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        cout << "====================" << endl;
        cout << "供货商报告：" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "供货商ID: " << row[0] << endl;
            cout << "供货商名称: " << row[1] << endl;
            cout << "供货商联系方式: " << row[2] << endl;
            cout << "--------------------" << endl;
        }
        cout << "====================" << endl;
        mysql_free_result(res);
    }
};
class PurchaseOrder {
private:
    Database* db;

public:
    PurchaseOrder(Database* database) : db(database) {}

    void addOrder(int id, const string& date, int supplierId, int productId, int quantity) {
        string query = "INSERT INTO purchase_orders(id, date, supplier_id, product_id, quantity) VALUES(" + to_string(id) + ", '" + date + "', " + to_string(supplierId) + ", " + to_string(productId) + ", " + to_string(quantity) + ")";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "添加采购订单失败: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "采购订单添加成功！" << endl;
            cout << "====================" << endl;
        }
    }

    void updateOrder(int id, const string& date, int supplierId, int productId, int quantity) {
        string query = "UPDATE purchase_orders SET date = '" + date + "', supplier_id = " + to_string(supplierId) + ", product_id = " + to_string(productId) + ", quantity = " + to_string(quantity) + " WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "更新采购订单失败: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "采购订单更新成功！" << endl;
            cout << "====================" << endl;
        }
    }

    void deleteOrder(int id) {
        string query = "DELETE FROM purchase_orders WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "删除采购订单失败: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "采购订单删除成功！" << endl;
            cout << "====================" << endl;
        }
    }

    void queryOrder(int id) {
        string query = "SELECT * FROM purchase_orders WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "查询采购订单失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() 失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        if ((row = mysql_fetch_row(res))) {
            cout << "====================" << endl;
            cout << "订单ID: " << row[0] << endl;
            cout << "日期: " << row[1] << endl;
            cout << "供货商ID: " << row[2] << endl;
            cout << "商品ID: " << row[3] << endl;
            cout << "数量: " << row[4] << endl;
            cout << "====================" << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "未找到订单。" << endl;
            cout << "====================" << endl;
        }
        mysql_free_result(res);
    }

    void generatePurchaseHistory() {
        string query = "SELECT * FROM purchase_orders";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "查询采购订单失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() 失败: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        cout << "====================" << endl;
        cout << "采购订单历史：" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "订单ID: " << row[0] << endl;
            cout << "日期: " << row[1] << endl;
            cout << "供货商ID: " << row[2] << endl;
            cout << "商品ID: " << row[3] << endl;
            cout << "数量: " << row[4] << endl;
            cout << "--------------------" << endl;
        }
        cout << "====================" << endl;
        mysql_free_result(res);
    }
};

class SupermarketSystem {
private:
    Database db;
    User user;
    Product product;
    Supplier supplier;
    bool loggedIn;

public:
    SupermarketSystem() : user(&db), product(&db), supplier(&db), loggedIn(false) {}

    void run() {
        string username, password;
        int choice, id;
        string name, contact;
        double price;
        int quantity;
        int threshold;

        while (true) {
            cout << "====================" << endl;
            cout << "1. 注册\n2. 登录\n3. 添加商品\n4. 查询商品\n5. 删除商品\n6. 查询所有商品\n7. 设置库存警报\n8. 添加供货商\n9. 更新供货商\n10. 删除供货商\n11. 查询供货商\n12. 生成供货商报告\n13. 退出登录\n14. 退出系统\n请选择：";
            cout << "====================" << endl;
            cout << "你的选择: ";
            cin >> choice;

            switch (choice) {
            case 1:
                cout << "输入用户名：";
                cin >> username;
                cout << "输入密码：";
                cin >> password;
                user.registerUser(username, password);
                break;
            case 2:
                cout << "输入用户名：";
                cin >> username;
                cout << "输入密码：";
                cin >> password;
                loggedIn = user.loginUser(username, password);
                break;
            case 3:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "输入商品ID：";
                cin >> id;
                cout << "输入商品名称：";
                cin >> name;
                cout << "输入商品价格：";
                cin >> price;
                cout << "输入商品数量：";
                cin >> quantity;
                product.addProduct(id, name, price, quantity);
                break;
            case 4:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "输入商品ID：";
                cin >> id;
                product.queryProduct(id);
                break;
            case 5:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "输入商品ID：";
                cin >> id;
                product.deleteProduct(id);
                break;
            case 6:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                product.queryAllProducts();
                break;
            case 7:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "设置库存警报阈值：";
                cin >> threshold;
                product.checkLowStock(threshold);
                break;
            case 8:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "输入供货商ID：";
                cin >> id;
                cout << "输入供货商名称：";
                cin >> name;
                cout << "输入供货商联系方式：";
                cin >> contact;
                supplier.addSupplier(id, name, contact);
                break;
            case 9:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "输入供货商ID：";
                cin >> id;
                cout << "输入供货商名称：";
                cin >> name;
                cout << "输入供货商联系方式：";
                cin >> contact;
                supplier.updateSupplier(id, name, contact);
                break;
            case 10:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "输入供货商ID：";
                cin >> id;
                supplier.deleteSupplier(id);
                break;
            case 11:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "输入供货商ID：";
                cin >> id;
                supplier.querySupplier(id);
                break;
            case 12:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "请先登录。" << endl;
                    cout << "====================" << endl;
                    break;
                }
                supplier.generateSupplierReport();
                break;
            case 13:
                loggedIn = false;
                cout << "====================" << endl;
                cout << "已退出登录。" << endl;
                cout << "====================" << endl;
                break;
            case 14:
                cout << "====================" << endl;
                cout << "系统退出。" << endl;
                cout << "====================" << endl;
                return;
            default:
                cout << "====================" << endl;
                cout << "无效的选择。" << endl;
                cout << "====================" << endl;
                break;
            }
        }
    }
};

int main() {
    //setConsoleEncoding();
    SupermarketSystem system;
    system.run();
    return 0;
}
