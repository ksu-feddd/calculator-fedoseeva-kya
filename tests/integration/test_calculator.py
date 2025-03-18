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
        self.assertEqual(stderr, "Недопустимый символ")
        self.assertNotEqual(returncode, 0)

    def test_negative_input_int(self):
        stdout, stderr, returncode = self.run_calculator("-5 + 3")
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Унарный минус не поддерживается")
        self.assertNotEqual(returncode, 0)

    def test_out_of_range_input_int(self):
        stdout, stderr, returncode = self.run_calculator("3000000000 + 1")
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Результат вне диапазона [-2e9, +2e9]")  # Результат превышает предел
        self.assertNotEqual(returncode, 0)

    def test_out_of_range_result_int(self):
        stdout, stderr, returncode = self.run_calculator("2000000000 + 1000000000")
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Результат вне диапазона [-2e9, +2e9]")
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
        self.assertEqual(stderr, "Унарный минус не поддерживается")
        self.assertNotEqual(returncode, 0)

    def test_out_of_range_input_float(self):
        stdout, stderr, returncode = self.run_calculator("2.5e10 + 1", float_mode=True)
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Недопустимый символ")  # 'e' не разрешён
        self.assertNotEqual(returncode, 0)

    def test_out_of_range_result_float(self):
        stdout, stderr, returncode = self.run_calculator("2000000000.5 + 1", float_mode=True)
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Результат вне диапазона [-2e9, +2e9]")
        self.assertNotEqual(returncode, 0)

if __name__ == "__main__":
    unittest.main()
