#include "ruby.h"
#include "stdlib.h"
#include "math.h"
#define PI 3.14159265358979323846

static VALUE t_init(VALUE self, VALUE dotes);
static VALUE t_search(VALUE self, VALUE lng, VALUE lat);

VALUE NNSearcher = Qnil;

typedef struct {
  VALUE id;
  VALUE lng;
  VALUE lat;
} point_data;

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
  int iter = 0;
  
  if(GLOBAL_INIT){
    free(p_dotes);    
    p_dotes = malloc(dot_count * (sizeof(VALUE)+1));
  }else{
    p_dotes = (point_data *)malloc(dot_count * (sizeof(VALUE)+1));
  }
  global_dot_count = dot_count;
  while ( dot_count > 0 ){
    funcall_result = rb_ary_entry(dotes, index);
    index++;
    dot_count--;
    if ( funcall_result != Qnil){
      dot = funcall_result;
      if(dot != Qnil){
        VALUE name = rb_ary_entry(dot,0);
        VALUE lng  = rb_ary_entry(dot,1);
        VALUE lat  = rb_ary_entry(dot,2);
        p_dotes[counter].id  = name;
        p_dotes[counter].lng = lng;
        p_dotes[counter].lat = lat; 
        counter++;
      }  
    }else{
      break;
    }
  }
  if (!GLOBAL_INIT) GLOBAL_INIT = !GLOBAL_INIT;
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
  double min_dist = 10000000.0;
  double work_dist = 0.0;
  point_data neigh;
  neigh.id = Qnil;
  int i;
  for(i = 0; i < global_dot_count; i++){
    work_dist = ext_gcd(NUM2DBL(lng), NUM2DBL(lat), NUM2DBL(p_dotes[i].lng), NUM2DBL(p_dotes[i].lat));
    if(work_dist < min_dist){
      min_dist = work_dist;
      neigh = p_dotes[i];
    }
  }
  return neigh.id;
}

void Init_nnsearcher() {
  NNSearcher = rb_define_class("NNSearcher", rb_cObject);
  rb_define_method(NNSearcher, "initialize", t_init, 1);
  rb_define_method(NNSearcher, "nearest_neighbour", t_search, 2);
}


