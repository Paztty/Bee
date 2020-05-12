#ifndef SAMPLEFILTER_H_
#define SAMPLEFILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 1000 Hz

* 0 Hz - 50 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = 3.666895709039203 dB

* 100 Hz - 500 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -41.09174091632818 dB

*/

#define SAMPLEFILTER_TAP_NUM 27

typedef struct {
  double history[SAMPLEFILTER_TAP_NUM];
  unsigned int last_index;
} SampleFilter;

void SampleFilter_init(SampleFilter* f);
void SampleFilter_put(SampleFilter* f, double input);
double SampleFilter_get(SampleFilter* f);

#endif
