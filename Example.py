from DarkBot_RoboHat import DarkBotRoboHat

def main():
    # Initialize the DarkBotRoboHat library
    robo_hat = DarkBotRoboHat(port='/dev/ttyS0', baud_rate=1000000)

    # Set pin mode
    print("Setting pin mode...")
    response = robo_hat.set_pin_mode(13, 0)  # Set pin 13 as digital output
    print(response)

    # Set servo position
    print("Setting servo position...")
    response = robo_hat.set_servo_position(0, 90)  # Set servo 0 to 90 degrees
    print(response)

    # Invert servo direction
    print("Inverting servo direction...")
    response = robo_hat.invert_servo(0)  # Invert direction of servo 0
    print(response)

    # Set servo soft limits
    print("Setting servo soft limits...")
    response = robo_hat.set_servo_soft_limit_min(0, 30)  # Set minimum limit for servo 0
    print(response)
    response = robo_hat.set_servo_soft_limit_max(0, 150)  # Set maximum limit for servo 0
    print(response)

    # Set motor speed
    print("Setting motor speed...")
    response = robo_hat.set_motor_speed(0, 255)  # Set motor 0 speed to maximum forward
    print(response)

    # Invert motor direction
    print("Inverting motor direction...")
    response = robo_hat.invert_motor_direction(0)  # Invert direction of motor 0
    print(response)

if __name__ == "__main__":
    main()
