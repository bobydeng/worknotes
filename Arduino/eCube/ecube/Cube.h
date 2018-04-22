/*************************
 * 
 * Cube logic
 * 
 * 
 *************************/

 #ifndef __CUBE_H__
 #define __CUBE_h__

#include "common.h"

#define ON_TICK_PERIOD 5

typedef void (*pOnTurned) (enum COLOR (&)[6][9]);

struct FaceNeighber {
  enum FACE face;
  enum EDGE edge;
};

class CubeEdge {
  private:
    enum COLOR * raw_face_data;
    int * blk_idxs;
  public:
    CubeEdge(EDGE edge, COLOR* raw_face_data);
    enum COLOR get_color(int idx);
    
    //shift awc, set the first block with input param 'color', return the last one
    enum COLOR push(COLOR color); 

    //shift cw, set the last block with input param 'color', return the first one
    enum COLOR push_cw(COLOR color);
};

class CubeFace {
  private:
    enum COLOR * raw_data;
    CubeEdge* edges[4];
    CubeEdge* nb_edges[4];
  public:
    CubeFace(enum COLOR * raw_data);
    ~CubeFace();
    CubeEdge* get_edge(EDGE edge);
    void set_neighber_edge(EDGE edge, CubeEdge* nb_edge);
    void shift(enum DIR dir);
    void shift_nb(enum DIR dir);
};

class Cube {
  private:
    enum COLOR raw_data[6][9];
    CubeFace* faces[6];
    pOnTurned on_turned;
    int tick; //tick in 10ms

    //on turn action consist of 5 steps, nb--this--nb--this--nb
    int phase;
    enum FACE turning_face;
    enum DIR dir;

    //internal use, done in one step
    void _turn(enum FACE face, enum DIR dir); 
    void copy(int from_face, int from_edge, enum COLOR* edge_buf);
    void copy(enum COLOR* edge_buf, int to_face, int to_edge);
    void copy(int from_face, int from_edge, int to_face, int to_edge);
    void copy(int face, int from_edge, int to_edge);
  public:
    Cube();
    ~Cube();
    void reset();
    void print_faces();
    void turn(enum FACE face, enum DIR dir);
    void on_wipe(enum FACE face, enum EDGE edge, enum DIR dir);
    void on_tick_10ms();
    bool is_turning();
    void set_on_turned_callback(pOnTurned on_turned);
};

 #endif //__CUBE_H__
