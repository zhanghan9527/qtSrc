#ifndef _GP12_TRACK_H_
#define _GP12_TRACK_H_

#ifdef __cplusplus
extern "C" {
#endif

int track_get_memory_size(void);

void track_init(void *work_mem, int width, int height);

int track_get_search_cnt(void *work_mem);

void track_set_face_location(void *work_mem, gpRect *r, int face_cnt);

void track_reset_face_location(void *work_mem);

void track_run(void *work_mem, int face_cnt);

void *track_get_search_range(void *work_mem);

void *track_get_search_min_width(void *work_mem);

int track_get_min_wnd(void *work_mem);

int track_get_max_range(void *work_mem);

int *track_get_xstep(void *work_mem);

int *track_get_scale(void *work_mem);

#ifdef __cplusplus
}
#endif


#endif


