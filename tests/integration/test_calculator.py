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
        stdout, stderr, returncode = self.run_calculator("5 - 7")
        self.assertEqual(stdout, "-2")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_multiplication_int(self):
        stdout, stderr, returncode = self.run_calculator("4 * 3")
        self.assertEqual(stdout, "12")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_division_int(self):
        stdout, stderr, returncode = self.run_calculator("10 / 2")
        self.assertEqual(stdout, "5")
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
        self.assertEqual(stderr, "Отрицательные числа во входных данных не поддерживаются")
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
        stdout, stderr, returncode = self.run_calculator("2.5 - 3.7", float_mode=True)
        self.assertEqual(stdout, "-1.2000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_multiplication_float(self):
        stdout, stderr, returncode = self.run_calculator("2.5 * 3.0", float_mode=True)
        self.assertEqual(stdout, "7.5000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_division_float(self):
        stdout, stderr, returncode = self.run_calculator("10.0 / 4.0", float_mode=True)
        self.assertEqual(stdout, "2.5000")
        self.assertEqual(stderr, "")
        self.assertEqual(returncode, 0)

    def test_invalid_chars_float(self):
        stdout, stderr, returncode = self.run_calculator("2.5 + a", float_mode=True)
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Недопустимый символ")
        self.assertNotEqual(returncode, 0)

    def test_negative_input_float(self):
        stdout, stderr, returncode = self.run_calculator("-2.5 + 3.7", float_mode=True)
        self.assertEqual(stdout, "")
        self.assertEqual(stderr, "Отрицательные числа во входных данных не поддерживаются")
        self.assertNotEqual(returncode, 0)

if __name__ == "__main__":
    unittest.main()
