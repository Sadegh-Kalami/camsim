#ifndef _NSRSIMPARAMREADER_H_
#define _NSRSIMPARAMREADER_H_

enum RenderWhat {RENDER_LUMINANCE, RENDER_COLOR_IMAGE, RENDER_DEPTH};
enum ShowWhat {SHOW_MAP, SHOW_PATTERN};
enum DoWhat {DO_NOTHING, DO_SAVE_RAW, DO_IMAGE_PROC, DO_SAVE_ROS_BAG};

extern int execution_turn;
extern char execution_desc[];

extern int param_seed;
extern int param_render_what;
extern int param_show_what;
extern int param_do_what;

extern bool param_rosbag_compression;

extern char settings_dempath[];
extern char settings_mappath[];

extern double param_map_center_lat;
extern double param_map_center_lon;
extern double param_map_max_alt;
extern double param_map_max_dist;
extern int param_map_zoom; //auto

extern double param_pattern_metric_width;
extern char param_pattern_file[];

extern char param_path_file[];
enum PositionFmt {POSITION_FMT_LLA, POSITION_FMT_XYA, POSITION_FMT_XYZ};
extern int param_position_format;
enum TrackerType {TRACKER_TYPE_DESCRETE=0, TRACKER_TYPE_LINEAR=1, TRACKER_TYPE_1xSQRT=2, TRACKER_TYPE_2xSQRT=3, TRACKER_TYPE_3xSQRT=4, TRACKER_TYPE_2xSQRT_INOUT_FILTER=6};
extern int param_tracker_type;
extern double param_control_freq;
extern double param_max_acc;
extern double param_max_ang_acc;

extern double param_end_time;
extern double param_speed_factor;
extern double param_world_scale;
extern double param_alt_offset;

extern double param_camera_fps;
extern double param_camera_phase_percent;

//extern double param_resolution_scale;
extern int param_width;
extern int param_height;
extern double param_fx, param_fy;
extern double param_ox;
extern double param_oy;
extern double param_f_err;
extern double param_ox_err;
extern double param_oy_err;

extern int param_max_lens_iterations;
extern double param_max_pix_error;
extern double param_k1;
extern double param_k2;
extern double param_t1;
extern double param_t2;
extern double param_k3;
extern double param_k4;
extern double param_k5;
extern double param_k6;
extern double param_k1_err;
extern double param_k2_err;
extern double param_t1_err;
extern double param_t2_err;
extern double param_k3_err;

extern char param_vignet_file[];
extern double param_vignet_thresh1;
extern double param_vignet_thresh2;

extern double param_td;
extern double param_tr;
extern double param_te;
extern int param_motion_blur_extra_samples;
extern double param_ti;
extern double param_td_err;
extern double param_tr_err;

extern double param_extra_margin;
extern double param_extra_zoom;

extern double param_day_light;
extern double param_noise_amp_dynamic;
extern double param_noise_amp_static1;
extern double param_noise_amp_static2;
extern double param_max_fog_distance;

extern double param_cam_in_ac_roll;
extern double param_cam_in_ac_pitch;
extern double param_cam_in_ac_yaw;
extern double param_cam_in_ac_err_x;
extern double param_cam_in_ac_err_y;
extern double param_cam_in_ac_err_z;


#define SAFE_MODE 0

#ifdef __cplusplus
extern "C" {
#endif

void nsrReadSimParams(const char *filename);
void nsrSaveSimParams(const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* _NSRSIMPARAMREADER_H_ */
