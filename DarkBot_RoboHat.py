import serial
import time

class DarkBotRoboHat:
    def __init__(self, port='/dev/ttyS0', baud_rate=1000000):
        self.ser = serial.Serial(port, baud_rate, timeout=1)
        time.sleep(2)  # Wait for the serial connection to initialize

    def send_command(self, command):
        """Send a command to the Teensy over the serial port."""
        self.ser.write((command + '\n').encode())
        time.sleep(0.1)  # Small delay to ensure the command is sent
        response = self.ser.readline().decode().strip()
        return response

    def set_pin_mode(self, pin, state):
        """
        Set the pin mode on the Teensy.
        state: 0 for Digital Output, 1 for Digital Input, 2 for ADC Input
        """
        command = f"pinMode({pin},{state})"
        return self.send_command(command)

    def set_servo_position(self, index, position):
        """
        Set the position of a servo on the Teensy.
        position: Angle between 0 and 180
        """
        command = f"Servo({index},{position})"
        return self.send_command(command)

    def invert_servo(self, index):
        """Invert the direction of a servo on the Teensy."""
        command = f"ServoInvert({index})"
        return self.send_command(command)

    def set_servo_soft_limit_min(self, index, limit):
        """Set the minimum soft limit for a servo on the Teensy."""
        command = f"ServoSoftLimitMin({index},{limit})"
        return self.send_command(command)

    def set_servo_soft_limit_max(self, index, limit):
        """Set the maximum soft limit for a servo on the Teensy."""
        command = f"ServoSoftLimitMax({index},{limit})"
        return self.send_command(command)

    def set_motor_speed(self, index, speed):
        """
        Set the speed of a motor on the Teensy.
        speed: Positive for forward, negative for reverse
        """
        command = f"Motor({index},{speed})"
        return self.send_command(command)

    def invert_motor_direction(self, index):
        """Invert the direction of a motor on the Teensy."""
        command = f"MotorSetInverted({index})"
        return self.send_command(command)
