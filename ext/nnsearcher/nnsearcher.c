#include "ruby.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#define PI 3.14159265358979323846
#define MAX_DISTG 10000000.0
#define DOTZOOM 10000.0

typedef struct {
  VALUE id;
  VALUE lng;
  VALUE lat;
} point_data;


static VALUE t_init(VALUE self, VALUE dotes);
static VALUE t_search(VALUE self, VALUE lng, VALUE lat);
void quickSortR(point_data* a, long N);

VALUE NNSearcher = Qnil;

static point_data *p_dotes;
int global_dot_count = 0;
int GLOBAL_INIT = 0;
int cities_passed = 0;

static VALUE t_init(VALUE self, VALUE dotes)
{
  VALUE dot = Qnil;
  VALUE funcall_result = Qnil;
  VALUE ary_count = Qnil;
  ary_count = rb_funcall(dotes, rb_intern("count"),0);
  int dot_count = NUM2INT(ary_count);
  int counter = 0;
  int index = 0;
  if( dot_count > 0 ){
    if(GLOBAL_INIT){
      free(p_dotes);    
      p_dotes = malloc((dot_count) * sizeof(point_data));
    }else{
      GLOBAL_INIT = 1;
      p_dotes = malloc((dot_count) * sizeof(point_data));
    }
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
  if( global_dot_count > 0 ){
    quickSortR(p_dotes, counter - 1);
  }
  return self;
}

static double radians(double angle)
{
  return (angle/180.0) * PI;
}

static double ext_gcd(double lng1, double lat1, int lng2, int lat2)
{
  double lat1r = radians(lat1);
  double lat2r = radians((double)lat2 / DOTZOOM);
  double lng1r = radians(lng1);
  double lng2r = radians((double)lng2 / DOTZOOM);

  if( (lat1r - lat2r == 0) && (lng1r - lng2r == 0)){
    return 0.0;
  }else{
    return acos( sin(lat1r)*sin(lat2r) + cos(lat1r)*cos(lat2r)*cos( (lng1r-lng2r) ) );
  }
}

static VALUE t_search(VALUE self, VALUE lng, VALUE lat)
{
  if( global_dot_count <= 0 ){
    return Qnil;
  }
  double min_dist = MAX_DISTG;
  double work_dist = 0.0;
  double nl_dist = MAX_DISTG; double nr_dist = MAX_DISTG;
  point_data neigh;
  point_data neigh_l;
  point_data neigh_r;
  neigh.id = Qnil;
  neigh_r.id = Qnil;
  neigh_l.id = Qnil;
  int i;
  int j = 0;
  cities_passed = 0;

  while((double)NUM2INT(p_dotes[j].lat) / DOTZOOM < (double)NUM2DBL(lat)){
   j++;
  if(j>=global_dot_count - 1) break;
  }
  int center = j;
  int m; 
  int n;
  for( n=1; n<=j; n++){

    cities_passed = cities_passed + 1;
    if ( min_dist < ext_gcd(0, NUM2DBL(lat), 0, NUM2INT(p_dotes[j-n].lat))) break; 
    work_dist = ext_gcd(NUM2DBL(lng), NUM2DBL(lat), NUM2INT(p_dotes[j-n].lng), NUM2INT(p_dotes[j-n].lat));

    if(work_dist < min_dist){

      min_dist = work_dist;
      neigh_l = p_dotes[j-n];
    }
  }
  if(neigh_l.id != Qnil) nl_dist = min_dist;
  min_dist = MAX_DISTG;
  for(m=j;m<global_dot_count;m++)
  {

    cities_passed++;
    if ( min_dist < ext_gcd(0, NUM2DBL(lat), 0, NUM2INT(p_dotes[m].lat))) break; 
    work_dist = ext_gcd(NUM2DBL(lng), NUM2DBL(lat), NUM2INT(p_dotes[m].lng), NUM2INT(p_dotes[m].lat));

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
  return (neigh.id == Qnil ? Qnil : neigh.id);
}

static VALUE t_passed(VALUE self)
{
  return INT2NUM( cities_passed );
}

static VALUE t_globaldot(VALUE self)
{
  return INT2NUM( global_dot_count );
}

void Init_nnsearcher() {
  NNSearcher = rb_define_class("NNSearcher", rb_cObject);
  rb_define_method(NNSearcher, "initialize", t_init, 1);
  rb_define_method(NNSearcher, "nearest_neighbour", t_search, 2);
  rb_define_method(NNSearcher, "cities_passed", t_passed, 0);
  rb_define_method(NNSearcher, "global_dot_count", t_globaldot, 0);
}

void quickSortR(point_data* a, long N) 
{
  long i = 0, j = N; 		
  point_data temp, p;
  p = a[ N >> 1 ];		
  do {
    while ( NUM2INT( a[i].lat ) < NUM2INT( p.lat ) ) i++;
    while ( NUM2INT( a[j].lat ) > NUM2INT( p.lat ) ) j--;
    if (i <= j) {
      temp = a[i]; a[i] = a[j]; a[j] = temp;
      i++; j--;
    }
  } while ( i <= j );
  if ( j > 0 ) quickSortR(a, j);
  if ( N > i ) quickSortR(a+i, N-i);
}


