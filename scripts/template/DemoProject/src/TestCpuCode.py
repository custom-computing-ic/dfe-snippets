from Test import Test
from operator import add


def main():

    in1 = range(1, 5)
    in2 = range(5, 9)

    result = Test(in1, in2)

    exp = map(add, in1, in2)

    if result == exp:
        print 'Correct'
    else:
        print 'Incorrect results'


if __name__ == '__main__':
    main()
