from main import axis_to_velocity, velocity_command


def test_axis_to_velocity() -> None:
    assert axis_to_velocity(0.05, 0.1) == 0
    assert axis_to_velocity(0.5, 0.1) == 500
    assert axis_to_velocity(-1.0, 0.1) == -1000
    assert axis_to_velocity(0.5, 0.1, invert=True) == -500
    assert axis_to_velocity(2.0, 0.1) == 1000


def test_velocity_command() -> None:
    assert velocity_command(-500) == b"V -500\n"
    assert velocity_command(0) == b"V 0\n"

    try:
        velocity_command(1001)
    except ValueError:
        pass
    else:
        raise AssertionError("out-of-range velocity must fail")


if __name__ == "__main__":
    test_axis_to_velocity()
    test_velocity_command()
