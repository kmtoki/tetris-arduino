//#include <unistd.h>
//#include <iostream>
#include <Arduino.h>
#include <Tetris.h>

Tetris::Tetris(int y, int x) {
  size_y = y;
  size_x = x;
  pos_y = 0;
  pos_x = 0;
  next = random(0, (sizeof(BLOCKS) / sizeof(BLOCKS[0])));
  next_color = random(1, 9);

  field = new int*[y];
  for (int yy = 0; yy < y; yy++) {
    field[yy] = new int[x];
    for (int xx = 0; xx < x; xx++) {
      field[yy][xx] = 0;
    }
  }

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      block[y][x] = 0;
    }
  }
}

Tetris::~Tetris() {
  for (int y = 0; y < size_y; y++) {
    delete field[y];
  }
  delete field;
}

void Tetris::block_random() {
  int index = next;
  int color = next_color;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (BLOCKS[index][y][x] != 0) {
        block[y][x] = color;
      }
      else {
        block[y][x] = 0;
      }
    }
  }

  next = random(0, (sizeof(BLOCKS) / sizeof(BLOCKS[0])));
  next_color = random(1, 9);
}

void Tetris::block_control(int op) {
  switch (op) {
    case 1:
      block_rotate();
      break;
    case 2:
      block_left();
      break;
    case 3:
      block_right();
      break;
    case 4:
      block_down();
      break;
  }
}

bool Tetris::block_can_move(int ny, int nx) {
  for (int y = 0; y < 4; y++) {
    int yy = ny + y;
    for (int x = 0; x < 4; x++) {
      int xx = nx + x;
      if (block[y][x] != 0) {
        if (!(0 <= yy && yy < size_y && 0 <= xx && xx < size_x)) {
          return false;
        }

        if (field[yy][xx] != 0) {
          return false;
        }
      }
    }
  }
  return true;
}

void Tetris::block_rotate() {
  if (pos_x >= size_x - 4 && pos_x <= size_x - 1) {
    pos_x = size_x - 4;
  }

  int prev[4][4];
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      prev[y][x] = block[y][x];
    }
  }

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      block[y][x] = prev[x][(-y) + 3];
    }
  }

  if (!block_can_move(pos_y, pos_x)) {
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        block[y][x] = prev[y][x];
      }
    }
  }
}

void Tetris::block_left() {
  if (block_can_move(pos_y, pos_x - 1)) {
    pos_x -= 1;
  }
}

void Tetris::block_right() {
  if (block_can_move(pos_y, pos_x + 1)) {
    pos_x += 1;
  }
}

void Tetris::block_down() {
  score += 1;
  if (block_can_move(pos_y + 1, pos_x)) {
    pos_y += 1;
  }
  else {
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        int yy = pos_y + y;
        int xx = pos_x + x;
        if (0 <= yy && yy < size_y && 0 <= xx && xx < size_x && block[y][x] != 0) {
          field[yy][xx] = block[y][x];
        }
      }
    }

    pos_y = 0;
    pos_x = 0;
    block_random();
  }
}

void Tetris::field_remove_fill_line() {
  for (int y = size_y - 1; y >= 0; y--) {
    bool fill = true;
    for (int x = 0; x < size_x; x++) {
      fill = fill && (field[y][x] != 0);
      if (!fill) {
        break;
      }
    }

    if (fill) {
      for (int yy = y; yy > 0; yy--) {
        for (int x = 0; x < size_x; x++) {
          if (yy != 0) {
            field[yy][x] = field[yy - 1][x];
          }
          else {
            field[yy][x] = 0;
          }
        }
      }

      score *= 1.2;
    }
  }
}

//void Tetris::display() {
//  int s[size_y][size_x];
//  for (int y = 0; y < size_y; y++) {
//    for (int x = 0; x < size_x; x++) {
//      s[y][x] = field[y][x];
//    }
//  }

//  for (int y = 0; y < 4; y++) {
//    for (int x = 0; x < 4; x++) {
//      int yy = pos_y + y;
//      int xx = pos_x + x;
//      if (0 <= yy && yy < size_y && 0 <= xx && xx < size_x && block[y][x] != 0) {
//        s[yy][xx] = block[y][x];
//      }
//    }
//  }

//  for (int y = 0; y < size_y; y++) {
//    for (int x = 0; x < size_x; x++) {
//      std::cout << s[y][x];
//    }
//    std::cout << std::endl;
//  }
//}

//void Tetris::display_block() {
//  std::cout << pos_y << " " << pos_x << std::endl;
//  for (int y = 0; y < 4; y++) {
//    for (int x = 0; x < 4; x++) {
//      std::cout << block[y][x];
//    }
//    std::cout << std::endl;
//  }
//}

//int main() {
//  Tetris t = Tetris(20, 10);
//  t.block_random();
//  while (true) {
//    char op = 0;
//    std::cin >> op;
//    t.block_control(op - '0');
//    t.display();
//    std::cout << std::endl;
//    t.field_remove_fill_line();
//  }
//}
