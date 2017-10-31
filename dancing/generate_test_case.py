import sys, random

def generate(board_size, num_dancers, num_colors):
  board = [[0 for i in range(board_size)] for j in range(board_size)]
  for c in range(num_colors):
    print('Dancer positions for color {}'.format(c + 1))
    for d in range(num_dancers):
      x = random.randint(0, board_size - 1)
      y = random.randint(0, board_size - 1)
      while board[y][x] != 0:
        x = random.randint(0, board_size - 1)
        y = random.randint(0, board_size - 1)
      print('{} {}'.format(x, y))

if __name__ == '__main__':
  board_size = int(sys.argv[1])
  num_dancers = int(sys.argv[2])
  num_colors = int(sys.argv[3])
  generate(board_size, num_dancers, num_colors)
