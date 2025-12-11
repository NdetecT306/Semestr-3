#include <iostream>
#include <string>
#include <iomanip>
#include <postgresql/libpq-fe.h>
using namespace std;

class Database {
private:
    PGconn* connect;
public:
    Database(const string& conninfo) {
        connect = PQconnectdb(conninfo.c_str());
        if (PQstatus(connect) != CONNECTION_OK) {
            cerr << "Не удалось связаться с БД: " << PQerrorMessage(connect) << endl;
            PQfinish(connect);
            connect = nullptr;
        } 
    } 
    ~Database() {
        if (connect) {
            PQfinish(connect);
            cout << "Конец." << endl;
        }
    }
    bool isConnected() const {
        return connect != nullptr && PQstatus(connect) == CONNECTION_OK;
    }
    bool executeQuery(const string& query) {
        if (!isConnected()) {
            cerr << "Отсутствует связь с бд" << endl;
            return false;
        }
        PGresult* res = PQexec(connect, query.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            cerr << "Запрос провален: " << PQerrorMessage(connect) << endl;
            PQclear(res);
            return false;
        }
        int rows = PQntuples(res);
        int cols = PQnfields(res);
        cout << "\n" << string(80, '-') << endl;
        for (int j = 0; j < cols; j++) {
            string colName = PQfname(res, j);
            cout << colName << "\t";
        }
        cout << endl << string(80, '-') << endl;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                string value = PQgetvalue(res, i, j) ? PQgetvalue(res, i, j) : "NULL";
                cout << value << "\t";
            }
            cout << endl;
        }
        PQclear(res);
        return true;
    }
    void getMenuDetails() {
        string query = "SELECT * FROM menu_details ORDER BY dishname, cityname, categoryname";
        cout << "\nМЕНЮ (отсортировано по названию блюда)" << endl;
        executeQuery(query);
    }
    void getDishesByCategoryName(const string& categoryName) {
        string query = 
            "SELECT "
            "   cat.categoryName, "
            "   d.dishName, "
            "   md.cityName, "
            "   md.price, "
            "   md.address "  
            "FROM dishes d "
            "JOIN categories cat ON d.categoryCode = cat.categoryCode "
            "LEFT JOIN menu_details md ON d.dishName = md.dishName "
            "WHERE cat.categoryName = '" + categoryName + "' "
            "ORDER BY d.dishName, md.cityName";
        cout << "\nБЛЮДА КАТЕГОРИИ: " << categoryName << endl;
        executeQuery(query);
    }
    void getAllDishesWithCategories() {
        string query = 
            "SELECT "
            "   d.dishName, "
            "   cat.categoryName "
            "FROM dishes d "
            "JOIN categories cat ON d.categoryCode = cat.categoryCode "
            "ORDER BY cat.categoryName, d.dishName";
        cout << "\nВСЕ БЛЮДА С КАТЕГОРИЯМИ" << endl;
        executeQuery(query);
    }
    void getDishesNotInMenu(int cityCode) {
        string query = 
            "SELECT "
            "   d.dishName, "
            "   cat.categoryName "
            "FROM dishes d "
            "JOIN categories cat ON d.categoryCode = cat.categoryCode "
            "WHERE d.dishCode NOT IN ("
            "   SELECT dishCode FROM menu WHERE cityCode = " + to_string(cityCode) + ""
            ") "
            "ORDER BY cat.categoryName, d.dishName";
        cout << "\nБЛЮДА, ОТСУТСТВУЮЩИЕ В МЕНЮ ГОРОДА под номером " << cityCode << endl;
        executeQuery(query);
    }
    void addPizzaToMenuByCode(int cityCode, int dishCode, double price) {
        string query = 
            "INSERT INTO menu (cityCode, categoryCode, dishCode, price) "
            "VALUES (" + to_string(cityCode) + ", 2, " + to_string(dishCode) + ", " + to_string(price) + ")";
        cout << "\nДОБАВЛЕНИЕ НОВОЙ ПИЦЦЫ В МЕНЮ" << endl;
        executeQuery(query);
    }
    void addPizzaSHrenomToNovosibirsk() {
        double price = 550.00; 
        cout << "\nДОБАВЛЕНИЕ ПИЦЦЫ С ХРЕНОМ В НОВОСИБИРСК" << endl;
        string checkQuery = 
            "SELECT COUNT(*) FROM menu_details "
            "WHERE cityName = 'Новосибирск' AND dishName = 'Пицца с хреном'";
        PGresult* res = PQexec(connect, checkQuery.c_str());
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            int count = atoi(PQgetvalue(res, 0, 0));
            if (count > 0) {
                cout << "Пицца с хреном уже есть в меню Новосибирска!" << endl;
                PQclear(res);
                return;
            }
        }
        PQclear(res);
        addPizzaToMenuByCode(1, 6, price);
        cout << "\nОБНОВЛЕННОЕ МЕНЮ НОВОСИБИРСКА (отсортировано по названию блюда)" << endl;
        string menuQuery = "SELECT * FROM menu_details WHERE cityName = 'Новосибирск' ORDER BY dishName, categoryName";
        PGresult* menuRes = PQexec(connect, menuQuery.c_str());
        if (PQresultStatus(menuRes) == PGRES_TUPLES_OK) {
            int rows = PQntuples(menuRes);
            int cols = PQnfields(menuRes);
            cout << string(80, '-') << endl;
            for (int j = 0; j < cols; j++) {
                cout << PQfname(menuRes, j) << "\t";
            }
            cout << endl << string(80, '-') << endl;
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    cout << (PQgetvalue(menuRes, i, j) ? PQgetvalue(menuRes, i, j) : "NULL") << "\t";
                }
                cout << endl;
            }
        }
        PQclear(menuRes);
    }
    void deletePizzaSHrenomFromNovosibirsk() {
        cout << "\nУДАЛЕНИЕ ПИЦЦЫ С ХРЕНОМ ИЗ НОВОСИБИРСКА" << endl;
        string checkQuery = 
            "SELECT COUNT(*) FROM menu_details "
            "WHERE cityName = 'Новосибирск' AND dishName = 'Пицца с хреном'";
        PGresult* res = PQexec(connect, checkQuery.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            cerr << "Ошибка проверки блюда: " << PQerrorMessage(connect) << endl;
            PQclear(res);
            return;
        }
        int count = atoi(PQgetvalue(res, 0, 0));
        PQclear(res);
        if (count == 0) {
            cout << "Пицца с хреном отсутствует в меню Новосибирска!" << endl;
            return;
        }
        string getDishCodeQuery = 
            "SELECT dishCode FROM dishes WHERE dishName = 'Пицца с хреном'";
        PGresult* dishRes = PQexec(connect, getDishCodeQuery.c_str());
        if (PQresultStatus(dishRes) != PGRES_TUPLES_OK) {
            cerr << "Ошибка получения кода блюда: " << PQerrorMessage(connect) << endl;
            PQclear(dishRes);
            return;
        }
        if (PQntuples(dishRes) == 0) {
            cout << "Блюдо 'Пицца с хреном' не найдено в таблице dishes!" << endl;
            PQclear(dishRes);
            return;
        }
        int dishCode = atoi(PQgetvalue(dishRes, 0, 0));
        PQclear(dishRes);
        string deleteQuery = 
            "DELETE FROM menu WHERE cityCode = 1 AND dishCode = " + to_string(dishCode);
        cout << "Удаление пиццы с хреном (dishCode: " << dishCode << ") из меню Новосибирска..." << endl;
        PGresult* deleteRes = PQexec(connect, deleteQuery.c_str());
        if (PQresultStatus(deleteRes) != PGRES_COMMAND_OK) {
            cerr << "Ошибка удаления блюда: " << PQerrorMessage(connect) << endl;
            PQclear(deleteRes);
            return;
        }
        int rowsDeleted = atoi(PQcmdTuples(deleteRes));
        PQclear(deleteRes);
        cout << "Успешно удалено " << rowsDeleted << " запись(ей)" << endl;
        cout << "\nОБНОВЛЕННОЕ МЕНЮ НОВОСИБИРСКА:" << endl;
        string menuQuery = "SELECT * FROM menu_details WHERE cityName = 'Новосибирск' ORDER BY dishName, categoryName";
        executeQuery(menuQuery);
    }
    void deleteDishFromMenuByCode(int cityCode, int dishCode) {
        string query = 
            "DELETE FROM menu WHERE cityCode = " + to_string(cityCode) + 
            " AND dishCode = " + to_string(dishCode);
        cout << "\nУДАЛЕНИЕ БЛЮДА ИЗ МЕНЮ" << endl;
        cout << "Удаление блюда с кодом " << dishCode << " из меню города с кодом " << cityCode << endl;
        executeQuery(query);
    }
    void deleteChickenBoxFromMoscow() {
        cout << "\nУДАЛЕНИЕ ЗАКУСКИ 'ЧИКЕН БОКС' ИЗ МЕНЮ МОСКВЫ" << endl;
        string checkQuery = 
            "SELECT COUNT(*) FROM menu_details "
            "WHERE cityName = 'Москва' AND dishName = 'Чикен бокс'";
        PGresult* res = PQexec(connect, checkQuery.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            cerr << "Ошибка проверки блюда: " << PQerrorMessage(connect) << endl;
            PQclear(res);
            return;
        }
        int count = atoi(PQgetvalue(res, 0, 0));
        PQclear(res);
        if (count == 0) {
            cout << "Закуска 'Чикен бокс' уже отсутствует в меню Москвы!" << endl;
            return;
        }
        deleteDishFromMenuByCode(2, 4);
    }
    void getTotalPriceNovosibirsk() {
        cout << "\nСУММА СТОИМОСТИ ВСЕХ БЛЮД В МЕНЮ НОВОСИБИРСКА" << endl;
        string query = 
            "SELECT SUM(price) as total_price FROM menu_details "
            "WHERE cityName = 'Новосибирск'";
        PGresult* res = PQexec(connect, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            cerr << "Ошибка вычисления суммы: " << PQerrorMessage(connect) << endl;
            PQclear(res);
            return;
        }
        double total = 0.0;
        if (PQgetvalue(res, 0, 0) != NULL) {
            total = atof(PQgetvalue(res, 0, 0));
        }
        cout << fixed << setprecision(2);
        cout << "Общая стоимость всех блюд в меню Новосибирска: " << total << " руб." << endl;
        cout << "\nДетализация (отсортировано по названию блюда):" << endl;
        string detailQuery = 
            "SELECT dishName, price FROM menu_details "
            "WHERE cityName = 'Новосибирск' "
            "ORDER BY dishName";
        PGresult* detailRes = PQexec(connect, detailQuery.c_str());
        if (PQresultStatus(detailRes) == PGRES_TUPLES_OK) {
            int rows = PQntuples(detailRes);
            cout << string(40, '-') << endl;
            cout << "Блюдо\t\t\tЦена" << endl;
            cout << string(40, '-') << endl;
            double checkTotal = 0.0;
            for (int i = 0; i < rows; i++) {
                string dishName = PQgetvalue(detailRes, i, 0) ? PQgetvalue(detailRes, i, 0) : "";
                double price = atof(PQgetvalue(detailRes, i, 1));
                checkTotal += price;
                cout << dishName << "\t\t" << fixed << setprecision(2) << price << " руб." << endl;
            }
            cout << string(40, '-') << endl;
            cout << "Итого: " << checkTotal << " руб." << endl;
        }
        PQclear(detailRes);
        PQclear(res);
    }
    void removeMenuDuplicates() {
        cout << "\nУДАЛЕНИЕ ДУБЛИКАТОВ В МЕНЮ ЕСЛИ ЕСТЬ" << endl;
        string findDuplicatesQuery = 
            "SELECT cityCode, dishCode, COUNT(*) as duplicate_count "
            "FROM menu "
            "GROUP BY cityCode, dishCode "
            "HAVING COUNT(*) > 1 "
            "ORDER BY cityCode, dishCode";
        PGresult* dupRes = PQexec(connect, findDuplicatesQuery.c_str());
        if (PQresultStatus(dupRes) == PGRES_TUPLES_OK) {
            int dupRows = PQntuples(dupRes);
            if (dupRows > 0) {
                cout << "Найдены дубликаты в меню:" << endl;
                cout << string(50, '-') << endl;
                cout << "Код города\tКод блюда\tКоличество дубликатов" << endl;
                cout << string(50, '-') << endl;
                for (int i = 0; i < dupRows; i++) {
                    string cityCode = PQgetvalue(dupRes, i, 0);
                    string dishCode = PQgetvalue(dupRes, i, 1);
                    string count = PQgetvalue(dupRes, i, 2);
                    cout << cityCode << "\t\t" << dishCode << "\t\t" << count << endl;
                }
                cout << string(50, '-') << endl;
                string deleteDuplicatesQuery = 
                    "DELETE FROM menu m1 "
                    "WHERE EXISTS ("
                    "   SELECT 1 FROM menu m2 "
                    "   WHERE m2.cityCode = m1.cityCode "
                    "   AND m2.dishCode = m1.dishCode "
                    "   AND m2.ctid < m1.ctid"
                    ")";
                PGresult* delRes = PQexec(connect, deleteDuplicatesQuery.c_str());
                if (PQresultStatus(delRes) == PGRES_COMMAND_OK) {
                    int rowsDeleted = atoi(PQcmdTuples(delRes));
                    cout << "Удалено " << rowsDeleted << " дубликат(ов)" << endl;
                } else {
                    cerr << "Ошибка удаления дубликатов: " << PQerrorMessage(connect) << endl;
                }
                PQclear(delRes);
            } else {
                cout << "Дубликаты не найдены." << endl;
            }
        }
        PQclear(dupRes);
    }
    void getNovosibirskAddresses() {
        cout << "\nВСЕ АДРЕСА РЕСТОРАНОВ В НОВОСИБИРСКЕ" << endl;
        string query = 
            "SELECT DISTINCT b.address "
            "FROM branches b "
            "JOIN city_branches cb ON b.branchCode = cb.branchCode "
            "JOIN cities c ON cb.cityCode = c.cityCode "
            "WHERE c.cityName = 'Новосибирск' "
            "ORDER BY b.address";
        PGresult* res = PQexec(connect, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            cerr << "Ошибка получения адресов: " << PQerrorMessage(connect) << endl;
            PQclear(res);
            return;
        }
        int rows = PQntuples(res);
        if (rows == 0) {
            cout << "Адреса для Новосибирска не найдены." << endl;
        } else {
            cout << "Всего найдено " << rows << " уникальных адресов:" << endl;
            cout << string(60, '-') << endl;
            for (int i = 0; i < rows; i++) {
                string address = PQgetvalue(res, i, 0) ? PQgetvalue(res, i, 0) : "Адрес не указан";
                cout << (i + 1) << ". " << address << endl;
            }
            cout << string(60, '-') << endl;
        }
        PQclear(res);
    }
    void getMenuSortedByDish() {
        string query = "SELECT * FROM menu_details ORDER BY dishname ASC, cityname, categoryname";
        cout << "\nМЕНЮ (отсортировано по алфавиту блюд)" << endl;
        executeQuery(query);
    }
    void getAveragePizzaPriceMoscowStrict() {
        cout << "\nСРЕДНЯЯ ЦЕНА ПИЦЦЫ В МОСКВЕ" << endl;
        string query = 
            "SELECT "
            "   cityName, "
            "   categoryName, "
            "   ROUND(AVG(price)::numeric, 2) as average_price "
            "FROM menu_details "
            "GROUP BY cityName, categoryName "
            "ORDER BY cityName, categoryName";
        PGresult* res = PQexec(connect, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            cerr << "Ошибка вычисления средней цены: " << PQerrorMessage(connect) << endl;
            PQclear(res);
            return;
        }
        int rows = PQntuples(res);
        if (rows == 0) {
            cout << "Данные не найдены." << endl;
            PQclear(res);
            return;
        }
        cout << "ВСЕ СРЕДНИЕ ЦЕНЫ (группировка по городу и категории):" << endl;
        cout << string(80, '-') << endl;
        cout << "Город\t\tКатегория\tСредняя цена (руб.)" << endl;
        cout << string(80, '-') << endl;
        string moscowPizzaPrice = "0.00";
        bool found = false;
        for (int i = 0; i < rows; i++) {
            string cityName = PQgetvalue(res, i, 0) ? PQgetvalue(res, i, 0) : "";
            string categoryName = PQgetvalue(res, i, 1) ? PQgetvalue(res, i, 1) : "";
            string averagePrice = PQgetvalue(res, i, 2) ? PQgetvalue(res, i, 2) : "0.00";
            cout << cityName << "\t\t" << categoryName << "\t\t"  << averagePrice << " руб." << endl;
            if (cityName == "Москва" && categoryName == "Пицца") {
                moscowPizzaPrice = averagePrice;
                found = true;
            }
        }
        cout << string(80, '-') << endl;
        if (found) {
            cout << "\nРЕЗУЛЬТАТ ДЛЯ МОСКВЫ (ПИЦЦА): " 
                << moscowPizzaPrice << " руб." << endl;
        } else {
            cout << "\nДанные о пицце в Москве не найдены в результатах." << endl;
        }
        PQclear(res);
    }
};
int main() {
    string conninfo = "host=localhost dbname=pizzeria user=postgres port=5432";
    Database db(conninfo);
    if (db.isConnected()) {
        cout << "Успешное подключение к базе данных!\n" << endl;
        cout << "\n\n";
        db.getMenuDetails();
        cout << "\n\n";
        db.getNovosibirskAddresses();
        cout << "\n\n";
        db.removeMenuDuplicates();
        cout << "\n\n";
        db.getDishesByCategoryName("Пицца");
        cout << "\n\n";
        db.getDishesByCategoryName("Десерты");
        cout << "\n\n";
        db.getAllDishesWithCategories();
        cout << "\n\n";
        db.getDishesNotInMenu(1);
        cout << "\n\n";
        db.addPizzaToMenuByCode(1, 6, 550.00);
        cout << "\n\n";
        db.getMenuSortedByDish();
        cout << "\n\n";
        db.deletePizzaSHrenomFromNovosibirsk();
        cout << "\n\n";
        db.deleteChickenBoxFromMoscow();
        cout << "\n\n";
        db.getMenuSortedByDish();
        cout << "\n\n";
        db.getTotalPriceNovosibirsk();
        cout << "\n\n";
        db.getNovosibirskAddresses();
        cout << "\n\n";
        db.getAveragePizzaPriceMoscowStrict();
    } else {
        cerr << "Ошибка подключения к базе данных. Проверьте параметры подключения." << endl;
    }
    
    return 0;
}
