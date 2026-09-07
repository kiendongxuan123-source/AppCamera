#include <glib-object.h>
#include <stdio.h>
#include "app_camera.h"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    g_print("================== BẮT ĐẦU KỊCH BẢN TEST ==================\n\n");

    
    AppCamera *cam_gate = app_camera_new();
    AppCamera *cam_yard = app_camera_new();



    /* 1.1. Self-Set thông tin Public & Private */
    app_camera_set_id(cam_gate, "CAM_GATE_01");
    app_camera_set_name(cam_gate, "Camera Cong Chinh");
    app_camera_set_width(cam_gate, 1920);
    app_camera_set_height(cam_gate, 1080);
    app_camera_set_fps(cam_gate, 30);
    app_camera_set_rtsp_url(cam_gate, "rtsp://192.168.1.100/stream1");

    /* 1.2. Self-Get để kiểm tra lại */
    g_print("  [cam_gate Self-Get] ID      : %s\n", app_camera_get_id(cam_gate));
    g_print("  [cam_gate Self-Get] Name    : %s\n", app_camera_get_name(cam_gate));
    g_print("  [cam_gate Self-Get] Do phan giai: %ux%u @ %u FPS\n", 
            app_camera_get_width(cam_gate), 
            app_camera_get_height(cam_gate), 
            app_camera_get_fps(cam_gate));
    g_print("  [cam_gate Self-Get] RTSP URL: %s\n\n", app_camera_get_rtsp_url(cam_gate));


    /* 2.1. cam_yard ĐỌC (GET) thuộc tính của cam_gate qua Public API & GObject */
    const gchar *gate_id_read_by_yard   = app_camera_get_id(cam_gate); // Đọc Private Prop
    const gchar *gate_name_read_by_yard = app_camera_get_name(cam_gate); // Đọc Public Prop
    
    guint gate_fps_read_by_yard = 0;
    g_object_get(cam_gate, "fps", &gate_fps_read_by_yard, NULL); // Đọc qua g_object_get

    g_print("  -> [cam_yard GET từ cam_gate] Doc ID  = '%s'\n", gate_id_read_by_yard);
    g_print("  -> [cam_yard GET từ cam_gate] Doc Name = '%s'\n", gate_name_read_by_yard);
    g_print("  -> [cam_yard GET từ cam_gate] Doc FPS  = %u\n", gate_fps_read_by_yard);


    /* 2.2. cam_yard THAY ĐỔI (SET) thuộc tính của cam_gate */
    g_print("\n  -> [cam_yard SET lên cam_gate] Tien hanh doi cau hinh...\n");

    // cam_yard đổi tên và FPS của cam_gate thông qua Public API
    app_camera_set_name(cam_gate, "Camera Cong (Da duoc cam_yard doi ten)");
    
    // cam_yard dùng g_object_set để thay đổi độ phân giải và FPS của cam_gate
    g_object_set(cam_gate, 
                 "width", 2560,
                 "height", 1440,
                 "fps", 60,
                 NULL);

    // cam_yard đổi URL RTSP (Private property exposed via API) của cam_gate
    app_camera_set_rtsp_url(cam_gate, "rtsp://192.168.1.100/stream_2k_60fps");


    /* 2.3. Kiểm tra lại cam_gate sau khi bị cam_yard sửa đổi */
    g_print("\n  [Kien tra cam_gate sau khi bi cam_yard SET]:\n");
    g_print("  - Name moi        : %s\n", app_camera_get_name(cam_gate));
    g_print("  - Do phan giai moi: %ux%u @ %u FPS\n", 
            app_camera_get_width(cam_gate), 
            app_camera_get_height(cam_gate), 
            app_camera_get_fps(cam_gate));
    g_print("  - RTSP URL moi    : %s\n\n", app_camera_get_rtsp_url(cam_gate));


    /* =========================================================================
     * THU HỒI BỘ NHỚ CỦA CẢ 2 OBJECT
     * ========================================================================= */
    g_print("--- Huy object---\n");
    g_object_unref(cam_gate);
    g_object_unref(cam_yard);
    g_print("Da unref va don dep 2 camera!\n");

    g_print("\n================== KET THUC TEST ==================\n");
    return 0;
}