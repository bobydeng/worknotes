#include "Cube.h"
#include <Arduino.h>

/**
 *     face layout
 *         -------
 *        |       |
 *        |  TOP  |
 *        |       |
 *  -----------------------------
 * |      |       |       |      |
 * | LEFT | FRONT | RIGHT | BACK |
 * |      |       |       |      |
 *  -----------------------------
 *        |       |
 *        |BOTTOM |
 *        |       |
 *         -------
 *         
 *  pixel index, same for all faces in orientation specified above
 *   _____
 *  |1|4|7|
 *   -----
 *  |2|5|8|
 *  ------
 *  |3|6|9|  
 *   -----
 *   
 *   
 */

// index starts with zero, edge defined with block index in anti-clockwise direction
int edge_block_idxs[][3] = { 
  { 0, 1, 2}, //left edge
  { 2, 5, 8}, //bottom edge
  { 8, 7, 6}, //right edge
  { 6, 3, 0}, //top edge
};

// index starts with zero, edge defined with block index in anti-clockwise direction
int face_border_block_idxs[] = { 0, 1, 2, 5, 8, 7, 6, 3};

//NB of FRONT face
FaceNeighber nb_front[4] = { 
  {LEFT, E_RIGHT}, 
  {DOWN, E_TOP}, 
  {RIGHT, E_LEFT}, 
  {UP, E_BOTTOM} };

//NB of RIGHT face
FaceNeighber nb_right[4] = {   
  {FRONT, E_RIGHT}, 
  {DOWN, E_RIGHT}, 
  {BACK, E_LEFT}, 
  {UP, E_RIGHT} };

//NB of UP face
FaceNeighber nb_up[4] = {    
  {LEFT, E_TOP}, 
  {FRONT, E_TOP}, 
  {RIGHT, E_TOP}, 
  {BACK, E_TOP} };

//NB of BACK face
FaceNeighber nb_back[4] = {      
  {RIGHT, E_RIGHT}, 
  {DOWN, E_BOTTOM}, 
  {LEFT, E_LEFT}, 
  {UP, E_TOP} };
  
//NB of DOWN face
FaceNeighber nb_down[4] = {      
  {LEFT, E_BOTTOM}, 
  {BACK, E_BOTTOM}, 
  {RIGHT, E_BOTTOM}, 
  {FRONT, E_BOTTOM} };
  
//NB of LEFT face
FaceNeighber nb_left[4] = {      
  {BACK, E_RIGHT}, 
  {DOWN, E_LEFT}, 
  {FRONT, E_LEFT}, 
  {UP, E_LEFT} };

FaceNeighber* face_neighbers(enum FACE face) {
  switch (face) {
    case FRONT:
      return nb_front;
    case BACK:
      return nb_back;
    case LEFT:
      return nb_left;
    case RIGHT:
      return nb_right;
    case UP:
      return nb_up;
    case DOWN:
      return nb_down;
    default:
      return 0;
  }
}

CubeEdge::CubeEdge(EDGE edge, COLOR* raw_face_data) {
  this->raw_face_data = raw_face_data;
  this->blk_idxs = edge_block_idxs[edge];
}

enum COLOR CubeEdge::get_color(int idx) {
  return raw_face_data[blk_idxs[idx]];
}

//shift awc, set the first block with input param 'color', return the last one
enum COLOR CubeEdge::push(COLOR color){
  COLOR ret = raw_face_data[blk_idxs[2]];
  raw_face_data[blk_idxs[2]] = raw_face_data[blk_idxs[1]];
  raw_face_data[blk_idxs[1]] = raw_face_data[blk_idxs[0]];
  raw_face_data[blk_idxs[0]] = color;
  return ret;
}

//shift cw, set the last block with input param 'color', return the first one
enum COLOR CubeEdge::push_cw(COLOR color){
  COLOR ret = raw_face_data[blk_idxs[0]];
  raw_face_data[blk_idxs[0]] = raw_face_data[blk_idxs[1]];
  raw_face_data[blk_idxs[1]] = raw_face_data[blk_idxs[2]];
  raw_face_data[blk_idxs[2]] = color;
  return ret;
}

CubeFace::CubeFace(enum COLOR * raw_data) {
    this->raw_data = raw_data;
    for(int i=0;i<4;i++) {
      edges[i] = new CubeEdge(i, raw_data);
    }
}

CubeFace::~CubeFace() {
  for(int i=0;i<4;i++) {
    delete edges[i];
  }
}

CubeEdge* CubeFace::get_edge(EDGE edge){
  return edges[edge];
}

void CubeFace::set_neighber_edge(EDGE edge, CubeEdge* nb_edge){
  nb_edges[edge] = nb_edge;
}

void CubeFace::shift(enum DIR dir){
  COLOR tmp;
  int to_idx,from_idx, inc;
  if(dir == ACW) {
    to_idx = 7;
    inc = -1;
  } else {
    to_idx = 0;
    inc = 1;
  }
  tmp = raw_data[face_border_block_idxs[to_idx]];
  for(int i=0;i<7; i++ ){
    from_idx = to_idx + inc;
    raw_data[face_border_block_idxs[to_idx]] = raw_data[face_border_block_idxs[from_idx]];
    to_idx = from_idx;
  }
  raw_data[face_border_block_idxs[to_idx]] = tmp;
}

void CubeFace::shift_nb(enum DIR dir){
  COLOR tmp;
  if(dir == CW) {
    tmp = nb_edges[0]->get_color(3);
    for(int i=3;i>=0;i--) {
      tmp = nb_edges[i]->push(tmp); //neighber shifts in ops dir
    }
  } else {
    tmp = nb_edges[3]->get_color(0);
    for(int i=0;i<4;i++) {
      tmp = nb_edges[i]->push_cw(tmp); //neighber shifts in ops dir
    }
  }
}

Cube::Cube() {
  for(int i=0;i<6;i++) {
    faces[i] = new CubeFace(raw_data[i]);
  }

  for(int i=0;i<6;i++) {
    FaceNeighber* fn = face_neighbers(i);
    for(int j=0;j<4;j++) {
      FACE nb_face = fn[j].face;
      EDGE nb_edge = fn[j].edge;
      faces[i]->set_neighber_edge(j, faces[nb_face]->get_edge(nb_edge));
    }
  }
  
  reset();
}

Cube::~Cube() {
  for(int i=0;i<6;i++) {
    delete faces[i];
  }
}

void Cube::reset() {
  for(int i=0; i< 6; i++) {
    for( int j=0; j<9; j++) {
      raw_data[i][j] = i;//WHITE;
    }
    /*if(i==UP) {
    for( int j=0; j<3; j++) {
      raw_data[i][j] = 0;
    }
    for( int j=3; j<6; j++) {
      raw_data[i][j] = 1;
    }
    for( int j=6; j<9; j++) {
      raw_data[i][j] = 2;
    }
    }*/
  }
  //print_faces();
  phase = 5; //finished
  if(on_turned) {
    on_turned(raw_data);
  }
}

void Cube::print_faces() {
  for(int i=0;i<6;i++) {
    for(int j=0;j<9;j++){
      Serial.print(raw_data[i][j]);
      Serial.print("\t");
    }
    Serial.print("\n");
  }
   Serial.print("-------------------------\n");
}


void Cube::turn(enum FACE face, enum DIR dir){
  if(is_turning()){
    return;
  }
  this->turning_face = face;
  this->dir = dir;
  phase = 0;
  //tick = 0;
  
}

void Cube::on_wipe(enum FACE face, enum EDGE edge, enum DIR dir){
  enum FACE nf = face_neighbers(face)[edge].face;
  /*Serial.write(get_face_name(nf));
  if(dir == ACW ) {
    Serial.print("'\n");
  }else {
    Serial.print("\n");
  }*/
  turn(nf, dir);
}

void Cube::on_tick_10ms(){
  if(!is_turning()) {
    return;
  }

  tick++;
  tick %= ON_TICK_PERIOD;
  if(tick == 0) {
    if(phase%2 == 0) {
      faces[turning_face]->shift_nb(dir);
    }else{
      faces[turning_face]->shift(dir);
    }
    phase++;
    on_turned(raw_data);
  }
}

bool Cube::is_turning(){
  return phase < 5 ;
}

void Cube::set_on_turned_callback(pOnTurned on_turned) {
  this->on_turned = on_turned;
}

void Cube::_turn(enum FACE face, enum DIR dir) {
  enum COLOR temp_nb_edge[3], temp_edge[3];
  int to_idx,from_idx, inc;
  if(dir == ACW) {
    to_idx = 3;
    inc = -1;
  } else {
    to_idx = 0;
    inc = 1;
  }

  //copy edge of the turnning face to temp buff
  copy(face, to_idx, temp_edge);
  FaceNeighber from_nb, to_nb = face_neighbers(face)[to_idx];
  //copy first neighber to temp array
  copy(to_nb.face, to_nb.edge, temp_nb_edge);
  for(int i=0;i<3; i++ ){
    from_idx = to_idx + inc;
    //copy edge of the turnning face
    copy(face, from_idx, to_idx);
    from_nb = face_neighbers(face)[from_idx];
    //copy edge of the neighber faces
    copy(from_nb.face, from_nb.edge, to_nb.face, to_nb.edge);
    to_idx = from_idx;
    to_nb = from_nb;
  }
  
  //copy temp buf to the edge of the turnning face
  copy(temp_edge, face, to_idx);
  //copy the temp array to the last neighber
  copy(temp_nb_edge, to_nb.face, to_nb.edge);
}

void Cube::copy(int from_face, int from_edge, enum COLOR* edge_buf) {
  for(int i=0;i<3;i++) {
     edge_buf[i] = raw_data[from_face][edge_block_idxs[from_edge][i]];
  }
}

void Cube::copy(enum COLOR* edge_buf, int to_face, int to_edge) {
  for(int i=0;i<3;i++) {
     raw_data[to_face][edge_block_idxs[to_edge][i]] = edge_buf[i];
  }
}

void Cube::copy(int from_face, int from_edge, int to_face, int to_edge) {
  for(int i=0;i<3;i++) {
     raw_data[to_face][edge_block_idxs[to_edge][i]] = raw_data[from_face][edge_block_idxs[from_edge][i]];
  }
}

void Cube::copy(int face, int from_edge, int to_edge) {
  enum COLOR temp_edge[3];
  copy(face, from_edge, temp_edge);
  copy(temp_edge, face, to_edge);
}
