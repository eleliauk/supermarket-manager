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
    SetConsoleOutputCP(CP_UTF8);  // ���ÿ���̨�������Ϊ UTF-8
    SetConsoleCP(CP_UTF8);        // ���ÿ���̨�������Ϊ UTF-8
    // ���д�����Ϊ��ȷ�� C++ �� cout ������ȷ���� UTF-8
    std::ios::sync_with_stdio(false);
    // ȡ�� cin ��ͬ���Դ��� UTF-8 ����
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
            cerr << "����û���ʧ��: " << mysql_error(db->getConnection()) << endl;
            return false;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() ʧ��: " << mysql_error(db->getConnection()) << endl;
            return false;
        }

        bool userExist = mysql_num_rows(res) > 0;
        mysql_free_result(res);

        return userExist;
    }

    bool registerUser(const string& username, const string& password) {
        if (isUsernameExist(username)) {
            cout << "====================" << endl;
            cout << "�û����Ѵ��ڣ���ѡ�������û�����" << endl;
            cout << "====================" << endl;
            return false;
        }

        string query = "INSERT INTO users(username, password) VALUES('" + username + "', '" + password + "')";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "ע���û�ʧ��: " << mysql_error(db->getConnection()) << endl;
            return false;
        }
        cout << "====================" << endl;
        cout << "�û�ע��ɹ���" << endl;
        cout << "====================" << endl;
        return true;
    }

    bool loginUser(const string& username, const string& password) {
        string query = "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "'";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "��¼�û�ʧ��: " << mysql_error(db->getConnection()) << endl;
            return false;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() ʧ��: " << mysql_error(db->getConnection()) << endl;
            return false;
        }

        bool loginSuccess = mysql_num_rows(res) > 0;
        mysql_free_result(res);

        if (loginSuccess) {
            cout << "====================" << endl;
            cout << "��¼�ɹ���" << endl;
            cout << "====================" << endl;
            return true;
        }
        else {
            cout << "====================" << endl;
            cout << "�û������������" << endl;
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
            cerr << "�����Ʒʧ��: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "��Ʒ��ӳɹ���" << endl;
            cout << "====================" << endl;
        }
    }

    void updateProduct(int id, const string& name, double price, int quantity) {
        string query = "UPDATE products SET name = '" + name + "', price = " + to_string(price) + ", quantity = " + to_string(quantity) + " WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "������Ʒʧ��: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "��Ʒ���³ɹ���" << endl;
            cout << "====================" << endl;
        }
    }

    void deleteProduct(int id) {
        string query = "DELETE FROM products WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "ɾ����Ʒʧ��: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "��Ʒɾ���ɹ���" << endl;
            cout << "====================" << endl;
        }
    }

    void queryProduct(int id) {
        string query = "SELECT * FROM products WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "��ѯ��Ʒʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        if ((row = mysql_fetch_row(res))) {
            cout << "====================" << endl;
            cout << "��ƷID: " << row[0] << endl;
            cout << "��Ʒ����: " << row[1] << endl;
            cout << "��Ʒ�۸�: " << row[2] << endl;
            cout << "��Ʒ����: " << row[3] << endl;
            cout << "====================" << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "δ�ҵ���Ʒ��" << endl;
            cout << "====================" << endl;
        }
        mysql_free_result(res);
    }

    void queryAllProducts() {
        string query = "SELECT * FROM products";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "��ѯ��Ʒʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        cout << "====================" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "��ƷID: " << row[0] << endl;
            cout << "��Ʒ����: " << row[1] << endl;
            cout << "��Ʒ�۸�: " << row[2] << endl;
            cout << "��Ʒ����: " << row[3] << endl;
            cout << "--------------------" << endl;
        }
        cout << "====================" << endl;
        mysql_free_result(res);
    }

    void checkLowStock(int threshold) {
        string query = "SELECT * FROM products WHERE quantity < " + to_string(threshold);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "��ѯ���ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        cout << "====================" << endl;
        cout << "�Ϳ����Ʒ��" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "��ƷID: " << row[0] << endl;
            cout << "��Ʒ����: " << row[1] << endl;
            cout << "��Ʒ�۸�: " << row[2] << endl;
            cout << "��Ʒ����: " << row[3] << endl;
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
            cerr << "��ӹ�����ʧ��: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "��������ӳɹ���" << endl;
            cout << "====================" << endl;
        }
    }

    void updateSupplier(int id, const string& name, const string& contact) {
        string query = "UPDATE suppliers SET name = '" + name + "', contact = '" + contact + "' WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "���¹�����ʧ��: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "�����̸��³ɹ���" << endl;
            cout << "====================" << endl;
        }
    }

    void deleteSupplier(int id) {
        string query = "DELETE FROM suppliers WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "ɾ��������ʧ��: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "������ɾ���ɹ���" << endl;
            cout << "====================" << endl;
        }
    }

    void querySupplier(int id) {
        string query = "SELECT * FROM suppliers WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "��ѯ������ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        if ((row = mysql_fetch_row(res))) {
            cout << "====================" << endl;
            cout << "������ID: " << row[0] << endl;
            cout << "����������: " << row[1] << endl;
            cout << "��������ϵ��ʽ: " << row[2] << endl;
            cout << "====================" << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "δ�ҵ������̡�" << endl;
            cout << "====================" << endl;
        }
        mysql_free_result(res);
    }

    void generateSupplierReport() {
        string query = "SELECT * FROM suppliers";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "��ѯ������ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        cout << "====================" << endl;
        cout << "�����̱��棺" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "������ID: " << row[0] << endl;
            cout << "����������: " << row[1] << endl;
            cout << "��������ϵ��ʽ: " << row[2] << endl;
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
            cerr << "��Ӳɹ�����ʧ��: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "�ɹ�������ӳɹ���" << endl;
            cout << "====================" << endl;
        }
    }

    void updateOrder(int id, const string& date, int supplierId, int productId, int quantity) {
        string query = "UPDATE purchase_orders SET date = '" + date + "', supplier_id = " + to_string(supplierId) + ", product_id = " + to_string(productId) + ", quantity = " + to_string(quantity) + " WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "���²ɹ�����ʧ��: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "�ɹ��������³ɹ���" << endl;
            cout << "====================" << endl;
        }
    }

    void deleteOrder(int id) {
        string query = "DELETE FROM purchase_orders WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "ɾ���ɹ�����ʧ��: " << mysql_error(db->getConnection()) << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "�ɹ�����ɾ���ɹ���" << endl;
            cout << "====================" << endl;
        }
    }

    void queryOrder(int id) {
        string query = "SELECT * FROM purchase_orders WHERE id = " + to_string(id);
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "��ѯ�ɹ�����ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        if ((row = mysql_fetch_row(res))) {
            cout << "====================" << endl;
            cout << "����ID: " << row[0] << endl;
            cout << "����: " << row[1] << endl;
            cout << "������ID: " << row[2] << endl;
            cout << "��ƷID: " << row[3] << endl;
            cout << "����: " << row[4] << endl;
            cout << "====================" << endl;
        }
        else {
            cout << "====================" << endl;
            cout << "δ�ҵ�������" << endl;
            cout << "====================" << endl;
        }
        mysql_free_result(res);
    }

    void generatePurchaseHistory() {
        string query = "SELECT * FROM purchase_orders";
        if (mysql_query(db->getConnection(), query.c_str())) {
            cerr << "��ѯ�ɹ�����ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(db->getConnection());
        if (res == NULL) {
            cerr << "mysql_store_result() ʧ��: " << mysql_error(db->getConnection()) << endl;
            return;
        }

        MYSQL_ROW row;
        cout << "====================" << endl;
        cout << "�ɹ�������ʷ��" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "����ID: " << row[0] << endl;
            cout << "����: " << row[1] << endl;
            cout << "������ID: " << row[2] << endl;
            cout << "��ƷID: " << row[3] << endl;
            cout << "����: " << row[4] << endl;
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
            cout << "1. ע��\n2. ��¼\n3. �����Ʒ\n4. ��ѯ��Ʒ\n5. ɾ����Ʒ\n6. ��ѯ������Ʒ\n7. ���ÿ�澯��\n8. ��ӹ�����\n9. ���¹�����\n10. ɾ��������\n11. ��ѯ������\n12. ���ɹ����̱���\n13. �˳���¼\n14. �˳�ϵͳ\n��ѡ��";
            cout << "====================" << endl;
            cout << "���ѡ��: ";
            cin >> choice;

            switch (choice) {
            case 1:
                cout << "�����û�����";
                cin >> username;
                cout << "�������룺";
                cin >> password;
                user.registerUser(username, password);
                break;
            case 2:
                cout << "�����û�����";
                cin >> username;
                cout << "�������룺";
                cin >> password;
                loggedIn = user.loginUser(username, password);
                break;
            case 3:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "������ƷID��";
                cin >> id;
                cout << "������Ʒ���ƣ�";
                cin >> name;
                cout << "������Ʒ�۸�";
                cin >> price;
                cout << "������Ʒ������";
                cin >> quantity;
                product.addProduct(id, name, price, quantity);
                break;
            case 4:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "������ƷID��";
                cin >> id;
                product.queryProduct(id);
                break;
            case 5:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "������ƷID��";
                cin >> id;
                product.deleteProduct(id);
                break;
            case 6:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                product.queryAllProducts();
                break;
            case 7:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "���ÿ�澯����ֵ��";
                cin >> threshold;
                product.checkLowStock(threshold);
                break;
            case 8:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "���빩����ID��";
                cin >> id;
                cout << "���빩�������ƣ�";
                cin >> name;
                cout << "���빩������ϵ��ʽ��";
                cin >> contact;
                supplier.addSupplier(id, name, contact);
                break;
            case 9:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "���빩����ID��";
                cin >> id;
                cout << "���빩�������ƣ�";
                cin >> name;
                cout << "���빩������ϵ��ʽ��";
                cin >> contact;
                supplier.updateSupplier(id, name, contact);
                break;
            case 10:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "���빩����ID��";
                cin >> id;
                supplier.deleteSupplier(id);
                break;
            case 11:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                cout << "���빩����ID��";
                cin >> id;
                supplier.querySupplier(id);
                break;
            case 12:
                if (!loggedIn) {
                    cout << "====================" << endl;
                    cout << "���ȵ�¼��" << endl;
                    cout << "====================" << endl;
                    break;
                }
                supplier.generateSupplierReport();
                break;
            case 13:
                loggedIn = false;
                cout << "====================" << endl;
                cout << "���˳���¼��" << endl;
                cout << "====================" << endl;
                break;
            case 14:
                cout << "====================" << endl;
                cout << "ϵͳ�˳���" << endl;
                cout << "====================" << endl;
                return;
            default:
                cout << "====================" << endl;
                cout << "��Ч��ѡ��" << endl;
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
