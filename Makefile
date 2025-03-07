CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra
CXXFLAGS = -Wall -Wextra -std=c++17

# Папка для артефактов
BUILD_DIR = build

# Основная цель
all: $(BUILD_DIR)/app.exe $(BUILD_DIR)/unit-tests.exe

# Сборка app.exe
$(BUILD_DIR)/app.exe: src/main.c src/calculator.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/app.exe src/main.c src/calculator.c

# Сборка unit-tests.exe
$(BUILD_DIR)/unit-tests.exe: tests/unit/test_calculator.cpp src/calculator.c
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/unit-tests.exe tests/unit/test_calculator.cpp src/calculator.c -Igoogletest/googletest/include -Lgoogletest/build/lib -lgtest -lgtest_main -pthread

# Очистка
clean:
	rm -rf $(BUILD_DIR)

# Запуск в целочисленном режиме
run-int: $(BUILD_DIR)/app.exe
	./$(BUILD_DIR)/app.exe

# Запуск в режиме с плавающей точкой
run-float: $(BUILD_DIR)/app.exe
	./$(BUILD_DIR)/app.exe --float

# Запуск юнит-тестов
run-unit-test: $(BUILD_DIR)/unit-tests.exe
	./$(BUILD_DIR)/unit-tests.exe

# Создание виртуального окружения
venv:
	python3 -m venv venv
	. venv/bin/activate && pip install --upgrade pip

# Запуск интеграционных тестов
run-integration-tests: $(BUILD_DIR)/app.exe venv
	. venv/bin/activate && python3 tests/integration/test_calculator.py

# Указание фиктивных целей
.PHONY: all clean run-int run-float run-unit-test venv run-integration-tests
