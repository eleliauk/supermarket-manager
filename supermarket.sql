USE supermarket;

CREATE TABLE users (
                       id INT AUTO_INCREMENT PRIMARY KEY,
                       username VARCHAR(255) NOT NULL,
                       password VARCHAR(255) NOT NULL
);

CREATE TABLE products (
                          id INT PRIMARY KEY,
                          name VARCHAR(255) NOT NULL,
                          price DOUBLE NOT NULL,
                          quantity INT NOT NULL
);

-- 创建 suppliers 表
CREATE TABLE suppliers (
                           id INT NOT NULL PRIMARY KEY,
                           name VARCHAR(100) NOT NULL,
                           contact VARCHAR(100)
);

-- 创建 purchase_orders 表
CREATE TABLE purchase_orders (
                                 id INT NOT NULL PRIMARY KEY,
                                 date DATE NOT NULL,
                                 supplier_id INT NOT NULL,
                                 product_id INT NOT NULL,
                                 quantity INT NOT NULL,
                                 FOREIGN KEY (supplier_id) REFERENCES suppliers(id),
                                 FOREIGN KEY (product_id) REFERENCES products(id)
);


CREATE TABLE supplier_products (
                                   supplier_id INT NOT NULL,
                                   product_id INT NOT NULL,
                                   PRIMARY KEY (supplier_id, product_id),
                                   FOREIGN KEY (supplier_id) REFERENCES suppliers(id),
                                   FOREIGN KEY (product_id) REFERENCES products(id)
);
ALTER TABLE products
    ADD COLUMN stock_threshold INT DEFAULT 0;