noutputs=16


def main():

    i = 0
    print 'IF (inReady(', i, ')) {'
    print '   in[', i, ' ].read <== true;'
    print '}'
    for i in range(1, noutputs):
        print 'ELSE IF (inReady(', i, ')) {'
        print '   in[', i, '].read <== true;'
        print '}'

    i = 0
    print 'IF (inReady[', i, ']) {'
    print '  consume(', i, ');'
    print '}'
    for i in range(1, noutputs):
        print 'ELSE IF (inReady[', i, ']) {'
        print '   consume(', i, ');'
        print '}'


if __name__ == '__main__':
    main()
