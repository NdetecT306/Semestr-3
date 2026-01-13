import requests
import json
import time
import random
from datetime import datetime

class BuyBot:
    def __init__(self, base_url="http://localhost:8080"):
        self.base_url = base_url
        self.user_key = None
        self.user_id = None
        self.bot_name = f"BuyBot_{random.randint(1000, 9999)}"
        # Только пары в которых есть рубли
        self.target_pairs = [1, 2, 3, 4, 5, 6]
        # Статистика
        self.orders_created = 0
        self.trades_executed = 0
        self.failed_trades = 0
        self.orders_cancelled = 0
        self.is_running = True
        self.max_orders_per_check = 1000000  
        self.random_order_chance = 0.3   
        self.cancel_probability = 0.15  
        self.active_orders = []  
        self.lots = {}  
        self.last_check_time = 0 
    def load_lots(self):
        try:
            response = requests.get(f"{self.base_url}/lot", timeout=5)
            if response.status_code == 200:
                lots_data = response.json()
                # Проверяем структуру ответа
                if isinstance(lots_data, dict):
                    if "lots" in lots_data:
                        lots_list = lots_data["lots"]
                    elif "data" in lots_data:
                        lots_list = lots_data["data"]
                    else:
                        lots_list = lots_data
                else:
                    lots_list = lots_data
                for lot in lots_list:
                    if isinstance(lot, dict):
                        lot_id = lot.get("lot_id") or lot.get("id")
                        lot_name = lot.get("name") or str(lot_id)
                        if lot_id:
                            self.lots[lot_id] = lot_name
                print(f"Загружено {len(self.lots)} лотов")
                return True
        except Exception as e:
            print(f"Ошибка загрузки лотов: {str(e)[:50]}")
        return False  
    def create_user(self):
        data = {"username": self.bot_name}
        try:
            response = requests.post(f"{self.base_url}/user", json=data, timeout=10)
            if response.status_code == 200:
                result = response.json()
                if "key" in result:
                    self.user_key = result["key"]
                    self.user_id = result.get("user_id")
                    print(f"Пользователь создан: {self.bot_name}")
                    self.load_lots()
                    return True
                elif "error" in result:
                    print(f"Ошибка: {result['error']}")
            else:
                print(f"HTTP ошибка: {response.status_code}") 
        except Exception as e:
            print(f"Ошибка создания пользователя: {e}")
        return False
    def get_user_orders(self):
        """Получает активные ордера пользователя"""
        if not self.user_key:
            return []
        try:
            response = requests.get(f"{self.base_url}/order", timeout=5)
            if response.status_code == 200:
                orders_data = response.json()
                # Нормализуем данные
                orders = []
                if isinstance(orders_data, list):
                    orders = orders_data
                elif isinstance(orders_data, dict):
                    if "orders" in orders_data:
                        orders = orders_data["orders"]
                    elif "data" in orders_data:
                        orders = orders_data["data"]
                user_orders = []
                for order in orders:
                    if isinstance(order, dict):
                        user_id = order.get("user_id")
                        closed = order.get("closed", "")
                        if (user_id == self.user_id or (self.user_id and str(user_id) == str(self.user_id))) and not closed:
                            user_orders.append({
                                "order_id": order.get("order_id"),"type": order.get("type"),
                                "pair_id": order.get("lot_id") or order.get("pair_id"),
                                "quantity": order.get("quantity"),"price": order.get("price")
                            })
                return user_orders
        except Exception as e:
            print(f"Ошибка получения ордеров: {e}")
        return []
    def cancel_order(self, order_id):
        if not self.user_key:
            return False
        headers = {
            "X-USER-KEY": self.user_key,
            "Content-Type": "application/json"
        }
        data = {
            "order_id": int(order_id)
        }
        try:
            response = requests.delete(f"{self.base_url}/order", json=data, headers=headers, timeout=5)
            if response.status_code == 200:
                result = response.json()
                if result.get("success"):
                    self.orders_cancelled += 1
                    print(f"Ордер {order_id} отменен (отменен пользователем)")
                    return True
                else:
                    error_msg = result.get("error", "Неизвестная ошибка")
                    print(f"Ошибка отмены ордера {order_id}: {error_msg}")
            else:
                print(f"HTTP ошибка {response.status_code} при отмене ордера")
        except Exception as e:
            print(f"Ошибка при отмене ордера: {e}")
        return False
    def cancel_random_order(self):
        """Случайно отменяет один из активных ордеров"""
        if not self.active_orders:
            return False
        # Выбираем случайный ордер для отмены
        order_to_cancel = random.choice(self.active_orders)
        order_id = order_to_cancel.get("order_id")
        if not order_id:
            return False
        success = self.cancel_order(order_id)
        if success:
            # Удаляем из списка активных
            self.active_orders = [o for o in self.active_orders if o.get("order_id") != order_id]
            print(f"Отменен случайный ордер {order_id}")
        return success
    def get_balance(self):
        if not self.user_key:
            return []
        headers = {
            "X-USER-KEY": self.user_key
        }
        try:
            response = requests.get(f"{self.base_url}/balance", headers=headers, timeout=5)
            if response.status_code == 200:
                response_data = response.json()
                if isinstance(response_data, dict):
                    if "balance" in response_data:
                        return response_data["balance"]
                    elif "data" in response_data:
                        return response_data["data"]
                    else:
                        return response_data
                elif isinstance(response_data, list):
                    return response_data
                else:
                    return []
            else:
                return []
        except Exception:
            return []
    def print_balance_summary(self):
        balance_data = self.get_balance()
        if not balance_data or not isinstance(balance_data, list):
            print(f"Нет данных о балансе")
            return
        print(f"Виртуальный кошелек (баланс)")
        has_non_zero_balance = False
        for item in balance_data:
            if isinstance(item, dict):
                # Получаем ID лота
                lot_id = item.get("lot_id") or item.get("id")
                # Получаем количество
                quantity_raw = item.get("quantity") or item.get("amount") or item.get("balance") or 0
                try:
                    quantity = float(quantity_raw)
                except (ValueError, TypeError):
                    continue
                # Выводим только ненулевые балансы
                if quantity != 0:
                    has_non_zero_balance = True
                    # Получаем название лота/валюты
                    lot_name = self.lots.get(lot_id, str(lot_id))
                    # Форматированный вывод
                    formatted_quantity = f"{quantity:.4f}"
                    if quantity >= 1000:
                        formatted_quantity = f"{quantity:.2f}"
                    elif quantity >= 100:
                        formatted_quantity = f"{quantity:.2f}"
                    elif quantity >= 1:
                        formatted_quantity = f"{quantity:.3f}"
                    print(f"    {lot_name}: {formatted_quantity}")
        if not has_non_zero_balance:
            print(f"Денег нет, но вы держитесь")
    def get_all_open_orders(self):
        """Получаем все открытые ордера"""
        try:
            response = requests.get(f"{self.base_url}/order", timeout=5)
            if response.status_code == 200:
                orders_data = response.json()
                # Нормализуем данные
                if isinstance(orders_data, list):
                    return orders_data
                elif isinstance(orders_data, dict):
                    if "orders" in orders_data:
                        return orders_data["orders"]
                    elif "data" in orders_data:
                        return orders_data["data"]
        except Exception as e:
            print(f"Ошибка получения ордеров: {e}")
        return []
    def find_sell_orders_for_pair(self, pair_id):
        all_orders = self.get_all_open_orders()
        sell_orders = []
        for order in all_orders:
            if not isinstance(order, dict):
                continue
            order_pair = order.get("pair_id")
            order_type = order.get("type", "").lower()
            closed = order.get("closed")
            # SELL ордер, не закрыт, наша пара
            if (order_pair == pair_id and 
                order_type == "sell" and 
                (closed is None or closed == "")):
                sell_orders.append(order)
        return sell_orders
    def create_buy_order(self, pair_id, price, quantity):
        try:
            data = {
                "pair_id": int(pair_id),
                "quantity": float(quantity),
                "price": float(price),
                "type": "buy"
            }
            headers = {
                "X-USER-KEY": self.user_key,
                "Content-Type": "application/json"
            }
            response = requests.post(f"{self.base_url}/order", json=data, headers=headers, timeout=8)
            if response.status_code == 200:
                result = response.json()
                if result.get("success"):
                    self.orders_created += 1
                    print(f"BUY ордер создан: пара {pair_id}, цена {price:.3f}, количество {quantity:.3f}")
                    # Добавляем в активные ордера
                    if "order_id" in result:
                        self.active_orders.append({
                            "order_id": result["order_id"],
                            "type": "buy",
                            "pair_id": pair_id,
                            "quantity": quantity,
                            "price": price
                        })
                    elif "order_ids" in result and result["order_ids"]:
                        order_id = result["order_ids"][0]
                        self.active_orders.append({
                            "order_id": order_id,
                            "type": "buy",
                            "pair_id": pair_id,
                            "quantity": quantity,
                            "price": price
                        })
                    # Проверяем, была ли сделка исполнена
                    if "message" in result and "исполнена" in str(result["message"]):
                        self.trades_executed += 1
                        print(f"СДЕЛКА ИСПОЛНЕНА! (#{self.trades_executed})")
                        # Если ордер исполнен, удаляем его из активных
                        if "order_id" in result:
                            self.active_orders = [o for o in self.active_orders if o.get("order_id") != result["order_id"]]
                    return True
                else:
                    error_msg = result.get("error", "")
                    print(f"Ошибка создания ордера: {error_msg}")
                    self.failed_trades += 1
            else:
                print(f"HTTP ошибка: {response.status_code}")
                self.failed_trades += 1  
        except Exception as e:
            print(f"Ошибка: {e}")
            self.failed_trades += 1
        return False
    def create_counter_buy_for_sell(self, pair_id, sell_order):
        sell_price = float(sell_order.get("price", 0))
        sell_quantity = float(sell_order.get("quantity", 0))
        if sell_price <= 0 or sell_quantity <= 0:
            return False
        # Разные стратегии цены 
        strategy = random.choice(['aggressive', 'moderate', 'conservative'])
        if strategy == 'aggressive':
            # АГРЕССИВНАЯ: на 1.6-2% ВЫШЕ (то ли медведи, то ли сливки)
            price_multiplier = random.uniform(1.016, 1.02)
        elif strategy == 'moderate':
            # УМЕРЕННАЯ: на 1.1-1.5% ВЫШЕ (норма)
            price_multiplier = random.uniform(1.011, 1.015)
        else:
            # КОНСЕРВАТИВНАЯ: на 0.5-1% ВЫШЕ (ну чтобы не быть китом и ничего не обвалить)
            price_multiplier = random.uniform(1.005, 1.01)
        buy_price = sell_price * price_multiplier
        # Разные стратегии количества - либо закрываем, либо он еще и делится
        quantity_strategy = random.choice(['full', 'partial'])
        if quantity_strategy == 'full':
            buy_quantity = sell_quantity
        else:
            buy_quantity = sell_quantity * random.uniform(1.0, 1.5)
        # Создаем ордер
        return self.create_buy_order(pair_id, buy_price, buy_quantity)
    def create_random_buy_order(self, pair_id):
        """Создает случайный BUY ордер"""
        buy_price = random.uniform(0.1, 5.0)
        buy_quantity = random.uniform(0.1, 5.0)
        return self.create_buy_order(pair_id, buy_price, buy_quantity)
    def create_multiple_orders_for_pair(self, pair_id, sell_orders):
        """Создает несколько ордеров для одной пары"""
        orders_created = 0
        if sell_orders:
            # Для каждого SELL ордера создаем встречный BUY
            for sell_order in sell_orders:
                if orders_created >= self.max_orders_per_check:
                    break
                if self.create_counter_buy_for_sell(pair_id, sell_order):
                    orders_created += 1
                    time.sleep(0.1)  
        # Если есть место для дополнительных ордеров, создаем случайные
        remaining_slots = self.max_orders_per_check - orders_created
        if remaining_slots > 0 and random.random() < self.random_order_chance:
            for _ in range(min(remaining_slots, random.randint(1, 3))):
                if self.create_random_buy_order(pair_id):
                    orders_created += 1
                    time.sleep(0.1)
        return orders_created
    def process_all_target_pairs(self):
        total_sell_found = 0
        total_buy_created = 0
        # Обновляем список активных ордеров
        if time.time() - self.last_check_time > 5:  # Каждые 5 секунд
            self.active_orders = self.get_user_orders()
            self.last_check_time = time.time()
        if self.active_orders and random.random() < self.cancel_probability:
            self.cancel_random_order()
        if self.orders_created % 5 == 0:
            self.print_balance_summary()
        # Проверяем каждую пару
        for pair_id in self.target_pairs:
            sell_orders = self.find_sell_orders_for_pair(pair_id)
            if sell_orders:
                print(f"Пара {pair_id}: найдено {len(sell_orders)} SELL ордеров")
                total_sell_found += len(sell_orders)
                # Создаем несколько ордеров для этой пары (Мамкины бизнесмены/вумены)
                orders_created = self.create_multiple_orders_for_pair(pair_id, sell_orders)
                total_buy_created += orders_created
                if orders_created > 0:
                    print(f"Создано {orders_created} BUY ордеров для пары {pair_id}")
            else:
                # Если нет SELL ордеров, создаем случайные BUY 
                if random.random() < self.random_order_chance * 1.5:  
                    print(f"Пара {pair_id}: нет SELL, создаю случайные BUY")
                    # Создаем несколько случайных ордеров - чем больше, тем лучше (мы все хотим кушать)
                    num_random_orders = random.randint(3, 7)
                    for i in range(num_random_orders):
                        if self.create_random_buy_order(pair_id):
                            total_buy_created += 1
                            time.sleep(0.1)
        # Краткая статистика
        if total_sell_found > 0 or total_buy_created > 0:
            print(f"Итог проверки: найдено {total_sell_found} SELL, создано {total_buy_created} BUY")
        return total_buy_created > 0
    def run_continuous_check(self):
        check_counter = 0
        last_statistics_time = time.time()
        try:
            while self.is_running:
                check_counter += 1
                # Выполняем проверку
                start_time = time.time()
                self.process_all_target_pairs()
                # Выводим статистику каждые 30 секунд
                current_time = time.time()
                if current_time - last_statistics_time >= 30:
                    print(f"Статистика: создано {self.orders_created} ордеров, "f"отменено {self.orders_cancelled}, "f"исполнено {self.trades_executed} сделок, "f"активных ордеров: {len(self.active_orders)}")
                    last_statistics_time = current_time
                # Короткая пауза между проверками
                time.sleep(1)   
        except KeyboardInterrupt:
            print(f"\n\nПолучен сигнал остановки")
        except Exception as e:
            print(f"\n\nКритическая ошибка: {e}")
        # Финальная статистика
        self.show_final_statistics()
    def show_final_statistics(self):
        print("ИТОГО")
        print(f"Имя бота: {self.bot_name}")
        print(f"Всего проверок: {self.orders_created + self.failed_trades}")
        print(f"Создано BUY ордеров: {self.orders_created}")
        print(f"Отменено ордеров: {self.orders_cancelled}")
        print(f"Исполнено сделок: {self.trades_executed}")
        print(f"Активных ордеров: {len(self.active_orders)}")
    def run(self):
        if not self.create_user():
            print("Не удалось создать пользователя")
            return
        self.run_continuous_check()
def main():
    bot = BuyBot()
    bot.run()
if __name__ == "__main__":
    main()
