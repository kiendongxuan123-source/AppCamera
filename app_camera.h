#ifndef APP_CAMERA_H_
#define APP_CAMERA_H_
#include <glib-object.h>

G_BEGIN_DECLS  // dam bao tuong thich giua C va C++

/* 
 * Khai báo Modern Macro: Chỉ 2 dòng này sẽ tự động sinh ra toàn bộ các macro
 * như APP_CAMERA(), APP_IS_CAMERA(), APP_TYPE_CAMERA... 
 */
#define APP_TYPE_CAMERA (app_camera_get_type())
G_DECLARE_DERIVABLE_TYPE (AppCamera, app_camera, APP, CAMERA, GObject)

typedef enum {
    APP_CAMERA_STATUS_DISCONNECTED,  /* Chưa kết nối / Mất nguồn / Rút dây */
    APP_CAMERA_STATUS_CONNECTING,    /* Đang bắt tay kết nối (RTSP/USB) */
    APP_CAMERA_STATUS_IDLE,          /* Đã bật, sẵn sàng nhưng chưa stream/record */
    APP_CAMERA_STATUS_STREAMING,     /* Đang phát luồng video live bình thường */
    APP_CAMERA_STATUS_RECORDING,     /* Đang ghi hình vào ổ cứng */
    APP_CAMERA_STATUS_ERROR          /* Bị lỗi (Hỏng cảm biến, sai mật khẩu, tràn RAM...) */
} AppCameraStatus;
#define APP_TYPE_CAMERA_STATUS (app_camera_status_get_type())
GType app_camera_status_get_type (void);
struct _AppCameraClass {
    GObjectClass parent_class;
};

/* Khởi tạo object */
AppCamera* app_camera_new(void);  // khai bao ham khoi tao khong co tham so dau vao

/* 
 * PUBLIC API CHO CÁC THUỘC TÍNH PRIVATE:
 * Những thuộc tính nhạy cảm (id, rtsp_url, codec) bị giấu trong file .c
 * nên bắt buộc phải expose API để object khác gọi.
 */
const gchar* app_camera_get_id(AppCamera *self);
void         app_camera_set_id(AppCamera *self, const gchar *id);

const gchar* app_camera_get_rtsp_url(AppCamera *self);
void         app_camera_set_rtsp_url(AppCamera *self, const gchar *rtsp_url);

const gchar* app_camera_get_codec(AppCamera *self);
void         app_camera_set_codec(AppCamera *self, const gchar *codec);

const gchar* app_camera_get_name(AppCamera *self);
void app_camera_set_name(AppCamera *self, const gchar *name);

AppCameraStatus app_camera_get_status(AppCamera *self);
void app_camera_set_status(AppCamera *self, AppCameraStatus status);

guint app_camera_get_width(AppCamera *self);
void app_camera_set_width(AppCamera *self, guint width);

guint app_camera_get_height(AppCamera *self);
void app_camera_set_height(AppCamera *self, guint height);

guint app_camera_get_fps(AppCamera *self);
void app_camera_set_fps(AppCamera *self, guint fps);

gdouble app_camera_get_aspect_ratio(AppCamera *self);

gboolean app_camera_get_enabled(AppCamera *self);
void app_camera_set_enabled(AppCamera *self, gboolean enabled);

gboolean app_camera_get_recording(AppCamera *self);
void app_camera_set_recording(AppCamera *self, gboolean recording);

gdouble app_camera_get_pan(AppCamera *self);
void app_camera_set_pan(AppCamera *self, gdouble pan);

gdouble app_camera_get_tilt(AppCamera *self);
void app_camera_set_tilt(AppCamera *self, gdouble tilt);

gdouble app_camera_get_zoom(AppCamera *self);
void app_camera_set_zoom(AppCamera *self, gdouble zoom);
G_END_DECLS
#endif