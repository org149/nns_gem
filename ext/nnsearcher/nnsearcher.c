#include "ruby.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#define PI 3.14159265358979323846
#define MAX_DIST 10000000.0

typedef struct {
  VALUE id;
  VALUE lng;
  VALUE lat;
} point_data;


static VALUE t_init(VALUE self, VALUE dotes);
static VALUE t_search(VALUE self, VALUE lng, VALUE lat);
void quickSortR(point_data* a, long N);

VALUE NNSearcher = Qnil;

point_data *p_dotes = NULL;
int global_dot_count;
int GLOBAL_INIT = 0;

static VALUE t_init(VALUE self, VALUE dotes)
{
  VALUE dot = Qnil;
  VALUE funcall_result = Qnil;
  VALUE ary_count = Qnil;
  ary_count = rb_funcall(dotes, rb_intern("count"),0);
  int dot_count = NUM2INT(ary_count);
  int counter = 0;
  int index = 0;
  if(GLOBAL_INIT){
    free(p_dotes);    
    p_dotes = malloc(dot_count * (sizeof(VALUE)+10));
  }else{
    GLOBAL_INIT = 1;
    p_dotes = (point_data *)malloc(dot_count * (sizeof(VALUE)+10));
  }
  while ( dot_count > 0 ){
    funcall_result = rb_ary_entry(dotes, index);
    index++;
    dot_count--;
    dot = funcall_result;
    if(dot != Qnil){
      VALUE name = rb_ary_entry(dot,0);
      VALUE lng  = rb_ary_entry(dot,1);
      VALUE lat  = rb_ary_entry(dot,2);
      p_dotes[counter].id  = name;
      p_dotes[counter].lng = lng;
      p_dotes[counter].lat = lat; 
      counter++; 
    }else{
      break;
    }
  }
  global_dot_count = counter;  
  quickSortR(p_dotes, counter - 1);
  return self;
}

static double radians(double angle)
{
  return (angle/180.0) * PI;
}

static double ext_gcd(double lng1, double lat1, double lng2, double lat2)
{
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  lng1 = radians(lng1);
  lng2 = radians(lng2);
  return acos( sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos( abs(lng2-lng1) ) );
}

static VALUE t_search(VALUE self, VALUE lng, VALUE lat)
{
  double min_dist = MAX_DIST;
  double work_dist = 0.0;
  double nl_dist = MAX_DIST; double nr_dist = MAX_DIST;
  point_data neigh;
  point_data neigh_l;
  point_data neigh_r;
  neigh.id = Qnil;
  neigh_r.id = Qnil;
  neigh_l.id = Qnil;
  int i;
  int j = 0;
  while(p_dotes[j].lng < lng) j++;
  int center = j;
  int m; int n;
  for(n=0;n<j;n++){
    if(min_dist < abs(p_dotes[j-n].lng-lng)) break;
    work_dist = ext_gcd(NUM2DBL(lng), NUM2DBL(lat), NUM2DBL(p_dotes[j-n].lng), NUM2DBL(p_dotes[j-n].lat));
    if(work_dist < min_dist){
      min_dist = work_dist;
      neigh_l = p_dotes[j-n];
    }
  }
  if(neigh_l.id != Qnil) nl_dist = min_dist;
  min_dist = MAX_DIST;
  for(m=j;m<global_dot_count;m++)
  {
    if(min_dist < abs(p_dotes[m].lng-lng)) break;
    work_dist = ext_gcd(NUM2DBL(lng), NUM2DBL(lat), NUM2DBL(p_dotes[m].lng), NUM2DBL(p_dotes[m].lat));
    if(work_dist < min_dist){
      min_dist = work_dist;
      neigh_r = p_dotes[m];
    }
  }
  if(neigh_r.id != Qnil) nr_dist = min_dist;

  if(nl_dist >= nr_dist)
  {
    neigh = neigh_r;
    if(neigh.id == Qnil) neigh = neigh_l;
  }else{
    neigh = neigh_l;
    if(neigh.id == Qnil) neigh = neigh_r;
  }
  
  /*for(i = 0; i < global_dot_count; i++){
    work_dist = ext_gcd(NUM2DBL(lng), NUM2DBL(lat), NUM2DBL(p_dotes[i].lng), NUM2DBL(p_dotes[i].lat));
    if(work_dist < min_dist){
      min_dist = work_dist;
      neigh = p_dotes[i];
    }
  }*/
  return (neigh.id == Qnil ? Qnil : neigh.id);
}

void Init_nnsearcher() {
  NNSearcher = rb_define_class("NNSearcher", rb_cObject);
  rb_define_method(NNSearcher, "initialize", t_init, 1);
  rb_define_method(NNSearcher, "nearest_neighbour", t_search, 2);
}

void quickSortR(point_data* a, long N) {
  long i = 0, j = N; 		
  point_data temp, p;
  p = a[ N >> 1 ];		
  do {
    while ( a[i].lat < p.lat ) i++;
    while ( a[j].lat > p.lat ) j--;
    if (i <= j) {
      temp = a[i]; a[i] = a[j]; a[j] = temp;
      i++; j--;
    }
  } while ( i<=j );
  if ( j > 0 ) quickSortR(a, j);
  if ( N > i ) quickSortR(a+i, N-i);
}


