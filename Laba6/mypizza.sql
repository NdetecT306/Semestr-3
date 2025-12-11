-- Создание базы данных
CREATE DATABASE pizzeria;
\c pizzeria;

-- Филиалы
CREATE TABLE branches (
    branchCode SERIAL PRIMARY KEY,
    address VARCHAR(255) NOT NULL
);

-- Города
CREATE TABLE cities (
    cityCode SERIAL PRIMARY KEY,
    cityName VARCHAR(100) NOT NULL UNIQUE
);

-- Связь городов и филиалов 
CREATE TABLE city_branches (
    cityCode INT REFERENCES cities(cityCode) ON DELETE CASCADE,
    branchCode INT REFERENCES branches(branchCode) ON DELETE CASCADE,
    PRIMARY KEY (cityCode, branchCode)
);

-- Категории
CREATE TABLE categories (
    categoryCode SERIAL PRIMARY KEY,
    categoryName VARCHAR(100) NOT NULL UNIQUE
);

-- Блюда
CREATE TABLE dishes (
    dishCode SERIAL PRIMARY KEY,
    dishName VARCHAR(150) NOT NULL,
    categoryCode INT REFERENCES categories(categoryCode) ON DELETE CASCADE
);

-- Меню
CREATE TABLE menu (
    recordCode SERIAL PRIMARY KEY,
    cityCode INT REFERENCES cities(cityCode) ON DELETE CASCADE,
    categoryCode INT REFERENCES categories(categoryCode) ON DELETE CASCADE,
    dishCode INT REFERENCES dishes(dishCode) ON DELETE CASCADE,
    price DECIMAL(10, 2) NOT NULL CHECK (price > 0),
    UNIQUE(cityCode, dishCode) 
);

CREATE INDEX idx_menu_city ON menu(cityCode);
CREATE INDEX idx_menu_dish ON menu(dishCode);
CREATE INDEX idx_menu_category ON menu(categoryCode);
CREATE INDEX idx_city_branches_city ON city_branches(cityCode);
CREATE INDEX idx_city_branches_branch ON city_branches(branchCode);
CREATE INDEX idx_dishes_category ON dishes(categoryCode);

-- Заполнение данными
INSERT INTO branches (address) VALUES
    ('ул. Красный проспект, 17'),
    ('ул. Первомайская, 51/23'),
    ('ул. Гоголя, 33/1');

INSERT INTO cities (cityName) VALUES
    ('Новосибирск'),
    ('Москва'),
    ('Санкт-Петербург');
    
-- Связываем города и филиалы
INSERT INTO city_branches (cityCode, branchCode) VALUES
    (1, 1), 
    (2, 2), 
    (2, 3); 

INSERT INTO categories (categoryName) VALUES
    ('Комбо'),
    ('Пицца'),
    ('Закуски'),
    ('Кофе'),
    ('Напитки'),
    ('Коктейли'),
    ('Десерты');

INSERT INTO dishes (dishName, categoryCode) VALUES
    ('3 пиццы', 1),
    ('2 пиццы', 1),
    ('4 закуски', 1),
    ('Чикен бокс', 1),
    ('Салат с закуской', 1),
    
    ('Пицца с хреном', 2),
    ('Мясная', 2),
    ('Додо', 2),
    ('Пепперони фреш', 2),
    ('Чесночный цыпленок', 2),
    
    ('Ланчбокс с хреном', 3),
    ('Паста мясная', 3),
    ('Холодный чикен ролл', 3),
    ('Додстер', 3),
    ('Додстер с ветчиной', 3),

    ('Американо', 4),
    ('Латте темный лес', 4),
    ('Капучино', 4),
    ('Ореховый латте', 4),
    ('Карамельный каппучино', 4),
    
    ('Добрый кола', 5),
    ('Добрый кола без сахара', 5),
    ('Добрый киви-виноград', 5),
    ('Добрый лимон-лайм', 5),
    ('Rich Tea зеленый', 5),
    
    ('Ежевика-малина', 6),
    ('Пина колада', 6),
    ('С печеньем орео', 6),
    ('Классический', 6),
    ('Клубника', 6),
    
    ('Фондан', 7),
    ('Тирамису', 7),
    ('Черничный маффин', 7),
    ('Сорбет Лимонный фреш', 7),
    ('Сырники', 7);

INSERT INTO menu (cityCode, categoryCode, dishCode, price) VALUES
    (1, 1, 4, 450.00),  
    (1, 2, 8, 550.00), 
    (1, 3, 12, 350.00),  
    (1, 4, 19, 300.00), 
    (1, 5, 23, 150.00),  
    (1, 6, 29, 280.00),
    (1, 7, 31, 280.00),
    
    (2, 1, 3, 430.00),  
    (2, 2, 7, 580.00), 
    (2, 3, 11, 370.00),  
    (2, 4, 16, 140.00),
    (2, 5, 21, 140.00),
    (2, 6, 28, 290.00), 
    (2, 7, 32, 290.00);

-- Представление с подробной информацией о меню
CREATE VIEW menu_details AS
SELECT 
    m.recordCode,
    c.cityName,
    cat.categoryName,
    d.dishName,
    m.price,
    b.address,
    STRING_AGG(b.address, ', ') AS branch_addresses 
FROM menu m
JOIN cities c ON m.cityCode = c.cityCode
JOIN categories cat ON m.categoryCode = cat.categoryCode
JOIN dishes d ON m.dishCode = d.dishCode
JOIN city_branches cb ON c.cityCode = cb.cityCode
JOIN branches b ON cb.branchCode = b.branchCode
GROUP BY m.recordCode, c.cityName, cat.categoryName, d.dishName, m.price, b.address;

