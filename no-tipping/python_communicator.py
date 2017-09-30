import os

read_fd, write_fd = map(int, os.sys.argv[1:3])

def write(msg):
    os.write(write_fd, msg + "\x00");

def read():
    msg = ''
    while 1:
        buf = os.read(read_fd, 1)
        msg += buf
        if buf == '\x00' or not buf:
            break

    return msg
