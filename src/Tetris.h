#ifndef _Tetris_h
#define _Tetris_h

const int BLOCKS[5][4][4] = {
  {
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },

  {
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },

  {
    {0, 1, 1, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {1, 1, 1, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0},
    {0, 1, 1, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
  },
};

class Tetris {
  public:
    int size_y;
    int size_x;
    int **field;
    int block[4][4];
    int pos_y;
    int pos_x;
    int score;
    int next;
    int next_color;

    Tetris(int y, int x);
    ~Tetris();

    void block_random();
    void block_control(int op);
    bool block_can_move(int ny, int nx);
    void block_rotate();
    void block_left();
    void block_right();
    void block_down();
    void field_remove_fill_line();
    void display();
    void display_block();
};

#endif
