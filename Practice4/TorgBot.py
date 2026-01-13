import requests
import json
import time
import random
import threading
import sys
from datetime import datetime
from collections import deque

class TorgBot:
    def __init__(self, base_url="http://localhost:8080", bot_id=0, username=None):
        self.base_url = base_url
        self.bot_id = bot_id 
        self.username = username 
        self.user_key = None 
        self.user_id = None 
        self.pairs = []  
        self.lots = {}   
        self.market_prices = {}  
        self.price_history = {} 
        self.last_execution_prices = {}  # Последние цены исполнения {pair_id: цена}
        self.is_running = False
        self.order_counter = 0
        self.cancel_counter = 0  
        self.executed_orders = 0
        self.errors_count = 0 
        self.active_orders = []  
        self.cancel_probability = 0.1  # Вероятность отмены ордера
        self.last_order_time = 0  # Время последнего созданного ордера
        self.order_interval = 1.0  # Интервал между ордерами 
    def create_user(self):
        if self.username is None:
            self.username = f"TorgBot_{self.bot_id}_{random.randint(1000, 9999)}"
        print(f"Бот {self.bot_id:2d}: Создание пользователя: {self.username}")
        data = {
            "username": self.username
        }
        try:
            response = requests.post(f"{self.base_url}/user", json=data, timeout=5)
            if response.status_code == 200:
                result = response.json()
                if "key" in result:
                    self.user_key = result["key"]
                    if "user_id" in result:
                        self.user_id = result["user_id"]
                    print(f"Бот {self.bot_id:2d} Пользователь создан")
                    return True
                elif "error" in result:
                    print(f"Бот {self.bot_id:2d} Ошибка: {result['error'][:50]}")
                    if "existing_user_id" in result:
                        self.username = f"trader_{self.bot_id}_{random.randint(10000, 99999)}"
                        return self.create_user()
            else:
                print(f"Бот {self.bot_id:2d}: HTTP ошибка: {response.status_code}")
        except requests.exceptions.ConnectionError:
            print(f"Бот {self.bot_id:2d}: Не удалось подключиться к API")
        except requests.exceptions.Timeout:
            print(f"Бот {self.bot_id:2d}: Таймаут подключения")
        except Exception as e:
            print(f"Бот {self.bot_id:2d}: Ошибка: {str(e)[:50]}")
        return False
    def get_user_orders(self):
        if not self.user_key:
            return []
        try:
            response = requests.get(f"{self.base_url}/order", timeout=5)
            if response.status_code == 200:
                response_data = response.json()
                orders = []
                if isinstance(response_data, dict):
                    if "orders" in response_data:
                        orders = response_data["orders"]
                    elif "data" in response_data:
                        orders = response_data["data"]
                    else:
                        orders = response_data
                else:
                    orders = response_data  
                user_orders = []
                for order in orders:
                    if isinstance(order, dict):
                        user_id = order.get("user_id")
                        closed = order.get("closed", "")
                        # Проверяем, что ордер наш и не закрыт
                        if (user_id == self.user_id or 
                            (self.user_id and str(user_id) == str(self.user_id))) and not closed:
                            user_orders.append({
                                "order_id": order.get("order_id"),
                                "type": order.get("type"),
                                "pair_id": order.get("lot_id") or order.get("pair_id"),
                                "quantity": order.get("quantity"),
                                "price": order.get("price")
                            })
                return user_orders
        except Exception as e:
            print(f"Бот {self.bot_id:2d}: Ошибка получения ордеров: {str(e)[:50]}")
        return []
    def cancel_random_order(self):
        if not self.user_key or not self.active_orders:
            return False
        # Выбираем случайный ордер для отмены
        order_to_cancel = random.choice(self.active_orders)
        order_id = order_to_cancel.get("order_id")
        if not order_id:
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
                    self.cancel_counter += 1
                    print(f"Бот {self.bot_id:2d}: Ордер {order_id} отменен")
                    # Удаляем из списка активных
                    self.active_orders = [o for o in self.active_orders if o.get("order_id") != order_id]
                    return True
                else:
                    error_msg = result.get("error", "Неизвестная ошибка")
                    print(f"Бот {self.bot_id:2d}: Ошибка отмены ордера {order_id}: {error_msg}")
            else:
                print(f"Бот {self.bot_id:2d}: HTTP ошибка {response.status_code} при отмене ордера")
        except Exception as e:
            print(f"Бот {self.bot_id:2d}: Ошибка при отмене ордера: {str(e)[:50]}")
        return False
    def load_market_data(self):
        print(f"Бот {self.bot_id:2d}: Загрузка рыночных данных...")
        try:
            # Загружаем пары
            response = requests.get(f"{self.base_url}/pair", timeout=5)
            if response.status_code == 200:
                pairs_data = response.json()
                # Проверяем структуру ответа
                if isinstance(pairs_data, dict):
                    if "pairs" in pairs_data:
                        self.pairs = pairs_data["pairs"]
                    elif "data" in pairs_data:
                        self.pairs = pairs_data["data"]
                    else:
                        self.pairs = pairs_data
                else:
                    self.pairs = pairs_data   
                print(f"Бот {self.bot_id:2d}: Загружено {len(self.pairs)} торговых пар")
                # Загружаем лоты
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
                    print(f"Бот {self.bot_id:2d}: Загружено {len(self.lots)} лотов")
                    # Инициализируем цены
                    self.initialize_market_prices()
                else:
                    print(f"Бот {self.bot_id:2d}: Ошибка загрузки лотов")
            else:
                print(f"Бот {self.bot_id:2d}: Ошибка загрузки пар")
        except Exception as e:
            print(f"Бот {self.bot_id:2d}: Ошибка загрузки данных: {str(e)[:50]}")
    def initialize_market_prices(self):
        for pair in self.pairs:
            if isinstance(pair, dict):
                pair_id = pair.get("pair_id") or pair.get("id")
                if pair_id:
                    # Начальная случайная цена между 0.5 и 1.5
                    base_price = random.uniform(0.5, 1.5)
                    self.market_prices[pair_id] = base_price
                    self.last_execution_prices[pair_id] = base_price
                    self.price_history[pair_id] = deque([base_price], maxlen=50)
    def get_recent_executions(self):
        try:
            response = requests.get(f"{self.base_url}/order", timeout=5)
            if response.status_code == 200:
                response_data = response.json()
                # Нормализуем структуру ответа
                orders = []
                if isinstance(response_data, dict):
                    if "orders" in response_data:
                        orders = response_data["orders"]
                    elif "data" in response_data:
                        orders = response_data["data"]
                    else:
                        orders = response_data
                else:
                    orders = response_data
                if not isinstance(orders, list):
                    orders = []
                # Ищем исполненные ордера
                executed_orders = []
                for order in orders:
                    if isinstance(order, dict):
                        # Проверяем, что ордер исполнен
                        closed = order.get("closed")
                        price = order.get("price")
                        if closed and price and "(отменен" not in str(closed):
                            executed_orders.append(order)
                # Обновляем цены
                for order in executed_orders[:5]:
                    pair_id = order.get("lot_id") or order.get("pair_id")
                    if pair_id:
                        try:
                            execution_price = float(order["price"])
                            self.last_execution_prices[pair_id] = execution_price
                            if pair_id in self.price_history:
                                self.price_history[pair_id].append(execution_price)
                                recent = list(self.price_history[pair_id])[-5:]
                                if recent:
                                    self.market_prices[pair_id] = sum(recent) / len(recent)
                            else:
                                self.price_history[pair_id] = deque([execution_price], maxlen=50)
                                self.market_prices[pair_id] = execution_price
                        except:
                            pass
        except Exception:
            pass  # Игнорируем ошибки при получении истории
    def simulate_market_movement(self):
        self.get_recent_executions()  # Пытаемся получить реальные данные
        for pair_id in list(self.market_prices.keys()):
            current_price = self.market_prices[pair_id]
            last_execution = self.last_execution_prices.get(pair_id, current_price)
            if last_execution != current_price:
                drift_factor = 0.3
                target_price = last_execution
            else:
                drift_factor = 0.1
                target_price = current_price * random.uniform(0.995, 1.005)
            new_price = current_price + drift_factor * (target_price - current_price)
            volatility = random.uniform(-0.002, 0.002)
            new_price = new_price * (1 + volatility)
            new_price = max(new_price, 0.01)
            self.market_prices[pair_id] = new_price
            if pair_id in self.price_history:
                self.price_history[pair_id].append(new_price)
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
            print(f"Бот {self.bot_id:2d}: Нет данных о балансе")
            return
        print(f"Бот {self.bot_id:2d}: виртуальный кошелек (баланс)")
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
    def create_market_based_order(self):
        if not self.pairs:
            return False
        # Проверяем время с последнего ордера
        current_time = time.time()
        time_since_last_order = current_time - self.last_order_time
        if time_since_last_order < self.order_interval:
            # Ждем, пока не пройдет достаточно времени
            sleep_time = self.order_interval - time_since_last_order
            if sleep_time > 0:
                time.sleep(sleep_time)
        # Выбираем случайную пару
        pair = random.choice(self.pairs)
        if not isinstance(pair, dict):
            return False
        pair_id = pair.get("pair_id") or pair.get("id")
        if not pair_id:
            return False
        # Получаем текущую рыночную цену
        market_price = self.market_prices.get(pair_id, random.uniform(0.5, 1.5))
        # Случайный тип ордера и цена
        order_type = random.choice(["buy", "sell"])
        if order_type == "buy":
            order_price = market_price * random.uniform(1.0, 1.02)
        else:
            order_price = market_price * random.uniform(0.98, 1.0)
        # Случайное количество
        quantity = random.uniform(0.1, 2.0)
        # Получаем названия лотов
        sale_lot_id = pair.get("sale_lot_id")
        buy_lot_id = pair.get("buy_lot_id")
        sale_lot_name = self.lots.get(sale_lot_id, "?") if sale_lot_id else "?"
        buy_lot_name = self.lots.get(buy_lot_id, "?") if buy_lot_id else "?"
        # Отправляем ордер на биржу
        success = self.send_order(pair_id, quantity, order_price, order_type)
        if success:
            self.order_counter += 1
            self.last_order_time = time.time()  # Обновляем время последнего ордера
            print(f"Бот {self.bot_id:2d}: Ордер: {order_type.upper()} {sale_lot_name}/{buy_lot_name} "f"price: {order_price:.3f} quantity: {quantity:.2f}")
            return True
        else:
            self.errors_count += 1
            return False
    def send_order(self, pair_id, quantity, price, order_type):
        data = {
            "pair_id": pair_id,
            "quantity": quantity,
            "price": price,
            "type": order_type
        }
        headers = {
            "X-USER-KEY": self.user_key,
            "Content-Type": "application/json"
        }
        max_retries = 2
        for attempt in range(max_retries):
            try:
                # УВЕЛИЧИВАЕМ таймаут до 10 секунд
                response = requests.post(f"{self.base_url}/order", json=data, headers=headers, timeout=10)
                if response.status_code == 200:
                    result = response.json()
                    if result.get("status") == "accepted":
                        request_id = result.get("request_id", "?")
                        print(f"Ордер принят")
                        # Добавляем ID созданного ордера в активные
                        if "order_ids" in result and result["order_ids"]:
                            order_id = result["order_ids"][0]
                            self.active_orders.append({
                                "order_id": order_id,
                                "type": order_type,
                                "pair_id": pair_id,
                                "quantity": quantity,
                                "price": price
                            })
                        return True
                    elif result.get("success"):
                        # Альтернативный формат ответа
                        if "order_id" in result:
                            self.active_orders.append({
                                "order_id": result["order_id"],
                                "type": order_type,
                                "pair_id": pair_id,
                                "quantity": quantity,
                                "price": price
                            })
                        return True
                    elif "error" in result:
                        error_msg = str(result['error'])[:50]
                        if "недостаточно средств" in error_msg.lower():
                            print(f"Недостаточно средств")
                        else:
                            print(f"Ошибка: {error_msg}")
                        return False   
                else:
                    print(f"HTTP ошибка: {response.status_code}") 
            except requests.exceptions.Timeout:
                print(f"Таймаут (попытка {attempt+1}/{max_retries})")
                if attempt < max_retries - 1:
                    time.sleep(0.5)
                continue
            except requests.exceptions.ConnectionError:
                print(f"Нет связи с сервером (попытка {attempt+1}/{max_retries})")
                if attempt < max_retries - 1:
                    time.sleep(1)
                continue
            except Exception as e:
                error_msg = str(e)[:50]
                print(f"Ошибка: {error_msg}")
                break
        return False
    def trading_cycle(self):
        # Обновляем рыночные цены
        self.simulate_market_movement()
        # Обновляем список активных ордеров
        if random.random() < 0.3:  # 30% шанс обновить список
            self.active_orders = self.get_user_orders()
        # Решаем: отменять ли ордер
        if self.active_orders and random.random() < self.cancel_probability:
            self.cancel_random_order()
        self.create_market_based_order()
        # Периодически выводим баланс
        if self.order_counter > 0 and self.order_counter % 10 == 0:
            self.print_balance_summary()
    def start_trading(self, interval=1.0):        
        # Создаем пользователя
        if not self.create_user():
            print(f"Бот {self.bot_id:2d}: Не удалось создать пользователя")
            return False
        # Загружаем рыночные данные
        self.load_market_data()
        if not self.pairs:
            print(f"Бот {self.bot_id:2d}: Нет доступных торговых пар")
            return False
        self.is_running = True
        self.start_time = time.time()
        self.last_order_time = time.time()  # Инициализируем время последнего ордера
        try:
            while self.is_running:
                cycle_start = time.time()
                # Выполняем торговый цикл
                self.trading_cycle()
                # Рассчитываем время до следующего цикла
                cycle_time = time.time() - cycle_start
                sleep_time = max(0, interval - cycle_time)
                # Выводим статистику каждые 30 секунд
                elapsed = time.time() - self.start_time
                if elapsed > 0 and int(elapsed) % 30 == 0:
                    print(f"Бот {self.bot_id:2d}: Ордеров: {self.order_counter}, "
                          f"Отменено: {self.cancel_counter}, "
                          f"Ошибок: {self.errors_count}, "
                          f"Активных: {len(self.active_orders)}, "
                          f"Время: {int(elapsed)}с")
                if sleep_time > 0:
                    time.sleep(sleep_time)     
        except KeyboardInterrupt:
            pass
        except Exception as e:
            print(f"Бот {self.bot_id:2d}: {str(e)[:50]}")
        finally:
            self.stop_trading()
        return True
    def stop_trading(self):
        self.is_running = False
        elapsed = time.time() - self.start_time if hasattr(self, 'start_time') else 0
        print(f"Бот {self.bot_id:2d}: Остановлен. "f"Ордеров: {self.order_counter}, "f"Отменено: {self.cancel_counter}, "f"Ошибок: {self.errors_count}, "f"Время: {int(elapsed)}с")
class BotManager:
    def __init__(self, base_url="http://localhost:8080", num_bots=10, interval=1.0):
        self.base_url = base_url
        self.num_bots = num_bots
        self.interval = interval
        self.bots = []
        self.threads = []
        self.is_running = False
        self.start_time = None
    def start_all_bots(self):
        self.is_running = True
        self.start_time = time.time()
        # Создаем и запускаем ботов
        for i in range(self.num_bots):
            bot = TorgBot(base_url=self.base_url, bot_id=i+1)
            self.bots.append(bot)
            # Создаем поток для каждого бота
            thread = threading.Thread(
                target=self.run_bot_in_thread,
                args=(bot,),
                daemon=True
            )
            self.threads.append(thread)
            thread.start()
            # Небольшая задержка между запуском ботов
            time.sleep(0.3)
        print(f"\nНа нас работают {len(self.bots)} ботов")
        try:
            last_stats_time = time.time()
            while self.is_running:
                time.sleep(1)
                # Выводим статистику каждые 15 секунд
                current_time = time.time()
                if current_time - last_stats_time >= 15:
                    self.print_overall_statistics()
                    last_stats_time = current_time
        except KeyboardInterrupt:
            print("\nОСТАНОВИСЬ, ВРЕМЯ!")
        finally:
            self.stop_all_bots()
    def run_bot_in_thread(self, bot):
        try:
            bot.start_trading(interval=self.interval)
        except Exception as e:
            print(f"Ошибка в боте {bot.bot_id}: {str(e)[:50]}")
    def print_overall_statistics(self):
        if not self.bots:
            return
        total_orders = sum(bot.order_counter for bot in self.bots)
        total_cancels = sum(bot.cancel_counter for bot in self.bots)
        total_errors = sum(bot.errors_count for bot in self.bots)
        active_bots = sum(1 for bot in self.bots if bot.is_running)
        total_active_orders = sum(len(bot.active_orders) for bot in self.bots)
        elapsed = time.time() - self.start_time
        print("СТАТИСТИКА:")
        print(f"Активных ботов: {active_bots}/{len(self.bots)}")
        print(f"Всего ордеров: {total_orders}")
        print(f"Всего отмен: {total_cancels}")
        print(f"Всего ошибок: {total_errors}")
        print(f"Активных ордеров: {total_active_orders}")
        print(f"Общее время: {int(elapsed)} секунд")
        if elapsed > 0:
            orders_per_sec = total_orders / elapsed
            cancels_per_sec = total_cancels / elapsed
            print(f"Скорость создания: {orders_per_sec:.2f} ордеров/сек")
            print(f"Скорость отмены: {cancels_per_sec:.2f} отмен/сек")
        print("\nСтатистика по ботам:")
        GREEN = "\033[92m"  # Зеленый
        RED = "\033[91m"    # Красный
        RESET = "\033[0m"   # Сброс цвета
        for i, bot in enumerate(self.bots, 1):
            if bot.is_running:
                status = f"{GREEN}Активный{RESET}"
            else:
                status = f"{RED}Выключенный{RESET}"
            print(f"  {status} Бот {bot.bot_id:2d}: {bot.order_counter:4d} ордеров, "
                  f"{bot.cancel_counter:3d} отмен, {bot.errors_count:3d} ошибок, "
                  f"{len(bot.active_orders):2d} активных")
    def stop_all_bots(self):
        self.is_running = False
        for bot in self.bots:
            bot.stop_trading()
        for thread in self.threads:
            thread.join(timeout=2)
        self.print_overall_statistics()

def main():
    import argparse
    parser = argparse.ArgumentParser(description='Торговые РОБОТники')
    parser.add_argument('--url', default='http://localhost:8080',help='URL API биржи (по умолчанию: http://localhost:8080)')
    parser.add_argument('--interval', type=float, default=1.0,help='Интервал между циклами в секундах (по умолчанию: 1.0)')
    parser.add_argument('--bots', type=int, default=10, help='Количество ботов')
    parser.add_argument('--cancel-chance', type=float, default=0.1,help='Вероятность отмены ордера (0.0-1.0, по умолчанию: 0.1)')
    parser.add_argument('--order-interval', type=float, default=1.0,help='Минимальный интервал между созданием ордеров в секундах (по умолчанию: 1.0)')
    args = parser.parse_args()
    # Создаем и запускаем менеджер ботов
    manager = BotManager(
        base_url=args.url, 
        num_bots=args.bots, 
        interval=args.interval
    )
    # Настраиваем параметры для ботов
    for bot in manager.bots:
        bot.cancel_probability = args.cancel_chance
        bot.order_interval = args.order_interval
    manager.start_all_bots()
if __name__ == "__main__":
    main()
