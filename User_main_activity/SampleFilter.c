#include "SampleFilter.h"

static double filter_taps[SAMPLEFILTER_TAP_NUM] = {
  -0.007775715121256268,
  -0.007938974136595613,
  -0.009534265788246128,
  -0.008779578259641298,
  -0.004381884421750879,
  0.004666131585205163,
  0.0188044731228937,
  0.03764144706001848,
  0.05992101812383003,
  0.08357444021744635,
  0.10601855702701225,
  0.12454015906119098,
  0.13674393462068657,
  0.14100385434561774,
  0.13674393462068657,
  0.12454015906119098,
  0.10601855702701225,
  0.08357444021744635,
  0.05992101812383003,
  0.03764144706001848,
  0.0188044731228937,
  0.004666131585205163,
  -0.004381884421750879,
  -0.008779578259641298,
  -0.009534265788246128,
  -0.007938974136595613,
  -0.007775715121256268
};

void SampleFilter_init(SampleFilter* f) {
  int i;
  for(i = 0; i < SAMPLEFILTER_TAP_NUM; i++)
    f->history[i] = 0;
		f->last_index = 0;
}

void SampleFilter_put(SampleFilter* f, double input) {
  f->history[f->last_index++] = input;
  if(f->last_index == SAMPLEFILTER_TAP_NUM)
    f->last_index = 0;
}

double SampleFilter_get(SampleFilter* f) {
  double acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SAMPLEFILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : SAMPLEFILTER_TAP_NUM-1;
    acc += f->history[index] * filter_taps[i];
  };
  return acc;
}