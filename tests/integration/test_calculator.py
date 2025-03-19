"""
test_calculator.py - Интеграционные тесты от Ксении Федосеевой
Проверяет работу калькулятора через stdin/stdout в разных режимах.
"""

import subprocess
import unittest

class TestCalculatorIntegration(unittest.TestCase):
    def run_calculator(self, input_str, float_mode=False):
        cmd = ["./build/app.exe"]
        if float_mode:
            cmd.append("--float")
        process = subprocess.run(
            cmd,
            input=input_str,
            text=True,
            capture_output=True
        )
        return process.stdout.strip(), process.stderr.strip(), process.returncode

    # Целочисленный режим
    def test_addition_int(self):
        stdout, stderr, returncode = self.run_calculator("2 + 3")
        self.assertEqual(stdout, "5")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_subtraction_int_positive(self):
        stdout, stderr, returncode = self.run_calculator("5 - 2")
        self.assertEqual(stdout, "3")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_subtraction_int_negative(self):
        stdout, stderr, returncode = self.run_calculator("100 - 101")
        self.assertEqual(stdout, "-1")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_multiplication_int(self):
        stdout, stderr, returncode = self.run_calculator("2000000 * 1000")
        self.assertEqual(stdout, "2000000000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_division_int(self):
        stdout, stderr, returncode = self.run_calculator("2000000000 / 2")
        self.assertEqual(stdout, "1000000000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_invalid_chars_int(self):
        stdout, stderr, returncode = self.run_calculator("2 + a")
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Ошибка: некорректное выражение")
        self.assertNotEqual(returncode, 0)

    def test_negative_input_int(self):
        stdout, stderr, returncode = self.run_calculator("-5 + 3")
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Ошибка: некорректное выражение")
        self.assertNotEqual(returncode, 0)

    def test_out_of_range_input_int(self):
        stdout, stderr, returncode = self.run_calculator("3000000000 + 1")
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Ошибка: некорректное выражение")
        self.assertNotEqual(returncode, 0)

    def test_out_of_range_result_int(self):
        stdout, stderr, returncode = self.run_calculator("2000000000 + 1000000000")
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Ошибка: некорректное выражение")
        self.assertNotEqual(returncode, 0)

    # Добавленные тесты для целочисленного режима с скобками
    def test_parentheses_int(self):
        stdout, stderr, returncode = self.run_calculator("(5 + 5) * 2")
        self.assertEqual(stdout, "20")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_mixed_operations_int(self):
        stdout, stderr, returncode = self.run_calculator("5 + 3 * (2 + 3)")
        self.assertEqual(stdout, "20")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_large_parentheses_int(self):
        stdout, stderr, returncode = self.run_calculator("(1000 + 2000) * (10 + 5)")
        self.assertEqual(stdout, "45000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_unmatched_parentheses_int(self):
        stdout, stderr, returncode = self.run_calculator("(10 + 5")
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Ошибка: некорректное выражение")
        self.assertNotEqual(returncode, 0)

    # Режим с плавающей точкой
    def test_addition_float(self):
        stdout, stderr, returncode = self.run_calculator("2.5 + 3.7", float_mode=True)
        self.assertEqual(stdout, "6.2000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_subtraction_float_positive(self):
        stdout, stderr, returncode = self.run_calculator("5.5 - 2.3", float_mode=True)
        self.assertEqual(stdout, "3.2000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_subtraction_float_negative(self):
        stdout, stderr, returncode = self.run_calculator("100.5 - 101.5", float_mode=True)
        self.assertEqual(stdout, "-1.0000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_multiplication_float(self):
        stdout, stderr, returncode = self.run_calculator("2000000.0 * 1000.0", float_mode=True)
        self.assertEqual(stdout, "2000000000.0000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_division_float(self):
        stdout, stderr, returncode = self.run_calculator("2000000000.0 / 2.0", float_mode=True)
        self.assertEqual(stdout, "1000000000.0000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_negative_input_float(self):
        stdout, stderr, returncode = self.run_calculator("-2.5 + 3", float_mode=True)
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Ошибка: некорректное выражение")
        self.assertNotEqual(returncode, 0)

    def test_out_of_range_input_float(self):
        stdout, stderr, returncode = self.run_calculator("3000000000.0 + 1.0", float_mode=True)
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Ошибка: некорректное выражение")
        self.assertNotEqual(returncode, 0)

    def test_out_of_range_result_float(self):
        stdout, stderr, returncode = self.run_calculator("2000000000.0 + 1000000000.0", float_mode=True)
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Ошибка: некорректное выражение")
        self.assertNotEqual(returncode, 0)

    # Добавленные тесты для float-режима с скобками
    def test_parentheses_float(self):
        stdout, stderr, returncode = self.run_calculator("(5.0 + 5.0) * 2.0 / 5.0", float_mode=True)
        self.assertEqual(stdout, "4.0000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_mixed_operations_float(self):
        stdout, stderr, returncode = self.run_calculator("5.0 + 3.0 * (2.0 + 3.0)", float_mode=True)
        self.assertEqual(stdout, "20.0000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_large_parentheses_float(self):
        stdout, stderr, returncode = self.run_calculator("(1000.0 + 2000.0) * (10.0 + 5.0)", float_mode=True)
        self.assertEqual(stdout, "45000.0000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_unmatched_parentheses_float(self):
        stdout, stderr, returncode = self.run_calculator("(10.0 + 5.0", float_mode=True)
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Ошибка: некорректное выражение")
        self.assertNotEqual(returncode, 0)

if __name__ == "__main__":
    unittest.main()
